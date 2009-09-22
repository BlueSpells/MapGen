#include "StdAfx.h"
#include "ResourcedGuiApplication.h"
#include "Common/LogEvent.h"
#include "Common/Utils.h"
#include "Common/Config.h"
#include "AppResources/ResourceExports.h"
#include "ApplicationUtils/AboutDlg.h"
#include "ResourcedAboutDlg.h"
//#include "DalCore/DalGlobals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char* LEGAL_STR = "Copyright (c) 2005";

//////////////////////////////////////////////////////////////////////////

class CResourcedMainDlg: public CTraceTabBaseDlg
{
public:
    CResourcedMainDlg(HINSTANCE ResourceDllHandle, HINSTANCE ExeHandle,
        int DialogId, int TraceListId, int IconId,
        int TabId, const char* Caption): 
                CTraceTabBaseDlg(DialogId, TraceListId,
                    -1, IconId, TabId, Caption),
                m_ExeHandle(ExeHandle),
                m_ResourceDllHandle(ResourceDllHandle)
    {
    }

    void AddDlg(CDialog& Child, int DialogId, CString TabName)
    {
        CTraceTabBaseDlg::AddDlg(Child, DialogId, TabName);
    }

    virtual BOOL OnInitDialog()
    {
        AfxSetResourceHandle(m_ResourceDllHandle);
        AfxSetResourceHandle(m_ExeHandle);

        //call the Trace Dlg and not TabbedBaseTraceDlg because
        //the dialogs that are displayed on the tab control 
        //should be taken from the Exe resources while the trace dialog
        //is taken from the Resource Dll
        CTraceTabBaseDlg::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon

        UpdateData(FALSE);

        return TRUE;  // return TRUE  unless you set the focus to a control
    }

private:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);

private:
    HINSTANCE m_ResourceDllHandle;
    HINSTANCE m_ExeHandle;
};

BEGIN_MESSAGE_MAP(CResourcedMainDlg, CTraceTabBaseDlg)
    //}}AFX_MSG_MAP
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, OnTcnSelchangeTabCtrl)
END_MESSAGE_MAP()

void CResourcedMainDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
    CTraceTabBaseDlg::OnSelChangeTab(pNMHDR, pResult);
}


//////////////////////////////////////////////////////////////////////////



CResourcedGuiApplication::CResourcedGuiApplication(const char* AppTitle,
                                                  const char* AppVersion,
                                                  int IconResourceId,
                                                  bool AllowMultipleApplication,
                                                  bool EditConfig): 
    CGuiApplication(AppTitle, AllowMultipleApplication),
    m_MainDlg(NULL),
    m_ResourceDLL(NULL),
    m_ResourceModuleHandle(NULL),
    m_ExeResourceHandle(NULL),
    m_AppVersion(AppVersion),
    m_IconId(IconResourceId),
    m_ConfirmCloseApplication(false),
    m_EditConfig(EditConfig),
    m_RememberTab(false)
{
}

CResourcedGuiApplication::~CResourcedGuiApplication()
{
}

