#include "StdAfx.h"
#include "TimeLine.h"
#include "Common/LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//				CLASS		TimeLine
// Class TimeLine keeps track over interesting time ranges within
// a time line.
// The interest in a time range is expressed by MARKing it.
// Each time range is marked using the RANGE_START and RANGE_END marks.
// The time line can be infinite and its beginning and end are
// determined by the the first and last range. 
// Marking a new range merges all the RANGE_START and RANGE_END
// marks contained in the new
// range into a single pair of RANGE_START - RANGE_END while maintaining
// the consistency of the preceding and succeeding time ranges.
// UnMarking a time range deletes all the RANGE_START and RANGE_END
// marks in the given
// FromTime - ToTime range while maintaining the consistency of
// the preceding and succeeding time ranges.

bool CTimeLine::TimeRange::operator== (const TimeRange& Other) const
{
	bool Result = false;
	if(this->FromTime == Other.FromTime && this->ToTime == Other.ToTime)
		Result = true;
	return Result;
}
bool CTimeLine::TimeRange::operator!= (const TimeRange& Other) const
{
	return !(*this == Other);
}
void CTimeLine::MarkTimeRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime,
                              CTimePeriod& ActualMarked)
{
	Assert( FromTime < ToTime );
    ActualMarked = ToTime - FromTime;
	CTimeStamp ExistingStartTime = CTimeStamp::Infinity;

	TimeMapIterator iter;

	iter = m_TimeRanges.lower_bound(FromTime);
	if(iter == m_TimeRanges.end()) // No ranges to merge
	{
		insert(FromTime, RANGE_START, iter);
		insert(ToTime, RANGE_END, iter);
        return;
	}

    //need to merge marked ranges
	RANGE_MARK CurrentMark = iter->second;
    CTimeStamp CurrentMarkTime = iter->first;
	if(CurrentMark == RANGE_START) // FromTime is placed before a start mark or on it --> Fix current range
	{
		ExistingStartTime = CurrentMarkTime;
		if(FromTime < CurrentMarkTime)
        {
            // Before a start mark --> Extend backwards the current time range.
            insert(FromTime, RANGE_START, iter);
        }
		// Else: FromTime == CurrentMarkTime --> Keep current mark
		++iter; // Move to the next time range
	}
    else
    {
        // Else: FromTime is placed before or on an end mark 
        // The period of FromTime - CurrentMarkTime is already marked. 
        ActualMarked -= (CurrentMarkTime - FromTime);

        //the CurrentMarkTime will be deleted by the Next iteration
    }


	// erase intermediate marks
	while(iter != m_TimeRanges.end())
	{
		CurrentMarkTime = iter->first;
        CurrentMark = iter->second;

        //remove existing periods from the ActualMarked
        if(CurrentMark == RANGE_START)
            ExistingStartTime = CurrentMarkTime;
        else
        {
            if(ExistingStartTime != CTimeStamp::Infinity)
            {
                ActualMarked -= (CurrentMarkTime - ExistingStartTime);
                ExistingStartTime = CTimeStamp::Infinity;
            }
        }
        
		if(CurrentMarkTime >= ToTime)
        {
            if(CurrentMark == RANGE_END)
                //in this case too much time was removed from the ActualMarked period
                //in the above code
                ActualMarked += (CurrentMarkTime - ToTime);
            break;
        }
		iter = m_TimeRanges.erase(iter);
	}

	if(iter != m_TimeRanges.end())
	{
		CurrentMark = iter->second;
		if(CurrentMark == RANGE_START) // ToTime is placed before or on a start mark --> Fix current range
		{
			CurrentMarkTime = iter->first;
			if(CurrentMarkTime == ToTime)
				iter = m_TimeRanges.erase(iter);
			else // ToTime < CurrentMarkTime 
				insert(ToTime, RANGE_END, iter);
		}
		// Else: ToTime is placed before or on an end mark --> Keep that mark
	}
	else
		// We've deleted all marks up to the end of the time line. The new range closes the time line
		insert(ToTime, RANGE_END, iter);

    Assert(ActualMarked >= CTimePeriod::ZeroLength);
}

