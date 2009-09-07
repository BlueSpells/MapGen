// CThreadWithQueue.cpp: implementation of the CThreadWithQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadWithQueue.h"
#include "LogEvent.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CThreadWithQueue::CThreadWithQueue(const char* ThreadName, int QueueSize, bool LogAddToQueueFailures):
    CSimpleThread(ThreadName)
	, m_LastTicksOnTimeout(0) 
    , m_LogAddToQueueFailures(LogAddToQueueFailures)
    , m_ThreadPerformanceInitialized(false)
    , m_LastLogTime(0)
    , m_NumAddToQueueFailures(0)
    , m_ThreadPerformanceLogFrequency(PERFORMANCE_LOG_FREQUENCY)
    , m_ThreadPerformanceLogLevel(LE_INFO)
    , m_NumRequestsHandled(0)
    , m_NumRequestsAdded(0)
{
	m_Queue.SetQueueSize(QueueSize);
}

CThreadWithQueue::~CThreadWithQueue()
{
	//clear the queue
    CloseThread(true);
//    CloseThread(true);
}

void CThreadWithQueue::CloseThread(bool ForceClosing)
{
    CSimpleThread::CloseThread(ForceClosing);
    ClearQueue();
}

void CThreadWithQueue::ClearQueue()
{
    while(m_Queue.NumItems())
    {
        IExternalHandler* Handler;
        if(m_Queue.Pop(Handler))
            delete Handler;
        else
          LogEvent(LE_ERROR, "CThreadWithQueue::ClearQueue false pop");
    }
}

bool CThreadWithQueue::AddToQueue(IExternalHandler* Handler)
 {
	if(m_Queue.Push(Handler))
    {
        InterlockedIncrement(&m_NumRequestsAdded);
        return true;
    }
	else
	{
        InterlockedIncrement(&m_NumAddToQueueFailures);
//        if(m_LogAddToQueueFailures && GetTickCount() - m_LastLogTime > 1000)
//        {
//            LogEvent(LE_WARNING, "CThreadWithQueue(%s)::AddRequest, Failed to add %d requests to queue",
//                        m_ThreadName.c_str(), m_NumAddToQueueFailures);
//            m_LastLogTime = GetTickCount();
//            m_NumAddToQueueFailures = 0;
//        }
		delete Handler;
		return false;
	}
}

void CThreadWithQueue::LogThreadPerformance()
{
    if(!m_ThreadPerformanceInitialized)
    {
        m_ThreadPerformanceInitialized = true;
        m_ThreadPerformanceLogFrequency = GetConfigInt("Threads", "ThreadPerformanceLogFrequency", PERFORMANCE_LOG_FREQUENCY);
        m_ThreadPerformanceLogLevel = (ELogSeverity)GetConfigInt("Threads", "ThreadPerformanceLogLevel", LE_INFOLOW);
    }

    if(m_ThreadPerformanceLogFrequency <= 0)
        return;

    DWORD LogIntervalInMilli = m_ThreadPerformanceLogFrequency * 1000;
    if(GetTickCount() - m_LastLogTime < LogIntervalInMilli)
        return;

    m_LastLogTime = GetTickCount();
    LONG NumAddFailures = InterlockedExchange(&m_NumAddToQueueFailures, 0);
    LONG NumAddSuccesses = InterlockedExchange(&m_NumRequestsAdded, 0);

    ELogSeverity LogLevel = m_ThreadPerformanceLogLevel;
    if(NumAddFailures > 0)
        LogLevel = LE_ERROR;
    LogEvent(LogLevel, "Thread (%s): Current Queue Size [%d] (Max [%d]), Handled [%d], Added [%d], Failed To Add [%d]",
        m_ThreadName.c_str(), m_Queue.NumItems(), m_Queue.GetMaxSize(), m_NumRequestsHandled, NumAddSuccesses, NumAddFailures);
    m_NumRequestsHandled = 0;
}

