// TraceDlg.cpp: implementation of the CTraceDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TraceDlg.h"
#include "AboutDlg.h"
#include "Common/LogEvent.h"
#include "Common/Config.h"
#include "Common/MenuManager.h"
#include "GuiApplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum 
{ 
    IDM_ABOUTBOX = 0x0010,

    IDM_LEVEL    = 0x0020,
    IDM_DEBUG	 = IDM_LEVEL + LE_DEBUG,
    IDM_INFOLOW  = IDM_LEVEL + LE_INFOLOW,
    IDM_INFO     = IDM_LEVEL + LE_INFO,
    IDM_INFOHIGH = IDM_LEVEL + LE_INFOHIGH,
    IDM_WARNING  = IDM_LEVEL + LE_WARNING,
    IDM_ERROR    = IDM_LEVEL + LE_ERROR,
    IDM_CLEAR	 = 0x0030,

    IDM_CONFIG	 = 0x0040,
    IDM_LOG      = 0x0050,

    IDM_FIND      = 0x0102,
    IDM_FIND_NEXT = 0x0103,
    IDM_COPY      = 0x0104,
};

/////////////////////////////////////////////////////////////////////////////
// CTraceDlg dialog

CTraceDlg::CTraceDlg(int Id, const char* Caption, CWnd* pParent /*=NULL*/)	: 
    CDialog(Id, pParent), 
    m_hIcon(NULL),
    m_ListBoxMargins(0, 0, 0, 0), 
    m_Caption(Caption),
    m_ListBoxId(-1), 
    m_AboutBoxId(-1),
    m_Sink(NULL),
    m_DialogClosed(false),
    m_EditConfig(true)	
 {
	m_TraceDlgConfigSection.Format("%s_%s", "TraceDlg", m_Caption);
}

CTraceDlg::~CTraceDlg()
{
}

void CTraceDlg::Advise(ITraceDlgEvents* Sink)
{
    m_Sink = Sink;
}

void CTraceDlg::AddMessage(ELogSeverity Severity, const char* Text)
{
    m_TraceListBox.AddMessage(Severity, Text);
}

void CTraceDlg::Clear()
{
    m_TraceListBox.Clear();
}

void CTraceDlg::SetListBoxId(int ListBoxId)
{
    m_ListBoxId = ListBoxId;
}

void CTraceDlg::SetAboutBoxId(int AboutBoxId)
{
    m_AboutBoxId = AboutBoxId;
}

void CTraceDlg::SetIconId(int IconId)
{
	m_hIcon = AfxGetApp()->LoadIcon(IconId);
}

void CTraceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTraceDlg)
	//}}AFX_DATA_MAP

    if (m_ListBoxId != -1)
	    DDX_Control(pDX, m_ListBoxId, m_TraceListBox);
}

BEGIN_MESSAGE_MAP(CTraceDlg, CDialog)
	//{{AFX_MSG_MAP(CTraceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
    ON_WM_QUERYENDSESSION()
END_MESSAGE_MAP()

BOOL CTraceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    ExtendSystemMenu();
    LoadLogLevel();

    if (m_hIcon != NULL)
	    SetIcon(m_hIcon, TRUE);

    if (!m_Caption.IsEmpty())
        SetWindowText(m_Caption);
	
    MarkListBoxMargins();
    LoadWindowPlacement();

#ifdef _DEBUG
    AddStringToCaption(" (Debug)");
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTraceDlg::SetLogLevel(ELogSeverity Severity, BOOL Force/* = FALSE*/)
{
    ELogSeverity CurrentSeverity = m_TraceListBox.GetLogLevel();
    if (Severity != CurrentSeverity || Force)
    {
        m_TraceListBox.SetLogLevel(Severity);

        m_LevelMenu.CheckMenuItem(IDM_LEVEL + CurrentSeverity, MF_BYCOMMAND | MF_UNCHECKED);
        m_LevelMenu.CheckMenuItem(IDM_LEVEL + Severity, MF_BYCOMMAND | MF_CHECKED);
    }
}

ELogSeverity CTraceDlg::GetLogLevel()
{
	return m_TraceListBox.GetLogLevel();
}

void CTraceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    switch (nID)
    {
        case IDM_ABOUTBOX:
        {
		    CAboutDlg AboutDlg(m_AboutBoxId);
		    AboutDlg.DoModal();
        }
            break;

        case IDM_CONFIG:
        {
            const char * IniFileName = ((CGuiApplication *)AfxGetApp())->GetIniFileName();
            ShellExecute(NULL, "open", IniFileName, NULL, NULL, SW_SHOW);
        }
            break;

        case IDM_LOG:
        {
            const char * LogFileName = GetLogFileName();
            ShellExecute(NULL, "open", LogFileName, NULL, NULL, SW_SHOW);
        }
            break;
		
		case IDM_CLEAR:
			Clear();
			break;
		
		case IDM_DEBUG:
        case IDM_INFOLOW:
        case IDM_INFO:
        case IDM_INFOHIGH:
        case IDM_WARNING:
        case IDM_ERROR:
            SetLogLevel((ELogSeverity)(nID - IDM_LEVEL));
            break;
        case IDM_COPY:
            m_TraceListBox.Copy();
            break;
		case IDM_FIND:
			m_TraceListBox.Find(false /*FindNext*/);
			break;
        case IDM_FIND_NEXT:
            m_TraceListBox.Find(true /*FindNext*/);
            break;
        default:
            if(!CMenuManager::GetTheMenuManager().OnMenuSelected(nID))
    		    CDialog::OnSysCommand(nID, lParam);
            break;
    }
}

void CTraceDlg::OnClose()
{
    SaveLogLevel();
    SaveWindowPlacement();
    if(m_Sink)
    {
        //notify the application that it needs to close,
        //that way the trace dialog will be closed only after
        //application cleanup.
        m_Sink->OnTraceDlgClose();
    }
    else
    {
        m_DialogClosed = true;
        EndDialog(0);
        m_TraceListBox.DestroyWindow();
        AfxPostQuitMessage(0);
        CDialog::OnClose();
    }
}

void CTraceDlg::OnDestroy()
{
	// debug TraceListBox leak
    if(!m_DialogClosed)
    {
        SaveLogLevel();
        SaveWindowPlacement();
        m_DialogClosed = true;
        EndDialog(0);
        m_TraceListBox.DestroyWindow();
    }
	OutputDebugString("CTraceDlg::OnDestroy");
}

void CTraceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

    if (m_TraceListBox.m_hWnd != NULL)
        AdjustListBoxSize();
}

void CTraceDlg::SetListBoxTopLeft(int Top, int Left)
{
    if (Top >= 0)
        m_ListBoxMargins.top = Top;
    if (Left >= 0)
        m_ListBoxMargins.left = Left;
    AdjustListBoxSize();
}

void CTraceDlg::MarkListBoxMargins()
{
    CRect WindowRect;
    GetWindowRect(&WindowRect);

    CRect ListBoxRect;
    m_TraceListBox.GetWindowRect(&ListBoxRect);

    m_ListBoxMargins.left   = ListBoxRect.left   - WindowRect.left;
    m_ListBoxMargins.top    = ListBoxRect.top    - WindowRect.top;
    m_ListBoxMargins.right  = ListBoxRect.right  - WindowRect.right;
    m_ListBoxMargins.bottom = ListBoxRect.bottom - WindowRect.bottom;
}

void CTraceDlg::AdjustListBoxSize()
{
    CRect WindowRect;
    GetWindowRect(&WindowRect);

    CRect ListBoxRect;
    ListBoxRect.left   = WindowRect.left   + m_ListBoxMargins.left;
    ListBoxRect.top    = WindowRect.top    + m_ListBoxMargins.top;
    ListBoxRect.right  = WindowRect.right  + m_ListBoxMargins.right;
    ListBoxRect.bottom = WindowRect.bottom + m_ListBoxMargins.bottom;

    ScreenToClient(&ListBoxRect);
    m_TraceListBox.MoveWindow(ListBoxRect);
}


/////////////////////////////////////////////////////////////////////////////
// CTraceDlg private methods

void CTraceDlg::AddToolsMenu(CMenu* pSysMenu)
{
    m_ToolsMenu.CreatePopupMenu();
    m_ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_COPY, "&Copy\tCtrl+C");
    m_ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_FIND, "&Find\tCtrl+F");
    m_ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_FIND_NEXT, "Find &Next\tF3");
    m_ToolsMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_CLEAR, "Clear\tCtrl+X");
    pSysMenu->AppendMenu(MF_POPUP, (UINT)m_ToolsMenu.GetSafeHmenu(), "&TraceTools");
}