void CTimeLine::UnMarkTimeRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime,
                                CTimePeriod& ActualUnmarked)
{
	Assert( FromTime < ToTime );
    ActualUnmarked = CTimePeriod::ZeroLength;
	TimeMapIterator iter;
	RANGE_MARK CurrentMark;

	iter = m_TimeRanges.lower_bound(FromTime);
	if(iter == m_TimeRanges.end())
    {
        // Nothing to UnMark
		LogEvent(LE_INFOLOW, "TimeLine::UnMarkTimeRange : Attempt to UnMark non existing time range. Start time %s. End time %s.",
		    FromTime.ToString().c_str(), ToTime.ToString().c_str());
        return;
    }

	CurrentMark = iter->second;
	if(CurrentMark == RANGE_END) // FromTime is placed before an end mark or on it --> Fix current range
	{
		const CTimeStamp& CurrentMarkTime = iter->first;
		if(FromTime < CurrentMarkTime)
        {
            // Before an end mark --> Close the previous time range.
            insert(FromTime, RANGE_END, iter);
            ActualUnmarked += (CurrentMarkTime - FromTime);
        }
		// Else: FromTime == CurrentMarkTime --> Keep current mark
		++iter; // Move to the next time range
	}
	// Else: FromTime is placed before or on a start mark --> Delete that mark

	// erase intermediate marks
    CTimeStamp ExistingStartTime = CTimeStamp::Infinity;
	while(iter != m_TimeRanges.end())
	{	
		const CTimeStamp& CurrentMarkTime = iter->first;
        CurrentMark = iter->second;

        //count existing periods the being UnMarked
        if(CurrentMark == RANGE_START)
            ExistingStartTime = CurrentMarkTime;
        else
        {
            if(ExistingStartTime != CTimeStamp::Infinity)
            {
                ActualUnmarked += (CurrentMarkTime - ExistingStartTime);
                ExistingStartTime = CTimeStamp::Infinity;
            }
        }

		if(CurrentMarkTime >= ToTime)
        {
            if(CurrentMark == RANGE_END)
                //in this case too much time was added to the ActualUnMarked period
                //in the above code
                ActualUnmarked -= (CurrentMarkTime - ToTime);
            break;

        }
		iter = m_TimeRanges.erase(iter);
	}

	if(iter != m_TimeRanges.end())
	{
		CurrentMark = iter->second;
		if(CurrentMark == RANGE_END) // ToTime is placed before or on an end mark --> Fix current range
		{
			const CTimeStamp& CurrentMarkTime = iter->first;
			if(CurrentMarkTime == ToTime)
				iter = m_TimeRanges.erase(iter);
			else // CurrentMarkTime > ToTime
				insert(ToTime, RANGE_START, iter);
		}
		// Else: ToTime is placed before or on a start mark --> Keep that mark
	}
    Assert(ActualUnmarked >= CTimePeriod::ZeroLength);
	// Else: Nothing to do. We've deleted all marks up to the end of the time line
}

void CTimeLine::AddTimeRanges(const TimeRanges& TimeRangesList, CTimePeriod& ActualMarked)
{
    ActualMarked = CTimePeriod::ZeroLength;
	TimeRangesConstIterator iter = TimeRangesList.begin();
	while(iter != TimeRangesList.end())
	{
        CTimePeriod RangeActualMarked = CTimePeriod::ZeroLength;
		MarkTimeRange(iter->FromTime, iter->ToTime, RangeActualMarked);
        ActualMarked += RangeActualMarked;
		++iter;
	}
}

void CTimeLine::GetAllTimeRanges(TimeRanges& TimeRangesList) const 
{
	CTimeStamp StartTime;
	if(!m_TimeRanges.empty())
	{
		StartTime = m_TimeRanges.begin()->first;
		GetTimeRanges(StartTime, CTimeStamp::Infinity, TimeRangesList);
	}
}

