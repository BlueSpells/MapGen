// LogEvent.h: declaration of the LogEvent routine.
//
//////////////////////////////////////////////////////////////////////

#pragma once
enum ELogSeverity
{
	LE_DEBUG = 0,
    LE_INFOLOW,     // Display on log as dimmed
    LE_INFO,        // Display on log
    LE_INFOHIGH,    // Display on log with some emphasize
    LE_NOTICE,      // Display on log with stronger emphasize
    LE_WARNING, 
    LE_ERROR, 
    LE_FATAL        // Error that we cannot live with
};

const char * GetLogEventLevelStr(int Level);
const int MAX_LOG_MESSAGE_SIZE = 2048;

void LogEvent(ELogSeverity Severity, const char* Format, ...);
void VLogEvent(ELogSeverity Severity, const char* Format, va_list Args);
void SetLogLevel(ELogSeverity LogLevel);
ELogSeverity GetLogLevel();
const char * GetLogFileName();

#ifdef  NDEBUG
#define Assert(exp) ((void)0)
#define Verify(f)   ((void)(f))
#define DebugOnly(x) ((void)0)
#define CheckExpReturn(exp, ContextStr) \
    if(!(exp)) \
    { \
        LogEvent(LE_ERROR, #exp " %s Failed", ContextStr); \
        return; \
    }

#define CheckExpReturnFalse(exp, ContextStr) \
    if(!(exp)) \
{ \
    LogEvent(LE_ERROR, #exp " %s Failed", ContextStr); \
    return false; \
}

#else
void DoAssert(const char *Exp, const char *File, unsigned Line);
#define Assert(Exp) (void)((Exp) || (DoAssert(#Exp, __FILE__, __LINE__), 0)) 
#define Verify(f)   Assert(f)
#define DebugOnly(x) x
#define CheckExpReturn(exp, ContextStr) Assert(exp)
#define CheckExpReturnFalse(exp, ContextStr) \
{  \
    bool Success = (exp); \
    Assert(Success); \
    if(!Success) \
        return false;  \
}

#endif

// Used to determine where the output of the log event goes
// default is OutputDebugString
typedef void (*LogEventOutput)(ELogSeverity Severity, const char* Text);
typedef ELogSeverity (*GetLogEventOutputSeverity)();
void SetLogEventOutput(LogEventOutput TheOutput, bool LogMessagesToFile, GetLogEventOutputSeverity GetSeverity = NULL, const char* LogMessagesBaseName = "");

void SetBreakOnAssert(bool Break);
void SetLogName(const char * Name);

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define FILE_LINE_MSG(x) message(__FILE__ "(" STRING(__LINE__) ") : *********** " x " ***********")
#define FILE_LINE_STR	__FILE__ " (" STRING(__LINE__) "):"

//////////////////////////////////////////////////////////////////////////
// The CFilteredLogEvent will help filter events that occur too often
#include <string>

class CFilteredLogEvent
{
public:
    CFilteredLogEvent();
    void Init(ELogSeverity Severity, const char* ContextStr, DWORD LogRateInMilli,
              bool LogIfZeroValue);
    bool IsInitialized() const;
    void NotifyEvent(int Value);

protected:
    void Reset(DWORD LastLogTime);

protected:
    DWORD m_LastLogTime;
private:
    int m_TotalValue;
    std::string m_ContextStr;
    DWORD m_LogRateInMilli;
    ELogSeverity m_Severity;
    bool m_LogIfZeroValue;
};


//////////////////////////////////////////////////////////////////////////
class CFilteredLogEventWithFirstOccurrence : public CFilteredLogEvent
{
public:
    CFilteredLogEventWithFirstOccurrence();
    void Init(ELogSeverity Severity, const char* ContextStr, DWORD LogRateInMilli,
              bool LogIfZeroValue, DWORD TimeToResetFirstOccurance);
    void NotifyEvent(int Value, ELogSeverity FirstOccuranceSeverity, const char* FirstOccuranceFormat, ...);

private:
    DWORD m_TimeToResetFirstOccurance;
};


//////////////////////////////////////////////////////////////////////////
class CLogSeverityPolicy
{
public:
    CLogSeverityPolicy(ELogSeverity FirstTimeSeverity, ELogSeverity GeneralSeverity, DWORD ResetTimeWhenNoLogs);

    ELogSeverity GetLogSeverity(bool NotifyLogWasMade);
    void NotifyLog(); // When log was made

private:
    ELogSeverity m_FirstTimeSeverity;
    ELogSeverity m_GeneralSeverity;
    DWORD m_ResetTimeWhenNoLogs;
    DWORD m_LastLogTime;
};


//////////////////////////////////////////////////////////////////////////
// a class that is allow logging event only once in a while
class CPeriodicLogEvent
{
public:
     CPeriodicLogEvent(DWORD LogRateInMilli);

     void LogEvent(ELogSeverity Severity, const char* Format, ...);
     void Reset(); // To enable loggin the next time LogEvent() is called

private:
    DWORD m_LogRateInMilli;
    DWORD m_LastLogTime;
};

