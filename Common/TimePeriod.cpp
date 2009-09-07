#include "stdafx.h"
#include "TimePeriod.h"
#include "Common/LogEvent.h"
#include "Common/Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/************************************************************************/
/* The TimePeriod class represents a time interval in a resolution of
	milliseconds. This time interval can be either positive or negative.
	The internal representation is composed from seconds and milliseconds.
	The interval sign (positive/negative) is held only in the seconds variable
	of the representation. The milliseconds variable is calculated according
	to the interval sign. To the user both the seconds and milliseconds hold
	the interval sign. This means that a user see a negative TimePeriod as
	composed from a number of negative seconds and a number of negative
	milliseconds.														*/
/************************************************************************/
const CTimePeriod CTimePeriod::ZeroLength;
const CTimePeriod CTimePeriod::InfiniteLength(0x7fffffff, 999);

// Constructors
CTimePeriod::CTimePeriod()
{
	m_Seconds	= 0;
	m_Milli		= 0;
}

CTimePeriod::CTimePeriod(int nMilli)
{
	m_Seconds	= nMilli / 1000;
	m_Milli		= nMilli % 1000;
	// If the TimePeriod is negative,
	// We hold the sign only in the seconds variable.
	if(m_Milli < 0)
	{
		m_Milli		+= 1000;
		m_Seconds	-= 1;
	}
}

//CTimePeriod& CTimePeriod::operator =(int NumMilli)
//{
//	m_Seconds	= NumMilli / 1000;
//	m_Milli = NumMilli % 1000;
//	return *this;
//}

CTimePeriod::CTimePeriod(time_t nSeconds, int nMilli)
{
	Assert(abs(nMilli) < 1000);
	Assert(sign(nSeconds) == sign(nMilli) || nMilli == 0 || nSeconds == 0);

	m_Seconds	= nSeconds;
	m_Milli		= nMilli;
	// If the TimePeriod is negative,
	// We hold the sign only in the seconds variable.
	if(m_Milli < 0)
	{
		m_Milli		+= 1000;
		m_Seconds	-= 1;
	}
}

// Operators
// Hazard: What happens if the combined time is not representable?
CTimePeriod& CTimePeriod::operator +=(const CTimePeriod& Other)
{
	m_Milli		+= Other.m_Milli;
	m_Seconds	+= Other.m_Seconds;
	if(m_Milli >= 1000)
	{
		m_Milli		-= 1000;
		m_Seconds	+= 1; 		
	}

	return *this;
}

CTimePeriod& CTimePeriod::operator -=(const CTimePeriod& Other)
{
	//if(m_Milli < Other.m_Milli)
	//{
	//	m_Milli += (1000 - Other.m_Milli);
	//	m_Seconds	-= 1;
	//}
	//else
	//	m_Milli = (unsigned short)(m_Milli - Other.m_Milli);
	
	m_Milli		-= Other.m_Milli;
	m_Seconds	-= Other.m_Seconds;
	if(m_Milli < 0)
	{
		m_Milli		+= 1000;
		m_Seconds	-= 1; 		
	}

	return *this;
}

double CTimePeriod::operator /(const CTimePeriod& Other) const
{
	// It is more precise to multiply the seconds by 1000
	// than to divide the milliseconds by 1000. We assume that we
	// only need 32 bits for seconds and 10 bits for milliseconds.
	// We rely on the based assumption that a double has 52 bits for
	// the mantissa.
	double Left	 = (m_Seconds		* 1000) + m_Milli;
	double Right = (Other.m_Seconds	* 1000) + Other.m_Milli;

    //patch for preventing divide by zero
    if(Right == 0)
        Right = 1;
	return Left / Right;
}

CTimePeriod CTimePeriod::operator /(int Divisor) const
{
	return operator/((double)(Divisor));
}

CTimePeriod CTimePeriod::operator /(double Divisor) const
{
	// It is more precise to multiply the seconds by 1000
	// than to divide the milliseconds by 1000. We assume that we
	// only need 32 bits for seconds and 10 bits for milliseconds.
	// We rely on the based assumption that a double has 52 bits for
	// the mantissa.
	double Time = (m_Seconds * 1000) + m_Milli;
	double DividedTime = Time / Divisor;
	int Seconds = (int)(DividedTime / 1000);
	int Millis	= (int)(DividedTime - Seconds * 1000);
	return CTimePeriod(Seconds, Millis);
}

CTimePeriod CTimePeriod::operator* (double Multiply) const
{
    double Time = (m_Seconds * 1000) + m_Milli;
    double MultiplyTime = Time * Multiply;
    int Seconds = (int)(MultiplyTime / 1000);
    int Millis	= (int)(MultiplyTime - Seconds * 1000);
    return CTimePeriod(Seconds, Millis);
}

