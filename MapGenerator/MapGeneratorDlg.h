// MapGeneratorDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "MapGenerator.h"
#include "MapViewer.h"
#include "BarcodeProcessor\BarcodeEncoder.h"

// CMapGeneratorDlg dialog
class CMapGeneratorDlg : public CDialog
{
// Construction
public:
	CMapGeneratorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MAPGENERATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedSaveButton();

	CComboBox m_ObjectType;

	DWORD m_X;

	DWORD m_Y;

	afx_msg void OnBnClickedMoreCheck();

	DWORD m_NumberOfObjects;

	afx_msg void OnBnClickedRadio1();

	afx_msg void OnBnClickedRadio2();

	bool m_MoreObjects;
	EDuplicationDirection m_Dup;
	CMapGenerator m_MapGenerator;
	CMapViewer	  m_MapViewer;
public:
	CButton m_RadioDown;
public:
	afx_msg void OnBnClickedAddObject();
public:
	afx_msg void OnBnClickedRadioDown();
public:
	afx_msg void OnBnClickedRadioRight();
public:
	CString m_FileName;
public:
	CStatic m_MapView;
public:
	afx_msg void OnBnClickedUpdateMapView();
public:
	CScrollBar m_vbar;
public:
	CScrollBar m_hbar;

	UINT sourcex, sourcey;
	SCROLLINFO info;
public:
	afx_msg void OnBnClickedZoomIn();
public:
	afx_msg void OnBnClickedZoomOut();
public:
	CString m_LoadFileName;
public:
	afx_msg void OnBnClickedLoadButton();
public:
	void UpdateScrolls(void);

	CBarcodeEncoder m_BarcodeEncoder;
};
