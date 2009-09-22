// TraceDlg.h: interface for the CTraceDlg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "TraceListBox.h"
#include "..\Common\ThreadWithQueue.h"

//the ITraceDlgEvents class was added in order to support closing application
//components before closing the trace dialog.
class ITraceDlgEvents
{
public:
    virtual void OnTraceDlgClose() = 0;
    virtual void OnSystemShutdown() = 0;
};

class CTraceDlg : public CDialog
{
// Construction
public:
	CTraceDlg(int Id, const char* Caption, CWnd* pParent = NULL); 
    virtual ~CTraceDlg();
    void Advise(ITraceDlgEvents* Sink);
    void SetListBoxId(int ListBoxId);
    void SetAboutBoxId(int AboutBoxId);
    void SetIconId(int IconId);

    void AddToolsMenu(CMenu* pSysMenu);

    // Severity values are defined in LogEvent.h
    void AddMessage(ELogSeverity Severity, const char* Text);

    void Clear();

    void GetCaption(CString& WindowText);
    void SetCaption(const char * Caption);
    void AddStringToCaption(const char * Str, bool ReplaceCurrent = false);
    void SetIdToCaption(int Id, const char * IdStr = " Id: ");
    void SetIdToCaption(const char * Id, const char * IdStr = " Id: ");
    void SetEditConfig(bool On);
    void AddProcessIdToCaption();
    ELogSeverity GetLogLevel();

// Dialog Data
	//{{AFX_DATA(CTraceDlg)
	CTraceListBox	m_TraceListBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTraceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    // Implemented inherited methods
    virtual void OnCancel() {}
    virtual void OnOK() {};

	// Generated message map functions
	//{{AFX_MSG(CTraceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnQueryEndSession();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
        
    void SetLogLevel(ELogSeverity Severity, BOOL Force = FALSE);
    void SetListBoxTopLeft(int Top, int Left); 

private:
    /////////////////////////////////////////////////////////////////////////////
    // CTraceDlg message handlers
    void ExtendSystemMenu();

    void AddLogLevelMenu( CMenu* pSysMenu );
    void SaveWindowPlacement();
    void LoadWindowPlacement();
    void LoadLogLevel();
    void SaveLogLevel();
    void MarkListBoxMargins();
    void AdjustListBoxSize();

private:
    HICON   m_hIcon;
    CRect   m_ListBoxMargins;
    CString m_Caption;
	CString m_TraceDlgConfigSection;
    int     m_ListBoxId;
    int     m_AboutBoxId;
    CMenu   m_LevelMenu;
    CMenu   m_ToolsMenu;
    bool    m_DialogClosed;
    bool    m_EditConfig;
    ITraceDlgEvents* m_Sink;
};


