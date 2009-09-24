// LogEvent.cpp: implementation of the LogEvent routine.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogEvent.h"
#include "Config.h"
#include "ConfigVariables.h"
#include "TimeStamp.h"
#include "LogManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char* LogSection = "LogManager";
static std::string g_LogMessagesBaseName;
static LogEventOutput g_TheLogEventOutput = NULL;
static GetLogEventOutputSeverity g_GetSeverity = NULL;
static bool g_LogMessagesToFile = false;

class CConfigLogSeverity : public CConfigVariable
{
public:
    CConfigLogSeverity() : CConfigVariable(LogSection, "LogSeverity")
    {
    }

    operator ELogSeverity()
    {
        if (HasToRefresh())
        {
            ELogSeverity Value = (ELogSeverity)GetConfigInt(GetSection(), GetKey(), LE_INFO);
            if (Value > LE_WARNING) // Always log Warnings and up
                Value = LE_WARNING;

            if (m_Value != Value)
            {
                if (m_Value != -1)
                    LogEvent(LE_INFOHIGH, "CConfigLogSeverity: LogSeverity set to %d (%s)", Value, GetLogEventLevelStr(Value));
                m_Value = Value;
            }
        }

        return m_Value;
    }

    void SetSeverity(ELogSeverity LogSeverity)
    {
        m_Value = LogSeverity;
        WriteConfigInt(GetSection(), GetKey(), m_Value);
    }

    static CConfigLogSeverity& GetTheLogSeverity()
    {
        static CConfigLogSeverity TheLogSeverity;
        return TheLogSeverity;
    }
private:
    ELogSeverity m_Value;
};


static CLogManager TheLogManager; // Global log files manager


static void AddToLog(const char* Line, ELogSeverity Severity)
{
    if (Severity < CConfigLogSeverity::GetTheLogSeverity() || !g_LogMessagesToFile)
        return;

    static bool IsInitialized = false;
    static bool FirstLogRequest = true;
    static int  LineNumber = 0;
    if( !TheLogManager.IsInitialized() && FirstLogRequest)
    {
        FirstLogRequest = false; // Attempt to initialize the LogManager only once (up to multiple thread access)
        static CCriticalSection Locker;
        Locker.Lock();
        if( !IsInitialized ) // To save performance. Lock is executed only once.
        {
            IsInitialized = true; // To avoid LogEvent messages from ThreadWithQueue
            std::string BaseName = GetConfigString(LogSection, "LogBaseName", g_LogMessagesBaseName.c_str());
            TheLogManager.Init(BaseName.c_str());
        }
        Locker.Unlock();
    }
    if( TheLogManager.IsInitialized() )
        TheLogManager.LogLine(Severity, Line);
    else
    {
        // This is a patch to pay the attention of the user to the fact that
        // the LogManager wasn't initialized.
        if (LineNumber % 100 == 0)
        {
            LogEventToDebugOutput("AddToLog : TheLogManager is not initialized. Ignoring log event.\n");
            if (g_TheLogEventOutput != NULL)
                g_TheLogEventOutput(LE_ERROR, "AddToLog : TheLogManager is not initialized. Ignoring log event.\n");
        }
        ++LineNumber;
    }
}

void SetLogEventOutput(LogEventOutput TheOutput, bool LogMessagesToFile, GetLogEventOutputSeverity GetSeverity /*= NULL*/, const char* LogMessagesBaseName /*= ""*/)
{
    g_LogMessagesToFile = LogMessagesToFile;
    g_GetSeverity = GetSeverity;
    g_TheLogEventOutput = TheOutput;
    g_LogMessagesBaseName = LogMessagesBaseName;
}


#include "TimeStamp.h"
void LogEvent(ELogSeverity Severity, const char* Format, ...)
{
    va_list Args;
    va_start(Args, Format);

    VLogEvent(Severity, Format, Args);

    va_end(Args);
}

