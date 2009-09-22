////////////////////////////////////////////////////////////////////////
// ListDisplay.h : implementation file
//

#include "stdafx.h"

#include "ListDisplay.h"
#include "SearchDlg.h"

//#define BaseClass CStatic
#define BaseClass CWnd

////////////////////////////////////////////////////////////////////////

CListDisplay::CListDisplay() :
    m_FirstLineIndex(0),
    m_CurrentIndex(-1),
    m_LinesPerPage(1),
    m_LineHeight(1),
    m_WindowWidth(0), m_WindowHeight(0),
    m_HorizontalExtent(1),
    m_HorizontalPos(0)
{
}

CListDisplay::~CListDisplay()
{
    if(m_SearchData.m_SearchDlg)
    {
        m_SearchData.m_SearchDlg->DestroyWindow();
        delete m_SearchData.m_SearchDlg;
    }
}

bool CListDisplay::Init(int MaxText, int MaxLines)
{
    if (!m_Text.Init(MaxText, MaxLines))
        return false;

    ResetContent();
    return true;
}

bool CListDisplay::Create(CWnd* Parent, int Id)
{
    DWORD Style = WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_BORDER/* | SS_NOTIFY*/;
    DWORD ExStyle = WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY;
    if (!BaseClass::CreateEx(ExStyle, NULL, "ListDisplay", Style, CRect(), Parent, Id))
        return false;

    ComputeLineHeight();
    return true;
}

bool CListDisplay::AddString(const char* Text)
{
    bool WasOnLast = m_CurrentIndex >= m_Text.GetLineCount() - 1;

    int DeletedLines = 0;
    m_Text.AddLine(Text, DeletedLines);
    m_FirstLineIndex -= DeletedLines;
    m_CurrentIndex   -= DeletedLines;

    if (m_FirstLineIndex < 0)
        ScrollLinesTo(0);
    if (m_CurrentIndex < 0)
        SetCurrentLine(0);

    UpdateVerticalScrollBarSize();

    int LineCount = m_Text.GetLineCount();
    if (WasOnLast)
        SetCurrentLine(LineCount - 1);
    else 
        InvalidateLine(LineCount - 1);

    return true;
}

void CListDisplay::SetHorizontalExtent(int Extent)
{
    m_HorizontalExtent = Extent;
    UpdateHorizontalScrollBarSize();
}

int CListDisplay::GetHorizontalExtent()
{
    return m_HorizontalExtent;
}

void CListDisplay::ResetContent()
{
    m_Text.Clear();

    m_FirstLineIndex = 0;
    m_CurrentIndex   = -1;

    if (m_hWnd != NULL)
        Invalidate();

    SetHorizontalExtent(0); 
    UpdateVerticalScrollBarSize();
}

void CListDisplay::Find(bool FindNext)
{
    if(FindNext)
        PerformSearch();
    else
    {
        if(m_SearchData.m_SearchDlg == NULL)
        {
            m_SearchData.m_SearchDlg = new CSearchDlg(this);
            m_SearchData.m_SearchDlg->Init();
        }
        m_SearchData.m_SearchDlg->Show();
    }
}

LRESULT CListDisplay::OnSearchTermReceived(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //m_SearchData.m_SearchDlg->ShowWindow(SW_HIDE);
    m_SearchData.m_CurrentSearchTerm = m_SearchData.m_SearchDlg->GetFindTerm();
    m_SearchData.m_IsCaseSensitive = m_SearchData.m_SearchDlg->GetIsCaseSensitive();
    PerformSearch();
    return 1;
}

inline int AreCharsEqual(const char CharA, const char CharB, bool CaseSensitive)
{
    if(CaseSensitive)
        return CharA == CharB;
    else
        return tolower(CharA) == tolower(CharB);
}

static bool WordExistsInLine(const char* Line, const char* SearchTerm, bool CaseSensitive)
{
    if(CaseSensitive)
        return (strstr(Line, SearchTerm) != NULL);

    const int Size = strlen(SearchTerm);
    const char* CurrentStartPos = Line;
    const int LineSize = strlen(Line);

    while(CurrentStartPos <= (Line + (LineSize - Size)))
    {
        int IndenticalChars = 0;
        for(; IndenticalChars < Size; ++IndenticalChars)
        {
            if(!AreCharsEqual(
                   *(CurrentStartPos + IndenticalChars), 
                   *(SearchTerm + IndenticalChars), 
                   CaseSensitive))
                break;
        }

        if(IndenticalChars == Size)
            return true;

        ++CurrentStartPos;
    }

    return false;
}

