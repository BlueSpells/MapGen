// LogManager.cpp: implementation of the CLogManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogManager.h"
#include "LogEvent.h"
#include "Config.h"
#include "Common/Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// class LogManager

static const char* LogExt = ".log";

const int MAX_ELEMENTS = 100000;

//Default Configuration Values
static const char* CONFIG_SECTION = "LogManager";
static const char* DEFAULT_LOG_FILE_DIRECTORY = "..\\ApplicationLogs";
static const int MEGA_BYTE = 1024 * 1024;
static const int DEFAULT_MAX_LOG_FILE_SIZE     = 5;  // In mega bytes
static const int DEFAULT_MAX_LOG_FILE_OPEN_TIME = 24 * 3600;        // In seconds
static const int DEFAULT_MAX_LOG_FILE_LIVE_TIME = 30 * 24 * 3600;   // In seconds
static const int DEFAULT_MAX_LOG_FILE_TOTAL_SIZE = 100;   // In mega bytes
static const int DEFAULT_LOG_FILE_FLUSH_TIME = 30;   // In seconds


CLogManager::CLogManager() : 
    CThreadWithQueue("LogManager", MAX_ELEMENTS, false), //do not log event when queue is full and insert to queue fails
    m_MaxLogFileSize(0),
    m_MaxLogFileOpenTime(0),
    m_MaxLogFileLiveTime(0),
	m_AllowMessage(true),
    m_IsInitialized(false)
{
	m_MaxLogFileTotalSize.QuadPart = 0;
}

CLogManager::~CLogManager()
{
	m_AllowMessage = false;
	CThreadWithQueue::CloseThread(true);
}

bool CLogManager::IsInitialized()
{
    return m_IsInitialized;
}

bool CLogManager::Init(const char* BaseName)
{
	bool WritebackMode = GetConfigWritebackMode();
    SetConfigWritebackMode(true);

    m_MaxLogFileSize = MEGA_BYTE *
        GetConfigInt(CONFIG_SECTION, "MaxLogFileSizeMega", DEFAULT_MAX_LOG_FILE_SIZE);
    m_MaxLogFileOpenTime = 
        GetConfigInt(CONFIG_SECTION, "MaxLogFileOpenTime", DEFAULT_MAX_LOG_FILE_OPEN_TIME);
    m_MaxLogFileLiveTime = 
        GetConfigInt(CONFIG_SECTION, "MaxLogFileLiveTime", DEFAULT_MAX_LOG_FILE_LIVE_TIME);
	// We assume that the total log size should not exceed 2^31 bytes
	// therefore using GetConfigInt should be satisfying
	m_MaxLogFileTotalSize.QuadPart = MEGA_BYTE *
		GetConfigInt(CONFIG_SECTION, "MaxLogFileTotalSizeMega", DEFAULT_MAX_LOG_FILE_TOTAL_SIZE);
    int LogFileFlushTime = 
        GetConfigInt(CONFIG_SECTION, "LogFileFlushTime", DEFAULT_LOG_FILE_FLUSH_TIME);
    m_LogDirectory = GetConfigString(CONFIG_SECTION, "LogFileDirectory", DEFAULT_LOG_FILE_DIRECTORY);
    SetConfigWritebackMode(WritebackMode);
    m_BaseName = BaseName;
    bool AddComputerNameToLogFileName = GetConfigBool(CONFIG_SECTION, "AddComputerNameToLogFileName", false);
    if (AddComputerNameToLogFileName)
    {
        char ComputerName[1024];
        DWORD ComputerNameSize = sizeof ComputerName;
        GetComputerName(ComputerName, &ComputerNameSize);
        m_BaseName = m_BaseName + "(" + ComputerName + ")";
    }
    return StartLogManager(LogFileFlushTime);
}

