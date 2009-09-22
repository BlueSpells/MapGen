#include "stdafx.h"
#include "CustomToolTip.h"

#ifdef _X_DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT DefaultShowDelay    =  500; // milliseconds
static const UINT DefaultHideDelay    = 3000; // milliseconds
static const UINT LineMargin          =    2;
static const UINT LeftRightMargin     =    4;
static const UINT LeftRightSmallMargin=    2;

static const UINT OffsetBelowCursor   =   20;
static const POINT NullPoint= { 0, 0 };


// --------------------------------------------------------------------------
static UINT GetLineSize(const TCHAR* Text)
{
    const TCHAR* End= _tcschr(Text, '\n');
    if (End != NULL)
        return End - Text;
    else
        return _tcslen(Text);
}

// ==========================================================================
// CCustomToolTip
// ==========================================================================

CCustomToolTip::CCustomToolTip() :
    m_CustomToolTipHelper(NULL),
    m_ShowDelay(DefaultShowDelay), m_HideDelay(DefaultHideDelay),
    m_hOwnerWnd(NULL),
    m_LastCursorPos(NullPoint),
    m_LastMoveTime(0),
    m_LastShowTime(0)
{
    // get the status/tooltip font
    NONCLIENTMETRICS NCInfo;
    NCInfo.cbSize= sizeof(NCInfo);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NCInfo), &NCInfo, 0);
    m_hFont= ::CreateFontIndirect(&NCInfo.lfStatusFont);
}

// --------------------------------------------------------------------------
CCustomToolTip::~CCustomToolTip()
{
    ::DeleteObject(m_hFont);
    DestroyWindow();
}

// --------------------------------------------------------------------------
bool CCustomToolTip::Create(HWND hOwnerWnd, ICustomToolTipHelper* CustomToolTipHelper)
{
//    CString ClassName = AfxRegisterWndClass(CS_SAVEBITS,
//        LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_INFOBK + 1), NULL);

    CString ClassName = AfxRegisterWndClass(CS_SAVEBITS);

    const CString WindowName = "Custom Tool Tip";

    // Style must be WS_EX_TOOLWINDOW in order to not appear in task bar
    if (CreateEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, ClassName, WindowName,
        WS_POPUP | WS_BORDER, 0, 0, 0, 0, NULL/*ParentWnd*/, NULL))
    {
        const int Timeout = 100;
        SetTimer(0, Timeout, NULL);

        m_hOwnerWnd = hOwnerWnd;
        m_CustomToolTipHelper = CustomToolTipHelper;
        return true;
    }
    else
        return false;
}

// --------------------------------------------------------------------------
// set Delay for showing and hiding (HideDelay 0 means infinite)
void CCustomToolTip::SetDelays(UINT ShowDelay, UINT HideDelay/* = 0*/)
{
    m_ShowDelay= ShowDelay;
    m_HideDelay= HideDelay;
}

// --------------------------------------------------------------------------
// to be called by client window and send mouse events messages
// (WM_MOUSEMOVE  WM_LBUTTONDOWN WM_RBUTTONDOWN)
void CCustomToolTip::RelayEvent(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
    if (uMsg == WM_SETCURSOR)
        uMsg= HIWORD(lParam);
    
    switch(uMsg)
    {
    case WM_MOUSEMOVE:
        OnMouseMove();
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        Hide();
        break;
    }
}

// --------------------------------------------------------------------------
// The CustomToolTip can be used manually:
// In order to do so, there is no need to pass the helper pointer on Create
// just call the following Show and Hide methods
// --------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CCustomToolTip, CWnd)
    //{{AFX_MSG_MAP(CCustomToolTip)
    ON_WM_PAINT()
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// --------------------------------------------------------------------------
void CCustomToolTip::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    HDC hDC = dc.m_hDC;

    RECT ClientRect;
    ::GetClientRect(m_hWnd, &ClientRect);

    Draw(hDC, ClientRect, m_Text.c_str(), m_BkColor, /*DrawFrame=*/false);
}

