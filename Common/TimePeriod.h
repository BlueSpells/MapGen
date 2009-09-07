#pragma once

#define _USE_32BIT_TIME_T
#include <sys/timeb.h>
#include <string>

class CTimePeriod
{
public: // Constructors
	CTimePeriod();
	CTimePeriod(int nMilli);
	CTimePeriod(time_t nSeconds, int nMilli);

	enum 
	{
		SECOND_IN_MILLISECONDS = 1000,
		MINUTE_IN_SECONDS = 60,
		HOUR_IN_SECONDS = 60 * 60,
		DAY_IN_SECONDS = 60 * 60 * 24,
	};

public: // Operators
	bool operator ==(const CTimePeriod& Other) const;
	bool operator < (const CTimePeriod& Other) const;
	bool operator > (const CTimePeriod& Other) const;

	bool operator !=(const CTimePeriod& Other) const;
	bool operator <=(const CTimePeriod& Other) const;
	bool operator >=(const CTimePeriod& Other) const;

	CTimePeriod& operator -=(const CTimePeriod& Other);
	CTimePeriod& operator +=(const CTimePeriod& Other);

	CTimePeriod operator -(const CTimePeriod& Other) const;
	CTimePeriod operator +(const CTimePeriod& Other) const;

	// The int version is required to avoid automatic casting
	double		operator /(const CTimePeriod& Other) const;
	CTimePeriod operator /(int	  Divisor) const;
	CTimePeriod operator /(double Divisor) const;
    CTimePeriod operator* (double Multiply) const;
	//CTimePeriod& operator =(int NumMilli);

public:
	int GetSeconds() const;
	int GetMilli()	 const;
	//operator int() const;
	int		Milli()	const;

    std::string ToString() const;

public:
	static const CTimePeriod ZeroLength;
	static const CTimePeriod InfiniteLength;

private:
	int	m_Seconds;
	int	m_Milli;
};

// Operators
inline bool CTimePeriod::operator == (const CTimePeriod& Other) const
{
	return m_Seconds == Other.m_Seconds
		&& m_Milli	 == Other.m_Milli;
}

inline bool CTimePeriod::operator < (const CTimePeriod& Other) const
{
	return  m_Seconds < Other.m_Seconds
		|| ((m_Seconds == Other.m_Seconds) && (m_Milli < Other.m_Milli));
}

inline bool CTimePeriod::operator > (const CTimePeriod& Other) const
{
	return  m_Seconds > Other.m_Seconds
		|| (m_Seconds == Other.m_Seconds && m_Milli > Other.m_Milli);
}

inline bool CTimePeriod::operator !=(const CTimePeriod& Other) const
{
	return ! operator==(Other);
}

inline bool CTimePeriod::operator <=(const CTimePeriod& Other) const
{
	return operator<(Other) || operator==(Other);
}

inline bool CTimePeriod::operator >=(const CTimePeriod& Other) const
{
	return operator>(Other) || operator==(Other);
}

inline CTimePeriod CTimePeriod::operator +(const CTimePeriod& Other) const
{
	CTimePeriod NewSpan = *this;
	return NewSpan += Other;
}

inline CTimePeriod CTimePeriod::operator -(const CTimePeriod& Other) const
{
	CTimePeriod NewSpan = *this;
	return NewSpan -= Other;
}

inline int CTimePeriod::Milli() const
{
	return (int)((m_Seconds * 1000) + m_Milli);
}

inline int CTimePeriod::GetSeconds() const
{
	// m_Seconds holds the sign of the TimePeriod
	// and can be either positive or negative. m_Milli
	// is always positive. The actual number of seconds in
	// the TimePeriod depends on the TimePeriod sign and the
	// value of m_Milli.
	if(m_Seconds >= 0)   // The TimePeriod is positive
		return m_Seconds;
	else if(m_Milli > 0) // The TimePeriod is negative
		return m_Seconds + 1;
	return m_Seconds;	 // The TimePeriod is negative and m_Milli == 0
}

inline int CTimePeriod::GetMilli() const
{
	// m_Seconds holds the sign of the TimePeriod
	// and can be either positive or negative. m_Milli
	// is always positive. The actual number of milliseconds in
	// the TimePeriod depends on the TimePeriod sign and the
	// value of m_Milli.
	if(m_Seconds >= 0)	 // The TimePeriod is positive
		return m_Milli;
	else if(m_Milli > 0) // The TimePeriod is negative
		return m_Milli - 1000;
	return m_Milli;		 // The TimePeriod is negative and m_Milli == 0
}