void CListDisplay::PerformSearch()
{
    if(m_SearchData.m_CurrentSearchTerm.empty())
        return;

    int LastIndex    = m_Text.GetLineCount();
	int CurrentIndex = m_CurrentIndex + 1; // Start search from next line

	while(CurrentIndex < LastIndex)
	{
		const char* Line = m_Text.GetLine(CurrentIndex);
		if(WordExistsInLine(Line, m_SearchData.m_CurrentSearchTerm.c_str(), m_SearchData.m_IsCaseSensitive))
		{
			SetCurrentLine(CurrentIndex);
			break;
		}

		++CurrentIndex;
	}
}

// Does not support variable height - so Index is ignored
int CListDisplay::SetItemHeight(int /*nIndex*/, UINT cyItemHeight)
{
    m_LineHeight = cyItemHeight;

    ComputeLinesPerPage();
    return 0;
}

BEGIN_MESSAGE_MAP(CListDisplay, BaseClass)
    //{{AFX_MSG_MAP(CListDisplay)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_LBUTTONDOWN()
    ON_WM_CHAR()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_MESSAGE(WM_SEARCH_REQUEST, OnSearchTermReceived)
    //}}AFX_MSG_MAP
    //ON_MESSAGE(QUEUE_NOTIFICATION_MSG, OnQueueMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListDisplay message handlers

void CListDisplay::PreSubclassWindow()
{
    CRect ClientRect;
    GetClientRect(&ClientRect);

    m_WindowWidth  = ClientRect.Width();
    m_WindowHeight = ClientRect.Height();

    ComputeLinesPerPage();

    m_Tooltip.Create(m_hWnd, this);
    m_Tooltip.SetDelays(1000);
}

BOOL CListDisplay::OnEraseBkgnd(CDC* /*pDC*/) 
{
//    return BaseClass::OnEraseBkgnd(pDC);
    return FALSE;
}

void CListDisplay::OnSize(UINT /*nType*/, int cx, int cy) 
{
    m_WindowWidth  = cx;
    m_WindowHeight = cy;
    ComputeLinesPerPage();
    UpdateVerticalScrollBarSize();

    UpdateHorizontalScrollBarSize();
//    Invalidate();
    SetCurrentLine(m_CurrentIndex, true);
}

void CListDisplay::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect ClientRect;
    GetClientRect(&ClientRect);

    dc.IntersectClipRect(&ClientRect);

//    dc.FillSolidRect(ClientRect, RGB(255, 0, 0));

    int BottomLine = m_FirstLineIndex + m_LinesPerPage;
    int LastDrawnLine = BottomLine;
    if (BottomLine > m_Text.GetLineCount())
        BottomLine = m_Text.GetLineCount();
    else if (BottomLine < m_Text.GetLineCount()) // Draw one extra line
        ++BottomLine;                            // 

    CFont* prevFont = NULL;
    CFont* Font = GetFont();
    if (Font != NULL)
        prevFont = dc.SelectObject(Font);

    // Fill the text lines
    for (int LineIndex = m_FirstLineIndex; LineIndex < BottomLine; ++LineIndex)
    {
        CRect Rect = GetLineRect(LineIndex);
        const char* Text = m_Text.GetLine(LineIndex);

        bool IsCurrent = LineIndex == m_CurrentIndex;
        bool HasFocus  = IsCurrent && GetFocus() == this;

        DrawLine(dc, Text, Rect, HasFocus, IsCurrent);
    }

    // fill with empty lines
     for (int LineIndex = BottomLine; LineIndex <= LastDrawnLine; ++LineIndex)
         DrawEmptyLine(dc, LineIndex);

    if (prevFont != NULL)
        dc.SelectObject(prevFont);
}

void CListDisplay::OnKillFocus(CWnd* /*pNewWnd*/)
{
    //BaseClass::OnKillFocus(pNewWnd);

    InvalidateCurrentLine();
}

