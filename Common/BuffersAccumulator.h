#pragma once

#include "Common/DataBuffer.h"
#include "Common/TimeStamp.h"
#include <queue>
#include <map>
#include "Common/LogEvent.h"
#include "Common/Utils.h"

//////////////////////////////////////////////////////////////////////////
// CBufferAccumulator
// Helper class that accumulate packets of a specific buffer/message. This
// class is internal to be used by the CBuffersAccumulator class.
// It expects packets to be received in a sequential order and does not
// try to reorder the packets.

class CBufferAccumulator
{
public:
    CBufferAccumulator();

    //accumulate packet data. returns false in case that it receives
    //first packet with counter that is not 1 or packet with counter
    //that is not sequential.
    bool AddPacket(const BYTE* PacketData, int DataSize, 
        int PacketCounter, bool IsLastPacket);

    //return if all packets of the buffer were received
    bool IsCompleted() const; 

    //retrieve the accumulated data
    bool GetData(const BYTE*& BufferData, int& DataSize);

    //helper function that will enable to find outdated messages that are not complete
    CTimeStamp GetLastUpdateTime() const;

private:

    bool AppendPacket(const BYTE* PacketData, int DataSize, 
        int PacketCounter, bool IsLast);
private:
    CDataBuffer m_DataBuffer;
    int m_LastPacketCounter;
    CTimeStamp m_LastUpdateTime;
    bool m_IsCompleted;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CBuffersAccumulator
// Accumulate buffers/messages that are being passed in packets. It uses a 
// template parameter as a key and context for the buffers. It uses a map of
// CBufferAccumulator objects per buffer.
// The class is not thread safe and assumes that all calls will be made on the
// same thread.
// When all the data of a buffer/message is received the CBuffersAccumulator will
// notify a client using an IBuffersAccumulatorEvents sink interface. 

template<typename BufferKeyType>
class IBuffersAccumulatorEvents
{
public:
    virtual void OnCompleteBuffer(const BufferKeyType& BufferKey, const BYTE* Data, int DataSize) = 0;
};

template<typename BufferKeyType>
class CBuffersAccumulator
{
public:
    CBuffersAccumulator();

    void Advise(IBuffersAccumulatorEvents<BufferKeyType>* Sink);

    //add packet of a specific buffer defined by a specific key.
    bool AddPacket(const BufferKeyType& BufferKey, const char* BufferContextStr,
        const BYTE* PacketData, int PacketDataSize, int PacketCounter, bool IsLast);

    //remove in progress accumulated buffers that their last packet
    //was received earlier than 'OlderTime'. Return a vector of removed items.
    void RemoveOldItems(const CTimeStamp& OlderTime, 
        std::vector<BufferKeyType>& RemovedItems);

    //clear in progress accumulated buffers
    void Clear();

    bool IsEmpty() const;

private:
    //return false in case unexpected failure
    bool AddPacketToAccumulator(const BufferKeyType& BufferKey, 
        const char* BufferContextStr, 
        CBufferAccumulator* BufferAccumulator,
        const BYTE* PacketData,   int PacketDataSize,
        int PacketCounter, bool IsLast, bool& IsCompleted);

