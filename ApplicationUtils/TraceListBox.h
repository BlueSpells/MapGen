#if !defined(AFX_TRACELISTBOX_H__564EFC53_E070_4982_B72D_D01E908CA6AD__INCLUDED_)
#define AFX_TRACELISTBOX_H__564EFC53_E070_4982_B72D_D01E908CA6AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TraceListBox.h : header file
//

#define USE_LIST_DISPLAY
#include "..\Common/LogEvent.h"
#include "..\Common/SafeQueue.h"

/////////////////////////////////////////////////////////////////////////////
// CTraceListBox window

#ifdef USE_LIST_DISPLAY
#include "ListDisplay.h"
class CTraceListBox : public CListDisplay
#else
class CTraceListBox : public CListBox
#endif
{
// Construction
public:
	CTraceListBox();
	virtual ~CTraceListBox();

    // Severity values are defined in LogEvent.h
    // returns true if the message was added to the list box
    bool AddMessage(ELogSeverity Severity, const char* Text);

    void Clear();

    void SetLogLevel(ELogSeverity Severity);
    ELogSeverity GetLogLevel();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTraceListBox)
	public:
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
#ifdef USE_LIST_DISPLAY
    virtual void DrawLine(CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent);
#else
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTraceListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
    LRESULT OnQueueMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
    CString Severity2String(ELogSeverity Severity);
    ELogSeverity String2Severity(const CString& Text);
    COLORREF GetColor(ELogSeverity Severity);
    void DisplayMessage(ELogSeverity Severity, const char* Text);
    void PrivateLogEvent(ELogSeverity Severity, const char* Format, ...);
//    CString ComposeTimeStringWithMilliSec();
//    BOOL CreateQueue();
//    void SetHorizontalExtent(int Extent);

//    CER_QueueOut   m_QueueOut;   
//	CER_WinQueueIn m_QueueIn;
    int GetFontHeight();
    void InitListData();
    struct TraceQueueItem
    {
        std::string Msg;
        ELogSeverity Severity;
    };
    typedef SafeQueue<TraceQueueItem> TraceQueue;
    TraceQueue       m_Queue;
    ELogSeverity     m_Severity;
//    int              m_CurrentExtent;
//    CCriticalSection m_Lock;
    long             m_LostMessages;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACELISTBOX_H__564EFC53_E070_4982_B72D_D01E908CA6AD__INCLUDED_)
