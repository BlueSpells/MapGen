// Utils.cpp: implementation of the Utils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Utils.h"
#include "LogEvent.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Copy a string up to a length and put null at the end
void StrCopy(char* Dest, const char* Source, int Size)
{
    strncpy_s(Dest, Size, Source, _TRUNCATE);
}

bool CompareNoCase(const std::string& Value, const char* Key)
{
    return _stricmp(Value.c_str(), Key) == 0;
}

bool SplitString(const char* StrToSplit, const char* Delimiter, 
                 std::string& FirstPart, std::string& SecondPart)
{
    const char* DelimiterPtr = strstr(StrToSplit, Delimiter);
    if (Delimiter == NULL)
    {
        LogEvent(LE_WARNING, "SplitString: Failed to split string [%s], Delimiter [%s] not found",
            StrToSplit, Delimiter);
        return false;
    }

    if (DelimiterPtr - StrToSplit <= 0)
    {
        LogEvent(LE_WARNING, "SplitString: Failed to split string [%s], No value before Delimiter [%s]",
            StrToSplit, Delimiter);
        return false;
    }

    FirstPart = "";
    FirstPart.insert(0, StrToSplit, DelimiterPtr - StrToSplit);

    int SecondPartStart = DelimiterPtr - StrToSplit + strlen(Delimiter);
    if ((unsigned int)SecondPartStart >= strlen(StrToSplit))
    {
        LogEvent(LE_WARNING, "SplitString: Failed to split string [%s], No value after Delimiter [%s]",
            StrToSplit, Delimiter);
        return false;
    }

    SecondPart = (StrToSplit + SecondPartStart);
    return true;
}


//////////////////////////////////////////////////////////////////////////
// Find the value of a parameter in a string of the format "Param1=value;param2=value;"
// If LogPrefix=="" no log events will be issued otherwise it will be the prefix of the Log message
bool FindParamValue(const char* StrToSearchOn, const char* ParamName,
                    const char* ParamsDelimiter, const std::string& LogPrefix,
                    std::string& ValueStr, ELogSeverity Severity)
{
    const char* ParamNamePtr = strstr(StrToSearchOn, ParamName);
    const char* StrEndPtr = StrToSearchOn +  strlen(StrToSearchOn);
    if (ParamNamePtr == NULL)
    {
        if (LogPrefix != "")
            LogEvent(Severity, "%s: Failed to Find parameter [%s] not found in string [%s]",
                      LogPrefix.c_str(), ParamName, StrToSearchOn);
        return false;
    }

    const char* ValueDelimiterPtr = ParamNamePtr + strlen(ParamName);

    //skip spaces on param name end
    while ((ValueDelimiterPtr < StrEndPtr) && (*ValueDelimiterPtr == ' '))
        ValueDelimiterPtr++;

    if (ValueDelimiterPtr >= StrEndPtr)
    {
        if (LogPrefix != "")
            LogEvent(Severity, "%s: No value for parameter [%s] in string [%s]",
                        LogPrefix.c_str(), ParamName, StrToSearchOn);
        return false;
    }

    const char ValueDelimiter = '=';
    if (*ValueDelimiterPtr != ValueDelimiter)
    {
        if (LogPrefix != "")
            LogEvent(Severity, "%s: No value delimiter for parameter [%s] in string [%s]",
                        LogPrefix.c_str(), ParamName, StrToSearchOn);
        return false;
    }

    const char* ValuePtr = ValueDelimiterPtr + 1; // move beyond the '=' sign

    //skip spaces on value start
    while ((ValuePtr < StrEndPtr) && (*ValuePtr == ' '))
        ValuePtr++;

    if (ValuePtr >= StrEndPtr)
    {
        if (LogPrefix != "")
            LogEvent(Severity, "%s: No value for parameter [%s] in string [%s]",
                       LogPrefix.c_str(), ParamName, StrToSearchOn);
        return false;
    }

    const char* ValueStartPtr = ValuePtr;
    ValuePtr += strcspn(ValueStartPtr, ParamsDelimiter);

    ValueStr = "";
    ValueStr.insert(0, ValueStartPtr, ValuePtr - ValueStartPtr);
    return true;
}

