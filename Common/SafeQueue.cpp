// SafeQueue.cpp: implementation of the SafeQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SafeQueue.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class SafeQueueTester
{
public:
	struct QueueData
	{
		char	Str[100];
		int		Num;
	};

    SafeQueueTester(): m_SendCounter(0),
        m_ReceiveCounter(0)
    {
    }

	void Init(int QueueSize)
	{
		m_Queue.SetQueueSize(QueueSize);
	}

	bool Push()
	{
		QueueData Data;
        Data.Num = ++m_SendCounter;
		return m_Queue.Push(Data);
	}

	bool Pop(QueueData& Data)
	{
		bool Success = m_Queue.Pop(Data);
        if(Success)
        {
            bool TheSame = (Data.Num == ++m_ReceiveCounter);
            Assert(TheSame);
        }
        return Success;
	}

	void PushPop(int NumItems, int Quata)
	{
		int Times = NumItems/Quata;
		for(int QuataIndex = 0; QuataIndex < Times; ++QuataIndex)
		{
			for(int i = 0; i < Quata; ++i)
			{
				Push();
			}

			for(int i = 0; i < Quata; ++i)
			{
				SafeQueueTester::QueueData Data;
 				Pop(Data);
			}
		}
	}

	void Clear()
	{
		m_Queue.Clear();
	}
private:

	typedef SafeQueue<QueueData> TestedQueue;
	TestedQueue m_Queue;
    int m_SendCounter;
    int m_ReceiveCounter;
};

void TestPushPopPerformance(int Items)
{
	SafeQueueTester Tester;

	DWORD Start = GetTickCount();
	Tester.Init(Items*2);
	for(int i = 0; i < Items; ++i )
	{
		Tester.Push();
	}

	DWORD Diff = GetTickCount()	 - Start;
	LogEvent(LE_INFOHIGH, "Push %d items to queue took %d milli", Items, Diff);

	Start = GetTickCount();
	for(int i = 0; i < Items; ++i )
	{
		SafeQueueTester::QueueData Data;
		Tester.Pop(Data);
	}
	Diff = GetTickCount()	 - Start;
	LogEvent(LE_INFOHIGH, "Pop %d items from queue took %d milli", Items, Diff);
}


void TestPushPopPerformance(int Items, int Quata)
{
	SafeQueueTester Tester;

	DWORD Start = GetTickCount();
	Tester.Init(Items);
	Tester.PushPop(Items, Quata);
	DWORD Diff = GetTickCount()	 - Start;
	LogEvent(LE_INFOHIGH, "Push and pop %d items (%d items per quata) to queue took %d milli", Items, Quata, Diff);
}

void TestIntegrity()
{
	SafeQueueTester Tester;
    SafeQueueTester::QueueData Data;

    Assert(!Tester.Pop(Data));
    for(unsigned int i = 0; i < 100; ++i)
        Tester.Push();
    for(unsigned int i = 0; i < 100; ++i)
        Tester.Pop(Data);
    Assert(!Tester.Pop(Data));
}

void TestSafeQueue()
{
    TestIntegrity();
	TestPushPopPerformance(10000);
	TestPushPopPerformance(100000);
	TestPushPopPerformance(100000, 100);
	TestPushPopPerformance(100000, 10);
	TestPushPopPerformance(100000, 5);
	TestPushPopPerformance(100000, 1);

}

