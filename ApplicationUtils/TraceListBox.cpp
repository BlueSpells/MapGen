// TraceListBox.cpp : implementation file
//

#include "stdafx.h"

#include "TraceListBox.h"
#include "Common/LogEvent.h"
#include "Common/Utils.h"
#ifdef USE_LIST_DISPLAY
#define BaseClass CListDisplay
#else
#define BaseClass CListBox
#endif

enum { 
    MAX_TRACE_LINES = 5000,
    AVERAGE_TRACE_LINE_SIZE = 256
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

enum 
{ 
    MAX_MESSAGES_IN_QUEUE = 1000, 
    MAX_TEXT_SIZE = 1024,

    QUEUE_NOTIFICATION_MSG = WM_USER + 100
};

struct CTextMsg
{
public:
    CTextMsg() : m_Severity(LE_DEBUG)
    {
    }

    CTextMsg(ELogSeverity Severity, const char* Text) : m_Severity(Severity)
    {
        GetSecondsAndMillis(m_Seconds, m_Millis);
        StrCopy(m_Text, Text, sizeof m_Text);
    }

    ELogSeverity m_Severity;
    time_t       m_Seconds;
    int          m_Millis;
    char         m_Text[MAX_TEXT_SIZE + 1];
};

/////////////////////////////////////////////////////////////////////////////
// CTraceListBox

//enum 
//{ 
//    MAX_QUEUE_SIZE = MAX_MESSAGES_IN_QUEUE * sizeof CTextMsg,
//};

static const char* TraceQueueName = "TraceQueue";

CTraceListBox::CTraceListBox() : 
//    m_Queue(sizeof CTextMsg, MAX_MESSAGES_IN_QUEUE),
//    m_QueueIn(MAX_QUEUE_SIZE, TraceQueueName),
//    m_QueueOut(TraceQueueName),
//    m_CurrentExtent(0),
    m_Severity(LE_INFOLOW)
{
    m_Queue.SetQueueSize(2000);
    InitListData();
}

CTraceListBox::~CTraceListBox()
{
}

void CTraceListBox::SetLogLevel(ELogSeverity Severity)
{
    m_Severity = Severity;
}

ELogSeverity CTraceListBox::GetLogLevel()
{
    return m_Severity;
}

bool CTraceListBox::AddMessage(ELogSeverity Severity, const char* Text)
{
    // Mask out less important severity 
    if (Severity < m_Severity)
        return false;

    if (m_hWnd == NULL)
        return false;

//    CTextMsg TextMsg(Severity, Text);

//    TRACE("CTraceListBox::AddMessage - %d [%s]\n", Severity, Text);
//    bool Result = m_Queue.Write(&TextMsg, sizeof TextMsg);
    TraceQueueItem Item;
    Item.Msg = Text;
    Item.Severity = Severity;
    bool Result = m_Queue.Push(Item);
    if (Result)
    {
        PostMessage(QUEUE_NOTIFICATION_MSG);

        // Clean Lost messages counter
        long Lost = InterlockedExchange(&m_LostMessages, 0);
        if (Lost > 0)
            PrivateLogEvent(LE_ERROR, 
                "CTraceListBox::AddMessage - Lost %d messages", Lost);

        return true;
    }
    else
    {
        InterlockedIncrement(&m_LostMessages);

        return false;
    }
}

void CTraceListBox::Clear()
{
    ResetContent();
//    SetHorizontalExtent(0);
}

BEGIN_MESSAGE_MAP(CTraceListBox, BaseClass)
    //{{AFX_MSG_MAP(CTraceListBox)
    ON_WM_CREATE()
	ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
    ON_MESSAGE(QUEUE_NOTIFICATION_MSG, OnQueueMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTraceListBox message handlers

int CTraceListBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (BaseClass::OnCreate(lpCreateStruct) == -1)
        return -1;

//    if (!CreateQueue())
//        return -1;

    return 0;
}

BOOL CTraceListBox::OnEraseBkgnd(CDC* pDC) 
{
#ifdef USE_LIST_DISPLAY
    return BaseClass::OnEraseBkgnd(pDC);
#else
    CRect ClientRect;
    GetClientRect(&ClientRect);

    // Try to find out if there is an empty space below items - only
    // in this case draw background
    BOOL Outside;
    CPoint Point(0, ClientRect.bottom - 1);
    ItemFromPoint(Point, Outside);
    if (Outside)
    	return BaseClass::OnEraseBkgnd(pDC);
    else
	    return FALSE;
#endif
}

void CTraceListBox::PreSubclassWindow() 
{
    const int Margins = 2;
    int Height = GetFontHeight();
    SetItemHeight(0, Height+Margins);

    BaseClass::PreSubclassWindow();
}

LRESULT CTraceListBox::OnQueueMessage(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    enum { ITEMS_TO_DISPLAY = 3 };
    for(int i = 0; i < ITEMS_TO_DISPLAY; ++i)
    {
        TraceQueueItem Item;
        if(m_Queue.Pop(Item))
            DisplayMessage(Item.Severity, Item.Msg.c_str());
//        CTextMsg TextMsg;
//        int Actual = m_Queue.Read(&TextMsg, sizeof TextMsg, 0/*No Wait*/);
//            
//        if (Actual != sizeof TextMsg)
//        {
//            if (Actual != 0) // 0 means no message - so we don't complain
//                PrivateLogEvent(LE_ERROR, 
//                    "CTraceListBox::OnQueueMessage - Error getting message (%d != %d)",
//                    Actual, sizeof TextMsg);
//            return 0;
//        }
//
//        DisplayMessage(TextMsg.m_Severity, TextMsg.m_Seconds, TextMsg.m_Millis, TextMsg.m_Text);
    }
    return 0;
}
#ifdef USE_LIST_DISPLAY
void CTraceListBox::DrawLine(
    CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent)
{
    ELogSeverity Severity = String2Severity(Text);
    COLORREF TextColor = GetColor(Severity);

    BaseClass::DrawLine(Dc, Text + 1, Rect, HasFocus, IsCurrent, TextColor);

    const int Margin = 2;
    CSize Size = Dc.GetTextExtent(Text + 1);
    int Width = Size.cx + 2 * Margin;
    if (Width > GetHorizontalExtent())
        SetHorizontalExtent(Width);
}

#else

void CTraceListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    int Index = lpDrawItemStruct->itemID;
    if (Index < 0)
        return;

