// WindowsMobileDemoDlg.h : header file
//

#pragma once
#include "graphmanager.h"

// CWindowsMobileDemoDlg dialog
class CWindowsMobileDemoDlg : public CDialog
{
// Construction
public:
	CWindowsMobileDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WINDOWSMOBILEDEMO_DIALOG };

protected:  // control bar embedded members
	CCommandBar m_dlgCommandBar;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	CGraphManager *m_pGraphManager;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void HandleCommand( /*WPARAM wParam */);
	afx_msg LRESULT OnMyMessage( WPARAM wParam, LPARAM lParam );

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
};
