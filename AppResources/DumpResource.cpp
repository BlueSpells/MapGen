#include "stdafx.h"

#include "Common/LogEvent.h"
#include "DumpResource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ==========================================================================
static void DumpBuffer(const TCHAR* Caption, const void* Buffer, int Size)
{
    const BYTE* Current = (const BYTE*)Buffer;
    CString CurrentLine, CurrentText;

    CurrentText.Format(_T("\nstatic unsigned char %s[] =\n{"), Caption);
    CurrentLine += CurrentText;
    for (int i = 0; i < Size; ++i, ++Current)
    {
        if ((i % 16) == 0)
        {
            LogEvent(LE_INFOHIGH, CurrentLine);
            CurrentLine = "\n    ";
        }
        CurrentText.Format(_T("0x%02x, "), *Current);
        CurrentLine += CurrentText;
    }
    CurrentText.Format(_T("\n};\nstatic const int %sCount = sizeof(%s) / sizeof(%s[0]);\n"),
        Caption, Caption, Caption);
    CurrentLine += CurrentText;
    LogEvent(LE_INFOHIGH, CurrentLine);
}

static bool LoadResource(int Id, const TCHAR* Type,
                         HGLOBAL& hGlobal, const BYTE*& Data, int& Size)
{
    // Load the resource
    HINSTANCE TheResource = AfxGetResourceHandle();

    HRSRC hResource = ::FindResource(TheResource, MAKEINTRESOURCE(Id), Type);
    if (hResource == NULL)
        return false;

    hGlobal = ::LoadResource(TheResource, hResource);
    if (hGlobal == NULL)
        return false;

    Size = ::SizeofResource(TheResource, hResource);
    Data = (BYTE*)LockResource(hGlobal);
    if (Data == NULL)
        return false;

    return true;
}

static void UnlockAndFreeResource(HGLOBAL hGlobal)
{
    UnlockResource(hGlobal);
    FreeResource(hGlobal);
}

bool DumpResource(int Id, const TCHAR* Caption, const TCHAR* Type)
{
    HGLOBAL hGlobal;
    int Size;
    const BYTE* Data;
    if (!LoadResource(Id, Type, hGlobal, Data, Size))
        return false;

    DumpBuffer(Caption, Data, Size);

    UnlockAndFreeResource(hGlobal);
    return true;
}

bool DumpBitmap(int Id, const TCHAR* Caption)
{
    return DumpResource(Id, Caption, RT_BITMAP);
}

bool DumpDialog(int Id, const TCHAR* Caption)
{
    return DumpResource(Id, Caption, RT_DIALOG);
}

#pragma pack(push, 1)

struct NEWHEADER
{
    WORD Reserved;
    WORD ResType;
    WORD ResCount;
};

struct CURSORDIR
{
    WORD Width;
    WORD Height;
};

struct ICONRESDIR
{
    BYTE Width;
    BYTE Height;
    BYTE ColorCount;
    BYTE reserved;
};

struct RESDIR
{
    union
    {
        ICONRESDIR   Icon;
        CURSORDIR    Cursor;
    } ResInfo;
    WORD    Planes;
    WORD    BitCount;
    DWORD   BytesInRes;
    WORD  IconCursorId;
};

#pragma pack(pop)

bool DumpCursor(int Id, const TCHAR* Caption)
{
    //    NEWHEADER
    //    RES_CURSOR

    HGLOBAL hGlobal;
    int Size;
    const BYTE* Data;
    if (!LoadResource(Id, RT_GROUP_CURSOR, hGlobal, Data, Size))
        return false;

    const NEWHEADER* Header = (const NEWHEADER*)Data;
    if (Header->ResType != RES_CURSOR || Header->ResCount < 1)
        return false;

    const RESDIR* Resdir = (const RESDIR*)(Header + 1);
    int IconId = Resdir->IconCursorId;

    UnlockAndFreeResource(hGlobal);

    DumpResource(IconId, Caption, RT_CURSOR);

    //    const int FormatVersion = 0x00030000;
    //    static HCURSOR hCursor =
    //        CreateIconFromResource(FilterBits, FilterBitsCount, FALSE/*!fIcon*/, FormatVersion);
    //    SetCursor(hCursor);

    return true;
}
