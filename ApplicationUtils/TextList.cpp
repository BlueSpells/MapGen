// TextList.cpp : implementation file - implement the CTextList data structure
//

// Data structure is as follows:
// m_Lines is managed as a queue and contains pointers to m_Text.
// m_Text is also managed as a queue. 
//

#include "stdafx.h"

#include "TextList.h"

CTextList::CTextList() :
    m_Lines(NULL),
    m_MaxLines(0),
    m_StartLine(0),
    m_ActualLines(0),
    m_TextBegin(NULL),
    m_TextEnd(NULL),
    m_FreeTextSize(0),
    m_TextSize(0)
{
}

CTextList::~CTextList()
{
    Close();
}

void CTextList::Close()
{
    delete [] m_Lines;
    m_Lines = NULL;
    delete [] m_TextBegin;
    m_TextBegin = NULL;
}

bool CTextList::Init(int MaxTextSize, int MaxLines)
{
    if (MaxLines < 1 || MaxTextSize < 1)
        return false;

    Close();

    m_Lines        = new char*[MaxLines];
    m_MaxLines     = MaxLines;

    m_TextBegin    = new char[MaxTextSize];
    m_TextEnd      = m_TextBegin + MaxTextSize;
    m_TextSize     = MaxTextSize;

    Clear();

    return true;
}

void CTextList::Clear()
{
    m_StartLine    = 0;
    m_ActualLines  = 0;

    m_FreeTextSize = m_TextSize;
    m_Lines[0]     = m_TextBegin;
}

bool CTextList::DeleteLinesUntilRoomEnough(int Length)
{
    while (m_FreeTextSize < Length && m_ActualLines > 0)
        DeleteOldestLine();

    if (m_FreeTextSize >= Length)
        return true;

    Assert(false);
    return false;
}

void CTextList::DeleteOldestLine()
{
    const char* PreviousOldestLine = m_Lines[m_StartLine];
    Assert(PreviousOldestLine >= m_TextBegin && PreviousOldestLine < m_TextEnd);

    --m_ActualLines;
    ++m_StartLine;
    Assert(m_StartLine <= m_MaxLines);
    if (m_StartLine == m_MaxLines)
        m_StartLine = 0;

    const char* CurrentOldestLine = m_Lines[m_StartLine];
    Assert(CurrentOldestLine >= m_TextBegin && CurrentOldestLine < m_TextEnd);

    // (1) Either the CurrentOldestLine > PreviousOldestLine
    if (CurrentOldestLine > PreviousOldestLine)
        m_FreeTextSize += CurrentOldestLine - PreviousOldestLine;
    // (2) CurrentOldestLine <= PreviousOldestLine - means a wrap around
    //     In this case CurrentOldestLine == m_Text
    else  
    {
        Assert(CurrentOldestLine == m_TextBegin);
        m_FreeTextSize += m_TextEnd - PreviousOldestLine;
    }
}

bool CTextList::AddLine(const char* Text, int& DeletedLines)
{
    int Length = strlen(Text) + 1;
    if (Length > m_TextSize) // A line shouldn't be larger than the whole Text
        return false;

    Assert(m_ActualLines <= m_MaxLines);
    int PreviousActualLines = m_ActualLines;

    // check if have a free line
    if (m_ActualLines == m_MaxLines)
        DeleteOldestLine();

    // check if have enough free text
    if (!DeleteLinesUntilRoomEnough(Length))
        return false;

    // If at the end and cannot have allocation in one segment - move to start
    char* Head = GetHead();
    if (Head + Length > m_TextEnd)
        m_FreeTextSize -= m_TextEnd - Head;

    // check again if have enough free text
    if (!DeleteLinesUntilRoomEnough(Length))
        return false;

    DeletedLines = PreviousActualLines - m_ActualLines;

    // Point to current text place.
    char* Start = GetHead();

    int ActualIndex = GetActualIndex(m_ActualLines);
    ++m_ActualLines;
    m_Lines[ActualIndex] = Start;
    Assert(Start + Length <= m_TextEnd);
    memcpy(Start, Text, Length);
    m_FreeTextSize -= Length;

    return true;
}

int CTextList::GetActualIndex(int Index) const
{
    int ActualIndex = m_StartLine + Index;
    if (ActualIndex >= m_MaxLines)
        ActualIndex -= m_MaxLines;
    Assert(ActualIndex >=0 && ActualIndex < m_MaxLines);
    return ActualIndex;
}

