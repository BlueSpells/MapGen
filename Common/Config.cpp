// Config.cpp: implementation of the Configuration routines.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Config.h"
#include "Utils.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CIniFile& GetTheConfigIniFile()
{
    static CIniFile TheConfigIniFile;

    return TheConfigIniFile;
}

void SetConfigFileName(const char* ConfigFileName)
{
    GetTheConfigIniFile().SetFileName(ConfigFileName);
}

std::string GetConfigFileName()
{
    return GetTheConfigIniFile().GetFileName();
}

// Write every parameter read from config back
void SetConfigWritebackMode(bool On)
{
    GetTheConfigIniFile().SetWriteBackMode(On);
}

bool GetConfigWritebackMode()
{
    return GetTheConfigIniFile().GetWriteBackMode();
}

bool DeleteConfigSection(const char* Section)
{
    return GetTheConfigIniFile().DeleteConfigSection(Section);
}

// Implemented as a wrapper of ErcommonLib/Util in order to hide some details

double GetConfigDouble(const char* Section, const char* Key, double Default,
                       EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
	return GetTheConfigIniFile().GetConfigDouble(Section, Key, Default, Mode);
}

bool WriteConfigDouble(const char* Section, const char* Key, double Value)
{
	return GetTheConfigIniFile().WriteConfigDouble(Section, Key, Value);
}

int GetConfigInt(const char* Section, const char* Key, int Default,
                 EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
	return GetTheConfigIniFile().GetConfigInt(Section, Key, Default, Mode);
}
 
bool WriteConfigInt(const char* Section, const char* Key, int Value)
{
	return GetTheConfigIniFile().WriteConfigInt(Section, Key, Value);
}

INT64 GetConfigInt64(const char* Section, const char* Key, INT64 Default,
                     EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    return GetTheConfigIniFile().GetConfigInt64(Section, Key, Default, Mode);
}

bool WriteConfigInt64(const char* Section, const char* Key, INT64 Value)
{
    return GetTheConfigIniFile().WriteConfigInt64(Section, Key, Value);
}

std::string GetConfigString(const char* Section, const char* Key, const char* Default,
                            EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    return GetTheConfigIniFile().GetConfigString(Section, Key, Default, Mode);
}

bool WriteConfigString(const char* Section, const char* Key, const char* Value)
{
    return GetTheConfigIniFile().WriteConfigString(Section, Key, Value);
}

bool GetConfigBool(const char* Section, const char* Key, bool Default,
                   EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
	return GetTheConfigIniFile().GetConfigBool(Section, Key, Default, Mode);
}

bool WriteConfigBool(const char* Section, const char* Key, bool Value)
{
	return GetTheConfigIniFile().WriteConfigBool(Section, Key, Value);
}


static std::string CreateConfigStringVectorKeyName(const char* Key, unsigned int Index)
{
    std::string CurrentKey = Key;
    char Buf[32];
    sprintf_s(Buf, "_%u", Index);
    CurrentKey += Buf;
    return CurrentKey;
}

static void DumpStringVector(const char* Context, const std::vector<std::string>& StringVector)
{
    size_t NumStrings = StringVector.size();
    for (size_t i = 0; i < NumStrings; ++i)
        LogEvent(LE_INFO, "%s: String[%u] = \"%s\"", Context, i, StringVector[i].c_str());
}

void GetConfigStringVector(const char* Section, const char* Key, std::vector<std::string>& StringVector)
{
    StringVector.clear();
    const std::string EndVecotr = std::string("__EndOf") + Key + "List__";
    for (unsigned int i=0;; ++i)
    {
        std::string CurrentKey = CreateConfigStringVectorKeyName(Key, i);
        std::string CurrentStr = GetConfigString(Section, CurrentKey.c_str(), EndVecotr.c_str());
        if (CurrentStr == EndVecotr)
            break;
        StringVector.push_back(CurrentStr);
    }
    //DumpStringVector(__FUNCTION__, StringVector);
}

bool WriteConfigStringVector(const char* Section, const char* Key, const std::vector<std::string>& StringVector)
{
    //DumpStringVector(__FUNCTION__, StringVector);
    bool Result = true;
    for (unsigned int i=0; i < StringVector.size(); ++i)
    {
        std::string CurrentKey = CreateConfigStringVectorKeyName(Key, i);
        Result = WriteConfigString(Section, CurrentKey.c_str(), StringVector[i].c_str()) && Result;
    }
    return Result;
}

void GetConfigIntVector(const char* Section, const char* Key, std::vector<int>& IntVector, const char* Default /*= ""*/)
{
    std::string Str = GetConfigString(Section, Key, Default);
    ParseIntVectorString(Str.c_str(), IntVector);
}

bool WriteConfigIntVector(const char* Section, const char* Key, const std::vector<int>& IntVector)
{
    std::string Str = IntVectorToStr(IntVector);
    return WriteConfigString(Section, Key, Str.c_str());
}

const char* Value2Name(const ValueAndName Names[], int Count, int Value, const char* Default)
{
	for (int i = 0; i < Count; ++i, ++Names)
		if (Names->Value == Value)
			return Names->Name;
	return Default;
}

