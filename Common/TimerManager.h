// TimerManager.h: interface for the CTimerManager class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class ITimerManagerEvents
{
public:
	virtual void OnTimer(int TimerID)=0;
};


class CTimerManager
{
public:
	virtual ~CTimerManager();
    CTimerManager();
	bool Init();
    int AddTimer(ITimerManagerEvents *TimerSink, int ElapseInMilli);
    bool KillTimer(int timerID);

//    static CTimerManager& GetTheTimerManager();

private:
    // Hide copy ctor and assignment operator
    CTimerManager(const CTimerManager &);
    CTimerManager & operator=(const CTimerManager &);

private:

    class CTimerWnd* m_TimerWnd; // Used as the source of timers
};