// Retrieves the TimeRanges in the interval [FromTime, ToTime]
// Collects a fictive start mark or end mark on the interval boundaries
// if the interval contains only part of these marks.
void CTimeLine::GetTimeRanges(const CTimeStamp& FromTime,
							  const CTimeStamp& ToTime,
							  TimeRanges& TimeRangesList) const
{
	Assert( FromTime < ToTime );

	
	RANGE_MARK CurrentMark;
	TimeRange Range;

    TimeMap::const_iterator Iter = m_TimeRanges.lower_bound(FromTime);
	if(Iter == m_TimeRanges.end()) // No ranges to retrieve
		return;

	CurrentMark = Iter->second;
	// FromTime is placed before an end mark or on it --> Create sub range
	if(CurrentMark == RANGE_END)
	{
		const CTimeStamp& CurrentMarkTime = Iter->first;
		if(FromTime < CurrentMarkTime)
		{
			// Before an end mark --> Create a sub range from FromTime to the end of the current range
			Range.FromTime	= FromTime;
			if(ToTime < CurrentMarkTime)
				Range.ToTime = ToTime;
			else
				Range.ToTime = CurrentMarkTime;
			TimeRangesList.push_back(Range);
		}
		// Else: FromTime == CurrentMarkTime --> Skip current range
		++Iter; // Move to the next time range
	}
	// Else: FromTime is placed before or on a start mark --> Deal with in the while loop

	// collect time ranges. Assume you are in front of a RANGE_START mark.
	while(Iter != m_TimeRanges.end())
	{	
		{
			const CTimeStamp& CurrentMarkTime = Iter->first;
			if(CurrentMarkTime >= ToTime)
				break;
			// Assemble RANGE_START and RANGE_END
			Range.FromTime = CurrentMarkTime; // Collect RANGE_START
			++Iter; // Look for RANGE_END
		}
		{
			const CTimeStamp& CurrentMarkTime = Iter->first;
			if(CurrentMarkTime >= ToTime) // Required data ends before the end if an actual range.
				Range.ToTime = ToTime;
			else
				Range.ToTime = CurrentMarkTime;
			TimeRangesList.push_back(Range);
		}

		++Iter;
	}
}

// Return range marks in the open section [FromTime, ToTime)
void CTimeLine::GetRangeMarks(const CTimeStamp& FromTime,
							  const CTimeStamp& ToTime,
							  TimeMap& RangeMarks)
{
	Assert(FromTime <= ToTime);
	TimeMapIterator Start = m_TimeRanges.lower_bound(FromTime);
	TimeMapIterator End = m_TimeRanges.lower_bound(ToTime);
	RangeMarks.insert<TimeMapIterator>(Start, End);
}

bool CTimeLine::HasMarksInRange(const CTimeStamp& FromTime, const CTimeStamp& ToTime) const
{
    Assert( FromTime < ToTime );
    TimeMap::const_iterator Iter = m_TimeRanges.upper_bound(FromTime);
    if(Iter == m_TimeRanges.end()) // No ranges to retrieve
        return false;

    const CTimeStamp& CurrentMarkTime = Iter->first;
    if (CurrentMarkTime > FromTime && CurrentMarkTime < ToTime)
        return true;

    RANGE_MARK CurrentMark = Iter->second;
    // FromTime is placed before an end mark and after start mark
    if(CurrentMark == RANGE_END)
        return true;

    return false;
}

bool CTimeLine::IsConsistent() const
{
    //size should be even
    if(m_TimeRanges.size() % 2 != 0)
    {
        Assert(false);
        return false;
    }

    //it should contain pairs of Start and End marks
    TimeMap::const_iterator Iter = m_TimeRanges.begin();
    TimeMap::const_iterator End = m_TimeRanges.end();

    bool ExpectingStartMark = true;
    for (; Iter != End; ++Iter)
    {
        RANGE_MARK MarkType = Iter->second;

        if( ((MarkType == RANGE_START) && !ExpectingStartMark) ||
            ((MarkType == RANGE_END) && ExpectingStartMark))
        {
            Assert(false);
            return false;
        }

        ExpectingStartMark = !ExpectingStartMark;
    }
    return true;
}


// Make sure TimePairSeperator != RangeSeperator
static const char* RangeSeperator = "\n";