void CThreadWithQueue::LoopFunction() // Entry point of the thread
{
    // Define constants for events.
    const int TerminateThread  = WAIT_OBJECT_0;     
    const int InputMessage     = WAIT_OBJECT_0 + 1;  
    const int InitiatedTimeout = WAIT_OBJECT_0 + 2;  
    const int Timeout          = WAIT_TIMEOUT;

    const int HandlesNum = 3;
    HANDLE WaitHandles[HandlesNum];
	WaitHandles[0] = m_CloseEvent;
	WaitHandles[1] = m_Queue;
    WaitHandles[2] = m_InitiatedTimeoutEvent;

    while (true)
    {
        LogThreadPerformance();

        // Wait for one of the trigger events
        int Result = WaitForMultipleObjects(HandlesNum, WaitHandles, FALSE, m_TimeoutInMilli);

        switch(Result)
        {
            case TerminateThread:
                return; // This will terminate the thread
			case InputMessage:
				HandleInputMessage();
				HandleTimeout();
				break;
            case InitiatedTimeout:
			case Timeout:
                HandleTimeout();
    			break;
			default:
				LogEvent(LE_ERROR, "CThreadWithQueue::LoopFunction: WaitForMultipleObjects FAILED %d, Exit thread %s",
                    Result, m_ThreadName.c_str());
                return; //exit
		}
	}
}

void CThreadWithQueue::OnThreadStart()
{
	LogEvent(LE_INFOLOW, "CThreadWithQueue(%s)::OnThreadStart", m_ThreadName.c_str());
}

void CThreadWithQueue::OnThreadClose()
{
	LogEvent(LE_INFOLOW, "CThreadWithQueue(%s)::OnThreadClose", m_ThreadName.c_str());
}

void CThreadWithQueue::OnTimeout()
{
	//LogEvent(LE_INFOHIGH, "CThreadWithQueue(%s)::OnTimeout", m_ThreadName.c_str());
}

void CThreadWithQueue::HandleInputMessage()
{
    // Pop and handle a message of the queue
	IExternalHandler* Handler;
    if(m_Queue.Pop(Handler))
	{
        m_NumRequestsHandled++;
        Handler->Handle();
		delete Handler;
	}
	else
		LogEvent(LE_INFOHIGH, "CThreadWithQueue::HandleInputMessage queue is empty");
}

void CThreadWithQueue::HandleTimeout()
{
	if(m_TimeoutInMilli == 0)
		return;

    DWORD CurrentTicks = GetTickCount();

    if (CurrentTicks - m_LastTicksOnTimeout >= m_TimeoutInMilli)
    {
        OnTimeout();
        m_LastTicksOnTimeout = CurrentTicks;
    }
}

int CThreadWithQueue::NumItemsInQueue()
{
	return m_Queue.NumItems();
}

//////////////////////////////////////////////////////////////////////////
// CThreadWithRequestsQueue
//////////////////////////////////////////////////////////////////////////

CThreadWithRequestsQueue::CThreadWithRequestsQueue(const char* ThreadName, 
                                                   int QueueSize,
                                                   bool LogAddRequestFailures) :
      CThreadWithQueue(ThreadName, QueueSize, LogAddRequestFailures)
{
}

CThreadWithRequestsQueue::~CThreadWithRequestsQueue()
{
    while(m_Queue.NumItems() > 0)
    {
        IExternalHandler* Handler;
        if(m_Queue.Pop(Handler))
        {
            ThreadRequestHandler* RequestHandler= 
                    (ThreadRequestHandler*)Handler;
            delete RequestHandler;
        }
    }
}

bool CThreadWithRequestsQueue::AddRequest(CThreadRequest* Request, bool DeleteRequest)
{
    RequestData Data  = {Request, DeleteRequest};
    ThreadRequestHandler* RequestHandler = new ThreadRequestHandler(this, 
        (TemplatedExternalHandler<RequestData>::DataHandler)&CThreadWithRequestsQueue::HandleThreadRequest, Data);
    return AddToQueue(RequestHandler);
}