void CListDisplay::OnSetFocus(CWnd* /*pOldWnd*/)
{
    //BaseClass::OnSetFocus(pOldWnd);

    InvalidateCurrentLine();
}

void CListDisplay::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
    SetFocus();

    int LineIndex = GetLineIndexFromPoint(point);
    SetCurrentLine(LineIndex);

    //BaseClass::OnLButtonDown(nFlags, point);
}

UINT CListDisplay::OnGetDlgCode()
{
    return DLGC_WANTARROWS | DLGC_WANTCHARS;
}

void CListDisplay::HandleChar(UINT Char)
{
    int NewIndex = m_CurrentIndex;
    bool SetNewIndex = false;

    switch (Char)
    {
    case VK_END:
        NewIndex = m_Text.GetLineCount() - 1;
        SetNewIndex = true;
        break;
    case VK_HOME:
        NewIndex = 0;
        SetNewIndex = true;
        break;
    case VK_UP:
        --NewIndex;
        SetNewIndex = true;
        break;
    case VK_DOWN:
        ++NewIndex;
        SetNewIndex = true;
        break;
    case VK_LEFT:
        m_HorizontalPos -= 6;
        UpdateHorizontalPos();
        break;
    case VK_RIGHT:
        m_HorizontalPos += 6;
        UpdateHorizontalPos();
        break;
    case VK_PRIOR:
        NewIndex -= m_LinesPerPage - 1;
        SetNewIndex = true;
        break;
    case VK_NEXT:
        NewIndex += m_LinesPerPage - 1;
        SetNewIndex = true;
        break;
    case 'C':
        if (GetKeyState(VK_CONTROL) & 0x8000)
            CopyCurrentLineToClipBoard();
        break;
    case 'X':
        if (GetKeyState(VK_CONTROL) & 0x8000)
            ResetContent();
        break;
    case 'F':
        if (GetKeyState(VK_CONTROL) & 0x8000)
            Find(false /* FindNext */);
        break;
    case VK_F3:
        Find(true /* FindNext */);
        break;
    default:
        //TRACE("CListDisplay::HandleChar: Got %d\n", Char);
        break;
    }

    if(SetNewIndex)
        SetCurrentLine(NewIndex);
    UpdateVerticalScrollBarPos();
}

void CListDisplay::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    HandleChar(nChar);

    BaseClass::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CListDisplay::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    HandleChar(nChar);

    BaseClass::OnChar(nChar, nRepCnt, nFlags);
}

void CListDisplay::OnTimer(UINT nIDEvent) 
{
    BaseClass::OnTimer(nIDEvent);
}

void CListDisplay::ScrollHelper(
    UINT nSBCode, UINT nPos, int& Current, int Page, int Line, int Max)
{
    switch (nSBCode)
    {
    case SB_TOP:
        Current = 0;
        break;
    case SB_BOTTOM:
        Current = Max - Page;
        break;
    case SB_LINEDOWN:
        Current += Line;
        break;
    case SB_LINEUP:
        Current -= Line;
        break;
    case SB_PAGEDOWN:
        Current += Page - Line;
        break;
    case SB_PAGEUP:
        Current -= Page - Line;
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        Current = nPos;
        break;
    case SB_ENDSCROLL:
    default:
        break;
    }
}

void CListDisplay::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    int FirstLineIndex = m_FirstLineIndex;

    ScrollHelper(
        nSBCode, nPos, FirstLineIndex, m_LinesPerPage, 1, m_Text.GetLineCount());

    ScrollLinesTo(FirstLineIndex);

    BaseClass::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CListDisplay::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/)
{
    const int PixelsPerLine = 6;

    ScrollHelper(
        nSBCode, nPos, m_HorizontalPos, m_WindowWidth, PixelsPerLine, m_HorizontalExtent);

    UpdateHorizontalPos();
}

void CListDisplay::UpdateHorizontalPos()
{
    AdjustPos(m_HorizontalPos, m_WindowWidth, m_HorizontalExtent);

    UpdateHorizontalScrollBarPos();
    Invalidate();
}