void VLogEvent(ELogSeverity Severity, const char* Format, va_list Args)
{
    //prevent printing of log events with debug level - according to configuration
    static CConfigBool PrintDebugMsgs(LogSection, "PrintDebugMsgs", false); 
    if(!PrintDebugMsgs && (Severity == LE_DEBUG) )
        return;

    if ( (Severity < CConfigLogSeverity::GetTheLogSeverity() || !g_LogMessagesToFile)  &&
        (g_GetSeverity != NULL && Severity < g_GetSeverity()))
        return;

    char Message[MAX_LOG_MESSAGE_SIZE];

    try
    {
        vsnprintf_s(Message, sizeof Message, _TRUNCATE, Format, Args);
        //    _vsnprintf_s(Message, _TRUNCATE, Format, Args); - this has some error in definition: _Size instead of _Count
    }
    catch (...)
    {
        bool ExceptionOcurredAtVLogEvent = true;
        Assert(!ExceptionOcurredAtVLogEvent);
        _snprintf_s(Message, sizeof Message, _TRUNCATE, "VLogEvent: the format [%s] caused an exception", Format);
        Severity = LE_ERROR;
    }

    // Just to see if we reached max
    Assert(strlen(Message) < sizeof Message);

    if (g_TheLogEventOutput != NULL)
    {
        // The outputs are responsible to add a TimeStamp
        // Make sure that the time stamp is added before moving to another thread.
        g_TheLogEventOutput(Severity, Message);
        AddToLog(Message, Severity);
    }
    else
    {
        CTimeStamp CurrentTime = CTimeStamp::Now();
        std::string MessageWithTime = CurrentTime.ToShortString();
        MessageWithTime.append(" : ");
        MessageWithTime.append(Message);

        std::string Msg = GetLogEventLevelStr(Severity);
        Msg.append("-");
        Msg.append(MessageWithTime);
        Msg.append("\n");
        OutputDebugString(Msg.c_str());
    }

#if 0
    if (Severity == LE_FATAL)
        FatalEx(Message, FF_DO_NOT_EXIT, 0);
#endif
}

void SetLogLevel(ELogSeverity LogLevel)
{
    CConfigLogSeverity::GetTheLogSeverity().SetSeverity(LogLevel);
}

ELogSeverity GetLogLevel()
{
    ELogSeverity Severity = CConfigLogSeverity::GetTheLogSeverity();
    if (g_GetSeverity != NULL)
        Severity = min(Severity, g_GetSeverity());

    return Severity;
}

const char * GetLogEventLevelStr(int Level)
{
    switch(Level)
    {
    RETURN_TYPE_STR(LE_DEBUG);
    RETURN_TYPE_STR(LE_INFOLOW);
    RETURN_TYPE_STR(LE_INFO);
    RETURN_TYPE_STR(LE_INFOHIGH);
    RETURN_TYPE_STR(LE_NOTICE);
    RETURN_TYPE_STR(LE_WARNING);
    RETURN_TYPE_STR(LE_ERROR);
    RETURN_TYPE_STR(LE_FATAL);
    default:
        return "LE_UNDEFINED";
    }
}


//////////////////////////////////////////////////////////////////////////
static bool ForceDoNotBreak = false;
static std::string DefaultLogName;

static std::string GetAssertionsFileName()
{
    char Drive[_MAX_DRIVE], Dir[_MAX_DIR], Fname[_MAX_FNAME], Ext[_MAX_EXT];
    char DefaultAssertionsFileName[_MAX_PATH];
    const char * LogFileName = GetLogFileName();
    if (LogFileName[0] != '\0')
        _splitpath_s(LogFileName, Drive, Dir, Fname, Ext);
    else
        _splitpath_s(DefaultLogName.c_str(), Drive, Dir, Fname, Ext);
    std::string AssertionsFileName = std::string(Fname) + "Assertions";
    _makepath_s(DefaultAssertionsFileName, Drive, Dir, AssertionsFileName.c_str(), Ext);
    std::string AssertLogFileName = GetConfigString(LogSection, "AssertLogFileName", DefaultAssertionsFileName);
    return AssertLogFileName;
}

void SetBreakOnAssert(bool Break)
{
    ForceDoNotBreak = !Break;
}

void SetLogName(const char * Name)
{
    DefaultLogName = Name;
}

