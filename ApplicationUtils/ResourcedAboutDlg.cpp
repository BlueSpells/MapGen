// ResourcedAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourcedAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CResourcedAboutDlg dialog

CResourcedAboutDlg::CResourcedAboutDlg(const char* AppTitle, const char* AppVersion,
                                       const char* InfrastructureVersion,
                                       const char* ExtraText,
                                       CWnd* pParent /*=NULL*/)
	: CDialog(CResourcedAboutDlg::IDD, pParent)
    , m_AppVersion(AppVersion)
    , m_InfrastructureVersion(InfrastructureVersion)
    , m_AppTitle(AppTitle)
    , m_ExtraText(ExtraText)

{
}

CResourcedAboutDlg::~CResourcedAboutDlg()
{
}

void CResourcedAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_APP_VERSION, m_AppVersion);
    DDX_Text(pDX, IDC_INFRASTRUCTURE_VERSION, m_InfrastructureVersion);
    DDX_Text(pDX, IDC_EXTRA_TEXT, m_ExtraText);
}

BOOL CResourcedAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetWindowText(m_AppTitle);
    return TRUE;
}



BEGIN_MESSAGE_MAP(CResourcedAboutDlg, CDialog)
END_MESSAGE_MAP()


// CResourcedAboutDlg message handlers