BOOL CListDisplay::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
//    TRACE("CListDisplay::OnMouseWheel: zDelta %d\n", zDelta);
    const int LinesPerWheel = 3;
    int Delta = zDelta > 0 ? -LinesPerWheel : LinesPerWheel;

    ScrollLinesTo(m_FirstLineIndex + Delta);

    return BaseClass::OnMouseWheel(fFlags, zDelta, point);
}

/////////////////////////////////////////////////////////////////////////////
// CListDisplay private methods

void CListDisplay::UpdateScrollInfo(int Bar, int Page, int Pos, int Max)
{
    SCROLLINFO ScrollInfo;

    ScrollInfo.cbSize = sizeof ScrollInfo;
    ScrollInfo.fMask  = SIF_ALL;
    GetScrollInfo(Bar, &ScrollInfo);

    if (Page > Max && m_hWnd != NULL)
    {
        ScrollLinesTo(m_FirstLineIndex);
        ShowScrollBar(Bar, FALSE);
    }

//    TRACE("CListDisplay::UpdateScrollInfo: Max %d, Page %d, Pos %d\n",
//        Max, Page, Pos);

    ScrollInfo.nMin  = 0;
    ScrollInfo.nMax  = Max - 1;
    ScrollInfo.nPage = Page;
    ScrollInfo.nPos  = Pos;
    ScrollInfo.nTrackPos = Pos;
    SetScrollInfo(Bar, &ScrollInfo);
}

void CListDisplay::UpdateVerticalScrollBarSize()
{
    UpdateScrollInfo(SB_VERT, m_LinesPerPage, m_FirstLineIndex, m_Text.GetLineCount());
}

void CListDisplay::UpdateVerticalScrollBarPos()
{
    SetScrollPos(SB_VERT, m_FirstLineIndex);
}

void CListDisplay::UpdateHorizontalScrollBarSize()
{
//    TRACE("CListDisplay::UpdateHorizontalScrollBarSize: Width %d, Pos %d, Extent %d\n", 
//        m_WindowWidth, m_HorizontalPos, m_HorizontalExtent);

    if (m_HorizontalPos + m_WindowWidth > m_HorizontalExtent)
    {
        int NewHorizontalPos = m_HorizontalExtent - m_WindowWidth;
        if (NewHorizontalPos < 0)
            NewHorizontalPos = 0;
        if (NewHorizontalPos != m_HorizontalPos)
            Invalidate();
        m_HorizontalPos = NewHorizontalPos;
    }
    if (m_WindowWidth != 0)
        UpdateScrollInfo(SB_HORZ, m_WindowWidth, m_HorizontalPos, m_HorizontalExtent);
}

void CListDisplay::UpdateHorizontalScrollBarPos()
{
    SetScrollPos(SB_HORZ, m_HorizontalPos);
}

void CListDisplay::ComputeLineHeight()
{
    CDC* pDC = GetDC();

    CFont* Font = GetFont();
    CFont* prevFont = NULL;
    if (Font != NULL)
        prevFont = pDC->SelectObject(Font);

    CSize Size = pDC->GetTextExtent("1234");
    m_LineHeight = Size.cy;

    if (prevFont != NULL)
        pDC->SelectObject(prevFont);
    ReleaseDC(pDC);
}

int CListDisplay::ComputeTextWidth(const std::string& Text)
{
    CDC* pDC = GetDC();

    CFont* Font = GetFont();
    CFont* prevFont = NULL;
    if (Font != NULL)
        prevFont = pDC->SelectObject(Font);

    CSize Size = pDC->GetTextExtent(Text.c_str());
    int TextWidth = Size.cx;

    if (prevFont != NULL)
        pDC->SelectObject(prevFont);
    ReleaseDC(pDC);

    return TextWidth;
}

void CListDisplay::ComputeLinesPerPage()
{
    m_LinesPerPage = m_WindowHeight / m_LineHeight;
}

void CListDisplay::InvalidateLine(int LineIndex)
{
    if (LineIndex >= m_FirstLineIndex && LineIndex < m_FirstLineIndex + m_LinesPerPage + 1) // the + 1 is for the last partial line
    {
        CRect Rect = GetLineRect(LineIndex);
        InvalidateRect(Rect);
    }
}

