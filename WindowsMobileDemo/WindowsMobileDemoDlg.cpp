// WindowsMobileDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsMobileDemo.h"
#include "WindowsMobileDemoDlg.h"
#include "appwindow.h"
#define CMD_STILL_IMAGE 1

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
	ON_COMMAND(CMD_STILL_IMAGE, HandleCommand)
	ON_MESSAGE(WM_USER, OnMyMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CWindowsMobileDemoDlg message handlers

// implementation for WM_MYMESSAGE handler
LRESULT CWindowsMobileDemoDlg::OnMyMessage( WPARAM wParam, LPARAM lParam ) 
{
	HRESULT hr = S_OK;
	Message *pMessage = (Message *)lParam;

	if( pMessage == NULL )
	{
		ERR( E_POINTER );
	}

	if(( pMessage->wzMessage == NULL ) || ( *(pMessage->wzMessage) == 0 ))
	{
		return S_OK;
	}

// 	if( m_wzStatusString )
// 	{
// 		LocalFree( m_wzStatusString );
// 		m_wzStatusString = NULL;
// 	}
// 
// 	m_wzStatusString = pMessage->wzMessage;

	if( pMessage->dwMessage == MESSAGE_ENDRECORDING )
	{
		//EnableWindow( m_stillButtonHwnd, TRUE );
	}
	else if( pMessage->dwMessage == MESSAGE_FILECAPTURED )
	{
		//EnableWindow( m_stillButtonHwnd, TRUE );
	}

	LocalFree( pMessage );
	//InvalidateRect( m_hwnd, NULL, TRUE );

Cleanup:
	return hr;
}

void CWindowsMobileDemoDlg::HandleCommand( /*WPARAM wParam */)
{
	if( m_pGraphManager == NULL )
	{
		//ERR( E_FAIL );
	}


	//EnableWindow( m_stillButtonHwnd, FALSE );
	m_pGraphManager->CaptureStillImage();
}

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
	HRESULT     hr = S_OK;
	
	// Launch COM
	CHK( CoInitialize( NULL ));

	m_pGraphManager = new CGraphManager();
	if( m_pGraphManager == NULL )
	{
		ERR( E_OUTOFMEMORY );
	}

	CHK( m_pGraphManager->RegisterNotificationWindow( AfxGetMainWnd()->GetSafeHwnd()));//AfxGetApp()->GetMainWnd()->m_hWnd/*m_hwnd */));

	CHK( m_pGraphManager->Init());
	CHK( m_pGraphManager->BuildCaptureGraph());

	CHK( m_pGraphManager->RunCaptureGraph());
	
// 	HINSTANCE hInstance = AfxGetInstanceHandle();
// 	// WinMain of MyCameraCapture project
// 	// FROM HERE
// 
// 	HRESULT hr = S_OK;
// 
// 	CAppWindow      appWindow;
// 
// 	// Launch COM
// 	CHK( CoInitialize( NULL ));
// 
// 	// Initialize the UI Controls
// 	CHK( appWindow.CreateControls( hInstance ));
// 
// 	// Start the UI and run the message pump
// 	CHK( appWindow.Run());
// 
Cleanup:
 	CoUninitialize();
	// TO HERE
	
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

