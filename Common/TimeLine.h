#pragma once
#include "Common/TimeStamp.h"
#include "LogEvent.h"
#include <vector>
#include <map>

//////////////////////////////////////////////////////////////////////////

// CTimeLine is a helper class that helps the Audio database maintain 
// a list of Time periods of specific property (like kept or silence periods).  
// The CTimeLine is based on std::map and is not thread safe object.
//////////////////////////////////////////////////////////////////////////

class CTimeLine
{
public:
	struct TimeRange
	{
		CTimeStamp FromTime;
		CTimeStamp ToTime;

		std::string ToString();
		bool FromString(const char* TimeRangeString);
		bool operator== (const TimeRange& Other) const;
		bool operator!= (const TimeRange& Other) const;
	};
	typedef std::vector<TimeRange>	TimeRanges;
	typedef	TimeRanges::iterator	TimeRangesIterator;
	typedef	TimeRanges::const_iterator	TimeRangesConstIterator;

	enum RANGE_MARK { RANGE_START, RANGE_END };
	typedef std::map<CTimeStamp, RANGE_MARK>	TimeMap;
	typedef TimeMap::iterator	TimeMapIterator;
	typedef TimeMap::value_type TimeMapValue;

	void MarkTimeRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime, CTimePeriod& ActualMarked);
	void UnMarkTimeRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime, CTimePeriod& ActualUnmarked);
	void GetTimeRanges(const CTimeStamp& FromTime, const CTimeStamp& ToTime, TimeRanges& TimeRangesList) const;
    bool HasMarksInRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime) const;

	void AddTimeRanges(const TimeRanges& TimeRangesList, CTimePeriod& ActualMarked);
	void GetAllTimeRanges(TimeRanges& TimeRangesList) const;

	std::string ToString();
	bool FromString(const char* TimeLineString);
    void Log(const char* ContextStr, ELogSeverity LogLevel);

	CTimeStamp EndTime() const;
	CTimeStamp StartTime() const;
	bool IsEmpty() const;
	void Clear();

	// a method implemented specifically for storage GetTags request
	void GetRangeMarks(const CTimeStamp& FromTime, const CTimeStamp& ToTime, TimeMap& RangeMarks);

	CTimeLine GetCommonTimeLine(const CTimeLine& OtherTimeLine, bool& IsSubSet, const CTimeStamp& FromTime = CTimeStamp::ZeroTime, const CTimeStamp& ToTime = CTimeStamp::Infinity) const;
	
    // Get the total time amount marked by the time line
    CTimePeriod GetTotalTime();

	///////////////////////////////////////
    //for testing
    bool IsConsistent() const;

private:
	TimeMap	m_TimeRanges;
private:
	bool insert(const CTimeStamp& Time, RANGE_MARK Mark, TimeMapIterator& Iter);
	void AddContainedTimeRanges(const TimeRange& OtherTimeRange, bool& IsSubSet, CTimeLine& ResultTimeLine) const;
};