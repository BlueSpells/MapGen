// ScriptCompiler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ScriptCompiler.h"
#include "ScriptCompilerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CScriptCompilerApp

BEGIN_MESSAGE_MAP(CScriptCompilerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CSpotBeamMapParserApp construction
const char* APP_VERSION = "0.1";


// CScriptCompilerApp construction

CScriptCompilerApp::CScriptCompilerApp() :
CResourcedGuiApplication("ScriptCompilerApp", APP_VERSION, IDR_MAINFRAME)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


void CScriptCompilerApp::AddDialogs()
{
	CResourcedGuiApplication::AddDialog(m_ScriptCompilerDlg, CScriptCompilerDlg::IDD, "ScriptCompiler");

	return;
}

// The one and only CScriptCompilerApp object

CScriptCompilerApp theApp;

