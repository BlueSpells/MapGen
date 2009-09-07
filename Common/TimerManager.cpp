// TimerManager.cpp: implementation of the CTimerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxwin.h>
#include "TimerManager.h"
#include "LogEvent.h"
#include <vector>
#include "Common/CriticalSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CTimerWnd : public CWnd
{
// Construction
public:
	CTimerWnd():LastTimerID(0) {};
	virtual ~CTimerWnd(){};

	UINT AddTimer(UINT ElapseInMilli, ITimerManagerEvents *Sink)
	{
		CCriticalSectionLocker Locker(m_ClientsLock);
		if (!Sink)
		{
			LogEvent(LE_ERROR, "CTimerWnd::AddTimer: Sink=NULL\n");
			return 0;
		}

		int NewID = GetNextTimerID();
		ClientData NewClient(NewID, Sink, ElapseInMilli);

		m_ClientVector.push_back(NewClient);
#ifdef _DEBUG
        m_ClientVector.reserve(1000);
#endif		
		return NewClient.TimerID;
	}

	bool KillTimer(int timerID)
	{
		CCriticalSectionLocker Locker(m_ClientsLock);

        int i;
		for (i = 0; (unsigned int)i < m_ClientVector.size(); ++i)
		{
			//const ObjectData& Data = m_Objects2[i];
			if(m_ClientVector[i].TimerID == timerID)
			{
				m_ClientVector.erase(m_ClientVector.begin() + i);
                return true;
			}
		}
        LogEvent(LE_ERROR, "CTimerWnd::KillTimer() failed to kill timer %d", timerID);
		return false;
	}

	// Generated message map functions
	bool Init()
	{
		if(!CreateEx(0, _T("STATIC"), "dummy", 
			WS_OVERLAPPED, CRect(0, 0, 20, 20), NULL, 0))
			return false;

		m_ClientVector.clear();
		const int InternalResolutionInMilli = 5;
		return !!SetTimer(1, InternalResolutionInMilli, NULL);
	}

protected:
	afx_msg void OnTimer(UINT nIDEvent)
	{
		CCriticalSectionLocker Locker(m_ClientsLock);

		DWORD CurrentTime = GetTickCount();

		for(int i = 0 ; (unsigned int)i < m_ClientVector.size() ; ++i)
		{
			if(CurrentTime - m_ClientVector[i].LastTime > 
				m_ClientVector[i].TimeElapse)
			{
				m_ClientVector[i].LastTime = CurrentTime;
				m_ClientVector[i].TimerSink->OnTimer(m_ClientVector[i].TimerID);
			}
		}
		
		CWnd::OnTimer(nIDEvent);
	}
	DECLARE_MESSAGE_MAP()

	int GetNextTimerID()
	{
		LastTimerID++;
		return LastTimerID;
	}

private:
	//ITimerEvents *m_TimerWndSink;

	struct ClientData
	{
		ClientData(int Id, ITimerManagerEvents *Sink, time_t Elapse) :
			TimerID(Id), TimerSink(Sink), 
			TimeElapse(Elapse), LastTime(0)
		{}

		int           TimerID;
		ITimerManagerEvents* TimerSink;
		DWORD		  TimeElapse;
		DWORD         LastTime;
	};

	typedef std::vector<ClientData> ClientVector;
	ClientVector	m_ClientVector;
	CCriticalSection m_ClientsLock;

	int LastTimerID;
};

