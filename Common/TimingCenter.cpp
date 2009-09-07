// TimerManager.cpp: implementation of the TimerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TimingCenter.h"
#include "CriticalSection.h"
#include "LogEvent.h"
#include "Utils.h"
#include "CollectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { TOO_MUCH_TIME_FOR_ONE_TIMER_WARNING = 500 };

//////////////////////////////////////////////////////////////
// Timer class

CTimingCenter::CTimingCenter(): m_LastTimerId(0), m_InTimeout(false), m_LastVisitedTimerId(0),
                                m_WarnIfExceedPeriod(TOO_MUCH_TIME_FOR_ONE_TIMER_WARNING)
{
}

bool CTimingCenter::AddTimer(ITimingCenterEvents* Timer, DWORD TimeoutInMill, DWORD& TimerId, const char * Context)
{
    // We lock since the timer table may be accessed by other
    // client and by the timer manager.
	CCriticalSectionLocker Locker(m_Lock);
	TimerId = GenerateTimerId();
	TimerData Data(TimerId, TimeoutInMill, GetTickCount() - TimeoutInMill, false, Context, Timer);
    LogEvent(LE_INFOLOW, "CTimingCenter::AddTimer: Id %d, Timeout %d, Context %s", Data.TimerId, Data.TimeoutInMilli, Data.Context.c_str());
    if (InsertValueToMap(m_Timers, TimerId, Data) == false)
        return false;

    return true;
}

DWORD CTimingCenter::GenerateTimerId()
{
	return ++m_LastTimerId;
}

bool CTimingCenter::DeleteTimer(DWORD TimerId)
{
	CCriticalSectionLocker Locker(m_Lock);

	TimersIterator Iter = m_Timers.find(TimerId);
    if (Iter == m_Timers.end())
    {
        LogEvent(LE_INFO, "CTimingCenter::DeleteTimer Can't find timer Id[%d] to delete", TimerId);
        return false;
    }

    TimerData& Timer = Iter->second;
    LogEvent(LE_INFOLOW, "CTimingCenter::DeleteTimer: Id %d, Timeout %d, Context %s", 
        Timer.TimerId, Timer.TimeoutInMilli, Timer.Context.c_str());
    if (m_InTimeout)
    {
        Timer.ToBeDeleted = true;
        bool Status = InsertValueToSet(m_DeletionPendingTimers, Timer.TimerId);
        Assert(Status); // must be true
        LogEvent(LE_INFOLOW, "CTimingCenter::DeleteTimer: m_InTimeout is true, timer %d will be deleted later", 
            Timer.TimerId);
    }
    else
    {
        m_Timers.erase(Iter);
    }

	return true;
}

