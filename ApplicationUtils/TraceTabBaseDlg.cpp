// TraceTabBaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TraceTabBaseDlg.h"
#include "Common/LogEvent.h"
#include "TraceTabBaseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTraceTabBaseDlg dialog

CTraceTabBaseDlg::CTraceTabBaseDlg(int DialogId, int TraceListId, 
								   int AboutBoxId, int MainframeId,
								   int TabId,
								   const char* Caption,
								   CWnd* pParent /*=NULL*/,
                                   bool AllowVerticalResize)
	: CTraceDlg(DialogId, Caption, pParent)
    , m_TabStyleRemove(0)
    , m_TabStyleAdd(0)
    , m_TabItem(0)
	, m_TabId(TabId)
    , m_AllowVerticalResize(AllowVerticalResize)
{
	if(MainframeId!=-1)
	{
		m_hIcon = AfxGetApp()->LoadIcon(MainframeId);
		SetIconId(MainframeId);
	}

	if(TraceListId!=-1)
		SetListBoxId(TraceListId);

	if(AboutBoxId!=-1)
		SetAboutBoxId(AboutBoxId);
}

void CTraceTabBaseDlg::ModifyTabStyle(DWORD dwRemove, DWORD dwAdd)
{
    m_TabStyleRemove = dwRemove;
    m_TabStyleAdd = dwAdd;
}

int CTraceTabBaseDlg::GetCurTab()
{
    return m_TabCtrl.GetCurSel();
}

void CTraceTabBaseDlg::SetTab(int Item)
{
    m_TabItem = Item;
    if (m_TabCtrl.m_hWnd!= NULL)
    {
        m_TabCtrl.SetCurSel(m_TabItem);
        ShowCurrentTab();
    }
}

void CTraceTabBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CTraceDlg::DoDataExchange(pDX);

	if(m_TabId != -1)
		DDX_Control(pDX, m_TabId, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CTraceTabBaseDlg, CTraceDlg)
	//{{AFX_MSG_MAP(CTraceTabBaseDlg)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
    ON_WM_SIZE()
	//ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
	//ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelChangeTab)
	//ON_CBN_SELCHANGE(IDC_COMBO_TRACE_LEVEL, OnSelchangeComboTraceLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTraceTabBaseDlg message handlers

BOOL CTraceTabBaseDlg::OnInitDialog()
{
	CTraceDlg::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    CreateAndPlaceDialogs();

    bool Result = ShowCurrentTab();
    UpdateData(FALSE);
    return Result ? TRUE : FALSE;  // return TRUE  unless you set the focus to a control
}

void CTraceTabBaseDlg::OnClose() 
{
	CTraceDlg::OnClose();
}

void CTraceTabBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CTraceDlg::OnSysCommand(nID, lParam);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTraceTabBaseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Allow only to resize width
LRESULT CTraceTabBaseDlg::OnNcHitTest(CPoint Point)
{
    if(m_AllowVerticalResize)
        return CTraceDlg::OnNcHitTest(Point);
    else
    {
        UINT Hit = CTraceDlg::OnNcHitTest(Point);

        switch(Hit)
        {
            case HTTOP:
            case HTTOPLEFT:
            case HTTOPRIGHT:
            case HTBOTTOM:
            case HTBOTTOMLEFT:
            case HTBOTTOMRIGHT:
                // Mask vertical sizing in any case
                return HTCLIENT;
            default:
                return Hit;
        }
    }
}

void CTraceTabBaseDlg::OnSize(UINT nType, int cx, int cy) 
{
    CTraceDlg::OnSize(nType, cx, cy);
    ResizeTabControl();
}

void CTraceTabBaseDlg::ResizeTabControl()
{
    if(m_TabCtrl.m_hWnd == NULL)
        return;

    CRect WindowRect;
    GetWindowRect(&WindowRect);

    CRect TabCtlRect;
    m_TabCtrl.GetWindowRect(TabCtlRect);
    TabCtlRect.bottom = WindowRect.bottom - 10;
    ScreenToClient(&TabCtlRect);
    m_TabCtrl.MoveWindow(TabCtlRect);
}