void DoAssert(const char *Exp, const char *File, unsigned Line)
{
    char Message[1024];
    sprintf_s(Message, "Assertion failed: %s, file %s, line %d", Exp, File, Line);
    static bool DoBreak = GetConfigBool(LogSection, "BreakOnAssert", true); // allow disabling break
    LogEvent(LE_ERROR, Message);
    if (DoBreak && !ForceDoNotBreak)
        DebugBreak();
    else
    {
        static bool FirstTime = true;

        static std::string AssertLogFileName = GetAssertionsFileName();
        //FILE *f = fopen(AssertLogFileName.c_str(), "at");
        FILE *f = NULL;
        //Verify (fopen_s(&f, AssertLogFileName.c_str(), "at") == 0);
        fopen_s(&f, AssertLogFileName.c_str(), "at");
        if (f != NULL)
        {
            if (FirstTime)
                fprintf(f, "--------------------------------------------------------------------------\n");
            FirstTime = false;
            time_t Seconds;
            int Millis;
            GetSecondsAndMillis(Seconds, Millis);
            std::string TimeString = FormatTime(Seconds, Millis);
            fprintf(f, "%s %s\n", TimeString.c_str(), Message);
            fclose(f);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
CFilteredLogEvent::CFilteredLogEvent():
    m_TotalValue(0), m_LastLogTime(0), m_Severity(LE_INFOLOW)
  , m_LogRateInMilli(1000)
  , m_LogIfZeroValue(true)
{
}

void CFilteredLogEvent::Init(ELogSeverity Severity, const char* ContextStr, DWORD LogRateInMilli, bool LogIfZeroValue)
{
    m_ContextStr = ContextStr;
    m_LogRateInMilli = LogRateInMilli;
    m_TotalValue = 0;
    m_LastLogTime = 0;
    m_LogIfZeroValue = LogIfZeroValue;
    m_Severity = Severity;
}

bool CFilteredLogEvent::IsInitialized() const
{
    return !m_ContextStr.empty();
}

void CFilteredLogEvent::Reset(DWORD LastLogTime)
{
    m_TotalValue = 0;
    m_LastLogTime = LastLogTime;
}

void CFilteredLogEvent::NotifyEvent(int Value)
{
    m_TotalValue += Value;
    DWORD CurrentTime = GetTickCount();
    DWORD Diff = CurrentTime - m_LastLogTime;
    if(Diff >= m_LogRateInMilli)
    {
        if(m_TotalValue > 0 || m_LogIfZeroValue)
            LogEvent(m_Severity, "%s : %d [over the last %d Milli]",
                m_ContextStr.c_str(), 
                m_TotalValue, Diff);
        Reset(CurrentTime);
    }
}


//////////////////////////////////////////////////////////////////////////
//CFilteredLogEventWithFirstOccurrence class
CFilteredLogEventWithFirstOccurrence::CFilteredLogEventWithFirstOccurrence() :
  CFilteredLogEvent()
{
}

void CFilteredLogEventWithFirstOccurrence::Init(ELogSeverity Severity, const char* ContextStr,
                                          DWORD LogRateInMilli, bool LogIfZeroValue,
                                          DWORD TimeToResetFirstOccurance)
{
    m_TimeToResetFirstOccurance = TimeToResetFirstOccurance;
    CFilteredLogEvent::Init(Severity, ContextStr, LogRateInMilli, LogIfZeroValue);
    return;
}

void CFilteredLogEventWithFirstOccurrence::NotifyEvent(int Value, ELogSeverity FirstOccuranceSeverity,
                                                 const char* FirstOccuranceFormat, ...)
{
    DWORD CurrentTime = GetTickCount();
    DWORD Diff = CurrentTime - m_LastLogTime;
    if (Diff > m_TimeToResetFirstOccurance)
        Reset(0);

    if (m_LastLogTime == 0)
    { // First time need to log
        va_list Args;
        va_start(Args, FirstOccuranceFormat);

        VLogEvent(FirstOccuranceSeverity, FirstOccuranceFormat, Args);

        m_LastLogTime = CurrentTime;
        return;
    }

    CFilteredLogEvent::NotifyEvent(Value);
    return;
}


//////////////////////////////////////////////////////////////////////////
//LogSeverityPolicy class
CLogSeverityPolicy::CLogSeverityPolicy(ELogSeverity FirstTimeSeverity, ELogSeverity GeneralSeverity, DWORD ResetTimeWhenNoLogs)
: m_LastLogTime(0), m_FirstTimeSeverity(FirstTimeSeverity), m_GeneralSeverity(GeneralSeverity),
  m_ResetTimeWhenNoLogs(ResetTimeWhenNoLogs)
{
}

ELogSeverity CLogSeverityPolicy::GetLogSeverity(bool NotifyLogWasMade)
{
    DWORD Diff =  GetTickCount() - m_LastLogTime;
    if (Diff > m_ResetTimeWhenNoLogs)
        m_LastLogTime = 0;

    ELogSeverity Severity = (m_LastLogTime == 0) ? m_FirstTimeSeverity : m_GeneralSeverity;
    if (NotifyLogWasMade == true)
        NotifyLog();
    return Severity;
}

void CLogSeverityPolicy::NotifyLog()
{
    m_LastLogTime = GetTickCount();
    return;
}

const char * GetLogFileName()
{
    return TheLogManager.GetLogFileName();
}

//////////////////////////////////////////////////////////////////////////
//CPeriodicLogEvent class
CPeriodicLogEvent::CPeriodicLogEvent(DWORD LogRateInMilli) :
m_LogRateInMilli(LogRateInMilli)
, m_LastLogTime(0)
{
}


//////////////////////////////////////////////////////////////////////////
void CPeriodicLogEvent::LogEvent(ELogSeverity Severity, const char* Format, ...)
{
    DWORD Now = GetTickCount();
    if (Now - m_LastLogTime < m_LogRateInMilli)
        return;

    m_LastLogTime = Now;
    va_list Args;
    va_start(Args, Format);

    VLogEvent(Severity, Format, Args);
    return;
}


//////////////////////////////////////////////////////////////////////////
void CPeriodicLogEvent::Reset()
{
    m_LastLogTime = GetTickCount() - m_LogRateInMilli - 1; // -1 is for last time to be less than (Now - LogRate)
}



//////////////////////////////////////////////////////////////////////////

#ifdef _TEST

void TestFilteredLogEventWithFirstOccurance()
{
    CFilteredLogEventWithFirstOccurrence Tester;
    DWORD LogInterval = 2000;
    DWORD ResetInterval = 5000;
    char Format [] = "This log is supposed to be printed each %d seconds";
    char Message[256];
    sprintf_s(Message, Format, (int)(LogInterval / 1000));
    Tester.Init(LE_INFOHIGH, Message, LogInterval, false, ResetInterval);

    DWORD Start = GetTickCount();
    while (GetTickCount() - Start < LogInterval * 4)
    {
        Tester.NotifyEvent(1, LE_WARNING, "First Occurrence only");
        WaitAndPumpMessages(1);
    }

    // Waiting for reset
    WaitAndPumpMessages(ResetInterval + 100);

    Tester.NotifyEvent(1, LE_WARNING, "This should be after reset [%d milli]", ResetInterval);
    return;
}

void TestPeriodicLogEvent()
{
    LogEvent(LE_INFOHIGH, "Start testing periodic log event");
    DWORD LogInterval = 2000;
    CPeriodicLogEvent Tester(LogInterval);
    DWORD Start = GetTickCount();
    while ( GetTickCount() - Start < LogInterval * 4)
    {
        Tester.LogEvent(LE_INFOHIGH, "This log is supposed to be printed each %d seconds", (int)(LogInterval / 1000));
        WaitAndPumpMessages(1);
    }


    LogEvent(LE_INFOHIGH, "Now doing reset and the next log is supposed to be log immediately");
    Tester.Reset();
    Tester.LogEvent(LE_INFOHIGH, "This log is supposed to be printed only once after reset");
    return;
}

void TestBasicLogEvent()
{
    LogEvent(LE_INFO, "Testing Log event [%s]", "INFO");
    LogEvent(LE_INFOHIGH, "Testing Log event [%s]", "INFOHIGH");
    LogEvent(LE_WARNING, "Testing Log event [%s]", "WARNING");
    LogEvent(LE_ERROR, "Testing Log event [%s]", "ERROR");
    LogEvent(LE_FATAL, "Testing Log event [%s]", "FATAL");

    Assert(true);
    //Assert(false);
}

void TestLogEvent()
{
    //TestBasicLogEvent();
    TestFilteredLogEventWithFirstOccurance();
    TestPeriodicLogEvent();
}

#endif // _TEST