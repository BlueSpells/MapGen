// ClusterCalculator.h: interface for the CClusterCalculator classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLUSTERCALCULATOR_H__BAB0682A_4D3B_475A_BEA1_CC5A12B007D7__INCLUDED_)
#define AFX_CLUSTERCALCULATOR_H__BAB0682A_4D3B_475A_BEA1_CC5A12B007D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <float.h>

//#include "Repository\String\BasicString.h"

//////////////////////////////////////////////////////////////////////
// The Cluster Calculator are used as template classes for the 
// Cluster Manager.
//
// Each Cluster Calculator should implement the following routines
//
//  void Clear()            - Clear data - Count <- 0
//  void Add(double Value)  - Add a Value
//  double Get()            - Get computed values - this is the actual
//                            result of the calculator. DBL_MAX means no value
//  GetLabelAndUnits(
//    TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//                          - Label and Units to be displayed at the graph
//                            If Units is empty - then it is retrieved from the
//                            data set.
//

// Returns the sum number of samples
//class CSumClusterCalculator
//{
//public:
//
//    CSumClusterCalculator() : m_lfSum(DBL_MAX)
//    {
//    }
//
//    void Clear()
//    {
//        m_lfSum= DBL_MAX; // no value
//    }
//
//    void Add(double lfValue)
//    {
//        if (m_lfSum == DBL_MAX)
//            m_lfSum= lfValue;
//        else
//            m_lfSum+= lfValue;
//    }
//
//    double Get() const
//    {
//        return m_lfSum;
//    }
//
//    void GetLabelAndUnits(
//        TCHAR* pcLabel, UINT uLabelSize, TCHAR* pcUnits, UINT uUnitsSize)
//    {
//        str_cpy(pcLabel, uLabelSize, "Sum");
//        str_cpy(pcUnits, uUnitsSize, "#"  );
//    }
//
//private:
//    
//    double m_lfSum;
//};

// Returns the precentage number of samples
//class CPrecentageClusterCalculator
//{
//public:
//
//    CPrecentageClusterCalculator() : m_nTotal(0), m_lfSum(0)
//    {
//    }
//
//    void Add(double Value) // Value can be 0 or 1
//    {
//        m_lfSum+= Value;
//        m_nTotal++;
//    }
//
//    void Clear()
//    {
//        m_lfSum = 0.;
//        m_nTotal= 0 ;
//    }
//
//    double Get()
//    {
//        if (m_nTotal == 0.)
//            return DBL_MAX;
//        else
//            return m_lfSum / m_nTotal * 100.;
//    }
//
//    void GetLabelAndUnits(
//        TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "Percentage");
//        str_cpy(Units, UnitsSize, "%"         );
//    }
//
//private:
//
//    int    m_nTotal;
//    double m_lfSum ;
//};

// Returns the number of samples
//class CCountClusterCalculator
//{
//public:
//    CCountClusterCalculator() : m_Count(0)
//    {
//    }
//
//    void Add(double /*Value*/)
//    {
//        ++m_Count;
//    }
//    void Clear()
//    {
//        m_Count = 0;
//    }
//    double Get()
//    {
//        return m_Count;
//    }
//
//    void GetLabelAndUnits(
//        TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "Count");
//        str_cpy(Units, UnitsSize, "#"    );
//    }
//private:
//    int m_Count;
//};

// Return the Average of samples
class CAverageClusterCalculator
{
public:
    CAverageClusterCalculator() : m_Average(0)/*m_Total(0)*/, m_Count(0)
    {
    }

    void Add(double Value)
    {
		double CurrentValueContribution = Value / (m_Count + 1);
		double PreviousValuesContribution = m_Average * (m_Count / (double)(m_Count + 1));
		m_Average = PreviousValuesContribution + CurrentValueContribution;
        ++m_Count;
//        m_Total += Value;
    }
    void Clear()
    {
        m_Count = 0;
		m_Average = 0;
//        m_Total = 0;
    }
    double Get()

	{
        if (m_Count == 0)
            return DBL_MAX;
        else
//            return m_Total / m_Count;
			return m_Average;
    }
    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
    {
        //str_cpy(Label, LabelSize,"Average");
        //str_cpy(Units, UnitsSize, ""       );
		LabelSize; UnitsSize;
		strcpy(Label, "Average");
		strcpy(Units, "");
    }
private:
//    double m_Total;
	double m_Average;
    int    m_Count;
};

// Return the Average of samples
class CAverageCalculator
{
public:
	CAverageCalculator() : m_Total(0), m_Count(0)
	{
	}

	void Add(double Value)
	{
		++m_Count;
		m_Total += Value;
	}
	void Clear()
	{
		m_Count = 0;
		m_Total = 0;
	}
	double Get()
	{
		if (m_Count == 0)
			return DBL_MAX;
		else
			return m_Total / m_Count;
	}
	void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
	{
		//str_cpy(Label, LabelSize,"Average");
		//str_cpy(Units, UnitsSize, ""       );
		LabelSize; UnitsSize;
		strcpy(Label, "Average");
		strcpy(Units, "");
	}
private:
	double m_Total;
	int    m_Count;
}; 