void CListDisplay::InvalidateCurrentLine()
{
    InvalidateLine(m_CurrentIndex);
}

void CListDisplay::SetCurrentLine(int Index, bool Force /*= false*/)
{
    if (Index < 0)
        Index = 0;
    if (Index >= m_Text.GetLineCount())
        Index = m_Text.GetLineCount() - 1;

    if (Index != m_CurrentIndex  ||  Force)
    {
        InvalidateCurrentLine();
        m_CurrentIndex = Index;
        InvalidateCurrentLine();

        int NewFirstLineIndex = m_FirstLineIndex;
        if (m_CurrentIndex < m_FirstLineIndex)
            NewFirstLineIndex = m_CurrentIndex; // try to set it as first line
        else if (m_CurrentIndex >= m_FirstLineIndex + m_LinesPerPage)
            NewFirstLineIndex = m_CurrentIndex - m_LinesPerPage + 1; // try to set it as last line
        if (NewFirstLineIndex != m_FirstLineIndex  ||  Force)
            ScrollLinesTo(NewFirstLineIndex);
    }
}

void CListDisplay::DrawLine(
    CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent, COLORREF TextColor)
{
    COLORREF newBkColor   = GetSysColor(COLOR_WINDOW);
    COLORREF oldBkColor   = Dc.GetBkColor();

    COLORREF newTextColor = TextColor;
    COLORREF oldTextColor = Dc.GetTextColor();

    if (HasFocus)
    {
        newBkColor   = GetSysColor(COLOR_HIGHLIGHT);
        newTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
    }
    else if (IsCurrent)
        newBkColor   = GetSysColor(COLOR_3DFACE);

    Dc.SetTextColor(newTextColor);
    Dc.SetBkColor(newBkColor);

    CBrush BkBrush(newBkColor);
    Dc.FillRect(Rect, &BkBrush);

    const int Margin = 2;
    CRect TextRect = Rect;
    TextRect.DeflateRect(Margin, 0);
    Dc.SetBkMode(TRANSPARENT);
    Dc.DrawText(Text, strlen(Text), TextRect, DT_LEFT | DT_NOPREFIX);

//     if (HasFocus)
//     {
//         CRect FocusRect = Rect;;
//         FocusRect.right++;
//         Dc.DrawFocusRect(FocusRect);
//     }
    if (HasFocus)
        Dc.DrawFocusRect(Rect);

    // Compute width

    Dc.SetBkColor(oldBkColor);
    Dc.SetTextColor(oldTextColor);
}

void CListDisplay::DrawLine(CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent)
{
    DrawLine(Dc, Text, Rect, HasFocus, IsCurrent, GetSysColor(COLOR_WINDOWTEXT));
}

void CListDisplay::DrawEmptyLine(CDC& Dc, int LineIndex)
{
    CRect Rect = GetLineRect(LineIndex);

    COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
    COLORREF oldBkColor = Dc.SetBkColor(newBkColor);

    CBrush BkBrush(newBkColor);
    Dc.FillRect(Rect, &BkBrush);

    Dc.SetBkColor(oldBkColor);
}

void CListDisplay::AdjustPos(int& Pos, int Page, int Max)
{
    if (Pos + Page > Max)
        Pos = Max - Page;
    if (Pos < 0)
        Pos = 0;
}

void CListDisplay::ScrollLinesTo(int NewFirstLineIndex)
{
    AdjustPos(NewFirstLineIndex, m_LinesPerPage, m_Text.GetLineCount());
    int Delta = m_FirstLineIndex - NewFirstLineIndex;
    if (Delta == 0)
        return;

	m_Tooltip.Hide();

//     if (abs(Delta) < m_LinesPerPage / 2)
//     {
//         CRect ScrollRect(0, 0, m_WindowWidth, m_WindowHeight);
//         ScrollWindowEx(0, Delta * m_LineHeight, &ScrollRect, &ScrollRect, NULL, NULL, SW_INVALIDATE);
//     }
//     else
    Invalidate();

    m_FirstLineIndex = NewFirstLineIndex;
    UpdateVerticalScrollBarPos();
}