    // You must override DrawItem and MeasureItem for LBS_OWNERDRAWVARIABLE
    Assert((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS)) ==
        (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS));

    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    CString Text;
    GetText(Index, Text);

    ELogSeverity Severity = String2Severity(Text);

    //COLORREF newTextColor = GetSysColor(COLOR_WINDOWTEXT);
    COLORREF newTextColor = GetColor(Severity);
    COLORREF oldTextColor = dc.SetTextColor(newTextColor);

    COLORREF newBkColor = GetSysColor(COLOR_WINDOW);
    COLORREF oldBkColor = dc.SetBkColor(newBkColor);

    if (((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
    {
        //dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
        dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
    }
    newBkColor = dc.GetBkColor();

    if (((lpDrawItemStruct->itemState & ODS_SELECTED) != 0))
    {
        dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
        dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
    }
    dc.DrawFocusRect(&(lpDrawItemStruct->rcItem));

    CBrush BkBrush(newBkColor);
    dc.FillRect(&lpDrawItemStruct->rcItem, &BkBrush);

    CRect TextRect(lpDrawItemStruct->rcItem);

    const int Margin = 2;
#if 0
    dc.ExtTextOut(TextRect.left + Margin, TextRect.top + Margin,
        ETO_OPAQUE, TextRect, (const char*)Text + 1, Text.GetLength() - 1, NULL);
#else
    TextRect.OffsetRect(Margin, Margin);
    dc.DrawText(
        (const char*)Text + 1, Text.GetLength() - 1, TextRect, DT_LEFT);
#endif
    dc.SetTextColor(oldTextColor);
    dc.SetBkColor(oldBkColor);

    if ((lpDrawItemStruct->itemAction & ODA_FOCUS) != 0)
        dc.DrawFocusRect(&(lpDrawItemStruct->rcItem));

    CSize Size = dc.GetTextExtent((const char*)Text + 1);
    int Width = Size.cx + 2 * Margin;
    if (Width > m_CurrentExtent)
        SetHorizontalExtent(Width);

    dc.Detach();
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CTraceListBox private methods


//#include <sys/timeb.h>
//
//CString CTraceListBox::ComposeTimeStringWithMilliSec()
//{
//    // Get seconds
//    CTime Time(time(NULL));  
//
//    // Get milliseconds
//    _timeb tstruct;
//    _ftime(&tstruct );
//
//    // TCHAR implementation
//    TCHAR sMilliSeconds[5];
//    _stprintf(sMilliSeconds,_T("%03u"),tstruct.millitm);
//
//    CString TimeString = Time.Format( "%a, %b %d, %Y %H:%M:%S." );
//    TimeString += sMilliSeconds;
//
//    return TimeString; 
//}

//BOOL CTraceListBox::CreateQueue() 
//{
//    return 0;
//}

CString CTraceListBox::Severity2String(ELogSeverity Severity)
{
    switch (Severity)
    {
    default:
    case LE_INFOLOW:
        return "1";
    case LE_INFO:
        return "2";
    case LE_INFOHIGH:
        return "3";
    case LE_WARNING:
        return "4";
    case LE_ERROR:
        return "5";
    case LE_FATAL:
        return "6";
    }
}

ELogSeverity CTraceListBox::String2Severity(const CString& Text)
{
    switch (Text[0])
    {
    default:
    case '1': 
        return LE_INFOLOW;
    case '2': 
        return LE_INFO;
    case '3': 
        return LE_INFOHIGH;
    case '4': 
        return LE_WARNING;
    case '5': 
        return LE_ERROR;
    case '6': 
        return LE_FATAL;
    }
}

COLORREF CTraceListBox::GetColor(ELogSeverity Severity)
{
    switch (Severity)
    {
    default:
    case LE_INFOLOW:
        return RGB(128, 128, 128);  // Gray
    case LE_INFO:
        return RGB(0, 0, 0);        // Black
    case LE_INFOHIGH:
        return RGB(0, 0, 255);      // Blue
    case LE_NOTICE:
		return RGB(0, 100, 0);      // Dark Green
    case LE_WARNING:
        return RGB(200, 100, 0);    // Orange
    case LE_ERROR:
        return RGB(255, 0, 0);      // Light Red
    case LE_FATAL:
        return RGB(160, 0, 160);    // Dark Purple
    }
}

//void CTraceListBox::SetHorizontalExtent(int Extent)
//{ 
////    int CurrentExtent = GetHorizontalExtent(); 
////    PrivateLogEvent(LE_INFOHIGH, 
////        "CTraceListBox::SetHorizontalExtent: CurrentExtent %d, New %d", 
////        CurrentExtent, Extent);
//
//    m_CurrentExtent = Extent;
//    BaseClass::SetHorizontalExtent(m_CurrentExtent); 
//}

void CTraceListBox::DisplayMessage(ELogSeverity Severity, const char* Text)
{
    enum { MAX_TRACE_LINES = 5000 };
    CString Display = Severity2String(Severity) /*+ " : "*/ + Text;

#ifdef USE_LIST_DISPLAY
    AddString(Display);
#else
    while (GetCount() >= MAX_TRACE_LINES)
        DeleteString(0);

//    std::string TimeString = FormatTime(Seconds, Millis, false/*!DateAlso*/);
//    CString TimeString = ComposeTimeStringWithMilliSec();
//    CString Display = Severity2String(Severity) + TimeString.c_str() + " : " + Text;


    int NewIndex = AddString(Display);

    // Make sure that if last line is on display we continue to display last line
    CRect ClientRect;
    GetClientRect(&ClientRect);
    BOOL Outside;
    int LastIndex = ItemFromPoint(CPoint(ClientRect.left, ClientRect.bottom), Outside);
    if (abs(LastIndex - NewIndex) <= 1) // Last line is displayed - keep it that way
        SetTopIndex(NewIndex);

    //TRACE("LastIndex %d, NewIndex %d, Equal %d, Outside %d\n", LastIndex, NewIndex, LastIndex == NewIndex, Outside);
#endif
}

// We need this in order to avoid recursion in displaying messages
void CTraceListBox::PrivateLogEvent(ELogSeverity Severity, const char* Format, ...)
{
    Severity;
    Format;
//    va_list Args;
//    va_start(Args, Format);
//    
//    char Message[MAX_LOG_MESSAGE_SIZE];
//    
//    _vsnprintf(Message, sizeof Message, Format, Args);
//
//    time_t Seconds;
//    int Millis;
//    GetSecondsAndMillis(Seconds, Millis);
//    DisplayMessage(Severity, Seconds, Millis, Message);
//    CLogManager::GetTheLogManager().LogLine(LE_ERROR, Message);
}
void CTraceListBox::InitListData()
{
#ifdef USE_LIST_DISPLAY
    Init(MAX_TRACE_LINES * AVERAGE_TRACE_LINE_SIZE, MAX_TRACE_LINES);
#endif
}

int CTraceListBox::GetFontHeight()
{
    CDC* cdc = GetDC();
    CFont* font = GetFont();
    CFont* pOldFont;
    
    if (font)
    {
        pOldFont = cdc->SelectObject(font);

        //TEXTMETRIC tm;
        //cdc->GetTextMetrics(&tm);

        CSize size = cdc->GetTextExtent(" ", 1 );
        //size.cx += tm.tmAveCharWidth;

        cdc->SelectObject(pOldFont);
        this->ReleaseDC(cdc);

        return size.cy;
    }
    ReleaseDC(cdc);
    return 0;
}