#include <map>

// Return the Median of samples
//class CMedianClusterCalculator
//{
//public:
//    CMedianClusterCalculator ()
//    {
//        m_MedianIter = m_Data.end();
//        m_LesserCount = m_BiggerCount = 0;
//    }
//    
//    void Add(double ValueToAdd)
//    {
//        
//        //insert the value in the map
//        DoublesCountMapIterator iter = m_Data.find(ValueToAdd);
//        if(iter == m_Data.end())
//        {
//            //the value is not in the map
//            DoublesCountMap::value_type pairValue(ValueToAdd, 1);
//            m_Data.insert(pairValue);
//        }
//        else
//        {
//            //the value is in the map
//            iter->second += 1;
//        }
//
//        if(m_Data.size() == 1)
//        {
//            //first item inserted
//            m_MedianIter = m_Data.begin();
//            return;
//        }
//
//
//        double MediaVal = m_MedianIter->first;
//        int MedianCount = m_MedianIter->second;
//        if(ValueToAdd > MediaVal)
//            m_BiggerCount +=1;
//        else if(ValueToAdd < MediaVal)
//            m_LesserCount +=1;
//
//        
//        //check out if we need to move the iterator
//        int TotalItems = m_LesserCount + MedianCount +  m_BiggerCount;
//        int RequestedMedianPos = (TotalItems + 1) / 2;
//        if(RequestedMedianPos <= m_LesserCount)
//        {
//            --m_MedianIter;
//            m_BiggerCount += MedianCount;
//
//            int mNewMedianCount = m_MedianIter->second;
//            m_LesserCount -= mNewMedianCount;
//        }
//        else if(RequestedMedianPos > m_LesserCount + MedianCount)
//        {
//            ++m_MedianIter;
//            m_LesserCount += MedianCount;
//            
//            int mNewMedianCount = m_MedianIter->second;
//            m_BiggerCount -= mNewMedianCount;
//        }
//    }
//
//    void Clear()
//    {
//        m_Data.clear();
//        m_BiggerCount = m_LesserCount = 0;
//        m_MedianIter = m_Data.end();
//    }
//
//    double Get()
//    {
//        if(m_Data.size() == 0)
//            return DBL_MAX;
//        
//        return m_MedianIter->first;
//    }
//
//    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "Median");
//        str_cpy(Units, UnitsSize, ""      );
//    }
//
//private:
//    typedef std::map<double, int> DoublesCountMap;
//    typedef DoublesCountMap::iterator DoublesCountMapIterator;
//    DoublesCountMap            m_Data;
//    DoublesCountMapIterator    m_MedianIter;
//    int                        m_LesserCount;
//    int                        m_BiggerCount;
//};

#include <float.h>

// Return the Minimum of samples
//class CMinimumClusterCalculator
//{
//public:
//    CMinimumClusterCalculator() : m_CurrentMinimum(DBL_MAX)
//    {
//    }
//    
//    void Add(double Value)
//    {
//        if (Value < m_CurrentMinimum)
//            m_CurrentMinimum  = Value;
//    }
//    void Clear()
//    {
//        m_CurrentMinimum = DBL_MAX;
//    }
//    double Get()
//    {
//        // What to return if no value ?
//        if (m_CurrentMinimum == DBL_MAX)
//            return DBL_MAX;
//        else
//            return m_CurrentMinimum;
//    }
//
//    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "Minimum");
//        str_cpy(Units, UnitsSize, ""       );
//    }
//    
//private:
//    double m_CurrentMinimum;
//}; 

// Return the Miximum of samples
class CMaximumClusterCalculator
{
public:
    CMaximumClusterCalculator() : m_CurrentMaximum(-DBL_MAX)
    {
    }
    
    void Add(double Value)
    {
        if (Value > m_CurrentMaximum)
            m_CurrentMaximum = Value;
    }
    void Clear()
    {
        m_CurrentMaximum = -DBL_MAX;
    }
    double Get()
    {
        // What to return if no value ?
        if (m_CurrentMaximum == -DBL_MAX)
            return DBL_MAX;
        else
            return m_CurrentMaximum;
    }

    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
    {
        //str_cpy(Label, LabelSize, "Maximum");
        //str_cpy(Units, UnitsSize, ""       );
		LabelSize; UnitsSize;
		strcpy(Label, "Maximum");
		strcpy(Units, ""       );
    }
    
private:
    double m_CurrentMaximum;
}; 

