// TextList.h : implementation file - declaration of the CTextList data structure
//

#pragma once

// The CTextList manages a list of text line with maximum number of lines and
// maximum total size.

class CTextList
{
public:
    CTextList();
    ~CTextList();

    bool Init(int MaxTextSize, int MaxLineCount);
    void Clear();

    bool AddLine(const char* Text, int& DeletedLines);

    inline int GetLineCount() const { return m_ActualLines; }
    const char* GetLine(int Index) const;

    bool IsValid(); // for testing
private:
    void Close();

    inline int CTextList::GetActualIndex(int Index) const;
    inline void DeleteOldestLine();
    inline char* GetHead();
    inline bool DeleteLinesUntilRoomEnough(int Length);

    // Line Management
    char** m_Lines;
    int    m_MaxLines;
    int    m_StartLine;
    int    m_ActualLines;

    // Text Management
    char*  m_TextBegin;
    char*  m_TextEnd;
    int    m_FreeTextSize;
    int    m_TextSize;
};
