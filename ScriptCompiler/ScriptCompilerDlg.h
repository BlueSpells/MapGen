// ScriptCompilerDlg.h : header file
//

#pragma once



#include "..\ApplicationUtils/TabDlg.h"
#include "afxwin.h"
#include "ScriptInterpreter/SimpleScriptReader.h"
#include "ScriptInterpreter/parkingscriptinterpreter.h"
#include "BarcodeProcessor/BarcodeEncoder.h"
#include "BarcodeProcessor/BitBufferCodec.h"


class IItem;
class IHeader;

// CScriptCompilerDlg dialog
class CScriptCompilerDlg : public CTabDlg
{
// Construction
public:
	CScriptCompilerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CScriptCompilerDlg();

// Dialog Data
	enum { IDD = IDD_SCRIPTCOMPILER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonCreate();
	DECLARE_MESSAGE_MAP()

	// Input Script file
	CString m_FileName;
	CButton m_LoadButton;
	CEdit m_FileNameEditBox;

	// Output Binary file
	CString m_OutPutFile;
	CEdit m_OutputFileEdit;
	CButton m_CreateBinaryButton;

	CSimpleScriptReader			m_ScriptReader;
	CParkingScriptInterpreter	m_Interpreter;
	CBarcodeEncoder				m_BarcodeEncoder;
	CBitBufferCodec				m_Codec;
	bool m_IsFileLoaded;

	std::vector<IItem *> m_ItemsList;
	std::vector<IHeader *> m_HeaderList;
	int *m_ItemsCounter;
	int *m_ItemsBitsCounter;
	int m_TotalBitsCounter;

	void CleanLists();
};
