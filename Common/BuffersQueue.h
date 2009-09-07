#pragma once

#include "Common/DataBuffer.h"
#include "Common/TimeStamp.h"
#include <queue>
#include <map>
#include "Common/LogEvent.h"
#include "Common/Utils.h"
#include "Common/SafeQueue.h"

//////////////////////////////////////////////////////////////////////////
// CBuffersQueue
// A templated class that is based on SafeQueue. It let clients insert buffers of data
// to the queue and to retrieve packets of the buffers from the queue.
// The Templated BufferContext parameter is client defined type that allow  
// the client to get a context parameter when retrieving the data from the queue.
// It is expected that mutiple threads will insert data to the queue and that
// only one thread will retrieve data from the queue.
// The Retrieval data is being done in 2 phases:
//  a. Peek - get a data from the queue, at this stage the data is not remove
//              from the queue.
//  b. Remove - remove the peeked data size.
// This retrieval method is used in order to enable clients to perform an operation
// on the retrieved data and to repeat the operation in case the operation temporarily fails.
//////////////////////////////////////////////////////////////////////////


template <typename BufferContext>
class CBuffersQueue
{
public:
    CBuffersQueue();

    void Init(const char* ContextStr, int MaxQueueSizeInBytes);

    //can be called from multiple treads
    bool Insert(const BufferContext& Context, const BYTE* Buffer, int BufferLen);

    //The peek and Remove functions should be called from one specific thread only.
    bool Peek(int RequestedDataSize, BufferContext& Context, const BYTE*& RetrievedBuffer, int& RetrievedBufferLen,
        int& PacketCounter, bool& IsLast);

    //should be called after successful peek
    bool Remove(int RequestedDataSize);

    //should be called in case the all top buffer is useless
    void RemoveFrontBuffer();

    //is the queue empty
    bool IsEmpty();

    //clear the queue
    void Clear();

private:

    //add to or subtract from the m_CurrentSizeInBytes
    void UpdateCurrentQueueSize(int DataSizeChange);

private:
    struct BufferDataItem
    {
        BufferContext Context;
        CDataBuffer DataBuffer;
        int DataPtr;
        int PacketCounter;

        BufferDataItem(): DataPtr(0), PacketCounter(0), DataBuffer(0)
        {
        }
    };
    typedef SafeQueue<BufferDataItem> BuffersQueue; 
    BuffersQueue m_BuffersQueue;
    std::string m_ContextStr;
    int m_MaxQueueSizeInBytes;
    int m_CurrentSizeInBytes;
    CCriticalSection m_Lock;

    enum
    {
        MAX_QUEUE_ITEMS = 10000
    };
};


//////////////////////////////////////////////////////////////////////////
// implementation

template <typename BufferContext>
CBuffersQueue<BufferContext>::CBuffersQueue(): 
    m_MaxQueueSizeInBytes(0), m_CurrentSizeInBytes(0)
{
}

template <typename BufferContext>
void  CBuffersQueue<BufferContext>::Init(const char* ContextStr, int MaxQueueSizeInBytes)
{
    m_ContextStr = ContextStr;
    m_MaxQueueSizeInBytes = MaxQueueSizeInBytes;
    m_BuffersQueue.SetQueueSize(MAX_QUEUE_ITEMS);
}

//can be called from multiple treads
template <typename BufferContext>
bool CBuffersQueue<BufferContext>::Insert(const BufferContext& Context, const BYTE* Buffer, int BufferLen)
{
    BufferDataItem Item;
    Item.Context = Context;
    Item.DataBuffer.Append(Buffer, BufferLen);
    Item.DataPtr = 0;
    Item.PacketCounter = 1;

    CCriticalSectionLocker Locker(m_Lock);
    if(m_CurrentSizeInBytes + BufferLen > m_MaxQueueSizeInBytes)
    {
        LogEvent(LE_WARNING, "CBuffersQueue(%s)::Insert, Queue is Full CurrentSizeInBytes[%d], Added Buffer Len [%d], Max Size [%d]",
            m_ContextStr.c_str(), m_CurrentSizeInBytes, BufferLen, m_MaxQueueSizeInBytes);
        return false;
    }

    if(!m_BuffersQueue.Push(Item))
    {
        LogEvent(LE_WARNING, "CBuffersQueue(%s)::Insert, m_BuffersQueue.Push failed",
            m_ContextStr.c_str());
        return false;
    }

    UpdateCurrentQueueSize(BufferLen);
    return true;
}