void CTraceDlg::ExtendSystemMenu()
{
	// Add "About..." and Log Level to system menu.
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
        AddLogLevelMenu(pSysMenu);

        AddToolsMenu(pSysMenu);
		// Add "Clear" "Logger" and "Test" menu item to system menu.
		pSysMenu->AppendMenu(MF_STRING, IDM_CLEAR, "Clear");
        pSysMenu->AppendMenu(MF_STRING, IDM_LOG, "View Log File...");
        if (m_EditConfig)
            pSysMenu->AppendMenu(MF_STRING, IDM_CONFIG, "Edit Configuration...");
        if (m_AboutBoxId != -1)
    		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, "About...");
	}
}

static const char* LogLevelKey = "LogLevel";

void CTraceDlg::SaveWindowPlacement()
{
    ::WriteWindowPlacement(this, m_TraceDlgConfigSection);
}

void CTraceDlg::LoadWindowPlacement()
{
    ::GetWindowPlacement(this, m_TraceDlgConfigSection, true);
}

void CTraceDlg::LoadLogLevel()
{
    ELogSeverity LogLevel = (ELogSeverity)GetConfigInt(m_TraceDlgConfigSection, LogLevelKey, LE_INFOLOW);
    SetLogLevel(LogLevel, TRUE/*Force*/);
}

void CTraceDlg::SaveLogLevel()
{
    int LogLevel = m_TraceListBox.GetLogLevel();
    WriteConfigInt(m_TraceDlgConfigSection, LogLevelKey, LogLevel);
}

void CTraceDlg::AddProcessIdToCaption()
{
    CString ProcessIdStr;
    ProcessIdStr.Format(" %d", GetCurrentProcessId());
    AddStringToCaption(ProcessIdStr);
}

void CTraceDlg::GetCaption(CString& Caption)
{
    GetWindowText(Caption);
}

void CTraceDlg::SetCaption(const char * Caption)
{
    SetWindowText(Caption);
}

void CTraceDlg::AddStringToCaption(const char * Str, bool ReplaceCurrent)
{

    CString Caption;
    if(ReplaceCurrent)
    {
        Caption = Str;
    }
    else
    {
        GetCaption(Caption);
        Caption += Str;
    }
    SetWindowText(Caption);
}

void CTraceDlg::SetIdToCaption(const char * Id, const char * IdStr /*= " Id: "*/)
{
    CString OldCaption, NewCaption, Str1, Str2;
    GetCaption(OldCaption);
    int Place = OldCaption.Find(IdStr);
    if (Place >= 0)
    {
        if (Place > 0)
            Str1 = OldCaption.Left(Place);
        Place = OldCaption.Find(" ", Place + strlen(IdStr));
        if (Place > 0)
            Str2 = OldCaption.Mid(Place);
    }
    else
        Str1 = OldCaption;
    if (Id[0] == '\0')
        IdStr = "";
    NewCaption.Format("%s%s%s%s", Str1, IdStr, Id, Str2);
    AddStringToCaption(NewCaption, true);
}

void CTraceDlg::SetIdToCaption(int Id, const char * IdStr /*= " Id: "*/)
{
    CString StrId;
    StrId.Format("%d", Id);
    SetIdToCaption(StrId, IdStr);
}

void CTraceDlg::SetEditConfig(bool On)
{
    m_EditConfig = On;
}

BOOL CTraceDlg::OnQueryEndSession()
{
//    AfxMessageBox("CTraceDlg::OnQueryEndSession");
    if (!CDialog::OnQueryEndSession())
        return FALSE;

    if(m_Sink)
        m_Sink->OnSystemShutdown();
    return TRUE;
}

void CTraceDlg::AddLogLevelMenu( CMenu* pSysMenu )
{
    m_LevelMenu.CreatePopupMenu();
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_DEBUG, "&Debug");
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_INFOLOW, "Info &Low");
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_INFO, "&Info");
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_INFOHIGH, "Info &High");
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_WARNING, "&Warning");
    m_LevelMenu.AppendMenu(MF_STRING | MF_ENABLED, IDM_ERROR, "&Error");

    pSysMenu->AppendMenu(MF_POPUP, (UINT)m_LevelMenu.GetSafeHmenu(), "Log &Level");
}
