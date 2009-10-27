#if !defined(AFX_WMIMGVWRDLG_H__4D0007C6_563F_45AF_8156_30EF180F2603__INCLUDED_)
#define AFX_WMIMGVWRDLG_H__4D0007C6_563F_45AF_8156_30EF180F2603__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CWmimgvwrDlg dialog

#include "resource.h"
#include "globals.h"

class CWmimgvwrDlg : public CDialog
{
// Construction
public:
	CWmimgvwrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWmimgvwrDlg)
	enum { IDD = IDD_WMIMGVWR_DIALOG };
	CStatic	m_btn_exit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWmimgvwrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWmimgvwrDlg)
   afx_msg void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnBtnAppExit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

   CDC     m_dcMem;
   CBitmap m_current_bmp;

   IVIMGDATA m_img_pos;

   BOOL      m_bmp_loaded;

private:

   BOOL uf_load_bitmap(CDC &_dc);
   BOOL uf_calc_bmp_pos(void);

   void uf_full_screen(BOOL _bMove = TRUE)
   {
      HWND hBar = ::SHFindMenuBar(this->m_hWnd);
      ::CommandBar_Show( hBar, FALSE );

      this->SetForegroundWindow();
      ::SHFullScreen( this->m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON | SHFS_HIDESTARTICON );

      RECT cliRect;
      this->GetClientRect(&cliRect);

      if ( _bMove )
      {
         cliRect.top -= vg_wintask_h;
         cliRect.bottom += vg_wintask_h;
         this->MoveWindow(&cliRect);
      }
   }

};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WMIMGVWRDLG_H__4D0007C6_563F_45AF_8156_30EF180F2603__INCLUDED_)
