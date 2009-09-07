#include "StdAfx.h"
#include "BuffersQueue.h"
#include "Common/SimpleThread.h"
#include "CollectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
//Test buffers queue
// The Buffers queue automatic test is being performed by 3 threads that
// insert random buffers to the queue and another thread that retrieve 
// packets from the queue. The test check that all inserted buffers are being
// retrieved from the queue and are equal.
// The test is using 3 test classes:
// CBuffersQueueVerifier -  Verifies that buffers retrieved from the queue equal to 
//                          the buffers that were inserted.
// CBuffersQueueInserter -  inherits from CSimpleThread and receives a reference 
//                          to the Buffers Queue. On Timeout it inserts random buffers
//                          to the queue and to the Verifier.
// CBuffersQueueTester -    The main class that contains the BuffersQueue,
//                          CBuffersQueueVerifier data member and
//                          array of 3 CBuffersQueueInserter. It is responsible to 
//                          initialize its data members and to retrieve data from the 
//                          buffers queue.
//////////////////////////////////////////////////////////////////////////



typedef int TestBufferContext;
typedef CBuffersQueue<TestBufferContext> TestBuffersQueue;

//////////////////////////////////////////////////////////////////////////
// CBuffersQueueVerifier

class CBuffersQueueVerifier
{
public:
    CBuffersQueueVerifier(): m_NumSuccessfullQueueRetrieve(0)
    {
    }

    ~CBuffersQueueVerifier()
    {
        Clear();
        LogEvent(LE_INFOHIGH, "BuffersQueueVerifier DTor:  Total NumSuccessfullQueueRetrieve [%d]", m_NumSuccessfullQueueRetrieve);
    }

    //the verifier takes ownership of the Buffer pointer parameter
    bool InsertNewBuffer(TestBufferContext BufferContext, const BYTE* Buffer, int BufferSize)
    {
        CCriticalSectionLocker Locker(m_MapLock);
        BufferDataToVerify Data;
        Data.InsertedData = (BYTE*)Buffer;
        Data.InsertedDataLen = BufferSize;
        Data.LastPacketCounter = 0;
        return InsertValueToMap(m_VerifiedBufferData, BufferContext, Data);
    }

    void RemoveInsertedBuffer(TestBufferContext BufferContext)
    {
        CCriticalSectionLocker Locker(m_MapLock);
        VerifiedBufferData::iterator Iter = m_VerifiedBufferData.find(BufferContext);
        if(Iter == m_VerifiedBufferData.end())
        {
            Assert(false);
            return;
        }

        BufferDataToVerify& BufferToDelete = Iter->second;
        delete [] BufferToDelete.InsertedData;
        m_VerifiedBufferData.erase(Iter);
    }

    bool UpdateQueueRetrievedData( TestBufferContext Context,
        const BYTE* QueueData,
        int QueueDataLen,
        int PacketCounter,
        bool IsLast)
    {
        CCriticalSectionLocker Locker(m_MapLock);

        VerifiedBufferData::iterator Iter = m_VerifiedBufferData.find(Context);
        if(Iter == m_VerifiedBufferData.end())
        {
            Assert(false);
            return false;
        }

        BufferDataToVerify& UpdatedBuffer = Iter->second;
        Assert(UpdatedBuffer.LastPacketCounter == PacketCounter - 1);
        UpdatedBuffer.LastPacketCounter = PacketCounter;
        bool Success = UpdatedBuffer.QueueRetrievedBuffer.Append(QueueData, QueueDataLen);
        Assert(Success);
        if(IsLast)
        {
            Assert(UpdatedBuffer.InsertedDataLen == UpdatedBuffer.QueueRetrievedBuffer.GetDataSize());
            Assert(memcmp(UpdatedBuffer.InsertedData,
                UpdatedBuffer.QueueRetrievedBuffer.GetData(),
                UpdatedBuffer.InsertedDataLen) == 0);
            delete [] UpdatedBuffer.InsertedData;
            m_VerifiedBufferData.erase(Iter);
            m_NumSuccessfullQueueRetrieve++;
            if(m_NumSuccessfullQueueRetrieve % 10 == 0)
                LogEvent(LE_INFOHIGH, "NumSuccessfullQueueRetrieve [%d]", m_NumSuccessfullQueueRetrieve);

        }
        else
        {
            Assert(UpdatedBuffer.InsertedDataLen > UpdatedBuffer.QueueRetrievedBuffer.GetDataSize());
        }
        return true;
    }

    void Clear()
    {
        CCriticalSectionLocker Locker(m_MapLock);
        VerifiedBufferData::iterator Iter = m_VerifiedBufferData.begin();
        VerifiedBufferData::iterator End = m_VerifiedBufferData.end();
        for (; Iter != End; ++Iter)
        {
            BufferDataToVerify& BufferToDelete = Iter->second;
            delete [] BufferToDelete.InsertedData;
        }

        m_VerifiedBufferData.clear();
    }

private:
    struct BufferDataToVerify
    {
        //updated by the class that insert to the queue
        BYTE* InsertedData;
        int InsertedDataLen;
        //updated by the class that retrieve data from the queue
        CDataBuffer QueueRetrievedBuffer;
        int LastPacketCounter;
    };

