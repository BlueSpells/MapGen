#pragma once
#include <string>
#include "IniFile.h"

class CPersistent64BitCounter
{
public:
	CPersistent64BitCounter(UINT CounterBits = 64, UINT64 FixedBitsValue = 0);
	~CPersistent64BitCounter(void);

	void Init(const char* ConfigSection, const char* ValuesIniFileName, const char* ValuesConfigSection = NULL);
	
	//Returns the current value 
	UINT64 GetCount();
	
	//Advances the count and returns the new value
	UINT64 AdvanceCount();

private:
	void WriteCounterToIniFile();

	CIniFile m_ValuesFile;
	std::string m_ValuesConfigSection;

	UINT	m_WriteInterval;
	UINT64	m_Count;

	UINT64	m_Mask;
	UINT64	m_FixedBits;
};
