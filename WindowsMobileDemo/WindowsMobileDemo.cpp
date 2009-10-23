// WindowsMobileDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WindowsMobileDemo.h"
#include "WindowsMobileDemoDlg.h"
#include "appwindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWindowsMobileDemoApp

BEGIN_MESSAGE_MAP(CWindowsMobileDemoApp, CWinApp)
END_MESSAGE_MAP()


// CWindowsMobileDemoApp construction
CWindowsMobileDemoApp::CWindowsMobileDemoApp()
	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CWindowsMobileDemoApp object
CWindowsMobileDemoApp theApp;

// CWindowsMobileDemoApp initialization

BOOL CWindowsMobileDemoApp::InitInstance()
{
/*
    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the Windows Mobile specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
*/
	
	
	/* Following Implementation is for running the CWmimgvwrDlg only*/
	// ===============================================================
	//    RECT tbRect;
	//    HWND hTaskBar = NULL;
	// 	CWmimgvwrDlg *lpDlg = new CWmimgvwrDlg();
	// 
	//    hTaskBar = ::FindWindow( L"HHTaskBar", L"" );
	//    if ( hTaskBar )
	//    {
	//       if ( ::GetWindowRect(hTaskBar, &tbRect) )
	//          vg_wintask_h = tbRect.bottom; 
	//    }
	// 
	//    vg_cmd_line = AfxGetApp()->m_lpCmdLine;
	// 
	// 	m_pMainWnd = lpDlg;
	// 
	// 	lpDlg->DoModal();
	// 
	// 	return FALSE;


	/* Following implementation starts the VideoCapture Window:*/
	// ==========================================================
	HINSTANCE hInstance = AfxGetInstanceHandle();

	HRESULT hr = S_OK;

	CAppWindow      appWindow;

	// Launch COM
	CHK( CoInitialize( NULL ));

	// Initialize the UI Controls
	CHK( appWindow.CreateControls( hInstance ));

	// Start the UI and run the message pump
	CHK( appWindow.Run());

Cleanup:
	CoUninitialize();
	return 0;

	
	/* Following implementation is when running a dialog in DoModal only: */
	// =====================================================================
// 	CWindowsMobileDemoDlg dlg;
// 	m_pMainWnd = &dlg;
// 	INT_PTR nResponse = dlg.DoModal();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: Place code here to handle when the dialog is
// 		//  dismissed with OK
// 	}
// 
// 	// Since the dialog has been closed, return FALSE so that we exit the
// 	//  application, rather than start the application's message pump.
// 	return FALSE;
}
