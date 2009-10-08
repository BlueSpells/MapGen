// ScriptCompilerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptCompiler.h"
#include "ScriptCompilerDlg.h"
#include "Common/Config.h"
#include "Common/collectionhelper.h"
#include "BarcodeProcessor/IHeader.h"
#include "BarcodeProcessor/IItem.h"
#include "BarcodeProcessor/BitLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScriptCompilerDlg dialog




CScriptCompilerDlg::CScriptCompilerDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(CScriptCompilerDlg::IDD, pParent)
	, m_FileName(_T("")), m_IsFileLoaded(false)
	, m_OutPutFile(_T("")), m_Interpreter(&m_ScriptReader)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScriptCompilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_BUTTON_LOAD, m_LoadButton);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_FileNameEditBox);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_FILENAME, m_OutPutFile);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_FILENAME, m_OutputFileEdit);
	DDX_Control(pDX, IDC_BUTTON_CREATE, m_CreateBinaryButton);
}

BEGIN_MESSAGE_MAP(CScriptCompilerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CScriptCompilerDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CScriptCompilerDlg::OnBnClickedButtonCreate)
END_MESSAGE_MAP()

const char *ScriptConfigSection = "ScriptCompilerSettings";

// CScriptCompilerDlg message handlers

BOOL CScriptCompilerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_OutputFileEdit.EnableWindow(FALSE);
	m_CreateBinaryButton.EnableWindow(FALSE);
	m_ScriptReader.Init();
	m_FileName = GetConfigString(ScriptConfigSection, "FileName", "").c_str();
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScriptCompilerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScriptCompilerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CScriptCompilerDlg::OnBnClickedButtonLoad()
{
	CleanLists();
	UpdateData();
	if (!m_IsFileLoaded)
	{
		if (!m_Interpreter.InterperetFile((std::string)m_FileName, m_HeaderList, m_ItemsList) /*m_ScriptReader.OpenScriptFile(m_FileName)*/)
		{
			return;
		}

		m_FileNameEditBox.EnableWindow(FALSE);
		m_LoadButton.SetWindowText("Close");
		m_OutputFileEdit.EnableWindow(TRUE);
		m_CreateBinaryButton.EnableWindow(TRUE);
		m_IsFileLoaded = true;
		WriteConfigString(ScriptConfigSection, "FileName", m_FileName);


		LogEvent(LE_DEBUG, __FUNCTION__ ": RAW STRINGS, as they appear in memory: ");
		for (unsigned int i = 0; i < m_HeaderList.size(); i++)
			LogEvent(LE_DEBUG, __FUNCTION__ ": Header #%d: %s", i, m_HeaderList[i]->GetBitBufferRawString().c_str());
		for (unsigned int i = 0; i < m_ItemsList.size(); i++)
			LogEvent(LE_DEBUG, __FUNCTION__ ": Item #%d: %s", i, m_ItemsList[i]->GetBitBufferRawString().c_str());

		LogEvent(LE_INFOHIGH, __FUNCTION__ ": PARSED STRINGS, parsed by every item according to the different fields: ");
		for (unsigned int i = 0; i < m_HeaderList.size(); i++)
			LogEvent(LE_INFOHIGH, __FUNCTION__ ": Header #%d: %s", i, m_HeaderList[i]->GetBitBufferParsedString().c_str());
		for (unsigned int i = 0; i < m_ItemsList.size(); i++)
			LogEvent(LE_INFOHIGH, __FUNCTION__ ": Item #%d: %s", i, m_ItemsList[i]->GetBitBufferParsedString().c_str());


		int BufferBitSize = 0;
		for (unsigned int i = 0; i < m_HeaderList.size(); i++)
			BufferBitSize += m_HeaderList[i]->GetBitBufferSize();
		for (unsigned int i = 0; i < m_ItemsList.size(); i++)
			BufferBitSize += m_ItemsList[i]->GetBitBufferSize();

		LogEvent(LE_INFOHIGH, __FUNCTION__ ": Script is encoded to a bit buffer with the size of: %d bits", BufferBitSize);
	}
	else
	{
		//m_ScriptReader.CloseScriptFile();
		m_FileNameEditBox.EnableWindow(TRUE);
		m_LoadButton.SetWindowText("Load");
		m_OutputFileEdit.EnableWindow(FALSE);
		m_CreateBinaryButton.EnableWindow(FALSE);
		m_IsFileLoaded = false;
	}
	
}

void CScriptCompilerDlg::OnBnClickedButtonCreate()
{
	// ToDo: In the future, in order to support longer scripts, create new thread here!
}

void CScriptCompilerDlg::CleanLists()
{
	while (m_ItemsList.size() > 0)
	{
		delete m_ItemsList[0];
		RemoveValueFromVector(m_ItemsList, m_ItemsList[0]);
	}

	while (m_HeaderList.size() > 0)
	{
		delete m_HeaderList[0];
		RemoveValueFromVector(m_HeaderList, m_HeaderList[0]);
	}
}

/*virtual*/ CScriptCompilerDlg::~CScriptCompilerDlg()
{
	CleanLists();
}