std::string CTimeLine::ToString()
{
	std::string Out;
	TimeRange Range;

	TimeMapIterator iter = m_TimeRanges.begin();
	while(iter != m_TimeRanges.end())
	{
		const CTimeStamp& CurrentMarkTime = iter->first;
		RANGE_MARK CurrentMark		 = iter->second;
		if(CurrentMark == RANGE_START)
		{
			Range.FromTime = CurrentMarkTime;
		}
		else
		{
			Range.ToTime = CurrentMarkTime;
			Out.append(Range.ToString());
			Out.append(RangeSeperator);
		}

		++iter;
	}

	return Out;
}

void CTimeLine::Log(const char* ContextStr, ELogSeverity LogLevel)
{
    TimeRange Range;

    LogEvent(LogLevel, "%s %d Time Ranges: ", ContextStr, m_TimeRanges.size());
    TimeMapIterator Iter = m_TimeRanges.begin();
    while(Iter != m_TimeRanges.end())
    {
        const CTimeStamp& CurrentMarkTime = Iter->first;
        RANGE_MARK CurrentMark		 = Iter->second;
        if(CurrentMark == RANGE_START)
        {
            Range.FromTime = CurrentMarkTime;
        }
        else
        {
            Range.ToTime = CurrentMarkTime;
            LogEvent(LogLevel, "   %s", Range.ToString().c_str());
        }

        ++Iter;
    }
}

bool CTimeLine::FromString(const char* TimeLineString)
{
	Assert(NULL != TimeLineString);
	if(0 == strlen(TimeLineString))
		return true;

	bool Success = false;
	TimeRange Range;

	std::string SearchString(TimeLineString);
	int RangeStart	= 0;
	int RangeEnd	= 0;

	while(true)
	{
		RangeEnd = SearchString.find(RangeSeperator, RangeStart);

		if(std::string::npos == (unsigned int)RangeEnd)
			break;
		Success = Range.FromString(SearchString.substr(RangeStart, RangeEnd - RangeStart).c_str());
		if(!Success)
			break;
		m_TimeRanges.insert(TimeMapValue(Range.FromTime, RANGE_START));
		m_TimeRanges.insert(TimeMapValue(Range.ToTime, RANGE_END));

		RangeStart = RangeEnd + strlen(RangeSeperator);
	}

	return Success;
}

CTimeStamp CTimeLine::StartTime() const
{
	if(m_TimeRanges.empty())
		return CTimeStamp::Infinity;

	TimeMap::const_iterator iter = m_TimeRanges.begin();

	return iter->first;
}

CTimeStamp CTimeLine::EndTime() const
{
	if(m_TimeRanges.empty())
		return CTimeStamp();

	TimeMap::const_iterator iter = m_TimeRanges.end();
	--iter;

	return iter->first;
}

bool CTimeLine::IsEmpty() const
{
	return m_TimeRanges.empty();
}

void CTimeLine::Clear()
{
	m_TimeRanges.clear();
}