BEGIN_MESSAGE_MAP(CTimerWnd, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimerManager::CTimerManager()
: m_TimerWnd(NULL)
{
	//Init();
}

CTimerManager::~CTimerManager()
{
	// REMARK: This code must be added to solve the
	//			TimerManager memory leak.
	if (m_TimerWnd)
		delete m_TimerWnd;
}

bool CTimerManager::Init()
{
	m_TimerWnd = new CTimerWnd();
	if(!m_TimerWnd->Init())
	{
		LogEvent(LE_ERROR, "CTimerManager::Init: Error init m_TimerWnd\n");
		return false;
	}
	return true;
}

int CTimerManager::AddTimer(ITimerManagerEvents *TimerSink, int ElapseInMilli)
{
	//m_TimerSink = TimerSink;
	return m_TimerWnd->AddTimer(ElapseInMilli, TimerSink);
}


bool CTimerManager::KillTimer(int timerID)
{
	//m_TimerSink = NULL;
	return m_TimerWnd->KillTimer(timerID);
}


/*static*/ 
//CTimerManager& CTimerManager::GetTheTimerManager()
//{
//	static CTimerManager TheTimerManager;
//	static bool Init = false;
//	if(!Init)
//	{
//		TheTimerManager.Init();
//		Init = true;
//	}
//
//	return TheTimerManager;
//}



/////////////////////////////////////////////////
// CTimerManager Test
/////////////////////////////////////////////////
#ifdef _TEST

class CTimerManagerTest : public ITimerManagerEvents
{
public:

	~CTimerManagerTest()
	{
		killtimer();
	}

	void Init()
	{
		m_Timer1ElapseInMilli = 30;
		m_Timer2ElapseInMilli = 200;

		
		m_Timer1ID = m_TimerManager.AddTimer(this, m_Timer1ElapseInMilli);;
		m_Timer2ID = m_TimerManager.AddTimer(this, m_Timer2ElapseInMilli);;

		m_Timer1LastTime = 0;
		m_Timer2LastTime = 0;
	}

	void killtimer()
	{
		m_TimerManager.KillTimer(m_Timer1ID);
		m_TimerManager.KillTimer(m_Timer2ID);

	}

private:
	virtual void OnTimer(int TimerID)
	{
		SYSTEMTIME LocalTime;
		GetLocalTime(&LocalTime);

		DWORD CurTime = GetTickCount();
		DWORD TimeDiff;
		int Elapase;

		// Find diff time:
		if(TimerID == m_Timer1ID) 
		{
			TimeDiff = CurTime - m_Timer1LastTime;
			m_Timer1LastTime = CurTime;
			Elapase = m_Timer1ElapseInMilli;
		}
		else if(TimerID == m_Timer2ID) 
		{
			TimeDiff = CurTime - m_Timer2LastTime;
			m_Timer2LastTime = CurTime;
			Elapase = m_Timer2ElapseInMilli;
		}
		else		
		{
			LogEvent(LE_ERROR,"CTimerManagerTest::OnTimer: no such timer ID %d", TimerID);
			return;
		}

		LogEvent(LE_INFO,"Got timer %d. Timediff=%d. Shoud be=%d", TimerID, TimeDiff, Elapase);

		Assert(TimeDiff > (unsigned int) Elapase);
		if (TimeDiff < (unsigned int) Elapase) 
		{
			LogEvent(LE_ERROR,"CTimerManagerTest::OnTimer: timer wake up before time\n");
		}
	}

	int m_Timer1ID;
	int m_Timer2ID;

	int m_Timer1ElapseInMilli;
	int m_Timer2ElapseInMilli;

	//DWORD CurrentTime = GetTickCount();
	DWORD m_Timer1LastTime;
	DWORD m_Timer2LastTime;
    CTimerManager   m_TimerManager;
};

static class CTimerManagerTest* TheTimerManagerTest = NULL;

void TestTimerManager()
{
    if (TheTimerManagerTest == NULL)
    {
        LogEvent(LE_INFOHIGH, "TestTimerManager: Created timer\n"); 
        TheTimerManagerTest = new CTimerManagerTest;
        TheTimerManagerTest->Init();
    }
    else
    {
        delete TheTimerManagerTest;
        TheTimerManagerTest = NULL;
        LogEvent(LE_INFOHIGH, "TestTimerManager: timer Deleted\n"); 
    }
}


#endif