const char* CTextList::GetLine(int Index) const
{
    Assert(Index >= 0 && Index < m_MaxLines);
    int ActualIndex = GetActualIndex(Index);
    return m_Lines[ActualIndex];
}

char* CTextList::GetHead()
{
    char* TextStart = m_Lines[m_StartLine];
    char* Start = TextStart + m_TextSize - m_FreeTextSize;
    if (Start >= m_TextEnd)
        Start -= m_TextSize;

    return Start;
}

bool CTextList::IsValid()
{
    Assert(m_ActualLines <= m_MaxLines);
    Assert(m_StartLine >= 0 && m_StartLine < m_MaxLines);

    int UsedTextSize = 0;
    int LastOffsetToEnd = m_TextSize;
    for (int i = 0; i < m_ActualLines; ++i)
    {
        int Index = GetActualIndex(i);
        Assert(Index >= 0 && Index < m_MaxLines);
        char* Text = m_Lines[Index];
        int Length = strlen(Text) + 1;
        Assert(Text >= m_TextBegin && Text + Length <= m_TextEnd);

        // There is a special consideration for the last line since it is
        // treated as if it uses also the gap to the end
        int OffsetToEnd = m_TextEnd - Text - Length;
        if (OffsetToEnd > LastOffsetToEnd) // wrap around - we add the last offset
            UsedTextSize += LastOffsetToEnd;

        UsedTextSize += Length;
        LastOffsetToEnd = OffsetToEnd;
    }
    Assert(UsedTextSize + m_FreeTextSize == m_TextSize);
    return true;
}

////////////////////////////////////////////////////////////////////////

#ifdef _TEST

#include <vector>
#include <string>

#include "Common/LogEvent.h"

class COtherTextList
{
public:
    bool Init(int MaxTextSize, int MaxLineCount)
    {
        m_MaxTextSize  = MaxTextSize;
        m_MaxLines     = MaxLineCount;
        m_FreeTextSize = MaxTextSize;
        return true;
    }

    ~COtherTextList()
    {
        int Count = m_Text.size();
        for (int i = 0; i < Count; ++i)
            delete m_Text[i];
    }

    bool AddLine(const char* Text, int& DeletedLines)
    {
        int Length = strlen(Text) + 1;
        if (Length > m_MaxTextSize)
            return false;

        int PrevCurrentLines = m_Text.size();

        if ((int)m_Text.size() == m_MaxLines)
            DeleteOldestLine();

        while (m_FreeTextSize < Length)
            DeleteOldestLine();
        DeletedLines = PrevCurrentLines - m_Text.size();
        m_Text.push_back(new std::string(Text));
        m_FreeTextSize -= Length;
        return true;
    }

    int GetLineCount() const
    {
        return m_Text.size();
    }

    const char* GetLine(int Index) const
    {
        Assert(Index >= 0 && Index < m_MaxLines);
        return m_Text[Index]->c_str();
    }

//     bool IsValid()
//     {
//         return true;
//     }
// 
    void DeleteOldestLine()
    {
        int Length = m_Text[0]->size() + 1;
        m_FreeTextSize += Length;
        Assert(m_FreeTextSize >= 0 &&  m_FreeTextSize < m_MaxTextSize);
        delete m_Text[0];
        m_Text.erase(m_Text.begin());
    }

private:
    std::vector<std::string*> m_Text;

    int                      m_MaxTextSize;
    int                      m_FreeTextSize;
    int                      m_MaxLines;
};


#include "Common/LogEvent.h"

static const int MAX_LINE = 1024;

class CTextListTester
{
public:
    CTextListTester()
    {
    }

    void FunctionalText()
    {
        const int MAX_LINES = 5;
        const int MAX_TEXT  = 5 * 10;

        Assert(Init(MAX_TEXT, MAX_LINES));
        
        Assert(!AddLine('a', 51));

        Check();
        Assert(AddLine('a', 9));
        Check();
        Assert(AddLine('b', 19));
        Check();
        Assert(AddLine('c', 13));
        Check();
        Assert(AddLine('d', 7));
        Check();
        Dump();
        Assert(AddLine('e', 10));
        Check();
        Dump();
    }