void CCustomToolTip::ComputeSize(HDC hDC, const TCHAR* Text, int& Width, int& Height)
{
    Width = Height = 0;

    HFONT hPrevFont= (HFONT)::SelectObject(hDC, m_hFont);
    for (const TCHAR* Current= Text; *Current != '\0';)
    {
        UINT LineSize = GetLineSize(Current);

        SIZE TextSize;
        GetTextExtentPoint32(hDC, Current, LineSize, &TextSize);
        if (TextSize.cx > Width)
            Width = TextSize.cx;

        Height += TextSize.cy + LineMargin;

        Current += LineSize;
        if (*Current == '\n')
            ++Current;
    }
    int Margin = m_SmallerFont ? LeftRightSmallMargin : LeftRightMargin;
    Width += 2 * Margin;

    ::SelectObject(hDC, hPrevFont);
}

void CCustomToolTip::Draw(HDC hDC, const RECT& Rect, const TCHAR* Text, COLORREF BkColor, bool DrawFrame)
{
    HFONT    hPrevFont     = (HFONT)::SelectObject(hDC, m_hFont);
    COLORREF TextColor     = GetSysColor(COLOR_INFOTEXT);
    COLORREF PrevTextColor = ::SetTextColor(hDC, TextColor);
    int      PrevBkMode    = SetBkMode(hDC, TRANSPARENT);

    FillRect(hDC, Rect, BkColor);

    int Width = RectWidth(Rect);

    SIZE TextSize;
    GetTextExtentPoint32(hDC, Text, 1, &TextSize);

    int PlaceY = 0;
    int Margin = m_SmallerFont ? LeftRightSmallMargin : LeftRightMargin;
    for (const TCHAR* Current= Text; *Current != '\0';)
    {
        int LineSize= GetLineSize(Current);

        CRect DrawRect(
            Rect.left + Margin, Rect.top + PlaceY, 
            Rect.left + Width, Rect.top + PlaceY + TextSize.cy);
        ::DrawText(hDC, Current, LineSize, DrawRect, DT_SINGLELINE | DT_VCENTER | DT_LEFT);

        PlaceY += TextSize.cy + LineMargin;

        Current += LineSize;
        if (*Current == '\n')
            ++Current;
    }
    //dc.SetTextAlign( PreviousTextAlign );

    if (DrawFrame)
        FrameRect(hDC, Rect);

    SetBkMode(hDC, PrevBkMode);
    SetTextColor(hDC, PrevTextColor);
    SelectObject(hDC, hPrevFont);
}

void CCustomToolTip::FrameRect(HDC hDC, const RECT& Rect, COLORREF Color)
{
    // Draw a frame of a rectangle
    HBRUSH hBrush = ::CreateSolidBrush(Color);
    ::FrameRect(hDC, &Rect, hBrush);
    ::DeleteObject(hBrush);
}

void CCustomToolTip::FrameRect(HDC hDC, const RECT& Rect)
{
    // Draw a frame with default color
    COLORREF Color = ::GetDCPenColor(hDC);
    FrameRect(hDC, Rect, Color);
}

void CCustomToolTip::FillRect(HDC hDC, const RECT& Rect, COLORREF Color)
{
    // Draw a full Rect in the DC with the requested color.
    ::SetBkColor(hDC, Color);
    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &Rect, NULL, 0, NULL);
}

int CCustomToolTip::RectWidth(const RECT& Rect)
{
    // Return rect dimensions
    return Rect.right - Rect.left;
}

// --------------------------------------------------------------------------
static bool operator==(const POINT& Point1, const POINT& Point2)
{
    return Point1.x == Point2.x && Point1.y == Point2.y;
}

// --------------------------------------------------------------------------
static bool operator!=(const POINT& Point1, const POINT& Point2)
{
    return !operator==(Point1, Point2);
}

