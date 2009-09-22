#pragma once

#include <vector>

// How to resize - should be a combination of these styles
enum {
    RESIZE_LEFT = 1,
    RESIZE_TOP = 2,
    RESIZE_RIGHT = 4,
    RESIZE_BOTTOM = 8,
};

struct DlgResizerUnit
{
    int     Id;         // Control Id
    int     Style;      // A combination of the above styles
};


class CDlgResizer
{
public:
    CDlgResizer();
    ~CDlgResizer();

    void ResizeInit(HWND hWndParent, DlgResizerUnit* Units, int Count);
    void ResizePrepareMinSize(POINT* MinSize);
    void ResizeUpdate();

private:
    struct DlgResizerInternalUnit
    {
        int     Id;         // Control Id
        int     Style;      // A combination of the above styles
        RECT    Offsets;    // Filled by the resizer
    };

    HWND m_WndParent;
    std::vector <DlgResizerInternalUnit> m_Units;

};
