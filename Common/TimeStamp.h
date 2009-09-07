#pragma once

#include "TimePeriod.h"
#include "LogEvent.h"

class CTimeStamp
{
public: // Constructors
	CTimeStamp(bool SetToNow = false);
    CTimeStamp(const FILETIME& FileTime);
	//CTimeStamp(int nMilli);
//	CTimeStamp(tm& tm_struct);
	CTimeStamp(time_t Time, int Milli = 0);

    // This constructor assumes time in GMT
	CTimeStamp(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nMilli);
	~CTimeStamp();

	static CTimeStamp Now();
	static const CTimeStamp Infinity;
    static const CTimeStamp ZeroTime;

public: // Operators
	bool operator ==(const CTimeStamp& Other) const;
	bool operator < (const CTimeStamp& Other) const;
	bool operator > (const CTimeStamp& Other) const;

	bool operator !=(const CTimeStamp& Other) const;
	bool operator <=(const CTimeStamp& Other) const;
	bool operator >=(const CTimeStamp& Other) const;

	CTimeStamp& operator -=(const CTimePeriod& Other);
	CTimeStamp& operator +=(const CTimePeriod& Other);

	CTimeStamp operator -(const CTimePeriod& Other) const;
	CTimeStamp operator +(const CTimePeriod& Other) const;

	CTimePeriod operator -(const CTimeStamp& Other) const;

public:
    //Local Time format
	std::string ToString()			const;
	std::string ToShortString()		const;
	std::string ToShortGMTString()	const;
	bool		FromString(const char* TimeString);
    bool        FromShortString(const char* ShortString, bool IsGmtString = false);
	bool		FromShortGMTString(const char* ShortGMTString);

	void ToLocalTime(tm* tm_struct) const;
	void ToGMTTime	(tm* tm_struct) const;

	time_t			GetSeconds() const;
	unsigned int	GetMilli()	 const;
    void            GetDate(int& Year, int& Month, int& Day) const;
public:
	void GetSystemTime();
	void Reset();
	//void SetInfinite();

private:
	_timeb m_TimeB;
};

// Operators
inline bool CTimeStamp::operator == (const CTimeStamp& Other) const
{
	return m_TimeB.time		== Other.m_TimeB.time
		&& m_TimeB.millitm	== Other.m_TimeB.millitm;
}

inline bool CTimeStamp::operator < (const CTimeStamp& Other) const
{
	return  m_TimeB.time < Other.m_TimeB.time
		|| (m_TimeB.time == Other.m_TimeB.time && m_TimeB.millitm < Other.m_TimeB.millitm);
}

inline bool CTimeStamp::operator > (const CTimeStamp& Other) const
{
	return  m_TimeB.time > Other.m_TimeB.time
		|| (m_TimeB.time == Other.m_TimeB.time && m_TimeB.millitm > Other.m_TimeB.millitm);
}

inline bool CTimeStamp::operator !=(const CTimeStamp& Other) const
{
	return ! operator==(Other);
}

inline bool CTimeStamp::operator <=(const CTimeStamp& Other) const
{
	return operator<(Other) || operator==(Other);
}

inline bool CTimeStamp::operator >=(const CTimeStamp& Other) const
{
	return operator>(Other) || operator==(Other);
}

// Combined operators
inline CTimeStamp CTimeStamp::operator -(const CTimePeriod& Other) const
{
	CTimeStamp NewTime = *this;
	return NewTime -= Other;
}

inline CTimeStamp CTimeStamp::operator +(const CTimePeriod& Other) const
{
	CTimeStamp NewTime = *this;
	return NewTime += Other;
}

inline void CTimeStamp::GetSystemTime()
{
	Verify (_ftime_s(& m_TimeB) == 0);
}

inline time_t CTimeStamp::GetSeconds() const
{
	return m_TimeB.time;
}

inline unsigned int CTimeStamp::GetMilli() const
{
	return m_TimeB.millitm;
}



//inline void CTimeStamp::SetInfinite()
//{
//	Reset();
//	m_TimeB.time	= ~ -1;
//	m_TimeB.millitm = ~ -1;
//}

CTimePeriod CalcTimeDiff(const CTimeStamp& Left, const CTimeStamp& Right);