bool CLogManager::InitDefaults(const char* BaseName, const char* LogFileDirectory, int MaxLogFileSizeMega )
{
    m_MaxLogFileSize = MaxLogFileSizeMega;
    m_LogDirectory = LogFileDirectory;
    m_BaseName = BaseName;
    m_MaxLogFileOpenTime = DEFAULT_MAX_LOG_FILE_OPEN_TIME;
    m_MaxLogFileLiveTime = DEFAULT_MAX_LOG_FILE_LIVE_TIME;
    // We assume that the total log size should not exceed 2^31 bytes
    // therefore using GetConfigInt should be satisfying
    m_MaxLogFileTotalSize.QuadPart = MEGA_BYTE * DEFAULT_MAX_LOG_FILE_TOTAL_SIZE;
    int LogFileFlushTime = DEFAULT_LOG_FILE_FLUSH_TIME;
    return StartLogManager(LogFileFlushTime);
}

void CLogManager::LogLine(ELogSeverity Severity, const char* Line)
{
	if( !m_AllowMessage || !m_IsInitialized )
		return; // required to avoid LogEvent Message while destructing the LogManager

    LineInfo Info;
    Info.Line     = Line;
    Info.Severity = Severity;
    GetSecondsAndMillis(Info.Seconds, Info.Millis);

    AddHandlerToQueue(&CLogManager::HandleLogLine, Info);
}

// private methods
void CLogManager::HandleLogLine(const LineInfo& Info)
{
    m_LogFile.LogLine(Info.Seconds, Info.Millis, Info.Severity, Info.Line.c_str());
}

void CLogManager::AddDateToLogFileName(time_t FileCreationTime)
{
    char DateString[64];

    //struct tm* Tm = localtime(&FileCreationTime);
    struct tm Tm;
    errno_t err = localtime_s(&Tm, &FileCreationTime);
    if (err == 0)
        sprintf_s(DateString, "%04d_%02d_%02d_%02d_%02d_", 
            Tm.tm_year + 1900, Tm.tm_mon + 1, Tm.tm_mday, Tm.tm_hour, Tm.tm_min);
    else
        sprintf_s(DateString, "!!Error!!");
    std::string NewFileName = m_LogDirectory + "/" + DateString + m_BaseName + LogExt;
    MoveFile(m_LogFileName.c_str(), NewFileName.c_str());
}

