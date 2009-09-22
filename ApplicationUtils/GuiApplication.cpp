// CGuiApplication.cpp: implementation of the CGuiApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxmt.h>
#include <afxdisp.h>
#include <afxsock.h>
#include "GuiApplication.h"
#include "TraceDlg.h"
#include "Common/LogEvent.h"
#include "Common/Config.h"
#include "Common/ConfigVariables.h"
#include "Common/Utils.h"
#include "Common/TimeStamp.h"
#include "Common/ExceptionReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static std::string GetFirstParameter(const char* Default)
{
    std::string Arg1 = GetCommandLineArg(1);
    if (Arg1.size() == 0)
        return Default;
    else
        return Arg1;
}

CGuiApplication::CGuiApplication(const char* ApplicationName,  bool AllowMultipleInstances):
    m_AppMutex(NULL),
    m_AllowMultipleInstances(AllowMultipleInstances)
{
    CString ProgramPath = GetProgramPath();
    SetCurrentDirectory(ProgramPath);

    std::string IniPath = GetFirstParameter(ApplicationName);
    char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    _splitpath_s(IniPath.c_str(), drive, sizeof drive, dir, sizeof dir, fname, sizeof fname, ext, sizeof ext);
    if (fname[0] != '\0')
        m_ApplicationName = fname;
    else
        m_ApplicationName = ApplicationName;
    std::string IniFileName = m_ApplicationName;
    if (ext[0] != '\0')
        IniFileName += ext;
    else
        IniFileName += ".ini";
    if (dir[0] != '\0' || drive[0] != '\0')
    {
        IniFileName = drive + (dir + IniFileName);
    }
    else
        IniFileName = (LPCSTR)ProgramPath + IniFileName;
    m_IniFileName = GetFullPathName(IniFileName);
    SetConfigFileName(m_IniFileName.c_str());
    
    //add support for crash dump
    std::string CrashFileName = std::string(GetProgramPath()) + m_ApplicationName + ".crash";
    CExceptionReport::GetTheExceptionReport().Init(CrashFileName.c_str());

    if(!AllowMultipleInstances)
    {
        std::string MutexName = GetConfigString("ApplicationData", "MutexName", "");
        if(MutexName.empty())
            MutexName = ApplicationName;
        m_AppMutex = new CMutex(false, MutexName.c_str());
    }
}


static CTraceDlg* TheTraceDlg = NULL;
static CCriticalSection OutputLock;
static CConfigBool PrintOutputDebugString("LogManager", "PrintOutputDebugString");

/*static*/ ELogSeverity CGuiApplication::GetGuiLogOutputSeverity()
{
    if (TheTraceDlg != NULL)
        return TheTraceDlg->GetLogLevel();
    else
        return LE_FATAL;
}

/*static*/ void CGuiApplication::GuiLogOutput(ELogSeverity Severity, const char* Text)
{
	CTimeStamp CurrentTime = CTimeStamp::Now();
	std::string MessageWithTime = CurrentTime.ToShortString();
	MessageWithTime.append(" : ");
	MessageWithTime.append(Text);

    if (TheTraceDlg != NULL)
        TheTraceDlg->AddMessage(Severity, MessageWithTime.c_str() /*Text*/);

    if (PrintOutputDebugString)
    {
        OutputLock.Lock();
        std::string OutputMessage = GetLogEventLevelStr(Severity);
        OutputMessage = OutputMessage + " " + MessageWithTime + "\n";
        OutputDebugString(OutputMessage.c_str());
        OutputLock.Unlock();
    }

    if (Severity == LE_FATAL)
        AfxMessageBox(Text, MB_OK | MB_ICONSTOP);
}

BOOL CGuiApplication::InitInstance()
{
	AfxEnableControlContainer();
    if(m_AppMutex != NULL)
    {
        if(WaitForSingleObject(*m_AppMutex, 1000) == WAIT_TIMEOUT)
        {
            std::string Msg = GetApplicationName();
            Msg += " is Already running";
            AfxMessageBox(Msg.c_str());
            return FALSE;
        }
        m_AppMutex->Lock();
    }

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif


    SetLogEventOutput(GuiLogOutput, true, GetGuiLogOutputSeverity, GetApplicationName());

    // Make sure that MFC framework uses the same ini file
    free((void*)m_pszProfileName);
    m_pszProfileName = _tcsdup(GetIniFileName());

	// REMARK: this LogEvent will never show on the TraceDialog since
	//			it is not available in this stage.
    LogEvent(LE_INFOHIGH, "%s Starting", GetApplicationName());
    LogEvent(LE_INFOHIGH, "%s Started", GetApplicationName());
#ifdef DEBUG
	SetThreadName( m_nThreadID, GetApplicationName());
#endif

    // Set Write-back mode only for startup
    SetConfigWritebackMode(true);
    BOOL Result = Init();
// Writeback any parameter that is read from the configuration.
//    SetConfigWritebackMode(false);
    return Result;
}

int CGuiApplication::ExitInstance()
{
    Close();

    LogEvent(LE_INFOHIGH, "%s Terminated", GetApplicationName());
    
	SetLogEventOutput(NULL, false);

    if(m_AppMutex != NULL)
    {
        m_AppMutex->Unlock();
        delete m_AppMutex;
        m_AppMutex = NULL;
    }

    
    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return CWinApp::ExitInstance();
}

void CGuiApplication::SetTraceDlg(CTraceDlg* TraceDlg)
{
    TheTraceDlg = TraceDlg;
}

const char* CGuiApplication::GetApplicationName() 
{ 
    return m_ApplicationName.c_str(); 
}

const char* CGuiApplication::GetIniFileName() 
{ 
    return m_IniFileName.c_str(); 
}
