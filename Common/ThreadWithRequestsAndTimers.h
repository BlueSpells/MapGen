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
    DWORD m_LastProcessTimers;
    bool m_UseRequestsHighWaterMark;
};