int Name2Value(const ValueAndName Names[], int Count, const char* Name, int NotFound, const char* LogCaption/* = NULL*/)
{
	for (int i = 0; i < Count; ++i)
	{
		if (CompareNoCase(Names[i].Name, Name))
			return Names[i].Value;
	}
	if (LogCaption)
	{
		LogEvent(LE_WARNING, "Name2Value[%s]: Illegal Value [%s], Possible values are", Name);
		for (int i = 0; i < Count; ++i)
			LogEvent(LE_WARNING, " [%s]", Names[i].Name);
	}
	return NotFound;
}

int GetConfigEnum(const char* Section, const char* Key, const ValueAndName Enums[], int EnumCount, int Default)
{
	const char* Name = Value2Name(Enums, EnumCount, Default, "");

	std::string Value = GetConfigString(Section, Key, Name);
	if (Value.empty())
		return Default;

	for (int i = 0; i < EnumCount; ++i)
	{
		if (CompareNoCase(Value, Enums[i].Name))
			return Enums[i].Value;
	}

	LogEvent(LE_ERROR, "CConfigFile::GetConfigName: [%s], \"%s\": Value should be one of:", Section, Key);
	std::string Values;
	for (int i = 0; i < EnumCount; ++i)
	{
		Values += "\"";
		Values += Enums[i].Name;
		Values += "\"";
		if (i < EnumCount - 1)
			Values += ", ";
	}
	LogEvent(LE_ERROR, "  %s", Values.c_str());
	return Default;
}


//////////////////////////////////////////////////////////////////////////

std::string GetArrayKeyName(const char* ArrayName, int Index)
{
    const int MaxNameSize = 128;
    char Name[MaxNameSize];

    if (Index == -1)
        return ArrayName;

    sprintf_s(Name, "%s%d", ArrayName, Index);
    return Name;
}

//////////////////////////////////////////////////////////////////////////

LPCTSTR const PointFormat = _T("%d,%d");

CPoint GetConfigPoint(const char* Section, const char* Key, const CPoint& Default){
    std::string Value = GetConfigString(Section, Key, "");

    CPoint Point(Default);

    if (!Value.empty())
        _stscanf_s(Value.c_str(), PointFormat, &Point.x, &Point.y);
    return Point;
}

void WriteConfigPoint(const char* Section, const char* Key, const CPoint& Value){
    CString String;
    String.Format(PointFormat, Value.x, Value.y);

    WriteConfigString(Section, Key, String);
}

//////////////////////////////////////////////////////////////////////////

LPCTSTR const RectFormat = _T("%d,%d,%d,%d");

CRect GetConfigRect(const char* Section, const char* Key, const CRect& Default){
    std::string Value = GetConfigString(Section, Key, "");

    CRect Rect(Default);

    if (!Value.empty())
        _stscanf_s(Value.c_str(), RectFormat, 
            &Rect.left, &Rect.top, &Rect.right, &Rect.bottom); 
    return Rect;
}

bool WriteConfigRect(const char* Section, const char* Key, const CRect& Value){
    CString String;

    String.Format(RectFormat, 
        Value.left, Value.top, Value.right, Value.bottom); 

    return WriteConfigString(Section, Key, String);
}

//////////////////////////////////////////////////////////////////////////

LPCTSTR const RectKey = _T("Rect");
LPCTSTR const ShowKey = _T("Show");
LPCTSTR const OriginKey = _T("Origin");

void GetWindowPlacement(CWnd* Window,  const char* Section,  bool Show)
{
    CRect WindowRect;
    
    Window->GetWindowRect(&WindowRect);

    WindowRect = GetConfigRect(Section, RectKey, WindowRect);
    if (WindowRect.top < 0)
        WindowRect.top = 0;
    if (WindowRect.left < 0)
        WindowRect.left = 0;
    if (WindowRect.right > GetSystemMetrics(SM_CXSCREEN))
        WindowRect.right = GetSystemMetrics(SM_CXSCREEN);
    if (WindowRect.bottom > GetSystemMetrics(SM_CYSCREEN))
        WindowRect.bottom = GetSystemMetrics(SM_CYSCREEN);
        
    const int DefaultShow = SW_RESTORE;
    int ShowCommand = GetConfigInt(Section, ShowKey, DefaultShow);
    
    CPoint WindowOrigin;
    
    WINDOWPLACEMENT WindowPlace;
    Window->GetWindowPlacement(&WindowPlace);
    WindowOrigin = WindowPlace.ptMaxPosition;

    WindowOrigin = GetConfigPoint(Section, OriginKey, WindowOrigin);

    WindowPlace.length = sizeof(WindowPlace);
    WindowPlace.flags = 0;
    WindowPlace.showCmd = Show ? ShowCommand : SW_HIDE;
    WindowPlace.rcNormalPosition = WindowRect;
    WindowPlace.ptMaxPosition = WindowOrigin;
    Window->SetWindowPlacement(&WindowPlace);
}