//////////////////////////////////////////////////////////////////////////
bool FindParamIntValue(const char* StrToSearchOn, const char* ParamName,
                       const char* ParamsDelimiter, const std::string& LogPrefix,
                       int& Value)
{
    std::string ValueString;
    if (FindParamValue(StrToSearchOn, ParamName, ParamsDelimiter,
                       LogPrefix, ValueString) == false)
        return false;

    Value = atoi(ValueString.c_str());
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Try to find the int value in the string, if fails assign default value.
void FindParamIntValueWithDefault(const char* StrToSearchOn, const char* ParamName,
                                  const char* ParamsDelimiter, const std::string& LogPrefix,
                                  int& Value, int DefaultValue)
{
    if (FindParamIntValue(StrToSearchOn, ParamName, ParamsDelimiter, LogPrefix, Value) == false)
        Value = DefaultValue;

    return;
}


//////////////////////////////////////////////////////////////////////////
bool FindParamBoolValue(const char* StrToSearchOn, const char* ParamName,
                        const char* ParamsDelimiter, const std::string& LogPrefix,
                        bool& Value)
{
    std::string ValueString;
    if (FindParamValue(StrToSearchOn, ParamName, ParamsDelimiter,
                       LogPrefix, ValueString) == false)
        return false;

    if (ValueString == "true" || ValueString == "TRUE" || ValueString == "1")
        Value = true;
    else if (ValueString == "false" || ValueString == "FALSE" || ValueString == "0")
        Value = false;
    else
    {
        if (LogPrefix != "")
            LogEvent(LE_WARNING, "%s: Bool parameter [%s] - The value [%s] is not valid",
                     LogPrefix.c_str(), ParamName, ValueString.c_str());
        return false;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////
void FindParamBoolValueWithDefault(const char* StrToSearchOn, const char* ParamName,
                                   const char* ParamsDelimiter, const std::string& LogPrefix,
                                   bool& Value, bool DefaultValue)
{
    if (FindParamBoolValue(StrToSearchOn, ParamName, ParamsDelimiter,
                           LogPrefix, Value) == false)
        Value = DefaultValue;
    return;
}


//////////////////////////////////////////////////////////////////////////
void GetStringTokens(const std::string& String, std::vector<std::string>& Tokens,
                     const std::string& Delimiters)
{
    // Skip delimiters at beginning. 
    std::string::size_type LastPos = String.find_first_not_of(Delimiters, 0); 
    // Find first "non-delimiter". 
    std::string::size_type Position = String.find_first_of(Delimiters, LastPos);

    while (std::string::npos != Position) 
    { 
        // Found a token, add it to the vector. 
        Tokens.push_back(String.substr(LastPos, Position - LastPos)); 
        // Skip delimiters.  Note the "not_of" 
        LastPos = String.find_first_not_of(Delimiters, Position); 
        // Find next "non-delimiter" 
        Position = String.find_first_of(Delimiters, LastPos); 
    }

    // Adding the last token
    if (LastPos != std::string::npos)
        Tokens.push_back(String.substr(LastPos));

    return;
}


void DumpData(ELogSeverity Severity, const char* Caption, const void* Data, int DataSize, bool HideNonPrintables /*= false*/)
{
    const int MaxLineSize = 256;
    char Line[MaxLineSize];
    char Line1[MaxLineSize];
    int Left = DataSize;
    const unsigned char* Current = reinterpret_cast<const unsigned char*>(Data);
    for (; Left > 0;)
    {
        const int LineSize = Left > 16 ? 16 : Left;
        for (int j = 0; j < LineSize; ++j, ++Current, --Left)
        {
            //sprintf_s(Line + j * 3, sizeof Line - j * 3, j == 7 ? "%02X-" : j == LineSize - 1 ? "%02X" : "%02X ", *Current);
            sprintf_s(Line + (j == 0 ? 0 : j * 3 - 1), sizeof Line - j * 3, j == 0 ? "%02X" : j == 8 ? "-%02X" : " %02X", *Current);
            sprintf_s(Line1 + j, sizeof Line - j, "%c", *Current == '\0' ? '.' : HideNonPrintables && *Current > 0 && *Current < 32  ? '.' : *Current);
        }
        LogEvent(Severity, "%s: [%-47s] [%-16s]", Caption, Line, Line1);
    }
}

void DumpData(const char* Caption, const void* Data, int DataSize, bool HideNonPrintables /*= false*/)
{
	DumpData(LE_INFOLOW, Caption, Data, DataSize, HideNonPrintables);
}

//#include <winsock2.h>

std::string GetHostName()
{
    const int MaxNameLength = 128;
    char Name[MaxNameLength];

    int Result = gethostname(Name, MaxNameLength);
    if (Result != 0)
	{
        LogEvent(LE_ERROR, "GetHostName: Error %d", GetLastError());
		return "";
	}

    return std::string(Name);
}

bool IsValidIp(const char* Ip, bool ShowMessageBox /*= false*/)
{
    if (strlen(Ip) == 0)
    {
        if (ShowMessageBox)
            AfxMessageBox("Empty Ip Address\n");
        return false;
    }
    struct hostent* HostEnt = gethostbyname(Ip);
    if (HostEnt == NULL)
    {
        if (ShowMessageBox)
            AfxMessageBox("Invalid or Non-existing Ip Address\n" + CString(Ip));
        return false;
    }
    return true;
}

DWORD GetHostAddressAsDWORD()
{
    std::string Name = GetHostName();
    if (Name.empty())
        return 0;
    
    struct hostent* HostEnt = gethostbyname(Name.c_str());
    DWORD Address = *(DWORD*)(*HostEnt).h_addr_list[0];
    return Address;
}

std::string GetHostAddress()
{
	//cache the host address, finding the address is a CPU consumpting operation
	static std::string HostAddress;
	if (!HostAddress.empty())
		return HostAddress;

    DWORD Address = GetHostAddressAsDWORD();
    if (Address == 0)
        return "";

    const int MaxAddressSize = 32;
    char AddressString[MaxAddressSize];

    sprintf_s(AddressString, "%d.%d.%d.%d",
        (Address & 0x000000FF),
        (Address & 0x0000FF00) >> 8,
        (Address & 0x00FF0000) >> 16,
        (Address & 0xFF000000) >> 24
    );
	HostAddress = AddressString;
    return AddressString;
}

bool GetStandardHostIp(const char* HostName, std::string & StandardHostAddress)
{
    hostent* HostInfo;

    HostInfo = gethostbyname(HostName);
    if (HostInfo == NULL)
        return false;

    DWORD Address = *(DWORD*)(*HostInfo).h_addr_list[0];
    StandardHostAddress = GetStandardIpFromDWORD(Address);
    return true;
}

// The IpInDWORD should be network order.
std::string GetStandardIpFromDWORD(DWORD IpInDWORD)
{
    char AddressString[32];
    sprintf_s(AddressString, "%d.%d.%d.%d",
        (IpInDWORD & 0x000000FF),
        (IpInDWORD & 0x0000FF00) >> 8,
        (IpInDWORD & 0x00FF0000) >> 16,
        (IpInDWORD & 0xFF000000) >> 24);
    return AddressString;
}

// The result is DWORD in network order
// It seems that this function is very costly in performance
DWORD GetHostIPAsDWORD(const char* HostName)
{
	hostent* HostInfo;

	HostInfo = gethostbyname(HostName);
	if (HostInfo == NULL)
		return 0;

	DWORD Address = *(DWORD*)(*HostInfo).h_addr_list[0];
	return Address;
}

// returns true if both strings represent the same host, strings
// can be ip address or the computer name. returns false otherwise.
bool CompareHostNames(const char* NameLeft, const char* NameRight)
{
	if (0 == strcmp(NameLeft, NameRight))
		return true;

	in_addr AddressLeft, AddressRight;
	memset(&AddressLeft, 0, sizeof in_addr);
	memset(&AddressRight, 0, sizeof in_addr);

	// gethostbyname() uses only one internal structure
	// therefore we must copy its result between successive calls.
	hostent* HostInfo;

	HostInfo = gethostbyname(NameLeft);	
	if (NULL != HostInfo)
		AddressLeft = *((in_addr*)HostInfo->h_addr_list[0]);

	HostInfo = gethostbyname(NameRight);
	if (NULL != HostInfo)
		AddressRight = *((in_addr*)HostInfo->h_addr_list[0]);

	return AddressLeft.S_un.S_addr == AddressRight.S_un.S_addr;
}

int sign(int Value)
{
   if (Value < 0) return -1;
   if (Value > 0) return  1;
   return 0;
}

char HexDigit(int Value)
{
    const char Digits[] = "0123456789ABCDEF";
    const int DigitsSize = sizeof Digits / sizeof Digits[0];
    if (Value >= 0 && Value < DigitsSize)
        return Digits[Value];
    else
        return '?';
}

unsigned char HexValue(char Digit)
{
    if (Digit >= '0' && Digit <= '9')
        return Digit - '0';

    if (Digit >= 'A' && Digit <= 'F')
        return Digit - 'A' + 10;

    if (Digit >= 'a' && Digit <= 'f')
        return Digit - 'a' + 10;

    return 0;
}

std::string HexPrint(const UCHAR* Buffer, UINT BufferLength, UINT LineLength /*= 32*/, bool AddSpaces /*= true*/)
{
	std::string Output;
	for (UINT i=0;i<BufferLength;i++)
	{
        if (AddSpaces && (i > 0))
        {
				if (i % LineLength == 0)
					Output.append(1, '\n');
				else
					Output.append(1, ' ');
        }

        Output.append(1, HexDigit(Buffer[i] >> 4));
		Output.append(1, HexDigit(Buffer[i] & 0xF));
	}

	return Output;
}

void BufferToString(const void* Data, UINT DataLength, TCHAR* szDest, UINT DestLength)
{
    if (DestLength <= DataLength)
        DataLength = DestLength - 1;
    memcpy(szDest, Data, DataLength);
    szDest[DataLength] = '\0';
}

void StringToBuffer(const TCHAR* Str, BYTE* Data, UINT& len)
{
	len = strlen(Str);
	memcpy(Data, Str, len);
	Data[len] = '\0';
}

int HexStrPartToInt(const char* data, int len)
{
	char szTmp[10];
	Assert(len < sizeof(szTmp));

	if (len >= sizeof(szTmp))
		return 0;
    BufferToCharArray(data, len, szTmp);

    int valTotal = 0;
    sscanf_s(szTmp, "%x", &valTotal);
    return valTotal;
}

int ConvertHexToData(
    char* const Target, const int MaxTargetSize,
    const char* const HexSource, const int HexSourceSize)
{
    int i =0, TargetSize = 0;
    while (i < HexSourceSize)
    {
        int IntValue, CharsRead, ValuesRead;
        ValuesRead = sscanf_s(&HexSource[i], "%2x%n", &IntValue, &CharsRead);
        if (ValuesRead == 1)
        {
            if (TargetSize >= MaxTargetSize)
                return 0;
            Target[TargetSize] = (char)IntValue;
            TargetSize++;
        }
        if (ValuesRead == EOF)
            break;
        i += CharsRead;
    }
    return TargetSize;
}

void StringToWString(const std::string& Src, std::wstring& Target)
{
    // using mbstowcs to convert the string.

    // Null call to get the size
    //size_t TargetSizeNeeded = mbstowcs(NULL, &Src[0], Src.length());
    size_t TargetSizeNeeded;
    errno_t err = mbstowcs_s(&TargetSizeNeeded, NULL, 0, &Src[0], Src.length());
    Assert (err == 0);
    if (err != 0)
    {
        LogEvent(LE_ERROR, "StringToWString: mbstowcs_s error %d while computing needed buffer size", err);
        return;
    }
    // Allocating the needed size for the wstring
    Target.resize(TargetSizeNeeded);

    // Converting the String

    //mbstowcs(&Target[0], &Src[0], Src.length());
    size_t ActualTargetSize;
    err = mbstowcs_s(&ActualTargetSize, &Target[0], TargetSizeNeeded, &Src[0], Src.length());
    Assert (err == 0);
    Assert (ActualTargetSize == TargetSizeNeeded);
    if (err != 0)
        LogEvent(LE_ERROR, "StringToWString: mbstowcs_s error %d while converting", err);

    return;
}

void InvertStr(const std::string& Str, std::string& InvertedStr)
{
    std::string TempStr;
    size_t Size = Str.size();
    InvertedStr.resize(Size);
    for (size_t i = 0; i < Size; ++i)
        InvertedStr[i] = Str[Size-1-i];
}

void WaitAndPumpMessages(DWORD WaitTimeInMilli)
{
    DWORD StartTicks = GetTickCount();
    for (;;)
    {
        MSG Msg;
        while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
        {
#ifdef _DEBUG
            if (AfxGetThreadState()->m_nDisablePumpCount != 0 || !AfxGetThread()->PumpMessage())
#else
            if (!AfxGetThread()->PumpMessage())
#endif
                return;
			if (GetTickCount() - StartTicks >= WaitTimeInMilli)
				break;
        }

		Sleep(10);
        if (GetTickCount() - StartTicks >= WaitTimeInMilli)
            break;
    }
}

std::string GetFullPathName(const std::string FileName)
{
    const int FullPathNameSize = 256;
    char FullPathName[FullPathNameSize];
    char* FilePart;
	
    DWORD Result = GetFullPathName(FileName.c_str(), FullPathNameSize, FullPathName, &FilePart);
    if (Result > 0)
        return FullPathName;
    else
        return FileName;
}

bool DoCreateDir(LPCSTR Dir)
{
    if (CreateDirectory(Dir, NULL))
        return true;
    DWORD Err = GetLastError();
    if (Err == ERROR_ALREADY_EXISTS)
        return true;
    if (Err != ERROR_PATH_NOT_FOUND)
        return false;

	// Not replaced with std::string since it will require
	// a modification of the entire function. (Allocation and
	// deallocation both take place inside the function)
    char* path = _strdup(Dir);
    if (path == NULL)
        return false;

    int Len = strlen(path);
    if (path[Len-1] == '\\')
    {
        path[Len-1] = '\0';
        --Len;
    }
    char* Ptr;
    int i;
    bool Ret = false;
    for (i=Len-1, Ptr=path+i; i>=0; --i, --Ptr)
    {
        if (*Ptr == ':')
            break;
        if (*Ptr == '\\'  ||  *Ptr == '/')
        {
            *Ptr = '\0';
            if (CreateDirectory(path, NULL))
                break;
            if (GetLastError() != ERROR_PATH_NOT_FOUND)
                goto Exit;
        }
    }
    for (; i<Len; ++i, ++Ptr)
    {
        if (*Ptr == '\0')
        {
            *Ptr = '\\';
            if (!CreateDirectory(path, NULL))
                goto Exit;
        }
    }
    Ret = true;
Exit:
    free(path);
    return Ret;
}

//////////////////////////////////////////////////////////////////////

void LogEventToDebugOutput(const char* Format, ...)
{
	va_list Args;
	va_start(Args, Format);

	char Message[MAX_LOG_MESSAGE_SIZE];

	vsnprintf_s(Message, sizeof Message, _TRUNCATE, Format, Args);
	OutputDebugString(Message);
}

#define _USE_32BIT_TIME_T
#include <sys/timeb.h>

void GetSecondsAndMillis(time_t& Seconds, int& Millis)
{
	// Get milliseconds
	_timeb TimeB;
	Verify(_ftime_s(&TimeB) == 0);

	Seconds = TimeB.time;//time(NULL);
	Millis = TimeB.millitm;
	// Assert(Seconds == TimeB.time);
}

std::string FormatTime(time_t Seconds, int Millis)
{
	char TimeString[128];

	//struct tm* Tm = localtime(&Seconds);
    struct tm Tm;
    Verify(localtime_s(&Tm, &Seconds) == 0);
	const char* Format = "%d.%m.%y %H:%M:%S."; // "%a, %b %d, %Y %H:%M:%S."
	strftime(TimeString, sizeof TimeString, Format, &Tm);

	char MilliString[5];
	sprintf_s(MilliString, "%03u", Millis);

	return std::string(TimeString) + MilliString;
}

time_t ConvertFileTimeToTime(const FILETIME& FileTime)
{
	SYSTEMTIME FileSystemTime;

	FileTimeToSystemTime(&FileTime, &FileSystemTime);

	struct tm Tm;
	Tm.tm_sec   = FileSystemTime.wSecond;
	Tm.tm_min   = FileSystemTime.wMinute;
	Tm.tm_hour  = FileSystemTime.wHour;
	Tm.tm_mday  = FileSystemTime.wDay;
	Tm.tm_mon   = FileSystemTime.wMonth - 1;        // tm_mon is 0 based
	Tm.tm_year  = FileSystemTime.wYear - 1900;      // tm_year is 1900 based
	Tm.tm_isdst = 0;
    long timezone;
    Verify (_get_timezone(&timezone) == 0);
	return mktime(&Tm) - timezone;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CFileFinder

CFileFinder::CFileFinder() : m_hFind(INVALID_HANDLE_VALUE)
{
}

CFileFinder::~CFileFinder()
{
    Close();
}

bool CFileFinder::GetNextFile(LPCSTR FileName, WIN32_FIND_DATA* pFindData)
{
    do
    {
        if (FileName != NULL)
        {
            Close();
            m_FileName = FileName;
            m_hFind =  FindFirstFile(FileName, pFindData);
            FileName = NULL;
        }
        else if (!FindNextFile(m_hFind, pFindData))
        {
            Close();
        }
        
        if (strcmp(pFindData->cFileName, ".") && strcmp(pFindData->cFileName, ".."))
            break;
    }
    while (m_hFind != INVALID_HANDLE_VALUE);
    
    return m_hFind != INVALID_HANDLE_VALUE;
}

void CFileFinder::Close()
{
    if (m_hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(m_hFind);
        m_hFind = INVALID_HANDLE_VALUE;
    }
}

//////////////////////////////////////////////////////////////////////////

std::vector<std::string> CollectFileName(LPCSTR FileName, bool DirOnly)
{
	Assert(NULL != FileName);
    
	std::vector<std::string>	FileList;
	WIN32_FIND_DATA				FindData;
    CFileFinder					FileFinder;
    bool						FileFound;

    for (FileFound = FileFinder.GetNextFile(FileName, &FindData);
        FileFound;
        FileFound = FileFinder.GetNextFile(NULL, &FindData))
        {
			if (DirOnly)
			{
				if (FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
					FileList.push_back(FindData.cFileName);
			}
			else
				FileList.push_back(FindData.cFileName);

        }
    std::sort(FileList.begin(), FileList.end());
    return FileList;
}

#include "afxdlgs.h"

bool OpenOrSaveFile(CString& FileName, bool Open, const char* Filter)
{
    // Set dome reasonable default value
    DWORD Flags = 
        (Open ? OFN_FILEMUSTEXIST : OFN_OVERWRITEPROMPT) | OFN_HIDEREADONLY;
    
    CFileDialog FileDlg(Open, NULL /* Def Ext */, FileName, 
        Flags, Filter, NULL/* Parent */);
    
    if (FileDlg.DoModal() != IDOK)
        return FALSE;
    
    FileName = FileDlg.GetPathName();
    return TRUE;
}

bool IsFileExists(const char* FileName)
{
    CFileFind  Finder;
    return Finder.FindFile(FileName) == TRUE;
}

CString FormatCurrentTime(SYSTEMTIME* InLocalTime)
{
    SYSTEMTIME LocalTime;
    GetLocalTime(&LocalTime);

    CString Str;
    Str.Format(_T("%02d:%02d:%02d.%02d"), 
        LocalTime.wHour, LocalTime.wMinute, 
        LocalTime.wSecond, LocalTime.wMilliseconds / 10);

    if (InLocalTime != NULL)
        *InLocalTime = LocalTime;

    return Str;
}

CString GetProgramFullName()
{
#if 01
    char FileName[MAX_PATH];
    GetModuleFileName(NULL, FileName, sizeof FileName);
    return FileName;
#else
    CString CommandLine = GetCommandLine();
    CommandLine.Trim();
    int Place = CommandLine.Find("\"");
    int Place1 = -1;
    if (Place == 0)
    {
        ++Place;
        Place1 = CommandLine.Find("\"", Place);
    }
    else
    {
        Place = 0;
        Place1 = CommandLine.Find(" ", Place);
    }
    if (Place1 < 0)
        Place1 = CommandLine.GetLength() + 1;
    CString ProgramFullName;
    ProgramFullName = CommandLine.Mid(Place, Place1-1);
    return ProgramFullName;
#endif
}

std::string GetFileFolder(const char* FileNamePath)  //returns the path of the folder containing the file
{
    CString FileFullName(FileNamePath);
    CString FolderPath;
    int Place = FileFullName.ReverseFind('\\');
    if (Place >= 0)
        FolderPath = FileFullName.Mid(0, Place+1);
    else
        FolderPath = "";
    return std::string((const char*)FolderPath);
}

CString GetProgramPath()
{
    CString ProgramFullName = GetProgramFullName();
    std::string ProgramPath = GetFileFolder(ProgramFullName);
    return CString(ProgramPath.c_str());
}

void PrintData(const unsigned char* Data, int Size, CString Str)
{
    int i;
    LogEvent(LE_DEBUG, "PrintData(%s): Data:\n", Str);
    for (i=0 ; i<Size ; ++i)
    {
        LogEvent(LE_DEBUG, "    %d.  0x%02X \n", i, Data[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
//  Audio manipulation functions
//////////////////////////////////////////////////////////////////////////

//expect the 2 mono buffers with size of at least half the size of the 
//stereo buffer
bool SplitStereo16BitBuffer(const BYTE* StereoBuffer, int StereoBufferSize, 
							BYTE* LeftChannelBuffer, BYTE* RightChannelBuffer)
{
    int MonoIndex = 0;
    int StereoIndex = 0;
	for (;StereoIndex < StereoBufferSize - 3;)
	{
		LeftChannelBuffer[MonoIndex] = StereoBuffer[StereoIndex];
		LeftChannelBuffer[MonoIndex + 1] = StereoBuffer[StereoIndex + 1];
		RightChannelBuffer[MonoIndex] = StereoBuffer[StereoIndex + 2];
		RightChannelBuffer[MonoIndex + 1] = StereoBuffer[StereoIndex + 3];
		StereoIndex += 4;
		MonoIndex +=2;
	}
    return true;
}

void SetAudioVolume(const BYTE* const SourceAudioBuffer, BYTE* const DestinationAudioBuffer, 
                    int AudioSizeInBytes, 
                    int BitsPerSample, 
                    int Volume //0 -100
                   ) 
{
    Assert(BitsPerSample == 8 || BitsPerSample == 16);

    double VolumeFactor = (double)Volume / 100;
    if (BitsPerSample == 8)
    {
        for (int i = 0 ; i < AudioSizeInBytes; i++)
        {
            //volume is with shift of 128
            signed char Temp  =  (signed char) (SourceAudioBuffer[i] - MONO_8_BIT_VOLUME_SHIFT);
            signed char VolumeDiff = (signed char)(Temp * VolumeFactor);
            DestinationAudioBuffer[i] =  (BYTE) (MONO_8_BIT_VOLUME_SHIFT + VolumeDiff);
        }
    }
    else if (BitsPerSample == 16)
    {
        short* SourcePtr = (short*)SourceAudioBuffer;
        short* DestPtr = (short*)DestinationAudioBuffer;
        for (int i = 0 ; i < AudioSizeInBytes / 2 ; i++)
            DestPtr[i] = (short) (VolumeFactor * ((short)SourcePtr[i]));
    }
    return;
}

void Mono16BitLinearToStereo(const short* MonoBuffer, int MonoBufferSize,
                             short* StereoBuffer, EConvertMonoToStereoType ConvertType)
{
    int StereoIndex = 0;
    int MonoIndex = 0;
    short FirstChannelMask = 0;
    if (ConvertType == STEREO_BOTH_CHANNELS ||
        ConvertType == STEREO_SECOND_CHANNEL_EMPTY)
        FirstChannelMask = 1;

    short SecondChannelMask = 0;
    if (ConvertType == STEREO_BOTH_CHANNELS ||
        ConvertType == STEREO_FIRST_CHANNEL_EMPTY)
        SecondChannelMask = 1;

    for (; MonoIndex < MonoBufferSize;)
    {
        StereoBuffer[StereoIndex] = MonoBuffer[MonoIndex] * FirstChannelMask;
        StereoBuffer[StereoIndex + 1] = MonoBuffer[MonoIndex] * SecondChannelMask;
        StereoIndex += 2;
        MonoIndex += 1;
    }
}

void Mono8BitLinearToStereo(const BYTE* MonoBuffer, int MonoBufferSize,
                            BYTE* StereoBuffer, EConvertMonoToStereoType ConvertType)
{
    int StereoIndex = 0;
    int MonoIndex = 0;
    short FirstChannelMask = 0;
    if (ConvertType == STEREO_BOTH_CHANNELS ||
        ConvertType == STEREO_SECOND_CHANNEL_EMPTY)
        FirstChannelMask = 1;

    short SecondChannelMask = 0;
    if (ConvertType == STEREO_BOTH_CHANNELS ||
        ConvertType == STEREO_FIRST_CHANNEL_EMPTY)
        SecondChannelMask = 1;

    for (; MonoIndex < MonoBufferSize;)
    {
        StereoBuffer[StereoIndex] =  FirstChannelMask? MonoBuffer[MonoIndex] : (BYTE) MONO_8_BIT_VOLUME_SHIFT;
        StereoBuffer[StereoIndex + 1] = SecondChannelMask? MonoBuffer[MonoIndex] : (BYTE) MONO_8_BIT_VOLUME_SHIFT;
        StereoIndex += 2;
        MonoIndex += 1;
    }
}


//////////////////////////////////////////////////////////////////////////
// helper function for converting formats of IP and handling IP control
std::string IpByteFormat2Str(BYTE (&IpByteFormat)[4])
{
    char IpFormatStr[30];
    sprintf_s(IpFormatStr, "%d.%d.%d.%d", 
        IpByteFormat[0],
        IpByteFormat[1],
        IpByteFormat[2],
        IpByteFormat[3]);
    return IpFormatStr;
}

bool IpStrFormat2IpByteFormat(const char* IpStrFormat, BYTE (&OutIpByteFormat)[4])
{
    int IpParts[4];
    std::string StandardIp;
    if (!GetStandardHostIp(IpStrFormat, StandardIp))
    {
        LogEvent(LE_ERROR, "IpStrFormat2IpByteFormat Invalid IP Address [%s]", IpStrFormat);
        return false;
    }

    int NumIpParts = sscanf_s(StandardIp.c_str(), "%d.%d.%d.%d", 
                            &IpParts[0], &IpParts[1], 
                            &IpParts[2], &IpParts[3]);
    if (NumIpParts != 4)
    {
        LogEvent(LE_ERROR, "IpStrFormat2IpByteFormat Invalid IP Address [%s]",
            IpStrFormat);
        return false;
    }

    for (int i =0; i < 4; ++i)
        OutIpByteFormat[i] = (BYTE) IpParts[i];

    return true;
}

bool IpStr2DWORD(const char* IpStrFormat, DWORD& DwordIp)
{
    DWORD IpParts[4];
    std::string StandardIp;
    if (!GetStandardHostIp(IpStrFormat, StandardIp))
    {
        LogEvent(LE_ERROR, "IpStr2DWORD Invalid IP Address [%s]", IpStrFormat);
        return false;
    }

    int NumIpParts = sscanf_s(StandardIp.c_str(), "%d.%d.%d.%d", 
                            &IpParts[0], &IpParts[1], 
                            &IpParts[2], &IpParts[3]);

    if (NumIpParts != 4)
    {
        LogEvent(LE_ERROR, "IpStr2DWORD(), Invalid IP Address [%s]",
            IpStrFormat);
        return false;
    }
   // DwordIp = (IpParts[0]<<24) + (IpParts[1]<<16) + (IpParts[2]<<8) + (IpParts[3]);
    //Network order
    DwordIp = (IpParts[3]<<24) + (IpParts[2]<<16) + (IpParts[1]<<8) + (IpParts[0]);
    return true;
}

std::string DwordIpFormat2Str(DWORD Address)
{
    const int MaxAddressSize = 32;
    char AddressString[MaxAddressSize];

    sprintf_s(AddressString, "%d.%d.%d.%d",
        (Address & 0x000000FF),
        (Address & 0x0000FF00) >> 8,
        (Address & 0x00FF0000) >> 16,
        (Address & 0xFF000000) >> 24
        );

    return AddressString;
}

void SetIpCtlAddress(CIPAddressCtrl& Ctl, const char* IpStr)
{
    BYTE IpByteFormat[4];

    memset(IpByteFormat, 0, sizeof IpByteFormat);
    IpStrFormat2IpByteFormat(IpStr, IpByteFormat);
    Ctl.SetAddress(IpByteFormat[0], IpByteFormat[1], 
        IpByteFormat[2], IpByteFormat[3]);
}

std::string GetIpCtlAddress(CIPAddressCtrl& Ctl)
{
    BYTE IpAddress[4];
    char IpStr[30];

    Ctl.GetAddress(IpAddress[0], IpAddress[1], IpAddress[2], IpAddress[3]);
    sprintf_s(IpStr, "%d.%d.%d.%d", IpAddress[0], IpAddress[1], IpAddress[2], IpAddress[3]);
    return IpStr;
}

bool IsValidIpAddress(const char* Ip)
{
    int IpParts[4];
    std::string StandardIp;
    if (!GetStandardHostIp(Ip, StandardIp))
    {
        LogEvent(LE_ERROR, "IsValidIpAddress Invalid IP Address [%s]", Ip);
        return false;
    }

    int NumIpParts = sscanf_s(StandardIp.c_str(), "%d.%d.%d.%d", 
                            &IpParts[0], &IpParts[1], 
                            &IpParts[2], &IpParts[3]);
    if (NumIpParts != 4)
        return false;

    //first part cannot be zero
    if (IpParts[0] <= 0)
        return false;

    for (int i = 0; i < 4; ++i)
    {
        if (IpParts[i] > 255 || IpParts[i] < 0)
            return false;
    }
    return true;
}


void PrintWin32Error(DWORD ErrorCode)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, ErrorCode, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf, 0, NULL);
	printf("%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
}

std::string GetWin32Error(DWORD ErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, ErrorCode, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf, 0, NULL);

	std::string RetVal((LPTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return RetVal;
}

std::string GetSystemErrorString(DWORD Error)
{
    char* lpMsgBuf;
    DWORD NumChars = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, Error, 0, (LPTSTR) &lpMsgBuf, 0, NULL);
    if (lpMsgBuf[NumChars-1] == '\n'  ||  lpMsgBuf[NumChars-1] == '\r')
    {
        lpMsgBuf[NumChars-1] = '\0';
        if (lpMsgBuf[NumChars-2] == '\n'  ||  lpMsgBuf[NumChars-2] == '\r')
            lpMsgBuf[NumChars-2] = '\0';
    }
    std::string ErrorString(lpMsgBuf);
    LocalFree(lpMsgBuf);
    return ErrorString;
}

void LogGetLastError(const char* Context)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        0, // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );
    // Display the string.
    LogEvent(LE_ERROR, "%s : %s", Context, lpMsgBuf);

    // Free the buffer.
    LocalFree(lpMsgBuf);
}

const char* BooleanStr(bool Val, bool Uppercase /*= false*/)
{
	if (Val)
        return Uppercase ? "TRUE" : "true";
	else
        return Uppercase ? "FALSE" : "false";
}

//The GetAllFiles function retrieves the file names found under a
//specific path recursively. return false if directory not exists
bool GetAllFiles(const char* BasePath, bool Recursive,
                 std::vector<std::string>& FileNames, const char* Pattern /* = "*" */)
{
    FileNames.clear();
    CFileFind Finder;
    CString DirSpec(BasePath);
    int DirSpecLength = DirSpec.GetLength();
    if (DirSpecLength == 0)
        return false;

    // Removing the last '\' if it is the last character. Otherwise find file will search
    // for a null ("") directory, and will fail
    int LastChar = DirSpecLength - 1;
    if (DirSpec.GetAt(LastChar) == '\\')
        DirSpec.Truncate(LastChar); // removing last char

    // First find if the directory exists (Find without the pattern because then
    // couldn't find "." and ".." to ensure directory not exists)
    BOOL Found = Finder.FindFile(DirSpec);
    if (Found == FALSE) // Means the dir not exists. If exists than should had found "." and ".."
        return false;

    // Directory exists (Return value of this function will be true)
    DirSpec += CString("\\") + Pattern;
    Found = Finder.FindFile(DirSpec);

    while (Found)
    {
        Found = Finder.FindNextFile();
        if (Finder.IsDirectory())
        {
            if (Recursive == false)
                continue;

            //go over directories recursively
			CString FoundFileName  = Finder.GetFileName();
            if (FoundFileName == "." ||
                FoundFileName == "..")
                continue;
            CString DirPath = CString(BasePath) + "\\" + FoundFileName;
            GetAllFiles(DirPath, true, FileNames, Pattern);
        }
        else
        {
            //add the file.
            std::string FileName = (const char*)Finder.GetFilePath();
            FileNames.push_back(FileName);
        }
    }
	return true;
}

bool IsNumber(const char* Str)
{
    const char* Ptr = Str;
    while (*Ptr == ' ' || *Ptr == '\t' || *Ptr == '-' || *Ptr == '+')
        ++Ptr;
    if (!isdigit(*Ptr))
        return false;
    while (isdigit(*Ptr))
        ++Ptr;
    for (; *Ptr != '\0'; ++Ptr)
        if (*Ptr != ' ' && *Ptr != '\t' && *Ptr != '\n' && *Ptr != '\r')
            return false;
    return true;
}

bool IsNumeric(const char* Str)
{
    //return true for integral and floating numbers
    const char* Ptr = Str;
    while (*Ptr == ' ' || *Ptr == '\t' || *Ptr == '-' || *Ptr == '+')
        ++Ptr;
    if (!isdigit(*Ptr))
        return false;
    while (isdigit(*Ptr) || *Ptr == '.')
        ++Ptr;
    for (; *Ptr != '\0'; ++Ptr)
        if (*Ptr != ' ' && *Ptr != '\t' && *Ptr != '\n' && *Ptr != '\r')
            return false;
    return true;

}

std::string IntToStr(int Value)
{
    char ValueStr[50];
    sprintf_s(ValueStr, "%d", Value);
    return ValueStr;
}

// Parses a string of the format "n1,...,n2-n3,..." to a vector of all the numbers defined by the string
void ParseIntVectorString(const char* Str, std::vector<int> & IntVector)
{
    IntVector.clear();
    CString Str1 = Str;
    CString Str2;

    while (!Str1.IsEmpty())
    {
        int Place = Str1.Find(',');
        if (Place >= 0)
        {
            Str2 = Str1.Left(Place);
            Str1 = Str1.Mid(Place+1);
        }
        else
        {
            Str2 = Str1;
            Str1 = "";
        }

        Place = Str2.Find('-', 1);
        if (Place >= 1)
        {
            int Num1 = atoi(Str2.Left(Place));
            int Num2 = atoi(Str2.Mid(Place+1));
            for (int i= Num1; i<=Num2; ++i)
                IntVector.push_back(i);
        }
        else
        {
            int Num = atoi(Str2);
            IntVector.push_back(Num);
        }
    }
}

std::string IntVectorToStr(const std::vector<int>& IntVector, bool UseSpaces /*= false*/)
{
    const char* Separator1 = UseSpaces ? ", " : ",";
    const char* Separator2 = UseSpaces ? " - " : "-";

    int VectorSize = IntVector.size();
    int LastInt = -1, LastPrintedInt = -1;
    std::string Str;
    char Buf[32];
    for (int i = 0; i < VectorSize; ++i)
    {
        int IntNum = IntVector[i];
        if (i == 0)
        {
            Str = OurItoa(IntNum, Buf, 10);
            LastPrintedInt = IntNum;
        }
        else if (IntNum != LastInt + 1)
        {
            if (LastInt != LastPrintedInt)
            {
                if (LastInt == LastPrintedInt + 1)
                    Str += Separator1;
                else
                    Str += Separator2;
                Str += OurItoa(LastInt, Buf, 10);
            }
            Str += Separator1;
            Str += OurItoa(IntNum, Buf, 10);
            LastPrintedInt = IntNum;
        }
        else if (i == VectorSize - 1)
        {
            if (IntNum == LastPrintedInt + 1)
                Str += Separator1;
            else
                Str += Separator2;
            Str += OurItoa(IntNum, Buf, 10);
        }
        LastInt = IntNum;
    }
    return Str;
}

std::string IntArrayToStr(const int Vals[], size_t NumVals)
{
    std::vector<int> ValVec;
    ValVec.resize(NumVals);
    for (size_t i = 0; i < NumVals; ++i)
        ValVec[i] = Vals[i];
    return IntVectorToStr(ValVec);
}

void TraceIntVector(ELogSeverity Severity, const char* Context, const char* Lable, const std::vector<int>& IntVector)
{
    LogEvent(Severity, "%s: %s %s", Context, Lable, IntVectorToStr(IntVector).c_str());
}

void TraceIntArray(ELogSeverity Severity, const char* Context, const char* Lable, const int Vals[], size_t NumVals)
{
    std::vector<int> ValVec;
    ValVec.resize(NumVals);
    for (size_t i = 0; i < NumVals; ++i)
        ValVec[i] = Vals[i];
    TraceIntVector(Severity, Context, Lable, ValVec);
}

std::string FloatToStr(float Value, int Precision /*= -1*/)
{
    char ValueStr[50];
    if (Precision < 0)
        sprintf_s(ValueStr, "%f", Value);
    else
        sprintf_s(ValueStr, "%.*f", Precision, Value);
    return ValueStr;
}

std::string FloatArrayToStr(const float FloatArray[], size_t FloatArraySize, int Precision /*= -1*/, bool UseSpaces /*= false*/)
{
    const char* Separator = UseSpaces ? ", " : ",";

    std::string Str;
    for (size_t i = 0; i < FloatArraySize; ++i)
    {
        if (i > 0)
            Str += Separator;
        Str += FloatToStr(FloatArray[i], Precision);
    }
    return Str;
}

int ThreadPriorityStr2Value(const char* Priority)
{
    if (CompareNoCase(Priority, "ABOVE_NORMAL"))
        return THREAD_PRIORITY_ABOVE_NORMAL;
    if (CompareNoCase(Priority, "BELOW_NORMAL"))
        return THREAD_PRIORITY_BELOW_NORMAL;
    if (CompareNoCase(Priority, "HIGHEST"))
        return THREAD_PRIORITY_HIGHEST;
    if (CompareNoCase(Priority, "IDLE"))
        return THREAD_PRIORITY_IDLE;
    if (CompareNoCase(Priority, "LOWEST"))
        return THREAD_PRIORITY_LOWEST;
    if (CompareNoCase(Priority, "NORMAL"))
        return THREAD_PRIORITY_NORMAL;
    if (CompareNoCase(Priority, "TIME_CRITICAL"))
        return THREAD_PRIORITY_TIME_CRITICAL;
    if (IsNumber(Priority))
        return atoi(Priority);

    LogEvent(LE_ERROR, "ThreadPriorityStr2Value : Unknown priority string %s, return normal.",
        Priority);
    return THREAD_PRIORITY_NORMAL;
}

int PriorityClassStr2Value(const char* Priority)
{
    if (CompareNoCase(Priority, "NORMAL"))
        return NORMAL_PRIORITY_CLASS;
    if (CompareNoCase(Priority, "IDLE"))
        return IDLE_PRIORITY_CLASS;
    if (CompareNoCase(Priority, "HIGH"))
        return HIGH_PRIORITY_CLASS;
    if (CompareNoCase(Priority, "REALTIME"))
        return REALTIME_PRIORITY_CLASS;

    LogEvent(LE_ERROR, "PriorityClassStr2Value : Unknown priority string %s, return normal.",
        Priority);
    return NORMAL_PRIORITY_CLASS;
}

bool FileExtensionIs(const char* const FileName, const char* const Extension)
{
	if (NULL == FileName || NULL == Extension)
		return false;

	const size_t FileNameLength	 = strlen(FileName);
	const size_t ExtensionLength = strlen(Extension);
	if (0 == FileNameLength || 0 == ExtensionLength)
		return false;

	const char* FindIndex = strstr(FileName, Extension);
	if (NULL == FindIndex)
		return false;

	size_t NameLength = FindIndex - FileName;
	return NameLength + ExtensionLength == FileNameLength; 
}

bool AreFilesEqual(const char* LeftFileName, const char* RightFileName)
{
    std::ifstream LeftStream;
    std::ifstream RightStream;

    LeftStream.open(LeftFileName);
    RightStream.open(RightFileName);

    bool Success = true;
    while (LeftStream.good() && RightStream.good())
        if (LeftStream.get() != RightStream.get())
            break;
    if (LeftStream.good() || RightStream.good())
        Success = false;

    return Success;
}

void DeleteOldFiles(const char* WorkingDirectory, const char* WildCardCriteria, const char* FileExtention, ULARGE_INTEGER MaxTotalSize, int MaxFileLiveTime, const char* ActiveFileName)
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    ULARGE_INTEGER  TotalDiskUsage;
    typedef std::map<std::string, ULARGE_INTEGER> FileNameAndSizeMap;
    FileNameAndSizeMap MatchingFiles;
    TotalDiskUsage.QuadPart = 0;
    time_t CurrentTime = time(NULL);

    std::string strWorkingDirectory(WorkingDirectory);
    std::string strWildCardCriteria(WildCardCriteria);
    std::string strActiveFileName(ActiveFileName);

    std::string WorkingDirectoryWithWildCard = strWorkingDirectory + "\\" +strWildCardCriteria + FileExtention;
    hFind = FindFirstFile(WorkingDirectoryWithWildCard.c_str(), &FindData);
    if (hFind == INVALID_HANDLE_VALUE) // No file
        return;

    do
    {
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        if (!strstr(FindData.cFileName, FileExtention))
            continue;

        time_t FileLastWriteTime = ConvertFileTimeToTime(FindData.ftLastWriteTime);
        std::string MatchingFileName = strWorkingDirectory + "\\" + FindData.cFileName;
        if (CurrentTime - FileLastWriteTime > MaxFileLiveTime)
            DeleteFile(MatchingFileName.c_str());
        else
        {
            ULARGE_INTEGER LogFileSize;
            LogFileSize.LowPart    = FindData.nFileSizeLow;
            LogFileSize.HighPart   = FindData.nFileSizeHigh;
            TotalDiskUsage.QuadPart += LogFileSize.QuadPart;
            if (strActiveFileName == MatchingFileName)
                continue; // Don't try to delete the active log file
            MatchingFiles[MatchingFileName]  = LogFileSize;
        }
    } while (FindNextFile(hFind, &FindData));

    FindClose(hFind);

    // Delete log files if their total size exceeds the maximum allowed
    while (TotalDiskUsage.QuadPart > MaxTotalSize.QuadPart)
    {
        FileNameAndSizeMap::iterator iter = MatchingFiles.begin();
        // We use the active log file to calculate the total log size
        // but we don't want to delete it. Therefore it is not included in
        // the MatchingFiles map. If the total log size is still too big and the
        // MatchingFiles map is empty, it means that we were left with the active
        // log file and that its size alone is bigger than the total allowed
        // size. In such a case we break the loop, wait until it will be
        // renamed and then delete it.
        if (iter == MatchingFiles.end())
            break;
        const std::string&	  MatchingFileName = iter->first;
        const ULARGE_INTEGER& LogFileSize = iter->second;
        if (!DeleteFile(MatchingFileName.c_str()))
        {
            LogEvent(LE_WARNING, "Utils::DeleteOldFiles : failed to delete %s.", MatchingFileName.c_str());
        }
        else
        {
            LogEvent(LE_INFOLOW, "Utils::DeleteOldFiles : File deleted : %s.", MatchingFileName.c_str());

        }
        TotalDiskUsage.QuadPart -= LogFileSize.QuadPart;
        MatchingFiles.erase(iter);
    }
}

bool GetStrFromBuffer(const char* ContextStr,
                      const BYTE* DataOrigin, int DataSize,
                      const BYTE*& DataPtr, std::string & StrToGet)
{
    int StringLength = strlen((char*)DataPtr) + 1;
    if (((DataPtr + StringLength) - DataOrigin) > DataSize) 
    { 
        LogEvent(LE_ERROR, "%s::GetStrFromBuffer, pass the end of the buffer",
            ContextStr); 
        return false; 
    } 
    StrToGet = (char*)DataPtr; 
    DataPtr += StringLength;
    return true;
}

void flip_bytes(void* pv, size_t len)
{ 
    char* p = (char*)pv; 
    size_t i, j; 
    char tmp; 

    for (i=0, j=len-1; i<j; i++, j--) { 
        tmp = p[i]; 
        p[i] = p[j]; 
        p[j] = tmp; 
    } 
} 

double SwitchDoubleBigendianAndLittleEndian(double Val)
{
    flip_bytes(&Val, sizeof(double));
    return Val;
}

// Based on Euclid equality: GCD(a,b) = GCD (b,(a mod b)). 
int GreatestCommonDivisor(int X, int Y)
{
    if (X <= 0 || Y <= 0)
        return 0; // Not supporting 0 or negetives

    int Smaller = X;
    int Bigger = Y;
    do 
    {
        if (Smaller > Bigger)
            std::swap(Smaller, Bigger);

        Bigger = Bigger % Smaller;
    } while (Bigger > 0);

    return Smaller;
}

int LeastCommonMultiple(int X, int Y)
{
    if (X <= 0 || Y <= 0)
        return 0; // Not supporting 0 or negetives

    return (X * Y) / GreatestCommonDivisor(X, Y);
}


//
// Usage: SetThreadName (-1, "MainThread");
//
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

time_t mkgmtime(struct tm* _tm)
{
                                          // Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec
    static int DaysPerMonth[2][12]      = {{ 31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 },
                                           { 31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 }};
    static int DaysFromYearStart[2][12] = {{  0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 },
                                           {  0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335 }};
    if (_tm->tm_year < 70  ||  _tm->tm_year > 138)
        return -1;
    if (_tm->tm_mon < 0  ||  _tm->tm_mon > 11)
        return -1;
    int LeapYear = IsLeapYear(1900+_tm->tm_year) ? 1 : 0;
    if (_tm->tm_mday < 1  ||  _tm->tm_mday > DaysPerMonth[LeapYear][_tm->tm_mon])
        return -1;
    if (_tm->tm_hour < 0  ||  _tm->tm_hour > 23)
        return -1;
    if (_tm->tm_min < 0  ||  _tm->tm_min > 59)
        return -1;
    if (_tm->tm_sec < 0  ||  _tm->tm_sec > 59)
        return -1;

    long Days = 365 * (_tm->tm_year - 70);
    int LastYear = 1900+_tm->tm_year-1;
    int NumLeapYears = (LastYear / 4) - (LastYear / 100) + (LastYear / 400);
    Days += NumLeapYears - 477;
    Days += DaysFromYearStart[LeapYear][_tm->tm_mon] + _tm->tm_mday - 1;
    long Seconds = (((Days * 24) + _tm->tm_hour) * 60 + _tm->tm_min) * 60 + _tm->tm_sec;

    return Seconds < 0 ? -1 : Seconds;
}

std::string ExceptionCodeToString(UINT Code)
{
    switch(Code)
    {
        RETURN_TYPE_STR(EXCEPTION_ACCESS_VIOLATION);
        RETURN_TYPE_STR(EXCEPTION_DATATYPE_MISALIGNMENT);
        RETURN_TYPE_STR(EXCEPTION_BREAKPOINT);
        RETURN_TYPE_STR(EXCEPTION_SINGLE_STEP);
        RETURN_TYPE_STR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
        RETURN_TYPE_STR(EXCEPTION_FLT_DENORMAL_OPERAND);
        RETURN_TYPE_STR(EXCEPTION_FLT_DIVIDE_BY_ZERO);
        RETURN_TYPE_STR(EXCEPTION_FLT_INEXACT_RESULT);
        RETURN_TYPE_STR(EXCEPTION_FLT_INVALID_OPERATION);
        RETURN_TYPE_STR(EXCEPTION_FLT_OVERFLOW);
        RETURN_TYPE_STR(EXCEPTION_FLT_STACK_CHECK);
        RETURN_TYPE_STR(EXCEPTION_FLT_UNDERFLOW);
        RETURN_TYPE_STR(EXCEPTION_INT_DIVIDE_BY_ZERO);
        RETURN_TYPE_STR(EXCEPTION_INT_OVERFLOW);
        RETURN_TYPE_STR(EXCEPTION_PRIV_INSTRUCTION);
        RETURN_TYPE_STR(EXCEPTION_IN_PAGE_ERROR);
        RETURN_TYPE_STR(EXCEPTION_ILLEGAL_INSTRUCTION);
        RETURN_TYPE_STR(EXCEPTION_NONCONTINUABLE_EXCEPTION);
        RETURN_TYPE_STR(EXCEPTION_STACK_OVERFLOW);
        RETURN_TYPE_STR(EXCEPTION_INVALID_DISPOSITION);
        RETURN_TYPE_STR(EXCEPTION_GUARD_PAGE);
        RETURN_TYPE_STR(EXCEPTION_INVALID_HANDLE);
    default:
        char TmpStr[64];
        sprintf_s(TmpStr, "EXCEPTION_UNKNOWN_%d", Code);
        return TmpStr;
    }
}

std::string VarType2Str(VARTYPE Type)
{
    switch(Type)
    {
        RETURN_TYPE_STR(VT_EMPTY);

        RETURN_TYPE_STR(VT_NULL);
        RETURN_TYPE_STR(VT_I2);
        RETURN_TYPE_STR(VT_I4);
        RETURN_TYPE_STR(VT_R4);
        RETURN_TYPE_STR(VT_R8);
        RETURN_TYPE_STR(VT_CY);
        RETURN_TYPE_STR(VT_DATE);
        RETURN_TYPE_STR(VT_BSTR);
        RETURN_TYPE_STR(VT_DISPATCH);
        RETURN_TYPE_STR(VT_ERROR);
        RETURN_TYPE_STR(VT_BOOL);
        RETURN_TYPE_STR(VT_VARIANT);
        RETURN_TYPE_STR(VT_UNKNOWN);
        RETURN_TYPE_STR(VT_DECIMAL);
        RETURN_TYPE_STR(VT_I1);
        RETURN_TYPE_STR(VT_UI1);
        RETURN_TYPE_STR(VT_UI2);
        RETURN_TYPE_STR(VT_UI4);
        RETURN_TYPE_STR(VT_I8);
        RETURN_TYPE_STR(VT_UI8);
        RETURN_TYPE_STR(VT_INT);
        RETURN_TYPE_STR(VT_UINT);
        RETURN_TYPE_STR(VT_VOID);
        RETURN_TYPE_STR(VT_HRESULT);
        RETURN_TYPE_STR(VT_PTR);
        RETURN_TYPE_STR(VT_SAFEARRAY);
        RETURN_TYPE_STR(VT_CARRAY);
        RETURN_TYPE_STR(VT_USERDEFINED);
        RETURN_TYPE_STR(VT_LPSTR);
        RETURN_TYPE_STR(VT_LPWSTR);
        RETURN_TYPE_STR(VT_RECORD);
        RETURN_TYPE_STR(VT_INT_PTR);
        RETURN_TYPE_STR(VT_UINT_PTR);
        RETURN_TYPE_STR(VT_FILETIME);
        RETURN_TYPE_STR(VT_BLOB);
        RETURN_TYPE_STR(VT_STREAM);
        RETURN_TYPE_STR(VT_STORAGE);
        RETURN_TYPE_STR(VT_STREAMED_OBJECT);
        RETURN_TYPE_STR(VT_STORED_OBJECT);
        RETURN_TYPE_STR(VT_BLOB_OBJECT);
        RETURN_TYPE_STR(VT_CF);
        RETURN_TYPE_STR(VT_CLSID);
        RETURN_TYPE_STR(VT_VERSIONED_STREAM);
        RETURN_TYPE_STR(VT_BSTR_BLOB);
        RETURN_TYPE_STR(VT_VECTOR);
        RETURN_TYPE_STR(VT_ARRAY);
        RETURN_TYPE_STR(VT_BYREF);
        RETURN_TYPE_STR(VT_RESERVED);
        RETURN_TYPE_STR(VT_ILLEGAL);
        //RETURN_TYPE_STR(VT_ILLEGALMASKED);
        //RETURN_TYPE_STR(VT_TYPEMASK);

    default:
        char TmpStr[64];
        sprintf_s(TmpStr, "VT_UNKNOWN_%d", Type);
        return TmpStr;
    }
}


std::wstring Str2Wchar(LPCSTR Str, size_t sizeInChars /*= 0*/, UINT CodePage /*= CP_ACP*/)
{
    int Len = sizeInChars == 0 ? strlen(Str) : strnlen(Str, sizeInChars);
    wchar_t* Wstr = new wchar_t[Len+1];
    MultiByteToWideChar(CodePage, 0, Str, Len, Wstr, (Len+1)*sizeof(wchar_t));
    Wstr[Len] = 0;
    std::wstring RetWstr = Wstr;
    delete [] Wstr;
    return RetWstr;
}

std::string Wchar2Str(LPCWSTR Wstr, size_t sizeInWchars /*= 0*/, UINT CodePage /*= CP_ACP*/)
{
    int Len = sizeInWchars == 0 ? wcslen(Wstr) : wcsnlen(Wstr, sizeInWchars);
    char* Str = new char[Len+1];
    WideCharToMultiByte(CodePage, 0, Wstr, Len, Str, Len+1, NULL, NULL);
    Str[Len] = '\0';
    std::string RetStr = Str;
    delete [] Str;
    return RetStr;
}

std::string GetCommandLineArg(int Arg)
{
    int NumArgs;
    LPWSTR* Args = CommandLineToArgvW(GetCommandLineW(), &NumArgs);
    if (Arg >= NumArgs)
        return "";
    else
        return Wchar2Str(Args[Arg]);
}

std::string GetStringValue(const char* StringToParse, const char* Key, const char* Default /*= ""*/)
{
	if (strstr(StringToParse, Key) == NULL) // it's possible the key is not there
		return Default;

    int BeginPos = strstr(StringToParse, Key) + strlen(Key) - StringToParse;
    int EndPos	 = strchr(StringToParse + BeginPos, ';') - StringToParse;
    if (EndPos < 0)
        EndPos = strlen(StringToParse);

    int SubStringLength = EndPos-BeginPos;
    char* Value = new char[SubStringLength+1];
    memcpy_s(Value, SubStringLength, StringToParse + BeginPos, SubStringLength);
    //strncpy_s(Value, SubStringLength+1, StringToParse + BeginPos, SubStringLength);
    Value[SubStringLength] = '\0';

    std::string StringValue = Value;
    delete Value;

    return StringValue;
}

int GetIntValue(const char* StringToParse, const char* Key, int Default /*= 0*/)
{
    std::string StrValue = GetStringValue(StringToParse, Key);	
    int Value = StrValue.empty() ? Default : atoi(StrValue.c_str());

    return Value;
}

double GetDoubleValue(const char* StringToParse, const char* Key, double Default /*= 0*/)
{
	std::string StrValue = GetStringValue(StringToParse, Key);	
    double Value = StrValue.empty() ? Default : atof(StrValue.c_str());

	return Value;
}

#define SIZE_OF_CHAR_BUFFER 64

void AddStringValue(std::string &String, const char* Key, std::string Value)
{
	String += Key;
	String += Value;
	String += ';';
}

void AddIntValue(std::string &String, const char* Key, int Value)
{
	char CharValue[SIZE_OF_CHAR_BUFFER];
	std::string StrValue = OurItoa(Value, CharValue, SIZE_OF_CHAR_BUFFER, 10);

	AddStringValue(String, Key, StrValue);
}


void AddDoubleValue(std::string &String, const char* Key, double Value, int NumberOfDigitsAfterDecimalPoint)
{
	char CharValue[SIZE_OF_CHAR_BUFFER];
	//int* dec = NULL; int* sign = NULL;
	int dec=0; int sign=0;
	std::string StrValue = OurFcvt(CharValue, Value, NumberOfDigitsAfterDecimalPoint, &dec, &sign);
	if (dec >= 0)
		StrValue.insert(dec, ".");
	if (sign != 0)
		StrValue.insert(0/*sign before the number*/, "-");

	AddStringValue(String, Key, StrValue);
}

CTokenParser::CTokenParser(const char* Str) :
    m_TheString(Str), m_CurrentPlace(0), m_Length(strlen(Str))
{
}

CTokenParser::CTokenParser(CTokenParser &TokenParser)
{
	TokenParser.CopyParams(m_TheString, m_CurrentPlace, m_Length);
}

void CTokenParser::CopyParams(const char* &TheString, int &CurrentPlace, int &Length)
{
	TheString = m_TheString;
	CurrentPlace = m_CurrentPlace;
	Length = m_Length;
}

std::string CTokenParser::GetNextToken(const char* Delimiters, bool Trim /*= false*/)
{
    if (m_CurrentPlace >= m_Length)
        return "";
    //const char* NextPtr = strstr(m_TheString + m_CurrentPlace, Delimiters);
    int CurrentPlace = m_CurrentPlace;
    m_CurrentPlace += strcspn(m_TheString + m_CurrentPlace, Delimiters) + 1;
   // m_CurrentPlace = NextPtr == NULL ? m_Length + 1 : NextPtr - m_TheString + 1;
    int NumTrailingSpaces = 0;
    int Length = m_CurrentPlace - CurrentPlace - 1;
    if (Trim)
    {
        for (const char *Ptr = m_TheString + CurrentPlace; *Ptr == ' ' || *Ptr == '\t'; ++Ptr)
            ++CurrentPlace;
        for (const char *Ptr = m_TheString + m_CurrentPlace - 1; (*Ptr == ' ' || *Ptr == '\t') && NumTrailingSpaces < Length; --Ptr)
            ++NumTrailingSpaces;
    }
    return std::string(m_TheString + CurrentPlace, Length - NumTrailingSpaces);
}

bool CTokenParser::MoreTokens() const
{
    return m_CurrentPlace < m_Length;
}


const char* DriveTypeToString(UINT Type)
{
    switch(Type)
    {
        RETURN_TYPE_STR(DRIVE_UNKNOWN);
        RETURN_TYPE_STR(DRIVE_NO_ROOT_DIR);
        RETURN_TYPE_STR(DRIVE_REMOVABLE);
        RETURN_TYPE_STR(DRIVE_FIXED);
        RETURN_TYPE_STR(DRIVE_REMOTE);
        RETURN_TYPE_STR(DRIVE_CDROM);
        RETURN_TYPE_STR(DRIVE_RAMDISK);
    default:
        return "DRIVE_ILLEGAL";
    }
}

bool GetLogicalDrives(std::vector<std::string>& StrVector, UINT Type, bool IgnoreSubsts /*= true*/)
{
    LogEvent(LE_INFOLOW, __FUNCTION__ ": Type [%s], IgnoreSubsts [%s]", DriveTypeToString(Type), BooleanStr(IgnoreSubsts));
    char Buf[256];
    char* Ptr = Buf;
    DWORD Actual = GetLogicalDriveStrings(sizeof Buf, Buf);
    StrVector.clear();
    for (size_t i = 0; i < Actual; i += strlen(Ptr) + 1, Ptr = Buf + i)
    {
        UINT DriveType = GetDriveType(Ptr);
        LogEvent(LE_INFOLOW, __FUNCTION__ ": Drive [%s], DriveType [%s]", Ptr, DriveTypeToString(DriveType));
        if (Type == DRIVE_UNKNOWN  ||  DriveType == Type)
        {
            if (IgnoreSubsts)
            {
                char DevName[16];
                strncpy_s(DevName, 3, Ptr, _TRUNCATE);
                char Path[2*MAX_PATH];
                DWORD Actual = QueryDosDevice(DevName, Path, sizeof Path);
                if (Actual > 0)
                {
                    LogEvent(LE_INFOLOW, __FUNCTION__ ": DevName [%s], Path [%s]", DevName, Path);
                    if (!strncmp(Path, "\\??\\", 4))
                        continue;
                }
                else
                    LogEvent(LE_ERROR, __FUNCTION__ ": DevName [%s]: QueryDosDevice error (Actual == 0)", DevName);
            }
            StrVector.push_back(Buf + i);
        }
    }
    return Actual > 0;
}

double GetDriveFreeSpacePercentage(const char* Drive)
{
    ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes;
    if (!GetDiskFreeSpaceEx(Drive, &FreeBytesAvailable, &TotalNumberOfBytes, NULL))
        return -1;
    if (TotalNumberOfBytes.QuadPart == 0)
    {
        if (FreeBytesAvailable.QuadPart == 0)
            return 0;
        else
            return 100;
    }

    return 100.0 * (double)FreeBytesAvailable.QuadPart / TotalNumberOfBytes.QuadPart;
}

//////////////////////////////////////////////////////////////////////
// CStopper (Old)
// class CStopper
// {
// public:
//     CStopper() { m_StartTime.QuadPart = LLONG_MIN;  m_EndTime.QuadPart = LLONG_MAX; }
//     bool SetStartTime() { return !!QueryPerformanceCounter(&m_StartTime); }
//     bool SetEndTime() { return !!QueryPerformanceCounter(&m_EndTime); }
//     LONGLONG GetLapsedTimeInNano() { return LONGLONG(((double)1000000000 * (m_EndTime.QuadPart - m_StartTime.QuadPart)) / GetFreqency()); }
//     static LONGLONG GetFreqency()
//     {
//         static LARGE_INTEGER Freq;
//         bool _FrequencyInitialized;
//         static bool FrequencyInitialized = (Freq.QuadPart = LLONG_MAX,_FrequencyInitialized = !!QueryPerformanceFrequency(&Freq),
//             LogEvent(_FrequencyInitialized ? LE_INFO : LE_WARNING, __FUNCTION__ ": QueryPerformanceFrequency returned %s, Freq = %I64d", BooleanStr(_FrequencyInitialized), Freq.QuadPart),
//             _FrequencyInitialized);
//         return Freq.QuadPart;
//     }
// private:
//     LARGE_INTEGER m_StartTime;
//     LARGE_INTEGER m_EndTime;
// };

//////////////////////////////////////////////////////////////////////
// CStopper

CStopper::CStopper() : m_IsRunning(false), m_LapsedTimeInMicro(0)
{
    GetFreqency();
}

bool CStopper::Start(bool Force /*= true*/)
{
    if (Force)
        m_IsRunning = false;
    if (m_IsRunning)
        return false;
    m_LapsedTimeInMicro = 0;
    return Continue();
}

bool CStopper::Continue()
{
    if (m_IsRunning)
        return false;
    bool Ret = !!QueryPerformanceCounter(&m_StartTime);
    if (!Ret)
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": QueryPerformanceCounter error");
        return false;
    }
    m_IsRunning = true;
    return true;
}

bool CStopper::Stop()
{
    if (!m_IsRunning)
        return false;
    m_IsRunning = false;
    LARGE_INTEGER EndTime;
    bool Ret = !!QueryPerformanceCounter(&EndTime);
    if (Ret)
        m_LapsedTimeInMicro += ((double)1000000 * (EndTime.QuadPart - m_StartTime.QuadPart)) / GetFreqency();
    else
        LogEvent(LE_ERROR, __FUNCTION__ ": QueryPerformanceCounter error");
    return Ret;        
}

double CStopper::GetLapsedTimeInMicro()
{
    if (!m_IsRunning)
        return m_LapsedTimeInMicro;
    LARGE_INTEGER EndTime;
    bool Ret = !!QueryPerformanceCounter(&EndTime);
    if (Ret)
        return m_LapsedTimeInMicro + ((double)1000000 * (EndTime.QuadPart - m_StartTime.QuadPart)) / GetFreqency();
    else
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": QueryPerformanceCounter error");
        return m_LapsedTimeInMicro;
    }
}

/*static*/ LONGLONG CStopper::GetFreqency()
{
    static LARGE_INTEGER Freq;
    bool _FrequencyInitialized;
    static bool FrequencyInitialized = (Freq.QuadPart = LLONG_MAX,_FrequencyInitialized = !!QueryPerformanceFrequency(&Freq),
        LogEvent(_FrequencyInitialized ? LE_INFO : LE_WARNING, __FUNCTION__ ": QueryPerformanceFrequency returned %s, Freq = %I64d, Precision = %d nano", BooleanStr(_FrequencyInitialized), Freq.QuadPart, (int)((double)1000000000 / Freq.QuadPart)),
        _FrequencyInitialized);
    return Freq.QuadPart;
}


//////////////////////////////////////////////////////////////////////
// BIT Utilities
#include "Psapi.h"
#pragma comment(lib, "Psapi.lib")
CMemoryInfo::CMemoryInfo(UCHAR Unit /*= 'K'*/)
{
    switch(Unit)
    {
    case 'K': m_Divisor = 1024; break;
    case 'M': m_Divisor = 1024 * 1024; break;
    default:  m_Divisor = 1;
    }
    m_hProcess = GetCurrentProcess();
}

bool CMemoryInfo::GetInfo(ULONG& MemUsage, ULONG& VMSize)
{
    PROCESS_MEMORY_COUNTERS MemoryInfo;
    if (!GetProcessMemoryInfo(m_hProcess, &MemoryInfo, sizeof(MemoryInfo)))
        return false;
    MemUsage = MemoryInfo.WorkingSetSize / m_Divisor;
    VMSize   = MemoryInfo.PagefileUsage / m_Divisor;
    return true;
}


enum
{
    SystemPerformanceInformation = 2,
    SystemTimeInformation        = 3
};

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

struct SYSTEM_PERFORMANCE_INFORMATION
{
    LARGE_INTEGER   IdleTime;
    DWORD           Spare[76];
};

struct SYSTEM_TIME_INFORMATION
{
    LARGE_INTEGER KeBootTime;
    LARGE_INTEGER KeSystemTime;
    LARGE_INTEGER ExpTimeZoneBias;
    ULONG         CurrentTimeZoneId;
    DWORD         Reserved;
};

CCpuUsage::CCpuUsage() : m_OldIdleTime(-1), m_OldSystemTime(-1)
{
    m_NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"), "NtQuerySystemInformation");
    if (m_NtQuerySystemInformation == NULL)
        LogEvent(LE_ERROR, __FUNCTION__ ": GetProcAddress(GetModuleHandle(\"ntdll\"), \"NtQuerySystemInformation\") error");

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    m_NumberOfProcessors = SystemInfo.dwNumberOfProcessors;

    GetAverageCpuUsage();
}

double CCpuUsage::GetAverageCpuUsage()
{
    if (m_NtQuerySystemInformation == NULL)
        return -1;

    SYSTEM_TIME_INFORMATION SysTimeInfo;
    SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    m_NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);
    m_NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);
    double IdleTime = Li2Double(SysPerfInfo.IdleTime);
    double SystemTime = Li2Double(SysTimeInfo.KeSystemTime);
    double CpuUsage = 0;
    if (m_OldIdleTime != -1  &&  m_OldSystemTime != -1)
    {
        double IdleTimeDiff = IdleTime - m_OldIdleTime;
        double SystemTimeDiff = SystemTime - m_OldSystemTime;
        if (SystemTimeDiff == 0)
        {
            if (IdleTimeDiff == 0)
                CpuUsage = 0;
            else
                CpuUsage = 100;
        }
        else
            CpuUsage = 100.0 - (IdleTimeDiff / SystemTimeDiff) * 100.0 / (double)m_NumberOfProcessors;
    }
    //LogEvent(LE_INFO, __FUNCTION__ ": CpuUsage %.2f", CpuUsage);
    m_OldIdleTime = IdleTime;
    m_OldSystemTime = SystemTime;
    return CpuUsage;
}


