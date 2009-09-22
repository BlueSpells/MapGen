// TraceTabBaseDlg.h : header file
//
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CTraceTabBaseDlg dialog
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include "TraceDlg.h"
#include <vector>


class CTraceTabBaseDlg : public CTraceDlg
{
// Construction
public:
	CTraceTabBaseDlg(int DialogId, int TraceListId, 
		int AboutBoxId, int MainframeId, 
		int TabId,
		const char* Caption=NULL,
		CWnd* pParent =NULL,
        bool AllowVerticalResize = true);	// standard constructor
    void ModifyTabStyle(DWORD dwRemove, DWORD dwAdd);
    void SetTab(int Item);
    int GetCurTab();

protected:
	CStatic		m_TopLeftStatic;
	CTabCtrl	m_TabCtrl;
    DWORD       m_TabStyleRemove;
    DWORD       m_TabStyleAdd;
    int         m_TabItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTraceTabBaseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNcHitTest(CPoint Point);
	//afx_msg void OnClearButton();
	afx_msg void OnSelChangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboTraceLevel();
    afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AddDlg(CDialog& Child, int DialogId, CString TabName);
    BOOL CreateAndPlaceDialogs();
    void ResizeTabControl();
    bool ShowCurrentTab();

	int m_TabId;
    bool m_AllowVerticalResize;
		
	struct TabDlgData
	{
		TabDlgData(CDialog* _Child, int _DialogId, CString _TabName) : 
			Child(_Child), DialogId(_DialogId), TabName(_TabName){}
		CDialog *Child;
		int DialogId;
		CString TabName;
	};


	typedef std::vector<TabDlgData> TabDlgVec;
	typedef TabDlgVec::iterator		TabDlgIter;
	TabDlgVec				m_TabDlgVec;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

