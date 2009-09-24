// SafeQueue.h: interface for the SafeQueue class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning (push)
#pragma warning (disable: 4702)
#include <queue>
#pragma warning (pop)
#include <afxmt.h>
#include "CriticalSection.h"

template<typename QueueItem>
class SafeQueue
{
public:
	SafeQueue(): m_QueueSize(QUEUE_DEFAULT_SIZE), 
		m_Event(FALSE, TRUE /* manual reset*/)
	{
		m_Event.ResetEvent();
	}

	~SafeQueue()
	{
		Clear();
	}

	void SetQueueSize(int MaxItems)
	{
		m_QueueSize = MaxItems;
//#define TEST_LEAKS
#if defined(_DEBUG) && defined(TEST_LEAKS)
        CCriticalSectionLocker Locker(m_QueueLock);
        // We can resize the queue only on its initialization
        Assert(m_Queue.empty()); // otherwise the pop operation will clean required items
        // accessing the container directly ...
        m_Queue.c.resize(m_QueueSize);
        while(!m_Queue.empty())
            m_Queue.pop();
#endif // defined(_DEBUG) && defined(TEST_LEAKS)
	}

	bool Pop(QueueItem& Item)
	{
	   CCriticalSectionLocker Locker(m_QueueLock);
	   if(m_Queue.size() > 0)
	   {
		   Item = m_Queue.front();
		   m_Queue.pop();
		   if(m_Queue.empty())
		   	   m_Event.ResetEvent();
		   else
			   m_Event.SetEvent();
		   return true;
	   }
	   return false;
	}

    //it is client responsibility to make sure that the queue 
    //is not empty before calling this function
    QueueItem& Peek()
    {
        //get a reference to the front item but do not take it out of the queue
        CCriticalSectionLocker Locker(m_QueueLock);
        if(m_Queue.size() <= 0)
        {
            Assert(false);
        }

        return m_Queue.front();
    }

	bool Push(const QueueItem& Item)
	{
	   CCriticalSectionLocker Locker(m_QueueLock);
	   //check queue size
	   if(m_Queue.size() >= m_QueueSize)
	   {
//		   LogEvent(LE_ERROR, "Push to queue failed");
		   return false;
	   }
	   m_Queue.push(Item);
	   m_Event.SetEvent();
	   return true;
	}

	void Clear()
	{
		CCriticalSectionLocker Locker(m_QueueLock);
		while (!m_Queue.empty())
		{
			m_Queue.pop();
		}
	}

	int NumItems() const
	{
		CCriticalSectionLocker Locker(m_QueueLock);
		return m_Queue.size();
	}

    bool IsEmpty() const
    {
        return NumItems() == 0;
    }

    unsigned int GetMaxSize() const
    {
        return m_QueueSize;
    }
    virtual operator HANDLE() const
    {
        return (HANDLE) m_Event;
    }

private:
	typedef std::queue<QueueItem> ItemsQueue;  
	ItemsQueue			m_Queue;
	mutable CCriticalSection	m_QueueLock;
	CEvent				m_Event;
	unsigned int		m_QueueSize;

	enum
	{ 
		QUEUE_DEFAULT_SIZE = 1000
	};
};