// --------------------------------------------------------------------------
void CCustomToolTip::OnTimer(UINT /*nIDEvent*/)
{
    if (!m_CustomToolTipHelper)
        return;

    POINT CursorPos       = GetCursorPos();
    BOOL  Moved           = m_LastCursorPos != CursorPos;
    BOOL  Active          = IsOwnerActive();
    DWORD CurrentTickCount= GetTickCount();

    if (::IsWindowVisible(m_hWnd))
    {
        if (!Active || Moved || (m_HideDelay > 0 &&
                (int)(CurrentTickCount - m_LastShowTime) > m_HideDelay))
            Hide();
    }
    else if (Active && !Moved && m_LastMoveTime != 0 &&
        (int)(CurrentTickCount - m_LastMoveTime) > m_ShowDelay)
    {
        POINT CursorPosOnClient = CursorPos;
        ::ScreenToClient(m_hOwnerWnd, &CursorPosOnClient);
        const TCHAR* pText = m_CustomToolTipHelper->GetToolTipText(CursorPosOnClient);
        if(pText)
        {
            m_Text = pText;
            m_BkColor = GetSysColor(COLOR_INFOBK);
            SizeAndShow(CursorPos.x, CursorPos.y + OffsetBelowCursor, LEFT_TOP);
        }
        m_LastMoveTime = CurrentTickCount;
        m_LastShowTime = CurrentTickCount;
    }
}

// --------------------------------------------------------------------------
POINT CCustomToolTip::GetCursorPos()
{
    POINT CursorPos;
    ::GetCursorPos(&CursorPos);
    return CursorPos;
}

// --------------------------------------------------------------------------
static HWND GetParentOwner(HWND hWnd)
{
    // otherwise, return parent in the Windows sense
    return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) ?
        ::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
}

// --------------------------------------------------------------------------
static HWND GetTopLevelParent(HWND hWnd)
{
    HWND hWndParent = hWnd;
    HWND hWndT;
    while ((hWndT = GetParentOwner(hWndParent)) != NULL)
        hWndParent = hWndT;

    return hWndParent;
}

// --------------------------------------------------------------------------
bool CCustomToolTip::IsOwnerActive()
{
    // Logic is stolen from MFC - IsTopParentActive
    HWND hTopLevel= ::GetTopLevelParent(m_hOwnerWnd);
    return ::GetForegroundWindow() == ::GetLastActivePopup(hTopLevel);
}

// --------------------------------------------------------------------------
void CCustomToolTip::OnMouseMove()
{
    POINT CursorPos= GetCursorPos();
    if (CursorPos == m_LastCursorPos)
        return;

    if (!::IsWindowVisible(m_hWnd))
        m_LastCursorPos = CursorPos;
    m_LastMoveTime = GetTickCount();
}

// --------------------------------------------------------------------------
void CCustomToolTip::ComputeSize(int& Width, int& Height)
{
    // Resize window according to text
    HDC hDC = ::GetDC(m_hWnd);

    ComputeSize(hDC, m_Text.c_str(), Width, Height);
    ::ReleaseDC(m_hWnd, hDC);
}

// --------------------------------------------------------------------------
void CCustomToolTip::AdjustOrigin(int& Left, int& Top, AttachSide Side, int Width, int Height)
{
    switch (Side)
    {
    case LEFT_TOP:
    default:
        break;
    case TOP:
        Left -= Width / 2;
        break;
    case RIGHT:
        Left -= Width;
        Top  -= Height / 2;
        break;
    }
}

// --------------------------------------------------------------------------
void CCustomToolTip::SizeAndShow(int Left, int Top, AttachSide Side)
{
    int Width, Height;
    ComputeSize(Width, Height);
    if (Width == 0 || Height == 0)
        return;

    AdjustOrigin(Left, Top, Side, Width, Height);

    // Make sure Window is on screen
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (Left + Width > ScreenWidth)
        Left = ScreenWidth - Width;
    if (Top + Height > ScreenHeight)
        Top -= Height; // we move message up

    ::MoveWindow(m_hWnd, Left, Top, Width, Height, TRUE/*!bRepaint*/);
    ShowWindow(SW_SHOWNA);
}

void CCustomToolTip::Hide()
{
    ShowWindow(SW_HIDE);
    m_LastMoveTime= 0;
}