void CTimingCenter::ProcessTimers(int MaxProcessPeriodInMilli, int& NextTimeoutInMilli)
{
    enum 
    {
        DEFAULT_NEXT_TIMER = 30,
        TOO_MUCH_TIME_FOR_ONE_TIMER = 40,
    };

	CCriticalSectionLocker Locker(m_Lock);
//    LogEvent(LE_DEBUG, "Timing center - Process timers, NumTimers[%d] pointer[%d]", m_Timers.size(), this);
	DWORD CurrentTime = GetTickCount();
    bool ExecutionHasTimeLimit = MaxProcessPeriodInMilli != -1 ? true : false;
    DWORD NextTimerTime = DEFAULT_NEXT_TIMER; // TO calculate when to receive next ProcessTimers()
    int NumTimersToProcess = m_Timers.size();
    bool ExceedTimeLimit = false;
    TimersIterator Iter = m_Timers.upper_bound(m_LastVisitedTimerId);
    
    while(NumTimersToProcess > 0 && ExceedTimeLimit == false)
    {
        Assert(m_Timers.empty() == false); // Can't be since each node can be visited only once and there is a lock.

        // the execution is cyclic
        if (Iter == m_Timers.end())
            Iter = m_Timers.begin();

        m_LastVisitedTimerId = Iter->first;
        TimerData& Timer = Iter->second;
        NumTimersToProcess--;
        ++Iter;

        if (Timer.ToBeDeleted)
            continue; // skip those pending deletion timers

        //calc if need to invoke the timer event
        DWORD CurrentTimerLeftTime = Timer.LeftTime(CurrentTime);
        if(CurrentTimerLeftTime == 0)
        {
            DWORD TimeBefore = GetTickCount();
            m_InTimeout = true;
            Assert(!Timer.ToBeDeleted);
            Timer.Client->OnTimeout();
            m_InTimeout = false;

            DWORD TimeDiff = GetTickCount() - TimeBefore;
            if (TimeDiff > TOO_MUCH_TIME_FOR_ONE_TIMER)
            {
                ELogSeverity Severity = (TimeDiff > m_WarnIfExceedPeriod) ? LE_WARNING : LE_INFO;
                LogEvent(Severity, "CTimingCenter::ProcessTimers: Timer[id %d, %s] took too much time[%d milli]",
                            Timer.TimerId, Timer.Context.c_str(), GetTickCount() - TimeBefore);
            }

            Timer.LastTimerTime = CurrentTime;
            CurrentTimerLeftTime = Timer.TimeoutInMilli;

            if (ExecutionHasTimeLimit == true)
            {
                int ExecutionTime = GetTickCount() - CurrentTime; // Time exceeded
                if (ExecutionTime >= MaxProcessPeriodInMilli)
                {
                    ExceedTimeLimit = true;
                    LogEvent(LE_INFO, "CTimingCenter::ProcessTimers - exceed time limit[%d milli]. Actual time[%d milli], Timers left to execute[%d]",
                                        MaxProcessPeriodInMilli, ExecutionTime, NumTimersToProcess);
                }
            }
        }

        //maintain the nearest time to execute
        if (NextTimerTime > CurrentTimerLeftTime)
            NextTimerTime = CurrentTimerLeftTime;

    }//while

    //consider the above execution time in order to 
    //calculate the next timer timeout
    if(!m_Timers.empty())
    {
        NextTimeoutInMilli = 0;
        DWORD ExecuteTime = GetTickCount() - CurrentTime;
        if(NextTimerTime > ExecuteTime )
            NextTimeoutInMilli = NextTimerTime - ExecuteTime;
    }

    // Delete the timers that were deleted during the execution but couldn't be deleted
    // until now in order not to spoil the timers data structure(pending deleted timers)
    ProcessPendingDeletedTimers();
}

void CTimingCenter::ProcessPendingDeletedTimers()
{
    // Delete the timers that were been deleted while executing the process timers
    CCriticalSectionLocker Locker(m_Lock);

    if (m_DeletionPendingTimers.size() == 0)
        return; // no timers to delete

    TimersSetIterator Iter = m_DeletionPendingTimers.begin();
    for ( ; Iter != m_DeletionPendingTimers.end() ; ++Iter)
    {
        bool Deleted = DeleteTimer(*Iter);
        Assert(Deleted); // Must be found
    }
    m_DeletionPendingTimers.clear();
}


//////////////////////////////////////////////////////////////////////////
CTimer::CTimer(ITimingCenter* TimingCenter, ITimerEvents* Sink):
	m_TimingCenter(TimingCenter), m_TimerId((DWORD)NULL_TIMER_ID), 
	m_TimeoutMilli(0), m_Sink(Sink)
{
}
	
CTimer::~CTimer()
{
	UnRegister();
}

void CTimer::Register(DWORD TimeoutInMilli, const char * Context)
{
    if(m_TimingCenter != NULL)
    {
        m_TimingCenter->AddTimer(this, TimeoutInMilli, m_TimerId, Context);
        m_TimeoutMilli = TimeoutInMilli;
    }
}

void CTimer::UnRegister()
{
	if(m_TimerId != NULL_TIMER_ID && m_TimingCenter != NULL)
	{
		m_TimingCenter->DeleteTimer(m_TimerId);
		m_TimerId = (DWORD) NULL_TIMER_ID;
	}
}

int	 CTimer::GetTimeout() const
{
	return m_TimeoutMilli;
}

void CTimer::OnTimeout()
{
	if(m_Sink)
		m_Sink->OnTimer();
}


//////////////////////////////////////////////////////////////////////////
#ifdef _TEST

class CTimingCenterClient: public ITimingCenterEvents
{
public:
	CTimingCenterClient(CTimingCenter& TimersCenter):
		  m_TimersCenter(TimersCenter), m_TimerId((DWORD)NULL_TIMER_ID), 
		m_TimeoutMilli(0), m_LastTimeout(0), m_NumFaulty(0),
        m_NumTimeouts(0), m_MaxTimeout(0), m_TimerIdToDelete((DWORD)NULL_TIMER_ID)
	{
	}
	