void CThreadWithRequestsQueue::HandleThreadRequest(const RequestData& Data)
{
    //do not lock the request if the handler is thread itself
    const bool IsInternalRequest = Data.Request->HasSameHandler(m_HandlerVerifier);
    if(IsInternalRequest)
        Data.Request->HandleRequest();
    else
    {
        const bool ValidRequestHandler = Data.Request->LockRequest();
        if(ValidRequestHandler)
            Data.Request->HandleRequest();
        // We must Unlock even if the Handler is invalid
        Data.Request->UnlockRequest();
        // The LogEvent is deliberately separated from the Unlock operation
        // to avoid possible deadlock in the trace thread
        if(!ValidRequestHandler)
            LogEvent(LE_WARNING, "ThreadWithRequestsQueue(%s)::HandleThreadRequest : Handler is invalid. Ignoring request.", m_ThreadName.c_str());
    }

    if(Data.DeleteRequest && Data.Request->IsCompleted())
        delete Data.Request;
}

void CThreadWithQueue::SetQueueSize(int QueueSize)
{
	m_Queue.SetQueueSize(QueueSize);

}

//////////////////////////////////////////////////////////////////////////
// Tests
//////////////////////////////////////////////////////////////////////////

#ifdef _TEST

#include "Utils.h"

class CTestThread : public CThreadWithQueue
{
    enum { QUEUE_SIZE = 1000};
public:
    CTestThread(int MessageCount) : CThreadWithQueue("CTestThread", QUEUE_SIZE), 
		m_LastGeneratedMsgId(1),
        m_Counter(0), m_MessageCount(MessageCount), 
        m_MaxDelta(0), m_TotalDelta(0),
		m_FirstMsgEnterQueueTime(0), m_LastMsgEnterQueueTime(0),
		m_FirstMsgExitQueueTime(0), m_LastMsgExitQueueTime(0)
    {
        SetTimeout(100);
    }

    void SendNoParams()
    {
        AddHandlerToQueue(&CTestThread::OnTest);
    }

    bool SendWithParams()
    {
		if(m_FirstMsgEnterQueueTime == 0)
			m_FirstMsgEnterQueueTime = GetTickCount();
		m_LastMsgEnterQueueTime = GetTickCount();
        TestData Data = { m_LastGeneratedMsgId, GetTickCount() };

        bool AddSuccess = AddHandlerToQueue(&CTestThread::OnTestData, Data);
		if(AddSuccess)
		{
			m_LastGeneratedMsgId++;
			return true;
		}
		else
			return false;
    }

    int GetMaxDelta()
    {
        return m_MaxDelta;
    }

    int GetAverageDelta()
    {
        return m_TotalDelta/ m_Counter;
    }

	int NumHandledMessages()
	{
		return m_Counter;
	}

	int HandlingTime()
	{
		return m_LastMsgExitQueueTime - m_FirstMsgExitQueueTime;
	}

private:
	struct TestData
	{
		DWORD	MsgId; 
		DWORD	MsgTime;
	};

    void OnTestData(const TestData& Data)
    {
        int Delta = GetTickCount() - Data.MsgTime;
        m_TotalDelta += Delta;
        if (Delta > m_MaxDelta)
            m_MaxDelta = Delta;
		if(m_FirstMsgExitQueueTime == 0)
			m_FirstMsgExitQueueTime = GetTickCount();
		m_LastMsgExitQueueTime = GetTickCount();
        if (m_Counter % 1000 == 0)
			LogEvent(LE_WARNING, "OnTestData last message(num %d): messages Enter %d milli, Message Exit %d, from first enter to last exit %d milli ", 
			   m_Counter,  
			   m_LastMsgEnterQueueTime - m_FirstMsgEnterQueueTime,
			   m_LastMsgExitQueueTime - m_FirstMsgExitQueueTime,
			   m_LastMsgExitQueueTime - m_FirstMsgEnterQueueTime);

        if (++m_Counter >= m_MessageCount)
		{
			LogEvent(LE_WARNING, "OnTestData last message(num %d): messages Enter %d milli, Message Exit %d, from first enter to last exit %d milli ", 
			   m_Counter,  
			   m_LastMsgEnterQueueTime - m_FirstMsgEnterQueueTime,
			   m_LastMsgExitQueueTime - m_FirstMsgExitQueueTime,
			   m_LastMsgExitQueueTime - m_FirstMsgEnterQueueTime);
            CloseThread();
		}
    }

    void OnTest()
    {
        LogEvent(LE_INFOHIGH, "OnTest");
    }

