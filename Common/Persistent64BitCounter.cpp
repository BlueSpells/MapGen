#include "StdAfx.h"
#include "Persistent64BitCounter.h"
#include "Config.h"
#include "LogEvent.h"

static const int	DEFAULT_COUNTER_WRITE_INTERVAL	= 1000;
static const INT64	DEFAULT_COUNTER_INITIAL_VALUE	= 0;

static const UINT64 ALL_ONES_64						= 0xFFFFFFFFFFFFFFFF;
static const UINT	BITS_NUM						= 64;

CPersistent64BitCounter::CPersistent64BitCounter(UINT CounterBits /*= 64*/, UINT64 FixedBitsValue /*= 0*/)
{
	m_Mask = ALL_ONES_64 >> (BITS_NUM - CounterBits);
	m_FixedBits = FixedBitsValue << CounterBits;
}

CPersistent64BitCounter::~CPersistent64BitCounter(void)
{
}

void CPersistent64BitCounter::Init(const char* ConfigSection, const char* ValuesIniFileName, const char* ValuesConfigSection /*= NULL*/)
{
	if (ValuesConfigSection == NULL)
	{
		ValuesConfigSection = ConfigSection;
	}

	m_WriteInterval =	GetConfigInt(ConfigSection, "CounterWriteInterval", DEFAULT_COUNTER_WRITE_INTERVAL);

	m_ValuesConfigSection = ValuesConfigSection;
	m_ValuesFile.SetFileName(ValuesIniFileName);
	m_ValuesFile.SetWriteBackMode(true);
	m_Count = m_ValuesFile.GetConfigInt64(m_ValuesConfigSection.c_str(), "Counter", DEFAULT_COUNTER_INITIAL_VALUE);
	m_Count = m_Count & m_Mask;
	
	//We immediately advance the count upon initialization to avoid the count getting the same value twice if no Advance was called
	WriteCounterToIniFile();
}

UINT64 CPersistent64BitCounter::GetCount()
{
	return m_Count | m_FixedBits;
}

UINT64 CPersistent64BitCounter::AdvanceCount()
{
	m_Count++;
	m_Count = m_Count & m_Mask;

	if ((m_Count % m_WriteInterval) == 0)
	{
		WriteCounterToIniFile();

		//AvoidEverReturning 0 on Advance count
		if (m_Count == 0)
		{
			AdvanceCount();
		}
	}

	return GetCount();
}

void CPersistent64BitCounter::WriteCounterToIniFile()
{
	//We write the new count + 2 * write interval to avoid duplicates in case the program fails
	UINT64 WriteValue = m_Count+2*m_WriteInterval;
	WriteValue = WriteValue & m_Mask;
	m_ValuesFile.WriteConfigInt64(m_ValuesConfigSection.c_str(), "Counter", WriteValue);
}

#ifdef _TEST
#include "LogEvent.h"
#include "RandomUtils.h"

void TestPersistent64BitCounter()
{
	const UINT64 TestCount2Mask =	0x0000FFFFFFFFFFFF;
	const UINT64 TestCount2Fixed =	0xBEEF000000000000;
	CPersistent64BitCounter Counter1;
	CPersistent64BitCounter Counter2(48, 0xBEEF);
	UINT64 TestCount1 = 0;
	UINT64 TestCount2 = 0;
	Counter1.Init("TestCounter", ".\\TestCounter.ini", "Counter1");

	//First we try a few interesting cases
	//Two inits in a raw
	TestCount1 = Counter1.GetCount() + 2000;
	Counter1.Init("TestCounter", ".\\TestCounter.ini", "Counter1");
	Assert(Counter1.GetCount() == TestCount1);

	//1 Less Than Needed to advance
	for (int i=0; i<999; i++)
	{
		TestCount1++;
		Assert(Counter1.AdvanceCount() == TestCount1);
	}

	TestCount1 = TestCount1 + 1001;
	Counter1.Init("TestCounter", ".\\TestCounter.ini", "Counter1");
	Assert(Counter1.GetCount() == TestCount1);

	//Exactlly one advance
	for (int i=0; i<1000; i++)
	{
		TestCount1++;
		Assert(Counter1.AdvanceCount() == TestCount1);
	}
	TestCount1 = TestCount1 + 2000;
	Counter1.Init("TestCounter", ".\\TestCounter.ini", "Counter1");
	Assert(Counter1.GetCount() == TestCount1);
	TestCount1 = TestCount1 + 2000;

	//Init the second counter and get value
	Counter2.Init("TestCounter", ".\\TestCounter.ini", "Counter2");
	TestCount2 = Counter2.GetCount() + 2000;

	//Then we do some random checking
	for (int i=0; i<1000; i++)
	{
		Counter1.Init("TestCounter", ".\\TestCounter.ini", "Counter1");
		Assert(Counter1.GetCount() == TestCount1);

		int Advances = GetRandomNumberInRange(1, 5000);
		for (int i=0; i<Advances; i++)
		{
			TestCount1++;
			Assert(Counter1.AdvanceCount() == TestCount1);
		}
		Counter2.Init("TestCounter", ".\\TestCounter.ini", "Counter2");
		Assert(Counter2.GetCount() == TestCount2);

		int Advances2 = GetRandomNumberInRange(1, 5000);
		for (int j=0; j<Advances2; j++)
		{
			TestCount1++;
			Assert(Counter1.AdvanceCount() == TestCount1);
			TestCount2++;
			TestCount2 = (TestCount2 & TestCount2Mask) | TestCount2Fixed;
			Assert(Counter2.AdvanceCount() == TestCount2);
		}

		TestCount1 = TestCount1 - (TestCount1 % 1000) + 2000;
		
		TestCount2 = (TestCount2 & TestCount2Mask);
		TestCount2 = TestCount2 - (TestCount2 % 1000) + 2000;
		TestCount2 = (TestCount2 & TestCount2Mask) | TestCount2Fixed;
	}
}

#endif _TEST