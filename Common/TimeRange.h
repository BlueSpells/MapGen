#pragma once
#include "TimeStamp.h"
#include "TimePeriod.h"
#include <string>


//////////////////////////////////////////////////////////////////////////
class CTimeRange
{
public:
    CTimeRange();
    CTimeRange(const CTimeStamp& StartTime, const CTimeStamp& EndTime);
    CTimeRange(const CTimeStamp& StartTime, const CTimePeriod& Length);
    CTimeRange(const CTimeRange& Range);
    ~CTimeRange();

    CTimeStamp GetFromTime() const;
    CTimeStamp GetToTime() const;

    void SetFromTime(const CTimeStamp& FromTime);
    void SetToTime(const CTimeStamp& ToTime);

    CTimePeriod GetRange() const;

    bool operator== (const CTimeRange& Other) const;
    bool operator!= (const CTimeRange& Other) const;

    std::string ToString() const;
    std::string ToString(const char* Separator) const;
    bool FromString(const char* TimeRangeString);
    bool FromString(const char* TimeRangeString, const char* Separator);

private:
    CTimeStamp m_FromTime;
    CTimeStamp m_ToTime;
};