//SecondsToPeriod - helper of the ToString()
static std::string SecondsToPeriod(int& Seconds, int PeriodInSeconds, const std::string& PeriodTitle)
{
    std::string PeriodStr;
    int NumPeriods = Seconds / PeriodInSeconds;
    if(NumPeriods != 0)
    {
        PeriodStr = IntToStr(NumPeriods) + " " + PeriodTitle + ",";
        Seconds -= NumPeriods * PeriodInSeconds;
    }
    return PeriodStr;
}

std::string CTimePeriod::ToString() const
{
    std::string PeriodStr;
    int Seconds = m_Seconds;
    int Milli = m_Milli;

    bool NegativeMilliAdjust = (Seconds < 0 && m_Milli != 0);
    if(NegativeMilliAdjust)
    {
        Seconds += 1;
        Milli -= 1000;
    }

    PeriodStr += SecondsToPeriod(Seconds, DAY_IN_SECONDS, "Days");
    PeriodStr += SecondsToPeriod(Seconds, HOUR_IN_SECONDS, "Hours");
    PeriodStr += SecondsToPeriod(Seconds, MINUTE_IN_SECONDS, "Minutes");
    PeriodStr += SecondsToPeriod(Seconds, 1, "Seconds");

    Assert(Seconds == 0);
    PeriodStr += IntToStr(Milli) + " Milli";
    return PeriodStr;

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _TEST

class CTimePeriodTester
{
public:
	void FunctionalTest();
	void TestDivision();
};

void CTimePeriodTester::TestDivision()
{
	Assert(2	== (CTimePeriod(10, 0) / CTimePeriod(5, 0)));
	Assert(0.5	== (CTimePeriod(5, 0) / CTimePeriod(10, 0)));
	Assert(20	== (CTimePeriod(5, 0) / CTimePeriod(0, 250)));
	Assert(0.25	== (CTimePeriod(0, 250) / CTimePeriod(1, 0)));
	Assert(4.5	== (CTimePeriod(5, 625) / CTimePeriod(1, 250)));

	Assert(CTimePeriod(1, 875) == CTimePeriod(5, 625) / 3);
	Assert(CTimePeriod(0, 250) == CTimePeriod(1, 250) / 5);
}

void CTimePeriodTester::FunctionalTest()
{
	CTimePeriod A, B, C;
	A = 1551;
	B = CTimePeriod(1, 551);
	Assert(A == B);
	A = -1551;
	B = CTimePeriod(-1, -551);
	Assert(A == B);
	C = -435;
	Assert(C > A && C >= A);
	Assert(A < C && A <= C);
	Assert(A != C);
	
	A = 2300;
	A -= 200;
	Assert(A == 2100);
	A += 400;
	Assert(A == 2500);
	A -= 3000;
	Assert(A == -500);
	A += 200;
	Assert(A == -300);
	Assert(A + CTimePeriod(100) == CTimePeriod(-200));
	Assert(A - CTimePeriod(100) == CTimePeriod(-400));

	A = 2300;
	Assert(A.GetSeconds() == 2 && A.GetMilli() == 300 && A.Milli() == 2300);
	A = -2300;
	Assert(A.GetSeconds() == -2 && A.GetMilli() == -300 && A.Milli() == -2300);
	A = CTimePeriod(1, 500);
	Assert(A.GetSeconds() == 1 && A.GetMilli() == 500 && A.Milli() == 1500);
	A = CTimePeriod(-1, -500);
	Assert(A.GetSeconds() == -1 && A.GetMilli() == -500 && A.Milli() == -1500);

	Assert(CTimePeriod::ZeroLength == 0 && CTimePeriod(0, 0) == CTimePeriod::ZeroLength);
}

static void TestPeriodStr(int Seconds, int Milli)
{
    CTimePeriod Period(Seconds, Milli);
    LogEvent(LE_INFOHIGH, "TestPeriodStr: Seconds [%d], Milli [%d], Str [%s]",
        Seconds, Milli, Period.ToString().c_str());
}
static void TestToString()
{
    LogEvent(LE_INFOHIGH, "TimePeriod Test of ToString()");
    WaitAndPumpMessages(1000);

    TestPeriodStr(0, 0);
    TestPeriodStr(100, 111);
    TestPeriodStr(10000, 222);
    TestPeriodStr(1000000, 333);
    TestPeriodStr(-100, -444);
    TestPeriodStr(-100000, -555);
}

void TestTimePeriod()
{
	CTimePeriodTester Tester;
	Tester.FunctionalTest();
	Tester.TestDivision();
    TestToString();
}

#endif