	virtual ~CTimingCenterClient()
	{
		UnRegister();
	}

	void Register(DWORD TimeoutInMilli, const char * Context)
	{
		m_TimersCenter.AddTimer(this, TimeoutInMilli, m_TimerId, Context);
		m_TimeoutMilli = TimeoutInMilli;
		m_LastTimeout = GetTickCount();
	}

	void UnRegister()
	{
		if(m_TimerId != NULL_TIMER_ID)
			m_TimersCenter.DeleteTimer(m_TimerId);
	}

    // This means that this client on its timeout will initiate deletion of
    // itself and another timer.
    void SetDeletingClient(DWORD AdditionalTimerIdToDelete)
    {
        m_TimerIdToDelete = AdditionalTimerIdToDelete;
    }

private:
	virtual void OnTimeout()
	{
		enum { TIMER_MARGIN = 20};
		//process timeout
        DWORD TimeFromLastTimeout = GetTickCount() - m_LastTimeout;
        bool TimerOk = true;
		if(TimeFromLastTimeout > (DWORD)(m_TimeoutMilli + TIMER_MARGIN)) 
        {
            TimerOk = false;
        }
		LogEvent(TimerOk? LE_INFO: LE_WARNING, "CTimerCenterClient::OnTimeout(), timer %d, %s timeout diff: time from last = %d, expected timeout %d",
			    m_TimerId, TimerOk? "Ok" : "WRONG", GetTickCount() - m_LastTimeout, m_TimeoutMilli);
        m_NumTimeouts++;
		m_LastTimeout = GetTickCount();

        // simulate some processing
        char Buffer[2000];
        for (int i = 0 ; i < sizeof(Buffer) ; i++)
            Buffer[i] = '5';

        // a chosen client will simulate deletion of the timer
        if (m_TimerIdToDelete != NULL_TIMER_ID)
        {
            LogEvent(LE_WARNING, "CTimerCenterClient::OnTimeout - Deleting timers - own timer[%d] and another timer[%d]",
                        m_TimerId, m_TimerIdToDelete);
            Assert(m_TimersCenter.DeleteTimer(m_TimerId));
            Assert(m_TimersCenter.DeleteTimer(m_TimerIdToDelete));
        }
	}

private:
	DWORD			m_TimerId;
	CTimingCenter&	m_TimersCenter;
	int				m_TimeoutMilli;
	DWORD			m_LastTimeout;
    int             m_NumTimeouts;
    int             m_NumFaulty;
    int             m_MaxTimeout;
    DWORD           m_TimerIdToDelete;

	enum
	{
		NULL_TIMER_ID = 0xFFFFFFFF
	};
};

void TestTimerCenter()
{
	const int NUM_TIMERS = 1000;
	const int ProcessingInSeconds = 40;

	CTimingCenter  TheTimersCenter;

	CTimingCenterClient* Timers[NUM_TIMERS];
	for(unsigned int i  = 0; i < NUM_TIMERS; ++i)
	{
		Timers[i] = new CTimingCenterClient(TheTimersCenter);
		Timers[i]->Register(i > 40 ? i: 10, "TestTimerCenter");
        if (i == NUM_TIMERS / 2)
            Timers[i]->SetDeletingClient(i + 2);
//		WaitAndPumpMessages(0);
	}

	DWORD Start = GetTickCount();
	while( GetTickCount() < (Start + ProcessingInSeconds* 1000))
	{
		//WaitAndPumpMessages(0);
		int MaxProcessing = 5;
        int NextProcessing = 10;
		DWORD ProcessingStart = GetTickCount();
		TheTimersCenter.ProcessTimers(MaxProcessing, NextProcessing);
		DWORD ProcessingEnd = GetTickCount();
		LogEvent(LE_WARNING, "TestTimerCenter(), Processing %d timers took %d Milli, Call next processing in %d milli",
			NUM_TIMERS, ProcessingEnd - ProcessingStart, NextProcessing);
		WaitAndPumpMessages(NextProcessing);
	}

	for(unsigned int i  = 0; i < NUM_TIMERS; ++i)
		delete Timers[i];
}


#endif	_TEST

