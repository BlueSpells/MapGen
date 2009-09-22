#include "StdAfx.h"
#include "DlgResizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgResizer::CDlgResizer()
{
}

CDlgResizer::~CDlgResizer()
{
}




//#include "GcCommon\GcTrace.h"


void CDlgResizer::ResizeInit(HWND hWndParent, DlgResizerUnit* Units, int Count)
{
    m_WndParent = hWndParent;
    DlgResizerInternalUnit ZeroUnit;
    memset((void*)&ZeroUnit, 0, sizeof(DlgResizerInternalUnit));
    m_Units.resize(Count, ZeroUnit);

    CRect ParentRect;
    GetWindowRect(m_WndParent, &ParentRect);

    for (int i = 0; i < Count; i++)
    {
        m_Units[i].Id = Units[i].Id;
        m_Units[i].Style = Units[i].Style;

        HWND hWndChild = GetDlgItem(m_WndParent, Units[i].Id);

        if (hWndChild != NULL)
        {
            CRect ChildRect, OffsetRect;
            GetWindowRect(hWndChild, &ChildRect);

            OffsetRect.left = ChildRect.left - ParentRect.left;
            OffsetRect.top = ChildRect.top - ParentRect.top;
            OffsetRect.right = ParentRect.right - ChildRect.right;
            OffsetRect.bottom = ParentRect.bottom - ChildRect.bottom;
#if 0
            // Debug - if was already initialized
            if (Units->Offsets.left != 0 || Units->Offsets.top != 0 ||
                Units->Offsets.right != 0 || Units->Offsets.bottom != 0)
            {
                if (
                    Units->Offsets.left != OffsetRect.left ||
                    Units->Offsets.top != OffsetRect.top ||
                    Units->Offsets.right != OffsetRect.right ||
                    Units->Offsets.bottom != OffsetRect.bottom
                    )
                    DebugBreak();
            }
#endif

            m_Units[i].Offsets = OffsetRect;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
void CDlgResizer::ResizePrepareMinSize(POINT* MinSize)
{
    CRect ParentRect;
    GetWindowRect(m_WndParent, &ParentRect);

    MinSize->x = ParentRect.Width();
    MinSize->y = ParentRect.Height();
}


//////////////////////////////////////////////////////////////////////////
void CDlgResizer::ResizeUpdate()
{
    CRect ParentRect;
    GetWindowRect(m_WndParent, &ParentRect);

    int Count = m_Units.size();
    for (int i = 0; i < Count; i++)
    {
        HWND hWndChild = GetDlgItem(m_WndParent, m_Units[i].Id);

        if (hWndChild != NULL)
        {
            CRect ChildRect;
            GetWindowRect(hWndChild, &ChildRect);
            int Style = m_Units[i].Style;
            CRect Offsets = m_Units[i].Offsets;

            // Resize left-right
            if (Style & RESIZE_LEFT && Style & RESIZE_RIGHT)
            {
                ChildRect.left = ParentRect.left + Offsets.left;
                ChildRect.right = ParentRect.right - Offsets.right;
            }
            else if (Style & RESIZE_LEFT)
                ChildRect.OffsetRect(ParentRect.left + Offsets.left - ChildRect.left, 0);
            else if (Style & RESIZE_RIGHT)
                ChildRect.OffsetRect(ParentRect.right - Offsets.right - ChildRect.right, 0);

            // Resize top-bottom
            if (Style & RESIZE_TOP && Style & RESIZE_BOTTOM)
            {
                ChildRect.top = ParentRect.top + Offsets.top;
                ChildRect.bottom = ParentRect.bottom - Offsets.bottom;
            }
            else if (Style & RESIZE_TOP)
                ChildRect.OffsetRect(0, ParentRect.top + Offsets.top - ChildRect.top);
            else if (Style & RESIZE_BOTTOM)
                ChildRect.OffsetRect(0, ParentRect.bottom - Offsets.bottom - ChildRect.bottom);

            // Compute Client of Rect Left Top point
            ::ScreenToClient(m_WndParent, (LPPOINT)&ChildRect);
            // Same with Right Bottom point
            ::ScreenToClient(m_WndParent, ((LPPOINT)&ChildRect)+1);
            MoveWindow(hWndChild, ChildRect.left, ChildRect.top, 
                ChildRect.Width(), ChildRect.Height(), TRUE);
        }
    }
}
