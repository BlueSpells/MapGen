// MapGeneratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MapGeneratorApp.h"
#include "MapGeneratorDlg.h"
#include "BarcodeProcessor\ParkingItem.h"
#include "BarcodeProcessor\ComplexItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMapGeneratorDlg dialog


CMapGeneratorDlg::CMapGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMapGeneratorDlg::IDD, pParent)
	, m_X(0)
	, m_Y(0)
	, m_NumberOfObjects(0)
	, m_MoreObjects(false)
	, m_Dup(DUPLICATE_DOWN)
	, m_FileName(_T(""))
#pragma warning( push, 3 )
#pragma warning(disable:4355) // not to worry. m_MapViewer only stores this and doesn't access it !!
	, m_MapViewer(this)
#pragma warning( pop )
	, sourcey(0)
	, sourcex(0)
	, m_LoadFileName(_T("AutoSave.Bmp"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMapGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ObjectType);
	DDX_Text(pDX, IDC_EDIT2, m_X);
	DDX_Text(pDX, IDC_EDIT4, m_Y);
	DDX_Text(pDX, IDC_EDIT1, m_NumberOfObjects);
	DDX_Control(pDX, IDC_RADIO_DOWN, m_RadioDown);
	DDX_Text(pDX, IDC_EDIT3, m_FileName);
	DDX_Control(pDX, IDC_MAP_VIEW, m_MapView);
	DDX_Control(pDX, IDC_SCROLLBAR1, m_vbar);
	DDX_Control(pDX, IDC_SCROLLBAR2, m_hbar);
	DDX_Text(pDX, IDC_EDIT5, m_LoadFileName);
}

BEGIN_MESSAGE_MAP(CMapGeneratorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SAVE_BUTTON, &CMapGeneratorDlg::OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_MORE_CHECK, &CMapGeneratorDlg::OnBnClickedMoreCheck)
	ON_BN_CLICKED(IDC_ADD_OBJECT, &CMapGeneratorDlg::OnBnClickedAddObject)
	ON_BN_CLICKED(IDC_RADIO_DOWN, &CMapGeneratorDlg::OnBnClickedRadioDown)
	ON_BN_CLICKED(IDC_RADIO_RIGHT, &CMapGeneratorDlg::OnBnClickedRadioRight)
	ON_BN_CLICKED(IDC_UPDATE_MAP_VIEW, &CMapGeneratorDlg::OnBnClickedUpdateMapView)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ZOOM_IN, &CMapGeneratorDlg::OnBnClickedZoomIn)
	ON_BN_CLICKED(IDC_ZOOM_OUT, &CMapGeneratorDlg::OnBnClickedZoomOut)
	ON_BN_CLICKED(IDC_LOAD_BUTTON, &CMapGeneratorDlg::OnBnClickedLoadButton)
END_MESSAGE_MAP()


// CMapGeneratorDlg message handlers

BOOL CMapGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_RadioDown.SetCheck(1);
	m_ObjectType.InsertString(0, "Normal Parking");
	m_ObjectType.InsertString(1, "Parallel Parking");
	m_ObjectType.InsertString(2, "Elevator");
	m_ObjectType.SetCurSel(0);

	m_FileName = "MapOutput.Bmp";
	m_NumberOfObjects = 1;
	UpdateData(FALSE);

	info.cbSize = sizeof(SCROLLINFO);
	info.fMask = SIF_ALL;
	info.nMin = 0;
	info.nPage =0;
	info.nPos = 0;
	info.nTrackPos=0;
	m_hbar.EnableWindow(false);
	m_vbar.EnableWindow(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMapGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMapGeneratorDlg::OnPaint()
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
HCURSOR CMapGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMapGeneratorDlg::OnBnClickedSaveButton()
{
	UpdateData();

	m_MapGenerator.SaveToBitmap(m_FileName);
}

void CMapGeneratorDlg::OnBnClickedMoreCheck()
{
	// TODO: Add your control notification handler code here
}

void CMapGeneratorDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
}

