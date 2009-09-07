#include "StdAfx.h"
#include "BuffersAccumulator.h"
#include "BuffersQueue.h"
#include "CollectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//CBufferAccumulator

CBufferAccumulator::CBufferAccumulator(): m_LastPacketCounter(0),
        m_LastUpdateTime(CTimeStamp::Now()), m_IsCompleted(false),
        m_DataBuffer(0)
{
}

bool CBufferAccumulator::IsCompleted() const 
{
    return m_IsCompleted;
}

CTimeStamp CBufferAccumulator::GetLastUpdateTime() const
{
    return m_LastUpdateTime;
}

bool CBufferAccumulator::AddPacket(const BYTE* PacketData, int DataSize, 
                                   int PacketCounter, bool IsLastPacket)
{
    if(m_LastPacketCounter == 0)
    {
        //first packet
        if(PacketCounter != 1)
        {
            LogEvent(LE_ERROR, "CBufferAccumulator::AddPacket(), first packet counter is not 1 [counter %d]",
                PacketCounter);
            return false;
        }

        return AppendPacket(PacketData, DataSize, PacketCounter, IsLastPacket);
    }
    else
    {
        //subsequent packets
        bool IsSequentialPacket = (PacketCounter == m_LastPacketCounter + 1);
        if(!IsSequentialPacket)
        {
            LogEvent(LE_ERROR, "CBufferAccumulator::AddPacket(), packet not sequential [incoming %d] [last %d]",
                PacketCounter, m_LastPacketCounter);
            return false;
        }

        return AppendPacket(PacketData, DataSize, PacketCounter, IsLastPacket);
    }
}

bool CBufferAccumulator::GetData(const BYTE*& BufferData, int& DataSize)
{
    if(!m_IsCompleted)
    {
        Assert(false);
        return false;
    }
    BufferData = m_DataBuffer.GetData();
    DataSize = m_DataBuffer.GetDataSize();
    return true;
}

bool CBufferAccumulator::AppendPacket(const BYTE* PacketData, int DataSize, int PacketCounter, bool IsLast)
{
    m_IsCompleted = IsLast;
    m_LastPacketCounter = PacketCounter;
    m_LastUpdateTime = CTimeStamp::Now();
    return m_DataBuffer.Append(PacketData, DataSize);
}


//////////////////////////////////////////////////////////////////////////
// Testing


struct TestBufferContext
{
    int ConnectionId;
    int RequestId;
    bool operator<(const TestBufferContext& Other) const
    {
        if(ConnectionId < Other.ConnectionId)
            return true;
        if(ConnectionId == Other.ConnectionId &&
            RequestId < Other.RequestId)
            return true;
        return false;
    }
};

typedef CBuffersAccumulator<TestBufferContext> IncomingBuffersAccumulator;
typedef IBuffersAccumulatorEvents<TestBufferContext> IncomingBuffersAccumulatorEvents;


class CBuffersAccumulatorTester: public IncomingBuffersAccumulatorEvents
{
public:
    void FunctionalTest()
    {
        m_AutoamticTest = false;
        IncomingBuffersAccumulator Accumulator;
        m_IncomingBuffersAccumulator.Advise(this);
        m_IncomingBuffersAccumulator.Clear();

        TestBufferContext BufferContext1 = { 1, 1};
        const char Buffer1[] = "1234567890";
        TestBufferContext BufferContext2 = { 1, 2};

        Assert(m_IncomingBuffersAccumulator.AddPacket(BufferContext1, 
            GetBufferContextStr(BufferContext1).c_str(), 
            (const BYTE*) Buffer1, sizeof Buffer1,
            1, false));
        Assert( m_IncomingBuffersAccumulator.AddPacket(BufferContext1, 
            GetBufferContextStr(BufferContext1).c_str(), 
            (const BYTE*)Buffer1, sizeof Buffer1,
            2, true));

        Assert(!m_IncomingBuffersAccumulator.AddPacket(BufferContext2, 
            GetBufferContextStr(BufferContext2).c_str(), 
            (const BYTE*) Buffer1, sizeof Buffer1,
            2, true));

        std::vector<TestBufferContext> RemoveItems;
        m_IncomingBuffersAccumulator.RemoveOldItems(CTimeStamp::Now(), RemoveItems);
    }

