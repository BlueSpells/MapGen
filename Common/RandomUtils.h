#pragma once

#include "TimeStamp.h"
#include "TimePeriod.h"

//////////////////////////////////////////////////////////////////////////
// Utils that manipulate random numbers.

// Get random DWORD number
DWORD GetRandomNumber();

bool GetRandomBool(int ChanceForTrueInPercentage = 50);

// Get a random number in a range
int GetRandomNumberInRange(int BeginOfRange, int EndOfRange);

// Get random time period or time stamp in a time range
CTimePeriod GetRandomTimeInRange(const CTimePeriod& Begin, const CTimePeriod& End);
CTimeStamp GetRandomTimeInRange(const CTimeStamp& Begin, const CTimeStamp& End);

// This function logics is as follows: To get random numbers whose average is the 
// given number, it will be enough to get random number between 0% to 200%
CTimePeriod GetRandomTimeAroundAverage(const CTimePeriod& Average);

std::string GetRandomString(int Min, int Max, int MinChar, int MaxChar);
std::string GetRandomString(int Min, int Max, const char* Chars);
