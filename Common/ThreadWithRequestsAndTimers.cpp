#include "StdAfx.h"
#include "ThreadWithRequestsAndTimers.h"
#include "Common/Utils.h"
#include "Common/LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum
{
    // Note that if the timeout time is bigger than Max execution time of the timers,
    // There could be some time when there are timers waiting and this thread goes to sleep(wait)
    TIMEOUT_TIME = 10,
    TIMERS_MAX_EXECUTION_TIME = 50,
    REQUESTS_MAX_EXECUTION_TIME = 50,
    REQUESTS_HIGH_WATER_MARK = 1000,
    REQUESTS_LOW_WATER_MARK = 400,
};

//////////////////////////////////////////////////////////////////////////
CThreadWithRequestsAndTimers::CThreadWithRequestsAndTimers(const char* const ThreadName,
												 int QueueSize)
: CThreadWithRequestsQueue(ThreadName, QueueSize)
, m_LastProcessTimers(0)
, m_UseRequestsHighWaterMark(false)
{
}


//////////////////////////////////////////////////////////////////////////
void CThreadWithRequestsAndTimers::LoopFunction()
{
	// Define constants for events.
	const int TerminateThread = WAIT_OBJECT_0;     
	const int InputMessage    = WAIT_OBJECT_0 + 1;  

	HANDLE WaitHandles[2];
	WaitHandles[0] = m_CloseEvent;
	WaitHandles[1] = m_Queue;
    int NextTimoutTime = TIMEOUT_TIME;

	OnThreadStart();
	while (true)
	{
        LogThreadPerformance();

		//wait for close event, queue item or timer that elapsed
		int Result = WaitForMultipleObjects(2, WaitHandles, FALSE, NextTimoutTime);
        NextTimoutTime = TIMEOUT_TIME; // reset the next timeout time

        if (Result == TerminateThread)
        {
            HandleThreadClose();
            return; // This will terminate the thread
        }

        if (Result == InputMessage)
            //requests are waiting - Start processing them
            ProcessRequests();

        // In any case check if need to process timers
        if (ShouldProcessTimers() == true)
        {
            m_LastProcessTimers = GetTickCount();
            CTimingCenter::ProcessTimers(TIMERS_MAX_EXECUTION_TIME, NextTimoutTime);
        }

        // Calls the thread's function that should provide the call to OnTimeout()
        HandleTimeout();
        // update the next timeout also according to the timeout val.
        NextTimoutTime = min(NextTimoutTime, (int)m_TimeoutInMilli);

	} // while
}


//////////////////////////////////////////////////////////////////////////
// Generally this function will process request until one of the following conditions are met:
// 1> There are no more requests in the queue
// 2> The thread is processing requests for more than max process time And if the
//    request water mark mechanism is used than the number of request satisfy the water mark mechanism
void CThreadWithRequestsAndTimers::ProcessRequests()
{
    DWORD LimitTime = GetTickCount() + REQUESTS_MAX_EXECUTION_TIME;
    bool ShouldWaitForLowWaterMark = 
        m_UseRequestsHighWaterMark == true && m_Queue.NumItems() > REQUESTS_HIGH_WATER_MARK;

    if (ShouldWaitForLowWaterMark == true)
        LogEvent(LE_INFO, "CThreadWithRequestsAndTimers(%s)::ProcessRequests() - High water mark has reached(Num %d) waiting till low water mark", 
            m_ThreadName.c_str(), m_Queue.NumItems());

    DWORD LASTLog = GetTickCount();

    while (true)
    {
        if (m_Queue.IsEmpty() == true)
            break;
        bool LimitTimeReached = (int)(LimitTime - GetTickCount()) < 0;
        if (LimitTimeReached == true &&
            (ShouldWaitForLowWaterMark == false || m_Queue.NumItems() < REQUESTS_LOW_WATER_MARK))
            break;

        if (GetTickCount() - LASTLog > 50)
        {
            LogEvent(LE_INFO, "CThreadWithRequestsAndTimers(%s)::ProcessRequests() - Still processing num items [%d]", 
                m_ThreadName.c_str(), m_Queue.NumItems());
            LASTLog = GetTickCount();
        }
        //Temp
        DWORD Before = GetTickCount();
        // Process single request
        HandleInputMessage();
        if (GetTickCount() - Before > 50)
            LogEvent(LE_INFO, "CThreadWithRequestsAndTimers(%s)::ProcessRequests() - Handling request tool too much time[%d]",
                m_ThreadName.c_str(), GetTickCount() - Before);

    }

    if (m_Queue.IsEmpty() == false)
    {
        const int MANY_REQUESTS_NOT_LOGGED = 100;
        ELogSeverity Severity = (m_Queue.NumItems() < MANY_REQUESTS_NOT_LOGGED) ? LE_INFOLOW : LE_INFO;
        LogEvent(Severity, "CThreadWithRequestsAndTimers(%s)::ProcessRequests - Limit time has being reached while queue is not empty[%d]",
            m_ThreadName.c_str(), m_Queue.NumItems());
    }

    return;
}


//////////////////////////////////////////////////////////////////////////
bool CThreadWithRequestsAndTimers::ShouldProcessTimers() const
{
    return (GetTickCount() - m_LastProcessTimers) >= TIMEOUT_TIME;
}


//////////////////////////////////////////////////////////////////////////
void CThreadWithRequestsAndTimers::UseRequestsHighWaterMarkMechanism(bool ShouldUse)
{
    m_UseRequestsHighWaterMark = ShouldUse;
    return;
}

