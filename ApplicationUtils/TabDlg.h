// TabDlg.h: interface for the CTabDlg class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
// TabDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabDlg dialog - base class of dialog that is a part of a tab control

class CTabDlg : public CDialog
{
// Construction
public:
	CTabDlg(UINT nIDTemplate, CWnd* pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabDlg)
	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    virtual void OnCancel() {}
    virtual void OnOK() {}
    virtual void OnGuiThread(WPARAM /*wParam*/) {}

    // Post an update so UpdateData is called in the correct thread context
    void PostUpdate(BOOL bSaveAndValidate);
    void GoToGuiThread(WPARAM wParam);

	// Generated message map functions
	//{{AFX_MSG(CTabDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
	//}}AFX_MSG
    LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
    LRESULT OnReachedGuiThread(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
    virtual void LoadData() {}
    virtual void SaveData() {}
};

