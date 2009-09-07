#pragma once

#include <string>
#include "LogEvent.h"
#include "LogFile.h"
#include "ThreadWithQueue.h"

//////////////////////////////////////////////////////////////////////
// Log lines to log files.
// o Files are managed in a log directory.
// o A file is closed when it reaches a configurable limit.
// o Closed files names repersent their creation time
// o Log files are deleted after a configurable live time.

class CLogManager : public CThreadWithQueue
{
public:
    CLogManager();
	~CLogManager();

    bool Init(const char* BaseName);
    bool InitDefaults(const char* BaseName, const char* LogFileDirectory, int MaxLogFileSizeMega );
    void LogLine(ELogSeverity Severity, const char* Line);
    bool IsInitialized();
    const char * GetLogFileName();
private:
    bool StartLogManager(int LogFileFlushTime);
    // Implemented CThreadWithQueue methods
    void OnTimeout();

    struct LineInfo
    {
        time_t       Seconds;
        int          Millis;
        ELogSeverity Severity;
        std::string  Line;
    };
    void HandleLogLine(const LineInfo& Info);

    // Close file and add creation date to file name, so
    // FEIS.log --> 2005_02_10_11_20_FEIS.log
    void AddDateToLogFileName(time_t FileCreationTime);

    //void DeleteOldFiles();
    void MoveToNextFile();

    std::string m_BaseName;
    CLogFile    m_LogFile;
    std::string m_LogDirectory;
    std::string m_LogFileName;
    int         m_MaxLogFileSize;
    int         m_MaxLogFileOpenTime;
    int         m_MaxLogFileLiveTime;
	ULARGE_INTEGER m_MaxLogFileTotalSize;
 	bool m_AllowMessage;
    bool m_IsInitialized ;
};