bool CResourcedGuiApplication::Init()
{
    //Load the template dialog resources from the AppResource.dll
    HICON ExeIcon = NULL;
    if(m_IconId != -1)
        ExeIcon = LoadIcon(m_IconId);

    m_ResourceDLL = ::LoadLibrary(RESOURCE_DLL_NAME);
    if(m_ResourceDLL == NULL)
    {
        AfxMessageBox("Failed to Load the Dal Resource Dll");
        return false;
    }

    m_ResourceModuleHandle = GetModuleHandle(RESOURCE_DLL_NAME);
    if(m_ResourceModuleHandle == NULL)
    {
        AfxMessageBox("Failed to Load the Dal Resource Dll");
        return false;
    }

    //Save the EXE resource handle and let the application use the resource dll 
    m_ExeResourceHandle = AfxGetResourceHandle();
    AfxSetResourceHandle(m_ResourceModuleHandle);

    m_MainDlg = new CResourcedMainDlg(m_ResourceModuleHandle, m_ExeResourceHandle,
        IDD_DAL_DIALOG, IDC_TRACE_LIST, -1, IDC_TAB_CTRL, GetApplicationName());
    if(m_MainDlg == NULL)
    {
        AfxMessageBox("Failed to Create the Trace Dialog object");
        return false;
    }

//    m_MainDlg->SetAboutBoxId(-1); //the about dialog box menu will be added and handled by the resourced gui application
//    m_MainDlg->SetListBoxId(IDC_TRACE_LIST);
    SetTraceDlg(m_MainDlg);
    m_pMainWnd = m_MainDlg;
    if(m_IconId == -1)
        m_MainDlg->SetIconId(IDI_DAL);      

    AfxSetResourceHandle(m_ExeResourceHandle);
    AddDialogs();
    AfxSetResourceHandle(m_ResourceModuleHandle);

    m_MainDlg->SetEditConfig(m_EditConfig);

    if(!m_MainDlg->Create(IDD_DAL_DIALOG))
    {
        AfxMessageBox("Failed to Create the Main Dialog");
        return false;
    }
    m_MainDlg->Advise(this);

    //override the default icon with app specific icon
    if(ExeIcon != NULL)
    {
        m_MainDlg->SetIcon(ExeIcon, FALSE);
        m_MainDlg->SetIcon(ExeIcon, TRUE);
    }

    CMenuManager::GetTheMenuManager().SetMainMenu(m_MainDlg->GetSystemMenu(FALSE));

    //call derived classes initialization function
    LogEvent(LE_INFO, "CResourcedGuiApplication::Init, %s Performing Initialization", GetApplicationName());
    bool InitSuccess = PerformInitalization();
    if(!InitSuccess)
    {
        LogEvent(LE_ERROR, "CResourcedGuiApplication(%s)::Init, PerformInitalization Failed",
            GetApplicationName());
        AfxMessageBox(" Failed to Perform application initialization");
        PerformCleanup();
        return false;
    }

    std::string AboutMenuText = "About ";
    AboutMenuText += GetApplicationName();

    AddMenuItem(IDM_ABOUT, NULL, AboutMenuText.c_str());
	AfxSetResourceHandle(m_ExeResourceHandle);

    return true;
}

void CResourcedGuiApplication::Close()
{
    LogEvent(LE_INFOHIGH, "CResourcedGuiApplication::Close(), Start Closing the %s Dialog",
        GetApplicationName());

    SetTraceDlg(NULL);
    if(m_MainDlg)
    {
        ::DestroyWindow(m_MainDlg->m_hWnd);
        //m_MainDlg->DestroyWindow();
        delete m_MainDlg;
        m_MainDlg = NULL;
    }

    if(m_ExeResourceHandle != NULL)
        AfxSetResourceHandle(m_ExeResourceHandle);
    if(m_ResourceDLL != NULL)
        FreeLibrary(m_ResourceDLL);
}

void CResourcedGuiApplication::CloseApplication()
{
    PostThreadMessage(WM_QUIT, 0, 0);
}

void CResourcedGuiApplication::ShutDown()
{
    if (m_RememberTab)
        WriteConfigInt(GetApplicationName(), "CurTab", m_MainDlg->GetCurTab());
    if (PerformCleanup())
        CloseApplication();
}

void CResourcedGuiApplication::PostCloseRequest()
{
    if(m_MainDlg == NULL)
        return;
    m_MainDlg->PostMessage(WM_CLOSE);
}

void CResourcedGuiApplication::OnTraceDlgClose()
{
    //call derived classes termination function
    bool CloseApp = true;
    if(m_ConfirmCloseApplication)
        CloseApp = ConfirmClose();

    if(CloseApp)
        ShutDown();
}

void CResourcedGuiApplication::OnSystemShutdown()
{
        ShutDown();
}

bool CResourcedGuiApplication::AddDialog(CDialog& DlgObject, int DialogId, const char* DialogTitle)
{
    if(m_MainDlg == NULL)
    {
        LogEvent(LE_ERROR, "CResourcedGuiApplication::AddDialog() Main Dlg is NULL");
        return false;
    }

    m_MainDlg->AddDlg(DlgObject, DialogId, DialogTitle);
    return true;
}

void CResourcedGuiApplication::OnMenuSelected(int Id)
{
    //LogEvent(LE_INFO, "CResourcedGuiApplication::OnMenuSelected, Id = %d", Id);
    if(Id == IDM_ABOUT)
        DisplayAboutBox();
    else
        LogEvent(LE_ERROR, "CResourcedGuiApplication::OnMenuSelected: Unknown menu id %d", Id);
}

void CResourcedGuiApplication::DisplayAboutBox()
{
    AfxSetResourceHandle(m_ResourceModuleHandle);

    std::string AppTitle("About ");
    AppTitle += GetApplicationName();
    CResourcedAboutDlg Dlg(AppTitle.c_str(), m_AppVersion.c_str(),
        /*GetDalVersion().c_str()*/"", LEGAL_STR);
    Dlg.DoModal();
    AfxSetResourceHandle(m_ExeResourceHandle);
}

