#pragma once
#include "Common/ThreadWithQueue.h"
#include "Common/TimingCenter.h"

class CThreadWithRequestsAndTimers
	: public CThreadWithRequestsQueue
	, public CTimingCenter
{
public:
	CThreadWithRequestsAndTimers(const char* const ThreadName, int QueueSize);

    // If using the mechanism, in case the request number exceeds the high water mark value,
    // The thread will process requests until the low water mark value reached.
    void UseRequestsHighWaterMarkMechanism(bool ShouldUse);

private:
	virtual void LoopFunction();

    bool ShouldProcessTimers() const;
    void ProcessRequests();

private:
    enum
    {
        // Note that if the timeout time is bigger than Max execution time of the timers,
        // There could be some time when there are timers waiting and this thread goes to sleep(wait)
        TIMEOUT_TIME = 10,
        TIMERS_MAX_EXECUTION_TIME = 30,
        REQUESTS_MAX_EXECUTION_TIME = 30,
        REQUESTS_HIGH_WATER_MARK = 1000,
        REQUESTS_LOW_WATER_MARK = 400,
    };

private:
    DWORD m_LastProcessTimers;
    bool m_UseRequestsHighWaterMark;
};