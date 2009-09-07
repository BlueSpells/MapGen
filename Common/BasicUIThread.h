#pragma once
// BasicUIThread.h : header file
//

#include "TimerManager.h"
/////////////////////////////////////////////////////////////////////////////
// CBasicUIThread thread

class CBasicUIThread : public CWinThread //, public ITimerManagerEvents
{
public:
    virtual ~CBasicUIThread();
    
    bool StartThread();
    int AddTimer(ITimerManagerEvents* TimerSink, UINT ElapseInMilli);
	bool KillTimer(int TimerId);
	bool KillThread();
	bool IsThreadActive() const;
	
private:
    // Hide copy ctor and assignment operator
    CBasicUIThread(const CBasicUIThread &);
    CBasicUIThread & operator=(const CBasicUIThread &);

    //DECLARE_DYNCREATE(CBasicUIThread)
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicUIThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnIdle(LONG /*lCount*/) { return FALSE; }
	//}}AFX_VIRTUAL

// Implementation
protected:
    CBasicUIThread();
	
    // To be implemented by derived classes
    virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	BOOL IsIdleMessage( MSG* pMsg );
//	virtual void OnTimer(UINT_PTR idEvent){};
    virtual void OnClose(){};
	
	// Generated message map functions
	//{{AFX_MSG(CBasicUIThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

private:
	CTimerManager* m_TimerManager;

	//HANDLE m_hEventDead;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