void CMapGeneratorDlg::OnBnClickedAddObject()
{
	UpdateData();

	int ObjectTypeIndex =  m_ObjectType.GetCurSel();
	if (ObjectTypeIndex == -1 || m_NumberOfObjects == 0)
		return;

	SLocation Location;
	Location.x = m_X;
	Location.y = m_Y;

	m_MapGenerator.AddObject((EMapObjectType)ObjectTypeIndex, Location, m_NumberOfObjects, m_Dup);
	
	// From here: Barcode Encoding \ Decoding Testing
	SStructureShape StructureShape(SStructureShape::None, SStructureShape::UShape());
	SMultiplicity	Multiplicity((m_NumberOfObjects > 1), m_NumberOfObjects);
	CItemStructure Structure;
	Structure.Encode(StructureShape, Multiplicity);

	IItem *ParkingItem = NULL;
	switch ((EMapObjectType)ObjectTypeIndex)
	{
	case NORMAL_PARKING:
		ParkingItem = new CParkingItem;
		((CParkingItem *)ParkingItem)->Encode(Perpendicular, Regular, false, false, Degrees0, Structure, false);
		break;
	case PARALLEL_PARKING:
		SPeriodicBetweenPoles Periodic(12, SPeriodicBetweenPoles::Circular);
		ParkingItem = new CParkingItem;
		((CParkingItem *)ParkingItem)->Encode(Parallel, Handicap, true, false, Degrees180, Structure, true, &Periodic);
		break;
	};

	std::vector<IItem *> ItemsInComplex;
	ItemsInComplex.push_back(ParkingItem);
	SReplication Rep(25, 5);

	CComplexItem Complex;
	Complex.Encode(ConvertIntToInt5Bit(3), ItemsInComplex, true, false, false, true, true, NULL, &Rep);


	int UsedBits = 0;
	Complex.Decode(Complex.GetBitBuffer(), UsedBits, NULL);

	UsedBits;
	int Context = 0;
	Complex.Decode(Complex.GetBitBuffer(), UsedBits, &Context);

	UsedBits = 0;
	ParkingItem->Decode(ParkingItem->GetBitBuffer(), UsedBits, NULL);

	/*if (ParkingItem)
		m_BarcodeEncoder.AddItem(ParkingItem);*/

	delete ParkingItem;
	OnBnClickedUpdateMapView();
}

void CMapGeneratorDlg::OnBnClickedRadioDown()
{
	m_Dup = DUPLICATE_DOWN;
}

void CMapGeneratorDlg::OnBnClickedRadioRight()
{
	m_Dup = DUPLICATE_RIGHT;
}

void CMapGeneratorDlg::OnBnClickedUpdateMapView()
{
	RECT ViewRect, WindowRect, RelativeRect;
	m_MapView.GetWindowRect(&ViewRect);
	GetWindowRect(&WindowRect);
	
	RelativeRect.top = ViewRect.top - WindowRect.top - 30;
	RelativeRect.left = ViewRect.left - WindowRect.left;

	RelativeRect.bottom = ViewRect.top - WindowRect.top + (ViewRect.bottom - ViewRect.top) - 30;
	RelativeRect.right = ViewRect.left - WindowRect.left + (ViewRect.right - ViewRect.left);
	
	m_MapGenerator.SquareImage();
	m_MapViewer.ShowMap(RelativeRect, *m_MapGenerator.GetMapObject());

	m_MapView.ShowWindow(SW_HIDE);

	UpdateScrolls();
}


afx_msg void CMapGeneratorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode)
	{
	case SB_TOP:
		sourcey = 0;
		break;
	case SB_BOTTOM:
		sourcey = INT_MAX;
		break;
	case SB_THUMBTRACK:
		sourcey= nPos;
		break;
	}	
	m_vbar.SetScrollPos(sourcey);
	RECT rc = m_MapViewer.GetCurrentView();
	m_MapViewer.ScrollDown(sourcey + (rc.bottom - rc.top)/2);
}

afx_msg void CMapGeneratorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
	switch (nSBCode)
	{
	case SB_TOP:
		sourcex = 0;
		break;
	case SB_BOTTOM:
		sourcex = INT_MAX;
		break;
	case SB_THUMBTRACK:
		sourcex= nPos;
		break;
	}	
	m_hbar.SetScrollPos(sourcex);
	RECT rc = m_MapViewer.GetCurrentView();
	m_MapViewer.ScrollRight(sourcex + (rc.right - rc.left)/2);
}
void CMapGeneratorDlg::OnBnClickedZoomIn()
{
	m_MapViewer.ZoomIn();
	UpdateScrolls();
}

void CMapGeneratorDlg::OnBnClickedZoomOut()
{
	m_MapViewer.ZoomOut();
	UpdateScrolls();
}

void CMapGeneratorDlg::OnBnClickedLoadButton()
{
	UpdateData();

	m_MapGenerator.LoadBitmap(m_LoadFileName);

	OnBnClickedUpdateMapView();
}

void CMapGeneratorDlg::UpdateScrolls(void)
{
	int MapWidth  = m_MapViewer.GetMapWidth();
	int MapHeight = m_MapViewer.GetMapHeight();

	RECT rc = m_MapViewer.GetCurrentView();

	info.nMax = MapWidth - (rc.right - rc.left);
	info.nPos = (rc.right + rc.left) / 2 - (rc.right - rc.left) / 2;
	m_hbar.SetScrollInfo(&info);
	info.nMax = MapHeight - (rc.bottom - rc.top);
	info.nPos = (rc.bottom + rc.top) / 2 - (rc.bottom - rc.top) / 2;
	m_vbar.SetScrollInfo(&info);

	if (m_MapViewer.IsInZoom())
	{
		m_hbar.EnableWindow(true);
		m_vbar.EnableWindow(true);
	}
	else
	{
		m_hbar.EnableWindow(false);
		m_vbar.EnableWindow(false);
	}
}