    typedef std::map<TestBufferContext, BufferDataToVerify> VerifiedBufferData;
    VerifiedBufferData m_VerifiedBufferData;
    CCriticalSection m_MapLock;
    int m_NumSuccessfullQueueRetrieve;
};

//////////////////////////////////////////////////////////////////////////
// CBuffersQueueInserter

class CBuffersQueueInserter: public CSimpleThread
{
public:
    CBuffersQueueInserter(const char* ContextName, TestBuffersQueue& TestQueue, CBuffersQueueVerifier& Verifier):
      CSimpleThread(ContextName),
      m_TestQueue(TestQueue),
      m_Verifier(Verifier),
      m_InsertedBuffers(0)
    {
    }

    ~CBuffersQueueInserter()
    {
        LogEvent(LE_INFOHIGH, "%s DTor: Total Inserted %d buffers",
            m_ThreadName.c_str(), m_InsertedBuffers);
    }

    bool Init()
    {
        SetTimeout(20);
        return StartThread();
    }

    void Close()
    {
        CloseThread(true);
    }

private:
    virtual void OnTimeout()
    {
        int BufferLen = rand() % 10000 + 10;
        BYTE* Buffer = new BYTE[BufferLen];

        bool BufferAdded = false;
        while(!BufferAdded)
        {
            TestBufferContext Context = rand() % 10000;
            if(m_Verifier.InsertNewBuffer(Context, Buffer, BufferLen))
            {
                if(!m_TestQueue.Insert(Context, Buffer, BufferLen)) //probably queue is full
                {
                    m_Verifier.RemoveInsertedBuffer(Context);
                    return;;
                }
                m_InsertedBuffers++;
                if(m_InsertedBuffers % 10 == 0)
                    LogEvent(LE_INFOHIGH, "%s Inserted %d buffers",
                    m_ThreadName.c_str(), m_InsertedBuffers);
                BufferAdded = true;
            }
        }
    }

private:
    TestBuffersQueue& m_TestQueue;
    CBuffersQueueVerifier& m_Verifier;
    int m_InsertedBuffers;
};

//////////////////////////////////////////////////////////////////////////
// CBuffersQueueTester

class CBuffersQueueTester: public CSimpleThread
{
public:
    CBuffersQueueTester():
      CSimpleThread("CBuffersQueueTester"), m_LastClearTime(CTimeStamp::Now())
    {
        m_TestQueue.Init("TestQueue", 1000000);
        for (int i = 0; i < NUM_INTERTION_THREADS; ++i)
        {
            std::string ThreadName = "Insert thread " + IntToStr(i);
            m_Inserters[i] = new CBuffersQueueInserter(ThreadName.c_str(), m_TestQueue, m_Verifier);
            Assert(m_Inserters[i]->Init());
        }
        SetTimeout(10);
        Assert(StartThread());
    }

    ~CBuffersQueueTester()
    {
        for (int i = 0; i < NUM_INTERTION_THREADS; ++i)
        {
            m_Inserters[i]->Close();
            delete m_Inserters[i] ;
        }
        while(!m_TestQueue.IsEmpty())
        {
            WaitAndPumpMessages(10);
        }

        CloseThread(true);
    }

private:
    virtual void OnTimeout()
    {
        for(int i = 0; i < 4; i++)
            RetrievePacket();
        if(CTimeStamp::Now() - m_LastClearTime > CTimePeriod(3, 0))
        {
            m_TestQueue.Clear();
            m_Verifier.Clear();
            m_LastClearTime = CTimeStamp::Now();
        }
    }

private:
    void RetrievePacket()
    {
        const int RequestedDataSize = 2000;
        TestBufferContext Context;
        const BYTE* PeekedData = NULL;
        int PeekedDataLen = 0;
        int PacketCounter = 0;
        bool IsLast;

        if(!m_TestQueue.Peek(RequestedDataSize, Context, PeekedData, PeekedDataLen,
            PacketCounter, IsLast))
            return;

        Assert(PeekedDataLen > 0);
        if(rand() % 10 == 0)
        {
            //do nothing: simulates a blocked packet, will retry next time
        }
        else
        {

            if(rand() % 3 == 0)
            {
                //failed send that need to be aborted, probably obsolete client 
                m_TestQueue.RemoveFrontBuffer();
            }
            else
            {
                //successfull send
                Assert(m_Verifier.UpdateQueueRetrievedData(Context, PeekedData, PeekedDataLen, 
                    PacketCounter, IsLast));

                m_TestQueue.Remove(PeekedDataLen);
            }
        }
    }

private:
    static const int NUM_INTERTION_THREADS= 3;
    CBuffersQueueInserter* m_Inserters[NUM_INTERTION_THREADS];
    TestBuffersQueue m_TestQueue;
    CBuffersQueueVerifier m_Verifier;
    CTimeStamp m_LastClearTime;
};

//////////////////////////////////////////////////////////////////////////

void AutoBuffersQueueTest()
{
    CBuffersQueueTester Tester;

    WaitAndPumpMessages(30000);

}
