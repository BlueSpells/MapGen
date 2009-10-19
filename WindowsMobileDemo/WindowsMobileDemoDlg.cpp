// WindowsMobileDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsMobileDemo.h"
#include "WindowsMobileDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWindowsMobileDemoDlg dialog

CWindowsMobileDemoDlg::CWindowsMobileDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindowsMobileDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWindowsMobileDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWindowsMobileDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CWindowsMobileDemoDlg message handlers

BOOL CWindowsMobileDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (!m_dlgCommandBar.Create(this) ||
	    !m_dlgCommandBar.InsertMenuBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create CommandBar\n");
		return FALSE;      // fail to create
	}
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CWindowsMobileDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	DRA::RelayoutDialog(
		AfxGetInstanceHandle(), 
		this->m_hWnd, 
		DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_WINDOWSMOBILEDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_WINDOWSMOBILEDEMO_DIALOG));
}
#endif

