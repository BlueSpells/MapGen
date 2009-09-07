#include "StdAfx.h"
#include "TimeRange.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
CTimeRange::CTimeRange() :
  m_FromTime(CTimeStamp::Infinity)
, m_ToTime(CTimeStamp::Infinity)
{
}


//////////////////////////////////////////////////////////////////////////
CTimeRange::CTimeRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime) :
  m_FromTime(FromTime)
, m_ToTime(ToTime)
{
}


//////////////////////////////////////////////////////////////////////////
CTimeRange::CTimeRange(const CTimeStamp& StartTime, const CTimePeriod& Length) :
  m_FromTime(StartTime)
, m_ToTime(StartTime + Length)
{
}


//////////////////////////////////////////////////////////////////////////
CTimeRange::CTimeRange(const CTimeRange& Range) :
  m_FromTime(Range.m_FromTime)
, m_ToTime(Range.m_ToTime)
{
}

//////////////////////////////////////////////////////////////////////////
CTimeRange::~CTimeRange()
{
}


//////////////////////////////////////////////////////////////////////////
CTimeStamp CTimeRange::GetFromTime() const
{
    return m_FromTime;
}


//////////////////////////////////////////////////////////////////////////
CTimeStamp CTimeRange::GetToTime() const
{
    return m_ToTime;
}


//////////////////////////////////////////////////////////////////////////
void CTimeRange::SetFromTime(const CTimeStamp& FromTime)
{
    m_FromTime = FromTime;
    return;
}


//////////////////////////////////////////////////////////////////////////
void CTimeRange::SetToTime(const CTimeStamp& ToTime)
{
    m_ToTime = ToTime;
    return;
}


//////////////////////////////////////////////////////////////////////////
CTimePeriod CTimeRange::GetRange() const
{
    return m_ToTime - m_FromTime;
}


//////////////////////////////////////////////////////////////////////////
bool CTimeRange::operator== (const CTimeRange& Other) const
{
    bool Result = false;
    if(this->m_FromTime == Other.m_FromTime && this->m_ToTime == Other.m_ToTime)
        Result = true;
    return Result;
}


//////////////////////////////////////////////////////////////////////////
bool CTimeRange::operator!= (const CTimeRange& Other) const
{
    return !(*this == Other);
}


//////////////////////////////////////////////////////////////////////////
// ToString() functions
static const char* TimeRangeSeperator = "\t";

//////////////////////////////////////////////////////////////////////////
std::string CTimeRange::ToString() const
{
    return ToString(TimeRangeSeperator);
}


//////////////////////////////////////////////////////////////////////////
bool CTimeRange::FromString(const char* TimeRangeString)
{
    return FromString(TimeRangeString, TimeRangeSeperator);
}


//////////////////////////////////////////////////////////////////////////
std::string CTimeRange::ToString(const char* Separator) const
{
    std::string TimeRangeString;
    TimeRangeString.append(m_FromTime.ToShortString());
    TimeRangeString.append(Separator);
    TimeRangeString.append(m_ToTime.ToShortString());

    return TimeRangeString;
}


//////////////////////////////////////////////////////////////////////////
bool CTimeRange::FromString(const char* TimeRangeString, const char* Separator)
{
    const char* FromTimeString	= TimeRangeString;
    const char* SeperatorString	= strstr(TimeRangeString, Separator);
    const char* ToTimeString	= SeperatorString + strlen(Separator);

    bool Success;
    Success = m_FromTime.FromShortString(FromTimeString);
    Success = ( Success && m_ToTime.FromShortString(ToTimeString) );

    return Success;
}


//////////////////////////////////////////////////////////////////////////
// _TEST
#ifdef _TEST

void TestTimeRange()
{
    LogEvent(LE_INFOHIGH, "TestTimeRange() -Start");

    CTimePeriod Period(1, 0);
    CTimeStamp FromTime = CTimeStamp::Now();
    CTimeStamp ToTime = FromTime + Period;

    CTimeRange Range(FromTime, ToTime);
    Assert(Range.GetRange() == Period);

    CTimeRange Range2(Range);
    Assert(Range == Range2);
    Assert(! (Range != Range2));

    CTimeRange RangeFromStr;
    std::string Str = Range.ToString();
    RangeFromStr.FromString(Str.c_str());
    Assert(Range == RangeFromStr);

    LogEvent(LE_INFOHIGH, "TestTimeRange() - Passed successfully");
    return;
}

#endif
