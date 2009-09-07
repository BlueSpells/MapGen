#include "StdAfx.h"
#include <Time.h>
#include "TimeStamp.h"
#include "Common/LogEvent.h"
#include "Common/Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// According to time(time_t*) and mktime(tm*) Documentation
const CTimeStamp CTimeStamp::Infinity(2038, 1, 18, 19, 14, 7, 0);
// const CTimeStamp CTimeStamp::Infinity(0x7fff735f); // An alternative impl
const CTimeStamp CTimeStamp::ZeroTime; // Default constructor is zero time.


//Static functions
static time_t LocalToGMTSeconds(int Year, int Month, int Day, int Hour, int Min, int Sec, int Dst)
{
    tm _Time;
    _Time.tm_sec    = Sec;          // 0 - 59
    _Time.tm_min    = Min;          // 0 - 59
    _Time.tm_hour   = Hour;         // 0 - 23
    _Time.tm_mday   = Day;          // 1 - 31
    _Time.tm_mon    = Month - 1;    // 0 - 11
    _Time.tm_year   = Year - 1900;  // Years since 1900
    _Time.tm_wday   = 0;            // Days since Sunday 0 - 6
    _Time.tm_yday   = 0;            // 0 - 365
    _Time.tm_isdst  = Dst;          // Daylight savings time flag

    return mktime(& _Time);
}

static time_t GMTToGMTSeconds(int Year, int Month, int Day, int Hour, int Min, int Sec)
{
    tm _Time;
    _Time.tm_sec    = Sec;          // 0 - 59
    _Time.tm_min    = Min;          // 0 - 59
    _Time.tm_hour   = Hour;         // 0 - 23
    _Time.tm_mday   = Day;          // 1 - 31
    _Time.tm_mon    = Month - 1;    // 0 - 11
    _Time.tm_year   = Year - 1900;  // Years since 1900
    _Time.tm_wday   = 0;            // Days since Sunday 0 - 6
    _Time.tm_yday   = 0;            // 0 - 365
    _Time.tm_isdst  = 0;            // Daylight savings time flag

    return mkgmtime(& _Time);
}


// Constructors
CTimeStamp::CTimeStamp(bool SetToNow /*= false*/)
{
	Reset();

	if (SetToNow)
	{
		GetSystemTime();
	}
}

CTimeStamp::CTimeStamp(const FILETIME& FileTime)
{
    Reset();

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
    m_TimeB.time	= mktime(&Tm) - timezone;
    m_TimeB.millitm = 0;
}


//CTimeStamp::CTimeStamp(int nMilli)
//{
//	Reset();
//	m_TimeB.time	= nMilli / 1000;
//	m_TimeB.millitm = nMilli % 1000;
//}

//CTimeStamp::CTimeStamp(tm& TM)
//{
//	Reset();
//	m_TimeB.time	= mktime(& TM);
//	m_TimeB.millitm	= 0;
//}

CTimeStamp::CTimeStamp(time_t Time, int Milli)
{
	Reset();
    Assert(Milli >= 0 && Milli < 1000);
	m_TimeB.time	= Time;
	m_TimeB.millitm = (unsigned short)Milli;
}

CTimeStamp::CTimeStamp(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMilli)
{
	Reset();
	m_TimeB.time	= GMTToGMTSeconds(nYear, nMonth, nDay, nHour, nMin, nSec);

	m_TimeB.millitm = (unsigned short)nMilli;
}

CTimeStamp::~CTimeStamp()
{
}

CTimeStamp CTimeStamp::Now()
{
	CTimeStamp CurrentTime(true);

	return CurrentTime;
}

// Combined Operators
CTimeStamp& CTimeStamp::operator -=(const CTimePeriod& Other)
{
	// We only subtract positive TimePeriods
	if(Other < CTimePeriod::ZeroLength)
		return operator +=(Other / -1);

	CTimePeriod Temp(m_TimeB.time, m_TimeB.millitm);
	Assert(Temp >= Other); // Don't know how to handle negative time

	if(m_TimeB.millitm < Other.GetMilli())
	{
		m_TimeB.millitm = m_TimeB.millitm + (unsigned short)(1000 - Other.GetMilli());
		m_TimeB.time	-= 1;
	}
	else
		m_TimeB.millitm = (unsigned short)(m_TimeB.millitm - Other.GetMilli());

	m_TimeB.time -= Other.GetSeconds();

	return *this;
}

