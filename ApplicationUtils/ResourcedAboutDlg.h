#pragma once

#include "AppResources/Resource.h"
// CResourcedAboutDlg dialog

class CResourcedAboutDlg : public CDialog
{
public:
	CResourcedAboutDlg(const char* AppTitle, const char* AppVersion,
        const char* InfrastructureVersion,
        const char* ExtraText = NULL,
        CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourcedAboutDlg();

// Dialog Data
	enum { IDD = IDD_RESOURCEDABOUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
    CString m_AppVersion;
    CString m_InfrastructureVersion;
    CString m_AppTitle;
    CString m_ExtraText;
};
