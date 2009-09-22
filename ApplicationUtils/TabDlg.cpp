// TabDlg.cpp: implementation of the CTabDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "TabDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int WM_UPDATE_DATA = WM_USER + 2000;
static const int WM_GUI_THREAD = WM_USER + 2001;

/////////////////////////////////////////////////////////////////////////////
// CTabDlg dialog

CTabDlg::CTabDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
}

//void CTabDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	//{{AFX_DATA_MAP(CTabDlg)
//	//}}AFX_DATA_MAP
//}

BEGIN_MESSAGE_MAP(CTabDlg, CDialog)
	//{{AFX_MSG_MAP(CTabDlg)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATE_DATA, OnUpdateData)
    ON_MESSAGE(WM_GUI_THREAD, OnReachedGuiThread)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabDlg message handlers

BOOL CTabDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    LoadData();

    UpdateData(FALSE/*!bSaveAndValidate*/);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTabDlg::OnDestroy() 
{
    UpdateData(TRUE/*bSaveAndValidate*/);
    
    SaveData();
    
    CDialog::OnDestroy();
}

LRESULT CTabDlg::OnUpdateData(WPARAM wParam, LPARAM /*lParam*/)
{
    UpdateData(wParam);
    return 0;
}

LRESULT CTabDlg::OnReachedGuiThread(WPARAM wParam, LPARAM /*lParam*/)
{
    OnGuiThread(wParam);
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// protected methods

void CTabDlg::PostUpdate(BOOL bSaveAndValidate)
{
    PostMessage(WM_UPDATE_DATA, bSaveAndValidate);
}

void CTabDlg::GoToGuiThread(WPARAM wParam)
{
    PostMessage(WM_GUI_THREAD, wParam);
}

/////////////////////////////////////////////////////////////////////////////
// private methods