    template <class TextList>
    void PerformanceTest(const char* Caption, TextList& Text, int MaxText, int MaxLines, int AverageLine, int MaxOffset, int Loops)
    {
        Assert(Text.Init(MaxText, MaxLines));

        DWORD StartTicks = GetTickCount();
        for (int i = 0; i < Loops; ++i)
        {
            int Deleted;
            int Size = AverageLine + (rand() % (2 * MaxOffset)) - MaxOffset;
            FillLine(i, Size);
            Assert(Text.AddLine(m_Buffer, Deleted));
            //Assert(m_Text.IsValid());
        }
        DWORD EndTicks = GetTickCount();
        LogEvent(LE_INFOHIGH, "%s: Loops %d, Ticks %d", Caption, Loops, EndTicks - StartTicks);
    }

    void PerformanceTest(int MaxText, int MaxLines, int AverageLine, int MaxOffset, int Loops)
    {
        PerformanceTest("TextList", m_Text, MaxText, MaxLines, AverageLine, MaxOffset, Loops);
    }

    void PerformanceTest2(int MaxText, int MaxLines, int AverageLine, int MaxOffset, int Loops)
    {
        PerformanceTest("OtherTextList", m_Text2, MaxText, MaxLines, AverageLine, MaxOffset, Loops);
    }

    void AutomaticText(int MaxText, int MaxLines, int AverageLine, int MaxOffset, int Loops)
    {
        Assert(Init(MaxText, MaxLines));

        for (int i = 0; i < Loops; ++i)
        {
            int Size = AverageLine + (rand() % (2 * MaxOffset)) - MaxOffset;

            Assert(AddLine(i, Size));
            Assert(Check());
        }
    }
private:
    void Dump()
    {
        int Count = m_Text.GetLineCount();
        LogEvent(LE_INFOHIGH, "#%d Lines", Count);
        for (int i = 0; i < Count; ++i)
        {
            const char* Text = m_Text.GetLine(i);
            LogEvent(LE_INFOHIGH, " (%d) - [%s]", i, Text);
        }
    }

    bool Check()
    {
        Assert(m_Text.IsValid());

        int Count = m_Text.GetLineCount();
        int Count2 = m_Text2.GetLineCount();
        Assert(Count == Count2);
        for (int i = 0; i < Count; ++i)
        {
            const char* Text = m_Text.GetLine(i);
            const char* Text2 = m_Text2.GetLine(i);
            Assert(strcmp(Text, Text2) == 0);
        }
        return true;
    }

    bool Init(int MaxText, int MaxLines)
    {
        bool Result = m_Text.Init(MaxText, MaxLines);
        bool Result2 = m_Text2.Init(MaxText, MaxLines);
        Assert(Result == Result2);
        return Result;
    }

    void FillLine(int Value, int Length)
    {
        if (Length > MAX_LINE)
            Length = MAX_LINE;
        memset(m_Buffer, Value, Length);
        m_Buffer[Length] = '\0';
    }

    bool AddLine(int Value, int Length)
    {
        FillLine(Value, Length);

        int Deleted, Deleted2;
        bool Result = m_Text.AddLine(m_Buffer, Deleted);
        bool Result2 = m_Text2.AddLine(m_Buffer, Deleted2);
        Assert(Result == Result2);

        // Since algorithm is not the same - the Text may delete more lines
        // than the other text. In this case we delete lines to make it equal
        if (Deleted != Deleted2)
        {
            Assert(Deleted > Deleted2);
            int Gap = Deleted - Deleted2;
            for (int i = 0; i < Gap; ++i)
                m_Text2.DeleteOldestLine();
        }
        return Result;
    }

    CTextList      m_Text;
    COtherTextList m_Text2;
    char           m_Buffer[MAX_LINE + 1];
};

void TestTextList()
{
    CTextListTester Tester;
//    Tester.FunctionalText();

    const int MAX_LINES    = 5000;
    const int AVERAGE_LINE = 128;
    const int MAX_OFFSET   = 64;
    const int MAX_TEXT     = 5000 * AVERAGE_LINE;
    const int LOOPS        = 10 * 1000 * 1000;

//    Tester.PerformanceTest(MAX_TEXT, MAX_LINES, AVERAGE_LINE, MAX_OFFSET, LOOPS);
//    Tester.PerformanceTest2(MAX_TEXT, MAX_LINES, AVERAGE_LINE, MAX_OFFSET, LOOPS);

    Tester.AutomaticText(MAX_TEXT, MAX_LINES, AVERAGE_LINE, MAX_OFFSET, LOOPS);

}

#endif