    void SendCompleteBuffer(const BufferKeyType& BufferKey, 
        const BYTE* CompleteBufferData, int BufferSize);
 
private:
    typedef std::map<BufferKeyType, CBufferAccumulator*> BuffersAccumulatorsMap;
    //typename BuffersAccumulatorsMap::iterator AccumulatorIterator;
    BuffersAccumulatorsMap m_BufferAccumulators;
    IBuffersAccumulatorEvents<BufferKeyType>* m_Sink;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// implementation of the CBuffersAccumulator

#ifdef _DEBUG
#define new DEBUG_NEW
#define THIS_FILE __FILE__
#endif

template<typename BufferKeyType>
CBuffersAccumulator<BufferKeyType>::CBuffersAccumulator(): m_Sink(NULL)
{
}

template<typename BufferKeyType>
void CBuffersAccumulator<BufferKeyType>::Advise(IBuffersAccumulatorEvents<BufferKeyType>* Sink)
{
    m_Sink = Sink;
}

template<typename BufferKeyType>
bool CBuffersAccumulator<BufferKeyType>::AddPacket(const BufferKeyType& BufferKey, const char* BufferContextStr,
               const BYTE* PacketData, int PacketDataSize, int PacketCounter, bool IsLast)
{
    LogEvent(LE_INFOLOW, "CBuffersAccumulator::AddPacket, %s, DataSize %d, Counter %d, Is Last %d",
        BufferContextStr, PacketDataSize, PacketCounter, IsLast);
    bool IsCompleted = false;

    //try to find the relevant buffers accumulator
    BuffersAccumulatorsMap::iterator Iter = m_BufferAccumulators.find(BufferKey);
    if(Iter == m_BufferAccumulators.end())
    {
        //add new accumulator
        CBufferAccumulator* NewAccumulator = new CBufferAccumulator;
        if(!AddPacketToAccumulator(BufferKey, BufferContextStr, NewAccumulator,
            PacketData, PacketDataSize, 
            PacketCounter, IsLast, IsCompleted))
            return false;

        if(IsCompleted)
        {
            //only one packet
            delete NewAccumulator;
            return true;
        }

        if(!InsertValueToMap(m_BufferAccumulators, BufferKey, NewAccumulator))
        {
            LogEvent(LE_ERROR, "CStreamsAccumulator::AddPacket, %s, DataSize %d, Counter %d, Failed to insert to map",
                BufferContextStr, PacketDataSize, PacketCounter);
            delete NewAccumulator;
            return false;
        }

        //Buffer was added to the map and is not completed 
        return true;
    }
    else
    {   
        //add the packet to the accumulator
        bool IsCompleted = false;
        CBufferAccumulator* BufferAccumulator = Iter->second;
        if(!AddPacketToAccumulator(BufferKey,
            BufferContextStr, BufferAccumulator, 
            PacketData, PacketDataSize, 
            PacketCounter, IsLast, IsCompleted))
            return false;
        if(IsCompleted)
        {
            delete BufferAccumulator;
            m_BufferAccumulators.erase(Iter);
        }
        return true;
    }
}

template<typename BufferKeyType>
void CBuffersAccumulator<BufferKeyType>::RemoveOldItems(
    const CTimeStamp& OlderTime, std::vector<BufferKeyType>& RemovedItems)
{
    BuffersAccumulatorsMap::iterator Iter = m_BufferAccumulators.begin();

    while (Iter != m_BufferAccumulators.end())
    {
        CBufferAccumulator* BufferAccumulator = Iter->second;
        if(BufferAccumulator->GetLastUpdateTime() < OlderTime)
        {
            BufferKeyType BufferToRemove = Iter->first;
            RemovedItems.push_back(BufferToRemove);

            //delete the accumulator and remove it from the map
            delete BufferAccumulator;
            m_BufferAccumulators.erase(BufferToRemove);
            Iter = m_BufferAccumulators.lower_bound(BufferToRemove);
        }
        else
            ++Iter;
    }
}

template<typename BufferKeyType>
void CBuffersAccumulator<BufferKeyType>::Clear()
{
    BuffersAccumulatorsMap::iterator Iter = m_BufferAccumulators.begin();
    while (Iter != m_BufferAccumulators.end())
    {
        CBufferAccumulator* BufferAccumulator = Iter->second;
        delete BufferAccumulator;
        Iter++;
    }

    m_BufferAccumulators.clear();
}

template<typename BufferKeyType>
bool CBuffersAccumulator<BufferKeyType>::IsEmpty() const
{
    return m_BufferAccumulators.empty();
}


//return false in case unexpected failure
template<typename BufferKeyType>
bool CBuffersAccumulator<BufferKeyType>::AddPacketToAccumulator(
    const BufferKeyType& BufferKey, const char* BufferContextStr, 
    CBufferAccumulator* BufferAccumulator,
    const BYTE* PacketData,   int PacketDataSize,
    int PacketCounter, bool IsLast, bool& IsCompleted)
{
    IsCompleted = false;
    if(!BufferAccumulator->AddPacket(PacketData, PacketDataSize, PacketCounter, IsLast))
    {
        LogEvent(LE_ERROR, "CBuffersAccumulator::AddPacketToAccumulator, %s, DataSize %d, int Counter, Failed to add packet to new accumulator",
            BufferContextStr, PacketDataSize, PacketCounter);
        return false;
    }

    if(BufferAccumulator->IsCompleted())
    {
        IsCompleted = true;
        const BYTE* CompleteBufferData = NULL;
        int BufferSize = 0;
        if(BufferAccumulator->GetData(CompleteBufferData, BufferSize))
        {
            SendCompleteBuffer(BufferKey, CompleteBufferData, BufferSize);
            return true;
        }
        else
        {
            Assert(false);
            return false;
        }
    }
    return true;
}

template<typename BufferKeyType>
void CBuffersAccumulator<BufferKeyType>::SendCompleteBuffer(
    const BufferKeyType& BufferKey, 
    const BYTE* CompleteBufferData, int BufferSize)
{
    if(m_Sink == NULL)
    {
        LogEvent(LE_ERROR, "CBuffersAccumulator()SendCompleteBuffer Sink is NULL");
        return;
    }
    m_Sink->OnCompleteBuffer(BufferKey, CompleteBufferData, BufferSize);
}

#ifdef _DEBUG
#define THIS_FILE __FILE__
#undef new
#endif