//void CLogManager::DeleteOldFiles()
//{
//    HANDLE hFind;
//    WIN32_FIND_DATA FindData;
//	ULARGE_INTEGER  TotalLogSize;
//	typedef std::map<std::string, ULARGE_INTEGER> LogFileNameAndSizeMap;
//	LogFileNameAndSizeMap LogFiles;
//	
//	TotalLogSize.QuadPart = 0;
//
//    time_t CurrentTime = time(NULL);
//
//    std::string LogDirectoryWithWildCard = m_LogDirectory + "\\*" + m_BaseName + LogExt;
//    hFind = FindFirstFile(LogDirectoryWithWildCard.c_str(), &FindData);
//    if (hFind == INVALID_HANDLE_VALUE) // No file
//        return;
//
//    do
//    {
//        if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            continue;
//
//        if(!strstr(FindData.cFileName, LogExt))
//            continue;
//
//        time_t FileLastWriteTime = ConvertFileTimeToTime(FindData.ftLastWriteTime);
//		std::string LogFileName = m_LogDirectory + "\\" + FindData.cFileName;
//        if (CurrentTime - FileLastWriteTime > m_MaxLogFileLiveTime)
//			DeleteFile(LogFileName.c_str());
//		else
//		{
//			ULARGE_INTEGER LogFileSize;
//			LogFileSize.LowPart    = FindData.nFileSizeLow;
//			LogFileSize.HighPart   = FindData.nFileSizeHigh;
//			TotalLogSize.QuadPart += LogFileSize.QuadPart;
//            if(m_LogFileName == LogFileName)
//                continue; // Don't try to delete the active log file
//			LogFiles[LogFileName]  = LogFileSize;
//		}
//    } while (FindNextFile(hFind, &FindData));
//
//    FindClose(hFind);
//
//	// Delete log files if their total size exceeds the maximum allowed
//	while(TotalLogSize.QuadPart > m_MaxLogFileTotalSize.QuadPart)
//	{
//		LogFileNameAndSizeMap::iterator iter = LogFiles.begin();
//		// We use the active log file to calculate the total log size
//        // but we don't want to delete it. Therefore it is not included in
//        // the LogFiles map. If the total log size is still too big and the
//        // LogFiles map is empty, it means that we were left with the active
//        // log file and that its size alone is bigger than the total allowed
//        // size. In such a case we break the loop, wait until it will be
//        // renamed and then delete it.
//		if(iter == LogFiles.end())
//            break;
//		const std::string&	  LogFileName = iter->first;
//		const ULARGE_INTEGER& LogFileSize = iter->second;
//		if(!DeleteFile(LogFileName.c_str()))
//			LogEvent(LE_WARNING, "LogManager::DeleteOldFiles : failed to delete %s.", LogFileName.c_str());
//		TotalLogSize.QuadPart -= LogFileSize.QuadPart;
//		LogFiles.erase(iter);
//	}
//}
bool CLogManager::StartLogManager(int LogFileFlushTime)
{
    CThreadWithQueue::SetTimeout(LogFileFlushTime * 1000);
    // If the path in the configuration is fully qualified 
    // don't attach to it the program path
//     if(std::string::npos == m_LogDirectory.find(":"))
//         m_LogDirectory = std::string(GetProgramPath()) +  m_LogDirectory;

    // This makes full path according to current path
    // We change path to program path, and assume that it does not change
    m_LogDirectory = GetFullPathName(m_LogDirectory);

    //    CreateDirectory(m_LogDirectory.c_str(), NULL);
    // Creates also intermediate directories
    SHCreateDirectoryEx(NULL, m_LogDirectory.c_str(), NULL);


    if (m_BaseName.size() > 0)
    {
        m_LogFileName = m_LogDirectory + "\\" + m_BaseName + ".log";
        if(m_LogFile.Open(m_LogFileName.c_str()))
        {
            
            bool SuccessfulStart = CThreadWithQueue::StartThread();			
            if(SuccessfulStart)
            {
                m_AllowMessage = true;
                m_IsInitialized = true;
            }
            return SuccessfulStart;
        }
    }

    m_AllowMessage = false;
    return false;
}

void CLogManager::OnTimeout()
{
    m_LogFile.Flush();

    time_t CurrentTime = time(NULL);
    time_t LogFileCreationTime = m_LogFile.GetCreationTime();
    DWORD FileSize = m_LogFile.GetFileSize();
    if (FileSize > (DWORD)m_MaxLogFileSize || 
        CurrentTime - LogFileCreationTime > m_MaxLogFileOpenTime)
    {
        time_t FileCreationTime = m_LogFile.GetCreationTime();
        m_LogFile.Close();
        AddDateToLogFileName(FileCreationTime);
        m_LogFile.Open(m_LogFileName.c_str());
        m_LogFile.MarkCurrentTimeAsCreationTime();
		std::string WildCart = "*" + m_BaseName;
        DeleteOldFiles(m_LogDirectory.c_str(), WildCart.c_str(), LogExt, m_MaxLogFileTotalSize, m_MaxLogFileLiveTime, m_LogFileName.c_str()); 
    }
}

const char * CLogManager::GetLogFileName()
{
    return m_LogFileName.c_str();
}
//////////////////////////////////////////////////////////////////////

#ifdef _TEST

#include "LogEvent.h"

void TestLogManager()
{
    {
    CLogManager LogMagager;

    LogMagager.Init("Test");

    for (int i = 0; i < 10000000; ++i)
    {
        char Line[64];
        sprintf_s(Line, "Log line %d", i);
        LogMagager.LogLine(LE_INFO, Line);
        //Sleep(10);
    }
    Sleep(1000);
    }
}

#endif