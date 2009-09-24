// LogFile.cpp: implementation of the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// class CLogFile

CLogFile::CLogFile() : m_hFile(INVALID_HANDLE_VALUE)
{
}

CLogFile::~CLogFile()
{
    Close();
}

bool CLogFile::Open(const char* Name)
{
//    DWORD FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH;
    DWORD FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	m_hFile = CreateFile(
        Name,                // file name
        GENERIC_WRITE,       // access mode
        FILE_SHARE_READ,     // share mode
        NULL,                // SD
        OPEN_ALWAYS,         // how to create
        FlagsAndAttributes,  // file attributes
        NULL                 // handle to template file
    );

    if (!ValidateOpen("Open"))
    {
        LogEventToDebugOutput("CLogFile::Open: Error opening %s, m_hFile == NULL\n", Name);
        return false;
    }

    SetFilePointer(m_hFile, 0, NULL, FILE_END); 
    return true;
}

void CLogFile::Close()
{
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        FlushFileBuffers(m_hFile);
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

bool CLogFile::IsOpen()
{
    return m_hFile != INVALID_HANDLE_VALUE;
}

const char* CLogFile::GetSeverityString(ELogSeverity Severity)
{
    switch (Severity)
    {
		case LE_DEBUG:		return "DEBUG   ";
        case LE_INFOLOW:    return "INFOLOW ";
        case LE_INFO:       return "INFO    ";
        case LE_INFOHIGH:   return "INFOHIGH";
        case LE_NOTICE:     return "NOTICE  ";
        case LE_WARNING:    return "WARNING ";
        case LE_ERROR:      return "ERROR   ";
        case LE_FATAL:      return "FATAL   ";
        default:            return "??      ";
    }
}

bool CLogFile::LogLine(time_t Seconds, int Millis, ELogSeverity Severity, const char* Line)
{
    if (!ValidateOpen("LogLine"))
        return false;

    std::string TimeString = FormatTime(Seconds, Millis);

    char Message[MAX_LOG_MESSAGE_SIZE];

    const char* SeverityString = GetSeverityString(Severity);
    _snprintf_s(Message, sizeof Message, _TRUNCATE, "%s|%s|%s\r\n", TimeString.c_str(), SeverityString, Line);
    //_snprintf_s(Message, _TRUNCATE, "%s|%s|%s\r\n", TimeString.c_str(), SeverityString, Line); - this has some error in definition: _Size instead of _Count

    DWORD Actual;
	WriteFile(m_hFile, Message, strlen(Message), &Actual, NULL);
    return true;
}

void CLogFile::Flush()
{
    if (!ValidateOpen("Flush"))
        return;

    FlushFileBuffers(m_hFile);
}

DWORD CLogFile::GetFileSize()
{
    if (!ValidateOpen("GetFileSize"))
        return 0;

    return ::GetFileSize(m_hFile, NULL);
}

// Force creation time - there was a problem with the creation time set by NTFS
void CLogFile::MarkCurrentTimeAsCreationTime()
{
	SYSTEMTIME SystemTime;
    GetSystemTime(&SystemTime);

    FILETIME FileTime;
	SystemTimeToFileTime(&SystemTime, &FileTime);
	SetFileTime(m_hFile, &FileTime, &FileTime, &FileTime);
}

time_t CLogFile::GetCreationTime()
{
    if (!ValidateOpen("GetCreationTime"))
        return 0;

    BY_HANDLE_FILE_INFORMATION FileInformation;
    GetFileInformationByHandle(m_hFile, &FileInformation);
    return ConvertFileTimeToTime(FileInformation.ftCreationTime);
}

// private methods

//////////////////////////////////////////////////////////////////////

#ifdef _TEST

#include "LogEvent.h"

static bool LogLine(CLogFile& LogFile, ELogSeverity Severity, const char* Line)
{
    time_t Seconds;
    int Millis;
    GetSecondsAndMillis(Seconds, Millis);
    return LogFile.LogLine(Seconds, Millis, Severity, Line);
}

void TestLogFile()
{

    CLogFile LogFile;
    Assert(!LogFile.IsOpen());
    Assert(!LogLine(LogFile, LE_INFOLOW,  "InfoLow"));

    const char* LogfileName = "./Test.log";
    DeleteFile(LogfileName);
    time_t CreationTime = time(NULL);
    Assert(LogFile.Open(LogfileName));
    LogFile.MarkCurrentTimeAsCreationTime();

    time_t FileCreationTime = LogFile.GetCreationTime();
    Assert(FileCreationTime == CreationTime);

    Assert(LogFile.GetFileSize() == 0);

    Assert(LogLine(LogFile, LE_INFOLOW,  "InfoLow"));
    Assert(LogLine(LogFile, LE_INFO,     "Info"));
    Assert(LogLine(LogFile, LE_INFOHIGH, "InfoHigh"));
    Assert(LogLine(LogFile, LE_NOTICE,   "Notice"));
    Assert(LogLine(LogFile, LE_WARNING,  "Warning"));
    Assert(LogLine(LogFile, LE_ERROR,    "Error"));
    DWORD Size = LogFile.GetFileSize();
    Assert(Size > 0);
    Assert(LogLine(LogFile, LE_FATAL,    "Fatal"));
    LogFile.Flush();
    DWORD Size2 = LogFile.GetFileSize();
    Assert(Size2 > Size);

    Assert(LogFile.IsOpen());
    LogFile.Close();
    Assert(!LogFile.IsOpen());
    Assert(LogFile.Open("./Test.log"));
    Assert(LogLine(LogFile, LE_INFOLOW,  "InfoLow"));

    Sleep(2000);
    FileCreationTime = LogFile.GetCreationTime();
    Assert(FileCreationTime == CreationTime);
}

#endif