CTimeStamp& CTimeStamp::operator +=(const CTimePeriod& Other)
{
	// We only sum positive TimePeriods. If they are negative
	// we subtract their absolute value. 
	if(Other < CTimePeriod::ZeroLength)
		return operator -=(Other / -1);

	m_TimeB.millitm = (unsigned short)(m_TimeB.millitm + Other.GetMilli());
	m_TimeB.time	+= Other.GetSeconds();
	if(m_TimeB.millitm >= 1000)
	{
		m_TimeB.time += 1;
		m_TimeB.millitm -= 1000;
	}

	return *this;
}

CTimePeriod CTimeStamp::operator -(const CTimeStamp& Other) const
{
	time_t	Seconds = m_TimeB.time		- Other.m_TimeB.time;
	int		Milli	= m_TimeB.millitm	- Other.m_TimeB.millitm;

    // Since TimePeriod can be constructed with Milli & Seconds with the same sign -
    // Need to change the sign of the Milli to match the sign of seconds
	if(Milli < 0 && Seconds > 0)
	{
		Milli	+= 1000;
		Seconds -= 1;
	}
    if(Milli > 0 && Seconds < 0)
    {
        Milli	-= 1000;
        Seconds += 1;
    }

	return CTimePeriod(Seconds, Milli);
}

// Methods
std::string CTimeStamp::ToString() const
{
	char Buffer[32];
	
	// Hazard: Thread safe? Same buffer for multiple executions of ctime, asctime...
	//char* TimeString = ctime(& (m_TimeB.time));
    char TimeString[64];
    errno_t err = ctime_s(TimeString, sizeof TimeString, & (m_TimeB.time));
    Assert (err == 0);
	{ // debug
		if(err != 0)
		{
			return "CTimeStamp::ToString error. ctime() returned NULL pointer";
		}
	}
	Assert(NULL != TimeString);

	// Add milliseconds to the time string
	sprintf_s(Buffer, "%.19s.%03hu %s", TimeString, m_TimeB.millitm, &TimeString[20]);
	
	if(Buffer[strlen(Buffer) - 1] == '\n')
		Buffer[strlen(Buffer) - 1] = '\0';

	return Buffer;
}

std::string CTimeStamp::ToShortString() const
{
	char TimeString[128];

	//struct tm* Tm = localtime(&m_TimeB.time);
    struct tm Tm;
    errno_t err = localtime_s(&Tm, &m_TimeB.time);
    Assert(err == 0);
    if (err != 0)
        return std::string("!!Error!!");

	const char* Format = "%d.%m.%y [%H:%M:%S.";
	strftime(TimeString, sizeof TimeString, Format, &Tm);

	char MilliString[5];
	sprintf_s(MilliString, "%03u]", m_TimeB.millitm);

	return std::string(TimeString) + MilliString;
}

std::string CTimeStamp::ToShortGMTString() const
{
	char TimeString[128];

	//struct tm* Tm = gmtime(&m_TimeB.time);
    struct tm Tm;
    errno_t err = gmtime_s(&Tm, &m_TimeB.time);
    Assert(err == 0);
    if (err != 0)
        return std::string("!!Error!!");

    const char* Format = "%d.%m.%y [%H:%M:%S.";
	strftime(TimeString, sizeof TimeString, Format, &Tm);

	char MilliString[5];
	sprintf_s(MilliString, "%03u]", m_TimeB.millitm);

	return std::string(TimeString) + MilliString;
}

