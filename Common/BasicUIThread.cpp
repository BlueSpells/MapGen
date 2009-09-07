// BasicUIThread.cpp : implementation file
//

#include "stdafx.h"
#include "BasicUIThread.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicUIThread

//IMPLEMENT_DYNCREATE(CBasicUIThread, CWinThread)
//static CBasicUIThread *pUIThread = NULL;

CBasicUIThread::CBasicUIThread():
	m_TimerManager(NULL)
{
	m_bAutoDelete = FALSE;
//	pUIThread = this;
}

CBasicUIThread::~CBasicUIThread()
{
    PostQuitMessage(0);
 //   KillThread();
    WaitForSingleObject(*this, INFINITE);
//	pUIThread = NULL;
}


bool CBasicUIThread::StartThread()
{
    if(IsThreadActive())
    {
        LogEvent(LE_INFOHIGH, "CBasicUIThread::StartThread, Thread already running");
        return true;
    }

	if(!CreateThread(0))
    {
        LogEvent(LE_ERROR, "CBasicUIThread::StartThread, CreateThread failed");
        return false;
    }
    Sleep(100);
    return true;
}


BOOL CBasicUIThread::InitInstance()
{
    m_TimerManager = new CTimerManager();
	m_TimerManager->Init();
	return TRUE;
}

int CBasicUIThread::ExitInstance()
{
    OnClose();
    if(m_TimerManager)
    {
        delete m_TimerManager;
        m_TimerManager = NULL;
    }
//	VERIFY(::SetEvent(m_hEventDead));

	return CWinThread::ExitInstance();
}

// according to the recommendations found in CWinThread::IsIdleMessage
// MSDN documentation
BOOL CBasicUIThread::IsIdleMessage( MSG* pMsg )
{
	if (!CWinThread::IsIdleMessage( pMsg ) || 
		pMsg->message == WM_TIMER) 
		return FALSE;
	else
		return TRUE;
}

BOOL CBasicUIThread::PreTranslateMessage(MSG* pMsg)
{
	return OnMessage(pMsg->message, pMsg->wParam,pMsg->lParam);
	//return FALSE;
}

// Called by other thread to kill this thread
bool CBasicUIThread::KillThread()
{
    LogEvent(LE_INFO, "CBasicUIThread::KillThread: Before");
    if(!IsThreadActive())
    {
        LogEvent(LE_INFOHIGH, "CBasicUIThread::KillThread(), Thread is already not active");
        return true;
    }

    PostThreadMessage(WM_QUIT, 0, 0);
    WaitForSingleObject(*this, INFINITE);
    LogEvent(LE_INFO, "CBasicUIThread::KillThread: After");
    m_hThread = NULL;
    m_nThreadID = 0;
	return true;
}

bool CBasicUIThread::IsThreadActive() const
{
    return m_hThread != NULL;
}

BEGIN_MESSAGE_MAP(CBasicUIThread, CWinThread)
	//{{AFX_MSG_MAP(CBasicUIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicUIThread message handlers

int CBasicUIThread::AddTimer(ITimerManagerEvents* TimerSink, UINT ElapseInMilli)
{
    if(m_TimerManager == NULL)
        return false;
	return m_TimerManager->AddTimer(TimerSink, ElapseInMilli);
}

bool CBasicUIThread::KillTimer(int TimerId)
{
    if(m_TimerManager == NULL)
        return true;
	return m_TimerManager->KillTimer(TimerId);
}



//////////////////////////////////////////////////////////////////////////
// Test

#ifdef _TEST

class CBasicUIThreadTester: public CBasicUIThread, public ITimerManagerEvents
{
public:
    void Start()
    {
        AddTimer(this, 100);
    }

    void Stop()
    {

    }

private: 
    virtual void OnTimer(int TimerID);

    class CTimerTester
    {
        CTimerTester(int TimerId, DWORD TimerRate, DWORD FaultMargin)
        {
            m_TimerId = TimerId;
            m_TimerRate = TimerRate;
            m_FaultMargin = FaultMargin;
            m_LastTimerTime = GetTickCount();
                                
        }

        int GetTimerId()
        {
            return m_TimerId;
        }

        bool OnTimer()
        {
            DWORD ExpectedTimerTime = m_LastTimerTime + m_TimerRate;
            DWORD CurrentTime = GetTickCount();
            m_LastTimerTime = CurrentTime;
            if((ExpectedTimerTime - CurrentTime) < m_FaultMargin)
                return true;
            else
                return false;

        }

    private:
        int     m_TimerId;
        DWORD   m_TimerRate;
        DWORD   m_LastTimerTime;
        DWORD     m_FaultMargin;
    };
  //  std::map<

};

#endif