//class CRangeClusterCalculator
//{
//public:
//    CRangeClusterCalculator()
//    {
//    }
//    void Add(double Value)
//    {
//        m_MinimumCalculator.Add(Value);
//        m_MaximumCalculator.Add(Value);
//    }
//    void Clear()
//    {
//        m_MinimumCalculator.Clear();
//        m_MaximumCalculator.Clear();
//    }
//    double Get()
//    {
//        double Max = m_MaximumCalculator.Get();
//        double Min = m_MinimumCalculator.Get();
//        if (Max == DBL_MAX)
//            return DBL_MAX;
//        else
//            return Max - Min;
//    }
//
//    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "Range");
//        str_cpy(Units, UnitsSize, ""     );
//    }
//    
//private:
//    CMinimumClusterCalculator m_MinimumCalculator;
//    CMaximumClusterCalculator m_MaximumCalculator;
//}; 

#include <math.h>

// Compute Standard Deviation
class CStandardDeviationClusterCalculator
{
public:
    CStandardDeviationClusterCalculator() : 
        m_Average(0), /*m_Total(0),*/ m_SquaredAverage(0), /*m_TotalSquared(0),*/ m_Count(0)
    {
    }

    void Add(double Value)
    {
		{
			double CurrentValueContribution = Value / (m_Count + 1);
			double PreviousValuesContribution = m_Average * (m_Count / (double)(m_Count + 1));
			m_Average = PreviousValuesContribution + CurrentValueContribution;
		}
		{
			double CurrentValueContribution = (Value * Value) / (m_Count + 1);
			double PreviousValuesContribution = m_SquaredAverage * (m_Count / (double)(m_Count + 1));
			m_SquaredAverage = PreviousValuesContribution + CurrentValueContribution;
		}

		++m_Count;
        //m_Total += Value;
        //m_TotalSquared += Value * Value;
    }
    void Clear()
    {
        m_Count = 0;
//        m_Total = 0;
		m_Average = 0;
//        m_TotalSquared = 0;
		m_SquaredAverage = 0;
    }
    // Standard Deviation =  
    double Get()
    {
        if (m_Count == 0)
            return DBL_MAX;
        // Do we cash this value?
        //double Average = m_Total / m_Count;
        //double Variation = 
        //    m_TotalSquared / m_Count - Average * Average;
		double Variation = m_SquaredAverage - m_Average * m_Average;
        return sqrt(Variation);
    }

    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
    {
        //str_cpy(Label, LabelSize, "Standard Deviation");
        //str_cpy(Units, UnitsSize, ""                  );
		LabelSize; UnitsSize;
		strcpy(Label, "Standard Deviation");
		strcpy(Units, ""                  );
    }
    
private:
    int    m_Count;
	double m_Average;
//    double m_Total;
	double m_SquaredAverage;
//    double m_TotalSquared;
}; 

class CStandardDeviationCalculator
{
public:
	CStandardDeviationCalculator() : 
	  m_Total(0), m_TotalSquared(0), m_Count(0)
	  {
	  }

	  void Add(double Value)
	  {
		  ++m_Count;
		  m_Total += Value;
		  m_TotalSquared += Value * Value;
	  }
	  void Clear()
	  {
		  m_Count = 0;
		  m_Total = 0;
		  m_TotalSquared = 0;
	  }
	  // Standard Deviation =  
	  double Get()
	  {
		  if (m_Count == 0)
			  return DBL_MAX;
		  // Do we cash this value?
		  double Average = m_Total / m_Count;
		  double Variation = 
		      m_TotalSquared / m_Count - Average * Average;
		  return sqrt(Variation);
	  }

	  void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
	  {
		  //str_cpy(Label, LabelSize, "Standard Deviation");
		  //str_cpy(Units, UnitsSize, ""                  );
		  LabelSize; UnitsSize;
		  strcpy(Label, "Standard Deviation");
		  strcpy(Units, ""                  );
	  }

private:
	int    m_Count;
    double m_Total;
	double m_TotalSquared;
}; 

// Compute R.M.S.
//class CRMSClusterCalculator
//{
//public:
//    CRMSClusterCalculator() : m_TotalSquared(0), m_Count(0)
//    {
//    }
//    void Add(double Value)
//    {
//        ++m_Count;
//        m_TotalSquared += Value * Value;
//    }
//    void Clear()
//    {
//        m_Count = 0;
//        m_TotalSquared = 0;
//    }
//    //R.M.S =    
//    double Get()
//    {
//        if (m_Count == 0)
//            return DBL_MAX;
//
//        // Do we cash this value?
//        double AverageSquare = m_TotalSquared / m_Count;
//        return sqrt(AverageSquare);
//    }
//
//    void GetLabelAndUnits(TCHAR* Label, UINT LabelSize, TCHAR* Units, UINT UnitsSize)
//    {
//        str_cpy(Label, LabelSize, "R.M.S.");
//        str_cpy(Units, UnitsSize, ""      );
//    }
//
//private:
//    int    m_Count;
//    double m_TotalSquared;
//}; 

#endif // !defined(AFX_CLUSTERCALCULATOR_H__BAB0682A_4D3B_475A_BEA1_CC5A12B007D7__INCLUDED_)