bool CTimeStamp::FromString(const char* TimeString)
{
	char* Months[12] = { "Jan", "Feb", "Mar", "Apr",
					 "May", "Jun", "Jul", "Aug",
					 "Sep", "Oct", "Nov", "Dec" };
	int Year, Month, Day, Hour, Min, Sec, Milli;
	char MonthString[4];
	bool Success;
	
	//										 Day  Mon MoD Hou Min Sec Mil Yea
	int FieldsAssigned = sscanf_s(TimeString, "%*3c %3c %2d %2d:%2d:%2d.%3d %4d",
												  &MonthString, &Day, &Hour, &Min, &Sec, &Milli, &Year);
	MonthString[3] = '\0';
	Month = 0;
	for(int i = 0; i < 12; ++i)
	{
		if(!strcmp(MonthString, Months[i]))
		{
			Month = i + 1;
			break;
		}
	}

	Success = (FieldsAssigned == 7 && Month != 0);
	if(Success)
	{
		// Set Time
		Reset();
		m_TimeB.time = LocalToGMTSeconds(Year, Month, Day, Hour, Min, Sec, -1); //Should take into acount DST configuration
		m_TimeB.millitm = (unsigned short)Milli;
	}

	return Success;
}

bool CTimeStamp::FromShortString(const char* ShortString, bool IsGmtString /*= false*/)
{
	int Year, Month, Day, Hour, Min, Sec, Milli;
	//										  Day Mon Yea  Hou Min Sec Mil 
	int FieldsAssigned = sscanf_s(ShortString, "%2d.%2d.%2d [%2d:%2d:%2d.%3d]",
		&Day, &Month, &Year, &Hour, &Min, &Sec, &Milli);

	// The year is represented in the string by the two last digits
	// we need two add the first two. The tm struct holds the year
	// with a relative offset to 1900.
    // If the year is less than 70, it is in the 21st century, otherwise is is in the 20th century

    if (Year < 70)
        Year += 2000;
    else
        Year += 1900;

	bool Success = (FieldsAssigned == 7);
	if(Success)
	{
		// Set Time
		Reset();
        if (IsGmtString)
		    m_TimeB.time = GMTToGMTSeconds(Year, Month, Day, Hour, Min, Sec);
        else // local time
            m_TimeB.time = LocalToGMTSeconds(Year, Month, Day, Hour, Min, Sec, -1); //Should take into acount DST configuration
		m_TimeB.millitm = (unsigned short)Milli;
	}

	return Success;
}

bool CTimeStamp::FromShortGMTString(const char* ShortGMTString)
{
    return FromShortString(ShortGMTString, true);
}

// ignores milli seconds
void CTimeStamp::ToLocalTime(tm* tm_struct) const
{
// 	tm* _Time = localtime(& m_TimeB.time);
// 	memcpy(tm_struct, _Time, sizeof tm);
    Verify(localtime_s(tm_struct, & m_TimeB.time) == 0);
}

// ignores milli seconds
void CTimeStamp::ToGMTTime(tm* tm_struct) const
{
// 	tm* _Time = gmtime(& m_TimeB.time);
// 	memcpy(tm_struct, _Time, sizeof tm);
    Verify(gmtime_s(tm_struct, & m_TimeB.time) == 0);
}

void CTimeStamp::Reset()
{
	// m_TimeB.dstflag = _daylight;
	// m_TimeB.timezone = _timezone / 60; // _timezone is in seconds whereas _timeb.timezone is in minutes
	// Used to set the previous two variables of _timeb
	//_ftime(& m_TimeB);
    //Verify(_ftime_s(& m_TimeB) == 0);
	
	m_TimeB.time	= 0;
	m_TimeB.millitm	= 0;

}

void CTimeStamp::GetDate(int& Year, int& Month, int& Day) const
{
    tm GMTTime;
    ToGMTTime(&GMTTime);

    Year = GMTTime.tm_year + 1900;
    Month = GMTTime.tm_mon + 1;
    Day = GMTTime.tm_mday;
}