void GetWindowOrigin(CWnd* Window, const char* Section) 
{
    CRect WindowRect;
    
    Window->GetWindowRect(&WindowRect);
    int Width = WindowRect.Width();
    int Height = WindowRect.Height();

    WindowRect = GetConfigRect(Section, RectKey, WindowRect);

    // Make sure window is not out of screen
    int MaxX = GetSystemMetrics(SM_CXSCREEN) - Width;
    int MaxY = GetSystemMetrics(SM_CYSCREEN) - Height;
    if (WindowRect.left < 0)
        WindowRect.left = 0;
    if (WindowRect.left >= MaxX)
        WindowRect.left = MaxX;
    if (WindowRect.top < 0)
        WindowRect.top = 0;
    if (WindowRect.top >= MaxY)
        WindowRect.top = MaxY;

    WindowRect.right = WindowRect.left + Width;
    WindowRect.bottom = WindowRect.top + Height;

    Window->MoveWindow(WindowRect);
}

void WriteWindowPlacement(CWnd* Window, const char* Section)
{
    WINDOWPLACEMENT WindowPlace;
    
    WindowPlace.length = sizeof(WindowPlace);
    Window->GetWindowPlacement(&WindowPlace);
    WriteConfigRect(Section, RectKey, WindowPlace.rcNormalPosition);
    WriteConfigPoint(Section, OriginKey, WindowPlace.ptMaxPosition);
    //int ShowCommand = Window->IsWindowVisible() ? WindowPlace.showCmd : SW_HIDE;
    WriteConfigInt(Section, ShowKey, WindowPlace.showCmd);
}

//////////////////////////////////////////////////////////////////////////

std::string GetLineString(const char*& Line, const char* Default)
{
    const char* Comma = strchr(Line, ',');
    if (Comma == NULL)
        return Default;

    std::string String = std::string(Line, Comma - Line);
    Line = Comma + 1; // Extra 1 to skip comma
    return String;
}

int GetLineInt(const char*& Line, int Default)
{
    std::string String = GetLineString(Line, "");
    if (String.empty())
        return Default;

    return atoi(String.c_str());
}

void WriteLineString(std::string& Line, const char* Value)
{
    Line += std::string(Value) + ",";
}

void WriteLineInt(std::string& Line, int Value)
{
    CString String;

    String.Format("%d,", Value);
    Line += String;
}

void GetListSection(const char* Section, const char* KeyPrefix, 
                    std::vector<ConfigListItem>& Result,
                    int StartId, int EndId, int MaxGap)
{
    GetTheConfigIniFile().GetListSection(Section, KeyPrefix, Result,
                                         StartId, EndId, MaxGap);
}

void WriteListSection(const char* Section, const char* KeyPrefix, 
                      const std::vector<std::string>& List, int StartId)
{
    GetTheConfigIniFile().WriteListSection(Section, KeyPrefix, List, StartId);
}

void GetAllEntriesOfSection(const char* SectionName, std::vector<KeyValueEntry>& Result)
{
    GetTheConfigIniFile().GetAllEntriesOfSection(SectionName, Result);
}


bool ReadConfigArray(const CString Section, const char* KeyBaseStr,
                     int Array[], int Size, int& ActualSize)
{
    CString ArraySizeStr;
    ArraySizeStr.Format("%sSize", KeyBaseStr);

    int ConfigArraySize;
    ConfigArraySize = GetConfigInt(Section, ArraySizeStr, 0);

    if(ConfigArraySize>Size)
    {
        Assert(false);
        LogEvent(LE_ERROR, "ReadConfigArray: ConfigArraySize>Size (%d>%d)",
            ConfigArraySize, Size);
        return false;
    }

    CString ElementStr;
    for(ActualSize=0 ; ActualSize<ConfigArraySize ; ++ActualSize)
    {
        ElementStr.Format("%s%d", KeyBaseStr, ActualSize+1);
        Array[ActualSize] = GetConfigInt(Section, ElementStr, -1);

        if(Array[ActualSize]<0)
            break;
    }

    return true;
}

bool WriteConfigArray(const CString Section, const char* KeyBaseStr, 
                      const int Array[], int Size)
{
    int i;

    if(Section.IsEmpty())
        return true;

    CString ArraySizeStr;
    ArraySizeStr.Format("%sSize", KeyBaseStr);

    int ConfigArraySize;
    ConfigArraySize = WriteConfigInt(Section, ArraySizeStr, Size);

    CString ElementStr;
    for(i=0 ; i<Size ; ++i)
    {
        ElementStr.Format("%s%d", KeyBaseStr, i+1);
        WriteConfigInt(Section, ElementStr, Array[i]);
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////

#ifdef _TEST

//use test.ini for this test that its content is:
//  [TestGetAllEntries]
//  Key1=Value1
//  Key2=Value2
//  Key3=Value3

void TestConfig()
{
    SetConfigFileName("Test.Ini");

    std::vector<KeyValueEntry> Result;
    GetAllEntriesOfSection("TestGetAllEntries", Result);
    Assert(Result.size() == 3);
}

#endif