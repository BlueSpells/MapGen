#include "StdAfx.h"
#include "RandomUtils.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
// Since the rand() function is returning integers only in range 0 - 32000,
// The following technic was used. Choosing 4 random numbers and from each number
// taking only 8 bits. Combining it together and receiving 32 bits.
DWORD GetRandomNumber()
{
    // In the first use of this function, planting the generating seed.
    static bool IsSeedGenerated = false;
    if (IsSeedGenerated == false)
    {
        srand(GetTickCount());
        IsSeedGenerated = true;
    }

    int Rand1 = rand();
    int Rand2 = rand();
    int Rand3 = rand();
    int Rand4 = rand();

    Rand1 = Rand1 & 0x00ff;
    Rand2 = Rand2 & 0xff00;
    Rand3 = Rand3 & 0x00ff;
    Rand4 = Rand4 & 0xff00;

    return Rand1 + Rand2 + ((Rand3 + Rand4) << 16);
}


//////////////////////////////////////////////////////////////////////////
bool GetRandomBool(int ChanceForTrueInPercentage /* = 50 */)
{
    int RandomNumber = GetRandomNumberInRange(1, 100);
    return (RandomNumber <= ChanceForTrueInPercentage) ? true : false;
}


//////////////////////////////////////////////////////////////////////////
int GetRandomNumberInRange(int BeginOfRange, int EndOfRange)
{
    // The range is +1 than the actual range. Since we are using this
    // range for % operation, than it should be +1 since the result of (% Range) operation
    // is in [0,Range -1]
    int Range = EndOfRange - BeginOfRange + 1;
    if (Range <= 0) // wrong params
        return BeginOfRange - 1;
    else
        return BeginOfRange + (GetRandomNumber() % Range);
}

//////////////////////////////////////////////////////////////////////////
// GetRandomTime - a static helper functions that is used by the GetRandomTimeInRange() functions

static void GetRandomTime(int StartSeconds, int StartMilli, int EndSeconds, int EndMilli,
                     int& RandomSeconds, int& RandomMilli)
{
    RandomSeconds = GetRandomNumberInRange(StartSeconds, EndSeconds);

    // To calculate the milli is more complex. Generally it is inside 0 - 999, unless
    // The random seconds are either equal to begin or end. In that case
    // restricting the milli to the range boundary.
    int BeginRangeForMilli = 0;
    int EndRangeForMilli = 999;
    if (RandomSeconds == StartSeconds)
        BeginRangeForMilli = StartMilli;
    if (RandomSeconds == EndSeconds)
        EndRangeForMilli = EndMilli;

    RandomMilli = GetRandomNumberInRange(BeginRangeForMilli, EndRangeForMilli);
}

//////////////////////////////////////////////////////////////////////////
CTimePeriod GetRandomTimeInRange(const CTimePeriod& Begin, const CTimePeriod& End)
{
    if (Begin >= End) // Wrong params
        return Begin;

    int RandomSeconds, RandomMilli;
    GetRandomTime(Begin.GetSeconds(), Begin.GetMilli(), 
        End.GetSeconds(), End.GetMilli(),
        RandomSeconds, RandomMilli);

    return CTimePeriod(RandomSeconds, RandomMilli);
}

//////////////////////////////////////////////////////////////////////////
CTimeStamp GetRandomTimeInRange(const CTimeStamp& Begin, const CTimeStamp& End)
{
    if (Begin >= End) // Wrong params
        return Begin;

    int RandomSeconds, RandomMilli;
    GetRandomTime(Begin.GetSeconds(), Begin.GetMilli(), 
        End.GetSeconds(), End.GetMilli(),
        RandomSeconds, RandomMilli);

    return CTimeStamp(RandomSeconds, RandomMilli);
}

//////////////////////////////////////////////////////////////////////////
// This function logics is as follows: To get random numbers whose average is the 
// given number, it will be enough to get random number between 0% to 200%
 
CTimePeriod GetRandomTimeAroundAverage(const CTimePeriod& Average)
{
    CTimePeriod TwiceAvarage = Average.Milli() * 2;
    if (TwiceAvarage < Average) // Overflow
    {
        LogEvent(LE_ERROR, "GetRandomTimeAroundAverage - TimePeriod is too big. Returning Average");
        //Assert(false);
        return Average;
    }
    return GetRandomTimeInRange(0, TwiceAvarage);
}


std::string GetRandomString(int Min, int Max, int MinChar, int MaxChar)
{
    if (Max < Min)
        return "";

    char* Buf = new char[Max+1];
    int Len = GetRandomNumberInRange(Min, Max);

    int i;
    char* Ptr;
    for (Ptr=Buf, i=0; i<Len; Ptr++, i++)
        *Ptr = (char)(GetRandomNumberInRange(MinChar, MaxChar));
    Buf[Len] = '\0';

    std::string Ret = Buf;
    delete [] Buf;
    return Ret;
}

std::string GetRandomString(int Min, int Max, const char* Chars)
{
    if (Max < Min)
        return "";

    char* Buf = new char[Max+1];
    int Len = GetRandomNumberInRange(Min, Max);

    int i;
    char* Ptr;

    int CharsNum = strlen(Chars);
    for (Ptr=Buf, i=0; i<Len; Ptr++, i++)
        *Ptr = Chars[GetRandomNumberInRange(0, CharsNum-1)];
    Buf[Len] = '\0';

    std::string Ret = Buf;
    delete [] Buf;
    return Ret;
}


//////////////////////////////////////////////////////////////////////////
#ifdef _TEST

void TestRandomUtils()
{
    LogEvent(LE_INFOHIGH, "------------ TestRandomUtils - Start Testing random utils -------------");

    // Seeding the random generator according to the time
    static bool Init = false;
    if (!Init)
        srand(GetTickCount());
    Init = true;

    const int BeginRange = -50000;
    const int EndRange = 50000;
    const int NumOfNumbersToPick = 10;
    LogEvent(LE_INFOHIGH, "TestRandomUtils - Select %d numbers in the range of [%d, %d]",
            NumOfNumbersToPick, BeginRange, EndRange);
    for (int i = 0 ; i < NumOfNumbersToPick ; i++)
    {
        int Choosen = GetRandomNumberInRange(BeginRange, EndRange);
        LogEvent(LE_INFOHIGH, "Pick number [%d] - Selected number is [%d]", i, Choosen);
        Assert(Choosen >= BeginRange && Choosen <= EndRange);
    }
    return;
}
#endif