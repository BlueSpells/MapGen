// ListDisplay.h : header file
//

#pragma once

#include "TextList.h"
#include "CustomToolTip.h"

/////////////////////////////////////////////////////////////////////////////
// CListDisplay window

//class CListDisplay : public CStatic
class CListDisplay : public CWnd, 
                     public ICustomToolTipHelper
{
public:
    CListDisplay();
    virtual ~CListDisplay();

    bool Init(int MaxText, int MaxLines);

    bool Create(CWnd* Parent, int Id);

    bool AddString(const char* Text);

    void SetHorizontalExtent(int Extent); 
    int GetHorizontalExtent();

    // May be called from application menu
    void ResetContent();
	void Find(bool FindNext);
    void Copy();

    // Does not support variable height - so Index is ignored
    int SetItemHeight(int nIndex, UINT cyItemHeight);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListDisplay)
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CListDisplay)
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
    afx_msg LRESULT OnSearchTermReceived(WPARAM wParam, LPARAM lParam);
    //}}AFX_MSG
//    LRESULT OnQueueMessage(WPARAM wParam, LPARAM lParam);
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
protected:
    // May be overridden by derived classes
    virtual void DrawLine(CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent);

    // To be used by derived classes
    void DrawLine(
        CDC& Dc, const char* Text, const CRect& Rect, bool HasFocus, bool IsCurrent, COLORREF TextColor);
private:
    void ScrollHelper(
        UINT nSBCode, UINT nPos, int& Current, int Page, int Line, int Max);
    void UpdateScrollInfo(int Bar, int Page, int Pos, int Max);

    void UpdateVerticalScrollBarSize();
    void UpdateVerticalScrollBarPos();
    void UpdateHorizontalScrollBarSize();
    void UpdateHorizontalScrollBarPos();
    void UpdateHorizontalPos();

    void HandleChar(UINT Char);

    void ComputeLineHeight();
    void ComputeLinesPerPage();
    int  ComputeTextWidth(const std::string& Text);

    void InvalidateLine(int LineIndex);
    void InvalidateCurrentLine();
    void SetCurrentLine(int Index, bool Force = false);

    void DrawEmptyLine(CDC& Dc, int LineIndex);

    inline void AdjustPos(int& Pos, int Page, int Max);
    void ScrollLinesTo(int NewFirstLineIndex);
//    void ScrollLinesBy(int Lines);

    void CopyTextToClipBoard(const char* Text);
    void CopyCurrentLineToClipBoard();

    int GetLineIndexFromPoint(const CPoint& Point);
    CRect GetLineRect(int Index);

    void PerformSearch();

    // Implement ICustomToolTipHelper
    virtual const TCHAR* GetToolTipText(const POINT& Point);

    void BreakTooltipLineIfNeeded();

    CTextList   m_Text;
    int         m_FirstLineIndex; // First line index displayed on screen 
    int         m_CurrentIndex;   // Index of current line
    int         m_LinesPerPage;
    int         m_LineHeight;
    int         m_WindowWidth, m_WindowHeight;
    int         m_HorizontalExtent;
    int         m_HorizontalPos;

    struct SearchData
    {
        SearchData() : m_SearchDlg(NULL), m_IsCaseSensitive(false) {}

        class CSearchDlg* m_SearchDlg;
        std::string       m_CurrentSearchTerm;
        bool              m_IsCaseSensitive;
    };

    SearchData m_SearchData;

    std::string   m_CurrentToolTipStr;
    CCustomToolTip m_Tooltip;
};