    void AutomaticTest(int NumBuffers)
    {
        m_AutoamticTest = true;
        m_IncomingBuffersAccumulator.Advise(this);

        m_IncomingBuffersAccumulator.Clear();
        for (int i = 0; i < NumBuffers; ++i)
        {
            GenerateTestBuffer();
            SendPackets();
        }

        while(!m_TestMap.empty())
            SendPackets();

        Assert(m_IncomingBuffersAccumulator.IsEmpty());
        m_IncomingBuffersAccumulator.Clear();
    }

private:
    //IncomingBuffersAccumulatorEvents
    virtual void OnCompleteBuffer(const TestBufferContext& BufferKey, 
        const BYTE* Data, int DataSize)  
    {
        if(!m_AutoamticTest)
            return;

        //compare to sent stream that should be saved in a map
        TestMap::iterator Iter = m_TestMap.find(BufferKey);
        if(Iter == m_TestMap.end())
        {
            Assert(false);
            return;
        }

        TestBufferData& OriginalData = Iter->second;
        if(DataSize != OriginalData.DataSize)
        {
            Assert(false);
            return;
        }

        if(memcmp(Data, OriginalData.Data, DataSize) != 0)
        {
            Assert(false);
            return;
        }
        delete[] OriginalData.Data;
        m_TestMap.erase(Iter);
    }


private:

    void GenerateTestBuffer()
    {
        //insert buffers into test map
        TestBufferContext BufferContext;
        BufferContext.ConnectionId = rand() % 100;
        BufferContext.RequestId = rand();

        TestBufferData BufferData;
        BufferData.DataSize = rand() % 1000000 + 10;
        BufferData.Data = new BYTE[BufferData.DataSize];
        BufferData.SendPosition = 0;
        BufferData.PacketCounter = 0;
        InsertValueToMap(m_TestMap, BufferContext, BufferData);
    }

    void SendPackets()
    {
        //pass packets of buffers from the test map to the buffers accumulator
        TestIterator Iter = m_TestMap.begin();
        while (Iter != m_TestMap.end())
        {
            TestBufferContext Key = Iter->first;
            TestBufferData& TestBuffer = Iter->second;
            int DataLeft = TestBuffer.DataSize - TestBuffer.SendPosition;
            Assert(DataLeft > 0);
            int DataToSend = rand() % 10000;
            if(DataToSend > DataLeft)
                DataToSend = DataLeft;

            const BYTE* DataPtr = TestBuffer.Data + TestBuffer.SendPosition;
            TestBuffer.SendPosition += DataToSend;
            bool Success = m_IncomingBuffersAccumulator.AddPacket(Key, GetBufferContextStr(Key).c_str(), 
                DataPtr, DataToSend,
                ++TestBuffer.PacketCounter, DataToSend == DataLeft);
            Assert(Success);

            //move to next using upper_bound since AddPacket can cause the iterator 
            //to become invalid
            Iter = m_TestMap.upper_bound(Key); 
        }
    }

    std::string GetBufferContextStr(const TestBufferContext& TestContext) const
    {
        return "Buffer [" + IntToStr(TestContext.ConnectionId) + "," + IntToStr(TestContext.RequestId) + "]";
    }


private:
    IncomingBuffersAccumulator m_IncomingBuffersAccumulator;

    struct TestBufferData
    {
        BYTE* Data;
        int DataSize;;
        int SendPosition;
        int PacketCounter;
    };

    typedef std::map<TestBufferContext, TestBufferData> TestMap;
    typedef TestMap::iterator TestIterator;
    TestMap m_TestMap;
    bool m_AutoamticTest;
};

void TestBuffersAccumulator()
{
    CBuffersAccumulatorTester Tester;
    Tester.FunctionalTest();
    Tester.AutomaticTest(100);
}