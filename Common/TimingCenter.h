// TimerManager.h: interface for the TimerManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "StdAfx.h"
#pragma warning (push)
#pragma warning (disable: 4702)
#include <map>
#pragma warning (pop)
#include <set>

class ITimingCenterEvents
{
public:
	virtual void OnTimeout() = 0;
};

class ITimingCenter
{
public:
    virtual bool AddTimer(ITimingCenterEvents* Timer, 
        DWORD TimeoutInMill, DWORD& TimerId, const char * Context) = 0;
    virtual bool DeleteTimer(DWORD TimerId) = 0;
};

class CTimingCenter : public ITimingCenter
{
public:
    CTimingCenter();
    void ProcessTimers(int MaxProcessPeriodInMilli, int& NextTimeoutInMilli); // If MaxProcessPeriod is -1 than there is no time limit

    //ITimingCenter implementation
    virtual bool AddTimer(ITimingCenterEvents* Timer, 
        DWORD TimeoutInMill, DWORD& TimerId, const char * Context);
    virtual bool DeleteTimer(DWORD TimerId);

protected:
    // If a timer exceed this period of time, the timing center will display warning
    DWORD m_WarnIfExceedPeriod;

private:
	DWORD GenerateTimerId();
    void ProcessPendingDeletedTimers();

	struct TimerData
	{
        TimerData(DWORD	_TimerId, DWORD _TimeoutInMilli, DWORD _LastTimerTime, bool _ToBeDeleted,
            std::string _Context, ITimingCenterEvents* _Client) :
                TimerId(_TimerId), TimeoutInMilli(_TimeoutInMilli), LastTimerTime(_LastTimerTime),
                ToBeDeleted(_ToBeDeleted), Context(_Context), Client(_Client) {}
		DWORD	TimerId;
		DWORD	TimeoutInMilli;
		DWORD	LastTimerTime;
        bool    ToBeDeleted;
        std::string Context;
		ITimingCenterEvents* Client;

		DWORD LeftTime(DWORD CurrentTime)
		{
			DWORD Elapsed = CurrentTime - LastTimerTime;
			if (Elapsed >= TimeoutInMilli)
				return 0;
			else
				return TimeoutInMilli - Elapsed;
		}
	};

	typedef std::map<DWORD /*TimerId*/, TimerData> TimersList;
	typedef TimersList::iterator TimersIterator;
	TimersList			m_Timers;
    CCriticalSection	m_Lock;
	DWORD				m_LastTimerId;
    bool                m_InTimeout;
    DWORD               m_LastVisitedTimerId;

    typedef std::set<DWORD/*TimerId*/> TimersSet;
    typedef TimersSet::iterator TimersSetIterator;
    std::set<DWORD/*TimerId*/> m_DeletionPendingTimers; // Timers waiting to be deleted
};

class ITimerEvents
{
public:
	virtual void OnTimer() = 0;
};

class CTimer: public ITimingCenterEvents
{
public:
	CTimer(ITimingCenter* TimersCenter, ITimerEvents* Sink);
	virtual ~CTimer();
	void Register(DWORD TimeoutInMilli, const char * Context);
	void UnRegister();
	int	 GetTimeout() const;

private:
	//ITimingCenterEvents implementation
	virtual void OnTimeout();

private:
	DWORD			m_TimerId;
	ITimingCenter*	m_TimingCenter;
	int				m_TimeoutMilli;
	ITimerEvents*	m_Sink;
	enum
	{
		NULL_TIMER_ID = 0xFFFFFFFF
	};
};