// void CListDisplay::ScrollLinesBy(int Lines)
// {
//     int NewFirstLineIndex = m_FirstLineIndex + Lines;
//     ScrollLinesTo(NewFirstLineIndex, /*UpdateScrollBar=*/true);
// }

void CListDisplay::CopyTextToClipBoard(const char* Text)
{
    if (!OpenClipboard())
        return;

    EmptyClipboard();

	size_t TextSize = strlen(Text) + 1;
    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE |GMEM_DDESHARE, TextSize);
    char *pText = (char *)GlobalLock(hText);
    strncpy_s(pText, TextSize, Text, _TRUNCATE);
    GlobalUnlock(hText);

    ::SetClipboardData(CF_TEXT, hText);

     CloseClipboard();
     GlobalFree(hText);
}

void CListDisplay::Copy()
{
    CopyCurrentLineToClipBoard();
}

void CListDisplay::CopyCurrentLineToClipBoard()
{
    if (m_CurrentIndex >= 0 && m_CurrentIndex < m_Text.GetLineCount())
        CopyTextToClipBoard(m_Text.GetLine(m_CurrentIndex));
}

int CListDisplay::GetLineIndexFromPoint(const CPoint& Point)
{
    int Index = Point.y / m_LineHeight;
    return Index + m_FirstLineIndex;
}

CRect CListDisplay::GetLineRect(int Index)
{
    Index -= m_FirstLineIndex;
    CRect Rect;
    Rect.top    = Index * m_LineHeight;
    Rect.bottom = Rect.top + m_LineHeight;
    Rect.left   = -m_HorizontalPos;
    Rect.right  = Rect.left + max(m_WindowWidth, m_HorizontalExtent);

    return Rect;
}

const TCHAR* CListDisplay::GetToolTipText(const POINT& Point)
{
    int Index = GetLineIndexFromPoint(Point);
    int LastIndex = m_Text.GetLineCount();

    if(Index >= LastIndex)
        return NULL;

    m_CurrentToolTipStr = (m_Text.GetLine(Index) + 1); // Ignore LogLevel int
    BreakTooltipLineIfNeeded();

    return m_CurrentToolTipStr.c_str();
}

void CListDisplay::BreakTooltipLineIfNeeded()
{
    int LineWidth = ComputeTextWidth(m_CurrentToolTipStr);
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);

    if(LineWidth <= ScreenWidth)
        return;

    int Factor = (LineWidth + (ScreenWidth - 1)) / ScreenWidth;
    int Skip   = m_CurrentToolTipStr.size() / Factor;

    for(int i = 1; i < Factor; ++i)
    {
        int StartPos = m_CurrentToolTipStr.size() - (Skip * i);
        int SpaceChar = m_CurrentToolTipStr.rfind(' ', StartPos);

        if(SpaceChar == -1)
            break;

        m_CurrentToolTipStr.insert(SpaceChar, "\n");
    }

}

LRESULT CListDisplay::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        m_Tooltip.RelayEvent(message, wParam, lParam);
    }

    return CWnd::WindowProc(message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////

#ifdef _TEST

#include "Common/Utils.h"

void TestListDisplay()
{
    CWnd* MainWnd = AfxGetMainWnd();
#if 01
    CListDisplay List;

    const int MaxLines = 5000;
    const int AverageLineLength = 256;
    List.Init(MaxLines * AverageLineLength, MaxLines);

    List.Create(MainWnd, -1);
#else
    CListBox List;

    DWORD Style = WS_TABSTOP | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE | WS_BORDER/* | SS_NOTIFY*/;
    List.Create(Style, CRect(), MainWnd, (UINT)-1);
#endif

    CRect MainClientRect;
    MainWnd->GetClientRect(MainClientRect);

    MainClientRect.left = 250;
    List.MoveWindow(MainClientRect);

    const int Count = 250;
    for (int i = 0; i < Count; ++i)
    {
        char Buffer[128];
        sprintf_s(Buffer, "Line Number %d - Line Number %d - Line Number %d - Line Number %d", i, i, i, i);
        List.AddString(Buffer);
    }
    List.SetHorizontalExtent(1000);

    WaitAndPumpMessages(60000);
    List.ResetContent();
    WaitAndPumpMessages(10000);
}


#endif