//////////////////////////////////////////////////////////////////////
// CMultimediaTimer
#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")
CMultimediaTimer::CMultimediaTimer(UINT ResolutionInMilli /*= 1*/) : m_TimerRes((UINT)(-1)), m_TimerID(0)
{
    TIMECAPS tc;
    MMRESULT Res = timeGetDevCaps(&tc, sizeof tc);
    if (Res != TIMERR_NOERROR)
        LogEvent(LE_ERROR, __FUNCTION__ ": timeGetDevCaps error %u", Res);
    else
    {
        m_TimerRes = max(tc.wPeriodMin, ResolutionInMilli);
        Res = timeBeginPeriod(m_TimerRes); 
        if (Res != TIMERR_NOERROR)
        {
            LogEvent(LE_ERROR, __FUNCTION__ ": timeBeginPeriod(%u) error %u", m_TimerRes, Res);
            m_TimerRes = (UINT)(-1);
        }
    }
}

CMultimediaTimer::~CMultimediaTimer()
{
    KillTimer(true);
}

bool CMultimediaTimer::SetTimer(UINT DelayInMilli, ETimerType TimerType, DWORD UserData, IMultimediaTimerEvent* Sink)
{
    KillTimer(true);
    if (m_TimerRes == (UINT)(-1))
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": m_TimerRes not initialized (see previous error in CMultimediaTimer::CMultimediaTimer)");
        return false;
    }
    if (Sink == NULL)
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": Sink == NULL");
        return false;
    }
    m_EventData.UserData = UserData;
    m_EventData.Sink = Sink;
    m_TimerID = timeSetEvent(DelayInMilli, m_TimerRes, TimerFunc, (DWORD)&m_EventData, TimerType == OneShotTimer ? TIME_ONESHOT : TIME_PERIODIC);
    if (m_TimerID == 0)
    {
        LogEvent(LE_ERROR, __FUNCTION__ ": timeSetEvent error");
        return false;
    }
    else
    {
        LogEvent(LE_INFO, __FUNCTION__ ": Timer set, TimerId [%u], Delay [%u], Type [%s], UserData [%u], Sink [%p]",
            m_TimerID, DelayInMilli, TimerType == OneShotTimer ? "OneShotTimer" : "PeriodicTimer", UserData, Sink);
        return true;
    }
}