// since we don't know how to handle negative TimePeriod
// this method spares its user the if that it performs.
CTimePeriod CalcTimeDiff(const CTimeStamp& Left,
						 const CTimeStamp& Right)
{
	if(Left >= Right)
		return Left - Right;
	else
		return Right - Left;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CTimeStampTester
{
public:
	bool FromStringTest();
	bool GeneralTest();
	bool TestShortGMTString();
};

bool CTimeStampTester::GeneralTest()
{
	CTimeStamp Yesterday;
	CTimeStamp Today;
	CTimeStamp Tommorow;
	CTimePeriod OneDay(24 * 60 * 60 * 1000);
	CTimePeriod TwoDays(48 * 60 * 60 * 1000);

	Today.GetSystemTime();
	Yesterday = Today - OneDay;
	Tommorow  = Today + OneDay;
	
	//LogEvent(LE_DEBUG, "OneDay: %s", OneDay.ToString().c_str());
	//LogEvent(LE_DEBUG, "TwoDays: %s", TwoDays.ToString().c_str());
//	LogEvent(LE_DEBUG, "CTimeStamp: Today: %s", Today.ToString().c_str());
//	LogEvent(LE_DEBUG, "CTimeStamp: Yesterday: %s", Yesterday.ToString().c_str());
//	LogEvent(LE_DEBUG, "CTimeStamp: Tommorow: %s", Tommorow.ToString().c_str());

	return Today > Yesterday
		&& Tommorow > Today
		&& Tommorow - Yesterday == TwoDays;
}

bool CTimeStampTester::FromStringTest()
{
	CTimeStamp CurrentTime;
	CTimeStamp TimeFromString;
	std::string CurrentTimeString;

	CurrentTime.GetSystemTime();
	CurrentTimeString = CurrentTime.ToString();

	TimeFromString.FromString(CurrentTimeString.c_str());

	return TimeFromString == CurrentTime;
}

bool CTimeStampTester::TestShortGMTString()
{
    for (time_t i = 0; i >= 0 && i <= 0x7fffffff; ++i)
    {
        //struct tm * OurTm = gmtime(&i);
        struct tm OurTm;
        errno_t err = gmtime_s(&OurTm, &i);
        if (err != 0)
        {
            LogEvent(LE_ERROR, "CTimeStampTester::TestShortGMTString: gmtime_s(%d) returned %d", i, err);
            return false;
            break;
        }
        int OurTime = GMTToGMTSeconds(1900+OurTm.tm_year, 1+OurTm.tm_mon, OurTm.tm_mday, OurTm.tm_hour, OurTm.tm_min, OurTm.tm_sec);
        if (OurTime != i)
        {
            LogEvent(LE_ERROR, "CTimeStampTester::TestShortGMTString: OurTime (%d) != i (%d)", OurTime, i);
            return false;
        }
    }

	CTimeStamp CurrentTime;
	CTimeStamp TimeFromString;
	std::string CurrentTimeString, TimeFromStringString;

	CurrentTime.GetSystemTime();
	CurrentTimeString = CurrentTime.ToShortGMTString();
    LogEvent(LE_INFO, "CurrentTime [%s]", CurrentTimeString.c_str());

	TimeFromString.FromShortGMTString(CurrentTimeString.c_str());
    TimeFromStringString = TimeFromString.ToShortGMTString();
    LogEvent(LE_INFO, "TimeFromString [%s]", TimeFromStringString.c_str());

	return TimeFromString == CurrentTime;
}

void TestLocalGmtTime()
{
//    time_t Current;
//    time(&Current);
//    struct tm LocalTm = *(localtime(&Current));
//    struct tm GmtTm = *(gmtime(&Current));
//
//    _timeb t;
//    _ftime(&t);
//    int diff = t.timezone;
}

#ifdef _TEST
void TestCTimeStamp()
{
//    TestLocalGmtTime();
	CTimeStampTester Tester;

	bool GeneralResult		= Tester.GeneralTest();
	bool FromStringResult	= Tester.FromStringTest();
	bool ShortGMTResult		= Tester.TestShortGMTString();

	bool Result = GeneralResult && FromStringResult && ShortGMTResult;

	if(Result)
		LogEvent(LE_INFO, "TestTimeStamp: Test passed");
	else
		LogEvent(LE_FATAL, "TestTimeStamp: Test failed");
}
#endif