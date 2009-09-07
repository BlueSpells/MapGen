#pragma once

#include "LogEvent.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// A class used by the log manager - encapsulates access to log file

class CLogFile
{
public:
    CLogFile();
    ~CLogFile();

    bool Open(const char* Name);
    void Close();

    bool IsOpen();

    bool LogLine(time_t Seconds, int Millis, ELogSeverity Severity, const char* Line);
    void Flush();

    DWORD GetFileSize();

    // Force creation time - there was a problem with the creation time set by NTFS
    // see Q172190 on MSDN
    void MarkCurrentTimeAsCreationTime();

    time_t GetCreationTime();
private:
    const char* GetSeverityString(ELogSeverity Severity);
    inline bool CLogFile::ValidateOpen(const char* Routine)
    {
        if (m_hFile != INVALID_HANDLE_VALUE)
            return true;
#ifdef _DEBUG
        LogEventToDebugOutput("CLogFile::%s: m_hFile == NULL\n", Routine);
#else
        Routine;
#endif
        return false;
    }

    time_t m_CreationTime;
    HANDLE m_hFile;
};