bool CMultimediaTimer::KillTimer(bool Force /*= false*/)
{
    if (m_TimerID == 0)
    {
        if (Force)
            return true;
        LogEvent(LE_ERROR, __FUNCTION__ ": m_TimerID == 0");
        return false;
    }
    bool Ret = true;
    if (timeKillEvent(m_TimerID) != TIMERR_NOERROR)
    {
        if (!Force)
        {
            LogEvent(LE_ERROR, __FUNCTION__ ": timeKillEvent(%u) error (timer not exists or already killed or expired)", m_TimerID);
            Ret = false;
        }
    }
    m_TimerID = 0;
    return Ret;
}

/*static*/ void CALLBACK CMultimediaTimer::TimerFunc(UINT TimerID, UINT /*msg*/, DWORD dwUser, DWORD /*dw1*/, DWORD /*dw2*/) 
{
    EventData* Ed = (EventData*)dwUser;
    LogEvent(LE_INFO, __FUNCTION__ ": TimerId [%u], UserData [%u], Sink [%p]", TimerID, Ed->UserData, Ed->Sink);
    Assert(Ed->Sink != NULL);
    Ed->Sink->OnMultimediaTimeout(Ed->UserData);
} 



bool CompareBuffers(const BYTE* Buffer1, const BYTE* Buffer2, int LengthToCompare)
{
	for (int i = 0; i < LengthToCompare; i++)
		if (Buffer1[i] != Buffer2[i])
			return false;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _TEST

void TestCompareHostNames()
{
	// don't forget to use WASStartup
	//WORD wVersionRequested;
	//WSADATA wsaData;
	//wVersionRequested = MAKEWORD(2, 2);
	//int Error = WSAStartup(wVersionRequested, &wsaData);

	char HostName1[1024];
	char HostName2[1024];
	char Answer[32];
	Answer[0] = 'y';
	do 
	{
		printf("Enter first host name...\n");
		scanf_s("%s", HostName1);
		printf("Enter second host name...\n");
		scanf_s("%s", HostName2);

		if (CompareHostNames(HostName1, HostName2))
			printf("'%s' is the same as '%s'.\n", HostName1, HostName2);
		else
			printf("'%s' is not the same as '%s'.\n", HostName1, HostName2);

		printf("Do you want to test another pair? (y/n)\n");
		scanf_s("%s", &Answer);
	} while ('y' == Answer[0]);	
}

void TestSplitString()
{
    std::string FirstPart;
    std::string SecondPart;

    Assert(SplitString(" xxyy __aaa bbb", "__", FirstPart, SecondPart));
    Assert(FirstPart == " xxyy ");
    Assert(SecondPart == "aaa bbb");
    Assert(!SplitString(" xxyy __aaa bbb", ";", FirstPart, SecondPart));
    Assert(!SplitString("; xxyy __aaa bbb", ";", FirstPart, SecondPart));
    Assert(!SplitString(" xxyy __aaa bbb;", ";", FirstPart, SecondPart));
    Assert(SplitString(" xxyy ;__aaa bbb;", ";", FirstPart, SecondPart));
}

void TestFindParamValue()
{
    std::string Value;

    Assert(FindParamValue("ParamName1=Blah blah blah;ParamName2 = Romeo & Juliet; Param3 = Stam",
                          "ParamName2", ";", "TestFindParamValue", Value, LE_WARNING));
    Assert(Value == "Romeo & Juliet");
    Assert(FindParamValue("ParamName1=-80;ParamName2 = Romeo & Juliet; Param3 = Stam",
                          "ParamName1", ";", "TestFindParamValue", Value, LE_WARNING));
    Assert(Value == "-80");
    Assert(FindParamValue("ParamName1=Blah blah blah;ParamName2 = Romeo & Juliet ; ParamName3 =89.9",
                            "ParamName3", ";", "TestFindParamValue", Value, LE_WARNING));
    Assert(Value == "89.9");
    Assert(!FindParamValue("ParamName1=Blah blah blah;ParamName2 = Romeo & Juliet ; ParamName3 =89.9",
                            "ParamName4", ";", "TestFindParamValue", Value, LE_WARNING));
    Assert(!FindParamValue("ParamName1=Blah blah blah;ParamName2  Romeo & Juliet ; ParamName3 =89.9",
                            "ParamName2", ";", "TestFindParamValue", Value, LE_WARNING));

}

void TestGetStringTokens()
{
    std::string String = "blah blah blah ";
    std::vector<std::string> Tokens;
    GetStringTokens(String, Tokens);

    Assert(Tokens.size() == 3 && Tokens[0] == "blah" &&
           Tokens[1] == "blah" && Tokens[2] == "blah");

    String = "-1-lll+ooo-bbbb\n+ppp";
    Tokens.clear();
    GetStringTokens(String, Tokens, "-+");

    Assert(Tokens.size() == 5 && Tokens[4] == "ppp");

    return;
}

void TestGcdAndLcm()
{
    Assert(GreatestCommonDivisor(20, 15) == 5);
    Assert(GreatestCommonDivisor(10, 5) == 5);
    Assert(GreatestCommonDivisor(31, 33) == 1);
    
    Assert(LeastCommonMultiple(20, 30) == 60);
    Assert(LeastCommonMultiple(20, 40) == 40);
    Assert(LeastCommonMultiple(9, 8) == 72);

    return;
}

struct SSizeofTester 
{
	BYTE ByteArray[19];
	UINT UIntArray[23];
};

void TestSizeof()
{
	UINT testarray[17];
	Assert(countof(testarray) == 17);
	LogEvent(LE_INFOHIGH, "countof testarray should be 17 is [%d]", countof(testarray));

	Assert(member_countof(SSizeofTester, ByteArray) == 19);
	LogEvent(LE_INFOHIGH, "member_countof SSizeofTester::ByteAttay should be 19 is [%d]", member_countof(SSizeofTester, ByteArray));

	Assert(member_sizeof(SSizeofTester, ByteArray) == 19);
	LogEvent(LE_INFOHIGH, "member_sizeof SSizeofTester::ByteAttay should be 19 is [%d]", member_sizeof(SSizeofTester, ByteArray));

	Assert(member_countof(SSizeofTester, UIntArray) == 23);
	LogEvent(LE_INFOHIGH, "member_countof SSizeofTester::UIntArray should be 23 is [%d]", member_countof(SSizeofTester, UIntArray));

	Assert(member_sizeof(SSizeofTester, UIntArray) == 23*4);
	LogEvent(LE_INFOHIGH, "member_sizeof SSizeofTester::UIntArray should be 23*4=92 is [%d]", member_sizeof(SSizeofTester, UIntArray));
}

void TestUtils()
{
    TestSplitString();
    TestFindParamValue();
    TestGetStringTokens();
    TestGcdAndLcm();
	TestSizeof();
}

void TestGetAllFiles()
{
	LogEvent(LE_INFOHIGH, "Start GetAllFiles");
	const char* Root = "C:\\Proj\\Elta\\Logger1\\";

    std::vector<std::string> FileNames;
	if (GetAllFiles(Root, false, FileNames, "*.txt") == false)
    {
        LogEvent(LE_INFOHIGH, "Directory not exists [%s]", Root);
        return;
    }

    int Num = FileNames.size();
	LogEvent(LE_INFOHIGH, "Num Files %d", Num);
    for (int i = 0 ; i < Num ; i++)
        LogEvent(LE_INFOHIGH, "File number %d: [%s]", i, FileNames[i].c_str());
    return;
}
#endif