//The peek and Remove functions should be called from one specific thread only.
template <typename BufferContext>
bool CBuffersQueue<BufferContext>::Peek(int RequestedDataSize, BufferContext& Context, const BYTE*& RetrievedBuffer, int& RetrievedBufferLen,
          int& PacketCounter, bool& IsLast)
{
    IsLast = false;
    if(m_BuffersQueue.IsEmpty())
        return false;

    BufferDataItem& Item = m_BuffersQueue.Peek();
    Assert(Item.DataPtr < Item.DataBuffer.GetDataSize());

    Context = Item.Context;
    PacketCounter = Item.PacketCounter;

    RetrievedBuffer = Item.DataBuffer.GetData() + Item.DataPtr;
    RetrievedBufferLen = RequestedDataSize;
    int LeftData = Item.DataBuffer.GetDataSize() - Item.DataPtr;
    if(LeftData <= RequestedDataSize)
    {
        RetrievedBufferLen = LeftData;
        IsLast = true;
    }
    return true;
}

//should be called after successful peek
template <typename BufferContext>
bool CBuffersQueue<BufferContext>::Remove(int RequestedDataSize)
{
    if(m_BuffersQueue.NumItems() == 0)
        return false;

    CCriticalSectionLocker Locker(m_Lock);

    BufferDataItem& Item = m_BuffersQueue.Peek();
    int LeftData = Item.DataBuffer.GetDataSize() - Item.DataPtr;
    Assert(LeftData > 0);

    if(RequestedDataSize > LeftData)
    {
        //Should not occur since Remove should be called for peeked data
        Assert(false);
        return false;
    }

    UpdateCurrentQueueSize(0 - RequestedDataSize);
    if(LeftData == RequestedDataSize)
    {
        //Last packet of item : delete the item
        BufferDataItem ItemToRemove;
        bool Popped = m_BuffersQueue.Pop(ItemToRemove);
        if(!Popped)
        {
            Assert(false);
            UpdateCurrentQueueSize(RequestedDataSize);
            return false;
        }
        return true;
    }

    //increment pointers to the next packet
    Assert(LeftData > RequestedDataSize);
    Item.DataPtr += RequestedDataSize;
    Item.PacketCounter++;
    return true;
}

template <typename BufferContext>
void CBuffersQueue<BufferContext>::RemoveFrontBuffer()
{
    if(m_BuffersQueue.IsEmpty())
        return;

    CCriticalSectionLocker Locker(m_Lock);
    BufferDataItem ItemToRemove;
    bool Popped = m_BuffersQueue.Pop(ItemToRemove);
    if(!Popped)
    {
        Assert(false);
        return;
    }
    int DeletedDataSize = ItemToRemove.DataBuffer.GetDataSize() - ItemToRemove.DataPtr;
    Assert(DeletedDataSize > 0);
    UpdateCurrentQueueSize(0 - DeletedDataSize);
}

template <typename BufferContext>
bool CBuffersQueue<BufferContext>::IsEmpty()
{
    return m_BuffersQueue.IsEmpty();
}

template <typename BufferContext>
void CBuffersQueue<BufferContext>::Clear()
{
    CCriticalSectionLocker Locker(m_Lock);
    m_BuffersQueue.Clear();
    m_CurrentSizeInBytes = 0;
}

//add to or subtract from the m_CurrentSizeInBytes
template <typename BufferContext>
void CBuffersQueue<BufferContext>::UpdateCurrentQueueSize(int DataSizeChange)
{
    CCriticalSectionLocker Locker(m_Lock);
    m_CurrentSizeInBytes += DataSizeChange;
    if(m_CurrentSizeInBytes < 0)
    {
        Assert(false);
        m_CurrentSizeInBytes = 0;
    }
}