void CResourcedGuiApplication::SetConfirmCloseApplication(bool ConfirmClose)
{
    m_ConfirmCloseApplication = ConfirmClose;
}

bool CResourcedGuiApplication::ConfirmClose()
{
    AfxSetResourceHandle(m_ExeResourceHandle);
    CString ConfirmTitle = CString("Are you sure you want to close the ") + GetApplicationName() + " Application";
    return AfxMessageBox(ConfirmTitle, MB_YESNO) == IDYES;
}

const CString CResourcedGuiApplication::GetCaption()
{
    CString Caption;
    m_MainDlg->GetCaption(Caption);
    return Caption;
}

void CResourcedGuiApplication::GetCaption(CString& Caption)
{
    m_MainDlg->GetCaption(Caption);
}

void CResourcedGuiApplication::SetCaption(const char * Caption)
{
    m_MainDlg->SetCaption(Caption);
}

void CResourcedGuiApplication::SetIdToCaption(int Id, const char * IdStr /*= " Id: "*/)
{
    if(m_MainDlg == NULL)
    {
        LogEvent(LE_ERROR, "CResourcedGuiApplication::SetIdToCaption() Main Dlg is NULL");
        return;
    }

    m_MainDlg->SetIdToCaption(Id, IdStr);
}

void CResourcedGuiApplication::SetIdToCaption(const char * Id, const char * IdStr /*= " Id: "*/)
{
    if(m_MainDlg == NULL)
    {
        LogEvent(LE_ERROR, "CResourcedGuiApplication::SetIdToCaption() Main Dlg is NULL");
        return;
    }

    m_MainDlg->SetIdToCaption(Id, IdStr);
}

void CResourcedGuiApplication::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd)
{
    Assert(m_MainDlg != NULL);
    m_MainDlg->ModifyTabStyle(dwRemove, dwAdd);
}

bool CResourcedGuiApplication::AddMenuItem(int MenuId, 
                                           const char* MenuName, 
                                           const char* SubMenuName)
{
    return CMenuManager::GetTheMenuManager().AddMenuItem(
        this, MenuName, SubMenuName, MenuId);
}                                      

//////////////////////////////////////////////////////////////////////////
// default implementation for virtual functions that should be
// override by derived classes

bool CResourcedGuiApplication::PerformInitalization()
{
    //default implementation returns ok
    return true;
}

bool CResourcedGuiApplication::PerformCleanup()
{
    return true;
}

void CResourcedGuiApplication::AddDialogs()
{
    //to be implemented by derived
}

bool CResourcedGuiApplication::SetAffinity()
{
    int ProcessAffinityMask = GetConfigInt(GetApplicationName(), "ProcessAffinityMask", -1);
    if (ProcessAffinityMask != -1)
    {
        HANDLE hProc = GetCurrentProcess();
        if (SetProcessAffinityMask(hProc, ProcessAffinityMask))
            LogEvent(LE_INFOHIGH, __FUNCTION__ ": Process 0x%x affinity mask set to %d", hProc, ProcessAffinityMask);
        else
        {
            LogEvent(LE_ERROR, __FUNCTION__ ": Error setting process 0x%x affinity mask to %d (%s)",
                hProc, ProcessAffinityMask, GetSystemErrorString(GetLastError()).c_str());
            return false;
        }
    }
    return true;
}

bool CResourcedGuiApplication::SetProcessPriority()
{
    HANDLE hProc = GetCurrentProcess();
    std::string ProcessPriority = GetConfigString(GetApplicationName(), "ProcessPriority", "NORMAL");
    DWORD PriorityValue = PriorityClassStr2Value(ProcessPriority.c_str());
    if(SetPriorityClass(hProc, PriorityValue))
        LogEvent(LE_INFOHIGH, __FUNCTION__ ": Process 0x%x priority set to %S", hProc, ProcessPriority.c_str());
    else
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": Error setting process 0x%x priority to %s (%s)",
            hProc, ProcessPriority.c_str(), GetSystemErrorString(GetLastError()).c_str());
        return false;
    }
    return true;
}

void CResourcedGuiApplication::RememberTab()
{
    m_RememberTab = true;
    Assert(m_MainDlg != NULL);
    m_MainDlg->SetTab(GetConfigInt(GetApplicationName(), "CurTab", 0));
}

void CResourcedGuiApplication::AddProcessIdToCaption()
{
    Assert(m_MainDlg != NULL);
    m_MainDlg->AddProcessIdToCaption();
}
