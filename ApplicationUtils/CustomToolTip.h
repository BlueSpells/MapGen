#pragma once

#include <string>

//////////////////////////////////////////////////////////////////////////
// ICustomToolTipHelper
// A client of the Tool Tip should provide this class - either by deriving from it
// or having a member derived from it
//////////////////////////////////////////////////////////////////////////

class ICustomToolTipHelper
{
public:
    // Used to get the displayed text - Text may contain several lines separated by \n
    virtual const TCHAR* GetToolTipText(const POINT& Point) = 0;
};

//////////////////////////////////////////////////////////////////////////
// CCustomToolTip
//////////////////////////////////////////////////////////////////////////

class CCustomToolTip : public CWnd
{
// Construction
public:
    CCustomToolTip();
    virtual ~CCustomToolTip();

    bool Create(HWND hOwnerWnd, ICustomToolTipHelper* CustomToolTipHelper);

    // Set Delay for showing and hiding (HideDelay 0 means infinite)
    void SetDelays(UINT ShowDelay, UINT HideDelay = 0);

    // To be called by client window and send mouse events messages
    // (WM_MOUSEMOVE  WM_LBUTTONDOWN WM_RBUTTONDOWN)
    void RelayEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Hide();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCustomToolTip)
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:
    //{{AFX_MSG(CCustomToolTip)
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    POINT GetCursorPos();
    bool IsOwnerActive();

    void OnMouseMove();
    void NotifyParent(int NotifyCode);
    void Resize();

    enum AttachSide // Attach side of the tool tip - other sides can be easily added
    {
        LEFT_TOP,
        TOP,
        RIGHT,
    };

    void ComputeSize (int& Width, int& Height);
    void AdjustOrigin(int& Left, int& Top, AttachSide Side, int Width, int Height);
    void SizeAndShow (int Left, int Top, AttachSide Side);

    void ComputeSize(HDC hDC, const TCHAR* Text, int& Width, int& Height);
    void Draw(HDC hDC, const RECT& Rect, const TCHAR* Text, COLORREF BkColor, bool DrawFrame);
    void FrameRect(HDC hDC, const RECT& Rect, COLORREF Color);
    void FrameRect(HDC hDC, const RECT& Rect);
    void FillRect(HDC hDC, const RECT& Rect, COLORREF Color);
    int RectWidth(const RECT& Rect);

    ICustomToolTipHelper* m_CustomToolTipHelper;
    UINT                 m_ShowDelay, m_HideDelay;
    HWND                 m_hOwnerWnd;
    std::string          m_Text;
    COLORREF             m_BkColor;
    POINT                m_LastCursorPos;
    DWORD                m_LastMoveTime;
    DWORD                m_LastShowTime;
    HFONT                m_hFont;
    bool                 m_SmallerFont;
};

// ==========================================================================
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