bool CTraceTabBaseDlg::ShowCurrentTab()
{
    int Current = m_TabCtrl.GetCurSel();
    bool TabFound = false;

    for(int i=0 ; (unsigned int)i < m_TabDlgVec.size() ; ++i)
    {
        bool IsCurrentTab = Current == i;
        m_TabDlgVec[i].Child->ShowWindow(IsCurrentTab ? SW_SHOW : SW_HIDE);
        if (IsCurrentTab)
            m_TabDlgVec[i].Child->Invalidate();
        TabFound |= IsCurrentTab;
    }
    return TabFound;
}

void CTraceTabBaseDlg::OnSelChangeTab(NMHDR* /* pNMHDR */, LRESULT* pResult) 
{
    bool Result = ShowCurrentTab();
    if (pResult)
	    *pResult = Result;
}

/////////////////////////////////////////////////////////////////////////////////

void CTraceTabBaseDlg::AddDlg(CDialog& Child, int DialogId, CString TabName)
{
	m_TabDlgVec.push_back(TabDlgData(&Child, DialogId, TabName));
}

BOOL CTraceTabBaseDlg::CreateAndPlaceDialogs()
{
    m_TabCtrl.ModifyStyle(m_TabStyleRemove, m_TabStyleAdd);

    size_t NumDlg = m_TabDlgVec.size();
    LONG Width = 0, Height = 0;
    for(size_t i = 0 ; i < NumDlg; ++i)
    {
        m_TabCtrl.InsertItem(m_TabDlgVec[i].DialogId , m_TabDlgVec[i].TabName);

        CDialog* pChild = m_TabDlgVec[i].Child;
        int DialogId = m_TabDlgVec[i].DialogId;
        if (!pChild->Create(DialogId, this))
        {
            LogEvent(LE_ERROR, "CTraceTabBaseDlg::CreateAndPlaceDialogs: Failed to build dialog. IDD = %d", DialogId);
            Assert(false);
            return FALSE;
        }
        CRect ChildRect;
        pChild->GetWindowRect(&ChildRect);
        Width = max(Width, ChildRect.right - ChildRect.left);
        Height = max(Height, ChildRect.bottom - ChildRect.top);
    }

    CRect TabWindowRect;
    m_TabCtrl.GetWindowRect(&TabWindowRect);
    LONG TabWidth = TabWindowRect.right - TabWindowRect.left;
    LONG TabHeight = TabWindowRect.bottom - TabWindowRect.top;
    Width = max(Width+10, TabWidth);
    Height = max(Height, TabHeight);
    TabWindowRect.right = TabWindowRect.left + Width;
    TabWindowRect.bottom = TabWindowRect.top + Height;
    LONG TabWindowLeft = TabWindowRect.left;
    LONG TabWindowTop = TabWindowRect.top;
    ScreenToClient(&TabWindowRect);
    m_TabCtrl.MoveWindow(TabWindowRect);
    //SetListBoxTopLeft(TabWindowRect.top + 5, 10+Width);
    //SetListBoxTopLeft(-1, 10+Width);
    SetListBoxTopLeft(-1, Width);

    LONG Left = MAXLONG, Top = 0;
    for(size_t i = 0 ; i < NumDlg; ++i)
	{
        CRect ItemRect;
        m_TabCtrl.GetItemRect(i, &ItemRect);
        Left = min(Left, ItemRect.left);
        Top = max(Top, ItemRect.bottom);
	}
    //Left += TabWindowLeft + 5;
    //Top += TabWindowTop + 5;
    Left += TabWindowLeft;
    Top += TabWindowTop + 2;
    for(size_t i = 0 ; i < NumDlg; ++i)
    {
        CDialog* pChild = m_TabDlgVec[i].Child;
        CRect ChildRect;
        pChild->GetWindowRect(&ChildRect);
        ChildRect.OffsetRect(Left - ChildRect.left, Top - ChildRect.top);
        ScreenToClient(ChildRect);
        pChild->MoveWindow(ChildRect);
    }

    if(!m_TabDlgVec.empty())
    {
        m_TabCtrl.SetCurSel(m_TabItem);
    }
    else
    {
        //no dialogs
        LogEvent(LE_INFO, "CTraceTabBaseDlg::CreateAndPlaceDialogs() No Dialogs so hide the Tab Control");
        m_TabCtrl.ShowWindow(SW_HIDE);
        SetListBoxTopLeft(-1, 8);
    }

    return TRUE;
}
