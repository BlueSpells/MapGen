// Statistics.h - elementary statistis calculation
#pragma once

class CStatistics
{
public:
    CStatistics();

    void Clear();
    void AddSample(double Sample);

    double GetAverage() const;
    double GetStandardDeviation() const;
    double GetMin() const;
    double GetMax() const;
    int GetCount() const;

private:
    double m_Total;
    double m_TotalSquared;
    double m_Min;
    double m_Max;
    int    m_Count; 
};

