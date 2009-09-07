// Statistics module - implementation of CStatistics

#include "stdafx.h"
#include <math.h>
#include "Statistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////

CStatistics::CStatistics()
{
    Clear();
}

void CStatistics::Clear()
{
    m_Total        = 0;
    m_TotalSquared = 0;
    m_Min          = 1e20;
    m_Max          = -1e20;
    m_Count        = 0; 
}

void CStatistics::AddSample(double Sample)
{
    m_Total        += Sample;
    m_TotalSquared += Sample * Sample;

    m_Count++;

    if (Sample < m_Min)
        m_Min = Sample;

    if (Sample > m_Max)
        m_Max = Sample;
}

double CStatistics::GetAverage() const
{
    return m_Count == 0 ? 0 : m_Total / m_Count;
}

double CStatistics::GetMin() const
{
    return m_Min;
}

double CStatistics::GetMax() const
{
    return m_Max;
}

int CStatistics::GetCount() const
{
    return m_Count;
}

double CStatistics::GetStandardDeviation() const
{
    if (m_Count == 0)
        return 0;

    double Average = m_Total / m_Count;
    double Variation = m_TotalSquared / m_Count - Average * Average;
    return sqrt(Variation);
}


//////////////////////////////////////////////////////////////////////
// Too simple class to have a test program.
void TestStatistics()
{
    double StandardDeviation, Average, Min, Max;
    CStatistics Statis;

    Average = Statis.GetAverage();

    Statis.AddSample(-1);
    Statis.AddSample(0);
    Statis.AddSample(2);

    Min = Statis.GetMin();
    Max = Statis.GetMax();
    Average = Statis.GetAverage();
    StandardDeviation = Statis.GetStandardDeviation();

}