    virtual void OnTimeout()
    {
        LogEvent(LE_INFOHIGH, "OnTimeout %d messages received, %d in queue ", 
			m_Counter, NumItemsInQueue());

        SendNoParams();
    }


private:
	int m_LastGeneratedMsgId;
    int m_Counter;
    int m_MessageCount;
    int m_MaxDelta, m_TotalDelta;
	DWORD m_FirstMsgEnterQueueTime, m_LastMsgEnterQueueTime;
	DWORD m_FirstMsgExitQueueTime, m_LastMsgExitQueueTime;
};

void TestThreadWithQueue()
{
    const int MessageCount = 1000000;
    
    CTestThread TestThread(MessageCount);

    TestThread.StartThread();

    int Start = GetTickCount();
    for (int i = 0; i < MessageCount; ++i)
	{
		if(!TestThread.SendWithParams())
		{ 
			i--;
			WaitAndPumpMessages(0);
		}
	}

    int End = GetTickCount();
    int Elapsed = End - Start;
	LogEvent(LE_WARNING, "%d Messages Sent in %d millisec", MessageCount, Elapsed);

	WaitAndPumpMessages(180000);

	
    LogEvent(LE_WARNING, "Took %d milli to Handle %d Requests, MaxDelta %d, Average %d", 
        TestThread.HandlingTime(), TestThread.NumHandledMessages(), 
		TestThread.GetMaxDelta(), TestThread.GetAverageDelta()
    );

	TestThread.CloseThread();

    WaitForSingleObject(TestThread, 1000);
}

class CThreadWithRequestsQueueTester
{
public:
    CThreadWithRequestsQueueTester(int QueueSize):
        m_ThreadWithRequestQueue("CThreadWithRequestsQueueTester", QueueSize),
        m_SentRequests(0), m_ReceivedRequests(0)
    {
    }

    bool Start()
    {
        return m_ThreadWithRequestQueue.StartThread();
    }

    void Stop()
    {
        m_ThreadWithRequestQueue.CloseThread();
    }

    void AddRequest()
    {
        TestThreadRequest* Request = new TestThreadRequest(this, m_HandlerVerifier, ++m_SentRequests);
        if(!m_ThreadWithRequestQueue.AddRequest(Request))
        {
            m_SentRequests--;
            LogEvent(LE_ERROR, "CThreadWithRequestsQueueTester::AddRequest Failed");
        }
    }

    void PrintResults()
    {
        LogEvent(LE_INFOHIGH, "CThreadWithRequestsQueueTester Results: Sent %d, Received %d",
            m_SentRequests, m_ReceivedRequests);

    }

private:
    class TestThreadRequest : public CThreadRequest
    {
    public:
        TestThreadRequest(CThreadWithRequestsQueueTester* Tester,
			const CHandlerVerifier& HandlerVerifier, int ContextData)
			: CThreadRequest(HandlerVerifier)
			, m_Tester(Tester)
			, m_Context(ContextData)
        {
        }
    private:
        //IThreadRequest implementation
        virtual void HandleRequest()
        {
			m_Tester->DoSomething();
        }

        CThreadWithRequestsQueueTester* m_Tester;
        int                             m_Context;
    };

    CThreadWithRequestsQueue m_ThreadWithRequestQueue;

private:
    int m_ReceivedRequests;
    int m_SentRequests;
	CHandlerVerifier m_HandlerVerifier;
	// When implementing don't copy m_HandlerVerifier
	CThreadWithRequestsQueueTester(const CThreadWithRequestsQueueTester&);
	void operator=(const CThreadWithRequestsQueueTester&);

    void DoSomething()
    {
        m_ReceivedRequests++;
    }
};                              

void TestThreadWithRequestsQueue()
{
    const int NUM_REQUESTS = 100000;
    const int QUEUE_SIZE = 1000;
    CThreadWithRequestsQueueTester Tester(QUEUE_SIZE);
    
    Tester.Start();
    for(int i = 0; i < NUM_REQUESTS; ++i)
    {
        Tester.AddRequest();
        if(i% 500 == 0)
        {
            WaitAndPumpMessages(0);
            Tester.PrintResults();
        }
    }
    Tester.Stop();
    Tester.PrintResults();
}

#endif // _TEST