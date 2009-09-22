#pragma once
#include <string>
#include "AppResources/Resource.h"
#include "ApplicationUtils\SmartComboBox.h"

//////////////////////////////////////////////////////////////////////////
// CSearchDlg
//////////////////////////////////////////////////////////////////////////

const int WM_SEARCH_REQUEST = WM_USER + 4001;

class CSearchDlg : public CDialog
{
public:
	CSearchDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSearchDlg();

    bool Init();
    void Show(); 

    std::string GetFindTerm() { return (LPCSTR)m_FindTerm; }
    bool GetIsCaseSensitive() { return m_CaseSensitive != 0; }

// Dialog Data
	enum { IDD = IDD_SEARCH_DIALOG };

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
    CWnd*   m_pParent;
    CString m_FindTerm;
    CSmartComboBox m_FindTermCtrl;
    BOOL m_CaseSensitive;
};