bool CTimeLine::insert(const CTimeStamp& Time, RANGE_MARK Mark, TimeMapIterator& iter)
{
	std::pair<TimeMapIterator, bool> Result = m_TimeRanges.insert(TimeMapValue(Time, Mark));
	if(!Result.second)
		LogEvent(LE_ERROR, "CTimeLine : Invalid iterator inserting time %s", Time.ToString().c_str());
	iter = Result.first;
	return Result.second;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Adds to ResultTimeLine contained Time Ranges within OtherTimeRange and check if OtherTimeRange is contained inside this TimeLine
void CTimeLine::AddContainedTimeRanges(const TimeRange& OtherTimeRange, bool& IsSubSet, CTimeLine& ResultTimeLine) const
{
	TimeRanges ResultTimeRanges;
	IsSubSet = true;
	GetTimeRanges(OtherTimeRange.FromTime, OtherTimeRange.ToTime, ResultTimeRanges);
	if( ResultTimeRanges.size() != 1 || 
		ResultTimeRanges[0] != OtherTimeRange)
	{
        IsSubSet = false;
	}
	CTimePeriod MarkedTime;
	ResultTimeLine.AddTimeRanges(ResultTimeRanges, MarkedTime);
	ResultTimeLine.IsConsistent();
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//Builds Common Time Line from ContainedTimeLine and checks containment
CTimeLine CTimeLine::GetCommonTimeLine(const CTimeLine& ContainedTimeLine, bool& IsSubSet,
					const CTimeStamp& FromTime /*= CTimeStamp.ZeroTime*/, const CTimeStamp& ToTime /*= CTimeStamp.Infinity*/) const
{
	CTimeLine ResultTimeLine;
	TimeRanges ContainedTimeRanges;
	ContainedTimeLine.GetTimeRanges(FromTime, ToTime, ContainedTimeRanges);
	IsSubSet = true;
	bool CurrentRangeIsSubSet = true;
	for (unsigned int i = 0; i < ContainedTimeRanges.size() ; i++)
	{
		AddContainedTimeRanges(ContainedTimeRanges[i], CurrentRangeIsSubSet, ResultTimeLine);
		ResultTimeLine.IsConsistent();
		if(!CurrentRangeIsSubSet)
		{
            IsSubSet = false;
		}
	}
	return ResultTimeLine;
}

//////////////////////////////////////////////////////////////////////////
CTimePeriod CTimeLine::GetTotalTime()
{
    CTimePeriod TotalTime;
    CTimeStamp TempStartTime;

    TimeMap::const_iterator Iter = m_TimeRanges.begin();
    TimeMap::const_iterator End = m_TimeRanges.end();
    bool StartMarkWasMet = false;
    for (; Iter != End; ++Iter)
    {
        CTimeStamp Time = Iter->first;
        RANGE_MARK MarkType = Iter->second;

        if(StartMarkWasMet)
        {
            Assert(MarkType == RANGE_END);
            CTimePeriod TimeToAdd = Time - TempStartTime;
            TotalTime += TimeToAdd;
            StartMarkWasMet = false;
        }
        else
        {
            Assert(MarkType == RANGE_START);
            TempStartTime = Time; //Storing it for the end tag
            StartMarkWasMet = false;
        }
    }
    return TotalTime;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// CTimeLine::TimeRange

// Make sure TimePairSeperator != RangeSeperator
static const char* TimePairSeperator = "\t";

std::string CTimeLine::TimeRange::ToString()
{
	std::string TimeRangeString;
	TimeRangeString.append(FromTime.ToShortGMTString());
	TimeRangeString.append(TimePairSeperator);
	TimeRangeString.append(ToTime.ToShortGMTString());

	return TimeRangeString;
}

bool CTimeLine::TimeRange::FromString(const char* TimeRangeString)
{
	const char* FromTimeString	= TimeRangeString;
	const char* SeperatorString	= strstr(TimeRangeString, TimePairSeperator);
	const char* ToTimeString	= SeperatorString + strlen(TimePairSeperator);

	bool Success;
	Success = FromTime.FromShortGMTString(FromTimeString);
	Success = ( Success && ToTime.FromShortGMTString(ToTimeString) );

	return Success;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _TEST

std::string TimeRangesToString(CTimeLine::TimeRanges& Ranges);
void PrintRangeMarks(const CTimeLine::TimeMap& RangeMarks);

class CTimeLineTester
{
public:
	bool FromStringTest();
	void GeneralTest();
	void TestGetRangeMarks();
    void AutomaticTest();
	void ContainedTimeLineTest();
    void TestHasMarksInRange();
};

void CTimeLineTester::ContainedTimeLineTest()
{
	CTimePeriod Actual;
	CTimeLine Line;
	CTimeLine ContainedLine;
	CTimeLine ResultedLine;
	CTimeStamp Now;
	bool SubSet = true;
	Now.GetSystemTime();
	// Case 1
	//
	// ContainedTimeLine:	|  /------------/
	// TimeLine:			|   /---/ /--/    
	Line.MarkTimeRange(Now + CTimePeriod(2), Now + CTimePeriod(5), Actual);
	Line.MarkTimeRange(Now + CTimePeriod(6), Now + CTimePeriod(8), Actual);
	ContainedLine.MarkTimeRange(Now + CTimePeriod(1), Now + CTimePeriod(9), Actual);
	ResultedLine = Line.GetCommonTimeLine(ContainedLine, SubSet, Now + CTimePeriod(1), Now + CTimePeriod(10));

	std::string TimeLineStr;
	TimeLineStr = Line.ToString();
    LogEvent(LE_INFO, "ContainedTimeLineTest  Line: %s", TimeLineStr.c_str());
	TimeLineStr = ContainedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ContainedLine: %s", TimeLineStr.c_str());
	TimeLineStr = ResultedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ResultedLine: %s", TimeLineStr.c_str());

	ContainedLine.Clear();
	ResultedLine.Clear();
	// Case 2
	// ContainedTimeLine:	|        /-/
	// TimeLine:			|   /---/ /--/ 
	ContainedLine.MarkTimeRange(Now + CTimePeriod(5), Now + CTimePeriod(6), Actual);
	ResultedLine = Line.GetCommonTimeLine(ContainedLine, SubSet, Now + CTimePeriod(1), Now + CTimePeriod(10));
	TimeLineStr = ContainedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ContainedLine: %s", TimeLineStr.c_str());
	TimeLineStr = ResultedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ResultedLine: %s", TimeLineStr.c_str());

	ContainedLine.Clear();
	ResultedLine.Clear();
	// Case 3
	// ContainedTimeLine:	|      /----/
	// TimeLine:			|   /---/ /--/ 
	ContainedLine.MarkTimeRange(Now + CTimePeriod(3), Now + CTimePeriod(7), Actual);
	ResultedLine = Line.GetCommonTimeLine(ContainedLine, SubSet);
	TimeLineStr = ContainedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ContainedLine: %s", TimeLineStr.c_str());
	TimeLineStr = ResultedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ResultedLine: %s", TimeLineStr.c_str());

	Line.Clear();
	ContainedLine.Clear();
	ResultedLine.Clear();
	// Case 4
	// ContainedTimeLine:	|      /----/ /---/
	// TimeLine:			|   /---/ /---/ 
	Line.MarkTimeRange(Now + CTimePeriod(2), Now + CTimePeriod(5), Actual);
	Line.MarkTimeRange(Now + CTimePeriod(6), Now + CTimePeriod(10), Actual);
	ContainedLine.MarkTimeRange(Now + CTimePeriod(3), Now + CTimePeriod(7), Actual);
	ContainedLine.MarkTimeRange(Now + CTimePeriod(8), Now + CTimePeriod(11), Actual);
	ResultedLine = Line.GetCommonTimeLine(ContainedLine, SubSet);
	TimeLineStr = Line.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  Line: %s", TimeLineStr.c_str());
	TimeLineStr = ContainedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ContainedLine: %s", TimeLineStr.c_str());
	TimeLineStr = ResultedLine.ToString();
	LogEvent(LE_INFO, "ContainedTimeLineTest  ResultedLine: %s", TimeLineStr.c_str());


 }

bool CTimeLineTester::FromStringTest()
{
    CTimePeriod Actual;
	CTimeLine Line;
	CTimeStamp Now;
	Now.GetSystemTime();

	Line.MarkTimeRange(Now + CTimePeriod(1), Now + CTimePeriod(5), Actual);
	Line.MarkTimeRange(Now + CTimePeriod(6), Now + CTimePeriod(8), Actual);
	Line.MarkTimeRange(Now + CTimePeriod(10), Now + CTimePeriod(14), Actual);

	std::string TimeLineString = Line.ToString();
	CTimeLine LineFromString;
	LineFromString.FromString(TimeLineString.c_str());

	return TimeLineString == LineFromString.ToString();
}

void CTimeLineTester::GeneralTest()
{
	CTimePeriod Actual;
	CTimeLine Line;
	CTimeLine::TimeRanges Ranges;
	CTimeStamp Now;
	Now.GetSystemTime();

	Line.MarkTimeRange(Now + CTimePeriod(1), Now + CTimePeriod(15), Actual);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", Line.ToString().c_str());
	Assert(Actual == CTimePeriod(14));

	Line.MarkTimeRange(Now + CTimePeriod(1), Now + CTimePeriod(15), Actual);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", Line.ToString().c_str());
	Assert(Actual == CTimePeriod(0));

	Line.MarkTimeRange(Now + CTimePeriod(12), Now + CTimePeriod(18), Actual);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", Line.ToString().c_str());
	Assert(Actual == CTimePeriod(3));

	Line.UnMarkTimeRange(Now + CTimePeriod(3), Now + CTimePeriod(7), Actual);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", Line.ToString().c_str());
	Assert(Actual == CTimePeriod(4));

	Line.UnMarkTimeRange(Now + CTimePeriod(3), Now + CTimePeriod(7), Actual);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", Line.ToString().c_str());
	Assert(Actual == CTimePeriod(0));

	Ranges.clear();
	Line.GetTimeRanges(Now + CTimePeriod(0), Now + CTimePeriod(20), Ranges);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", TimeRangesToString(Ranges).c_str());

	Ranges.clear();
	Line.GetTimeRanges(Now + CTimePeriod(2), Now + CTimePeriod(16), Ranges);
	LogEvent(LE_DEBUG, "TestCTimeLine : %s", TimeRangesToString(Ranges).c_str());
}

void CTimeLineTester::TestGetRangeMarks()
{
    CTimePeriod Actual;
	CTimeLine Line;
	CTimeStamp SomeTime(2000, 1, 1, 0, 0, 0, 0);

	Line.MarkTimeRange(SomeTime + CTimePeriod(1), SomeTime + CTimePeriod(7), Actual);
	Line.MarkTimeRange(SomeTime + CTimePeriod(8), SomeTime + CTimePeriod(16), Actual);
	Line.MarkTimeRange(SomeTime + CTimePeriod(17), SomeTime + CTimePeriod(20), Actual);

	CTimeLine::TimeMap RangeMarks;

	Line.GetRangeMarks(SomeTime, SomeTime + CTimePeriod(21), RangeMarks);
	LogEvent(LE_ERROR, "All Range Marks");
	PrintRangeMarks(RangeMarks);
	RangeMarks.clear();

	Line.GetRangeMarks(SomeTime, SomeTime + CTimePeriod(7), RangeMarks);
	LogEvent(LE_ERROR, "Only first mark");
	PrintRangeMarks(RangeMarks);
	RangeMarks.clear();

	Line.GetRangeMarks(SomeTime + CTimePeriod(7), SomeTime + CTimePeriod(16), RangeMarks);
	LogEvent(LE_ERROR, "Only two middle marks");
	PrintRangeMarks(RangeMarks);
	RangeMarks.clear();

	Line.GetRangeMarks(SomeTime + CTimePeriod(16), SomeTime + CTimePeriod(21), RangeMarks);
	LogEvent(LE_ERROR, "last three marks");
	PrintRangeMarks(RangeMarks);
	RangeMarks.clear();
}

void CTimeLineTester::TestHasMarksInRange()
{
    LogEvent(LE_INFOHIGH, "Start TimeLine TestHasMarksInRange test");

    CTimeLine Line;
    CTimeStamp Now = CTimeStamp::Now();
    CTimePeriod ActualMarked(0);

    CTimeStamp StartTime = Now + CTimePeriod(2);
    CTimeStamp EndTime = Now + CTimePeriod(10);
    Line.MarkTimeRange(StartTime, EndTime, ActualMarked);
    Assert(ActualMarked == EndTime - StartTime);

    Assert(Line.HasMarksInRange(StartTime, EndTime) == true);
    Assert(Line.HasMarksInRange(StartTime + CTimePeriod(1), EndTime) == true);
    Assert(Line.HasMarksInRange(StartTime - CTimePeriod(1), EndTime) == true);
    Assert(Line.HasMarksInRange(StartTime, EndTime - CTimePeriod(1)) == true);
    Assert(Line.HasMarksInRange(StartTime, EndTime + CTimePeriod(1)) == true);
    Assert(Line.HasMarksInRange(StartTime + CTimePeriod(1), EndTime - CTimePeriod(1)) == true);
    Assert(Line.HasMarksInRange(StartTime - CTimePeriod(1), EndTime + CTimePeriod(1)) == true);
    Assert(Line.HasMarksInRange(StartTime - CTimePeriod(10), StartTime - CTimePeriod(1)) == false);
    Assert(Line.HasMarksInRange(StartTime - CTimePeriod(1), StartTime) == false);
    Assert(Line.HasMarksInRange(EndTime, EndTime + CTimePeriod(1)) == false);

    LogEvent(LE_INFOHIGH, "End TimeLine TestHasMarksInRange test");
    return;
}

#include "Common/Utils.h"
#include "Common/RandomUtils.h"

static CTimePeriod GetTotalMarkedPeriod(const CTimeLine& TimeLine)
{
    CTimePeriod TotalMarked;
    CTimeLine::TimeRanges TimeRangesList;
    TimeLine.GetAllTimeRanges(TimeRangesList);
    for (unsigned int i = 0; i < TimeRangesList.size(); ++i)
        TotalMarked += (TimeRangesList[i].ToTime - TimeRangesList[i].FromTime);
    return TotalMarked;
}

//This test randomly Marks and UnMarks time ranges in the 
void CTimeLineTester::AutomaticTest()
{
    CTimeLine TimeLine;
    CTimePeriod AccumulatedMarkedPeriod = CTimePeriod::ZeroLength;

    LogEvent(LE_INFOHIGH, "Start TimeLine Automatic test");
    WaitAndPumpMessages(100);


    CTimePeriod TestTimeBoundry(100000, 0);
    CTimeStamp TestStartTimeBoundry = CTimeStamp::Now()  - TestTimeBoundry;
    CTimeStamp TestEndTime = CTimeStamp::Now();

    for(int i = 0; i < 10000; ++i)
    {
        //get random times
        CTimeStamp RandomFromTime = GetRandomTimeInRange(TestStartTimeBoundry, TestEndTime);
        CTimeStamp RandomToTime = GetRandomTimeInRange(TestStartTimeBoundry, TestEndTime);
        if(RandomToTime < RandomFromTime)
        {
            //switch in order to make sure 
            CTimeStamp Temp = RandomFromTime;
            RandomFromTime = RandomToTime;
            RandomToTime = Temp;
        }

        //perform mark or unmark operations
        int RandomOperation  = rand() % 2;
        if(RandomOperation == 0)
        {
            //Mark time range
            CTimePeriod ActualMarked;
            TimeLine.MarkTimeRange(RandomFromTime, RandomToTime, ActualMarked);
            AccumulatedMarkedPeriod += ActualMarked;
        }
        else 
        {
            //Mark time range
            CTimePeriod ActualUnMarked;
            TimeLine.UnMarkTimeRange(RandomFromTime, RandomToTime, ActualUnMarked);
            AccumulatedMarkedPeriod -= ActualUnMarked;
        }

        //test time line consistency and Actual time retrieved.
        Assert(TimeLine.IsConsistent());
        Assert(AccumulatedMarkedPeriod == GetTotalMarkedPeriod(TimeLine));
        WaitAndPumpMessages(10);
    }

    LogEvent(LE_INFOHIGH, "End TimeLine Automatic test");
}

void TestTimeLine()
{
	CTimeLineTester Tester;

	Tester.GeneralTest();
	Assert(Tester.FromStringTest());
	Tester.TestGetRangeMarks();
	Tester.ContainedTimeLineTest();
    Tester.TestHasMarksInRange();
    Tester.AutomaticTest();
}

std::string TimeRangesToString(CTimeLine::TimeRanges& Ranges)
{
	std::string Out;
	CTimeLine::TimeRanges::iterator iter = Ranges.begin();

	while(iter != Ranges.end())
	{
		Out.append( "S ");
		Out.append(iter->ToString());
		//		Out.append((*iter).FromTime.ToString());
		//		Out.append("  ");
		//		Out.append((*iter).ToTime.ToString());
		Out.append(" E  ");
		iter++;
	}

	return Out;
}

void PrintRangeMarks(const CTimeLine::TimeMap& RangeMarks)
{
	CTimeLine::TimeMap::const_iterator iter = RangeMarks.begin();
	for( ; iter != RangeMarks.end() ; ++iter)
	{
		const CTimeStamp& Time = iter->first;
		CTimeLine::RANGE_MARK Mark = iter->second;
		std::string MarkString = ((Mark == CTimeLine::RANGE_START) ? "RANGE_START" : "RAGNE_END");
		LogEvent(LE_INFOHIGH, "Mark %s, Time %s.", MarkString.c_str(), Time.ToString().c_str());
	}
}

#endif // _TEST