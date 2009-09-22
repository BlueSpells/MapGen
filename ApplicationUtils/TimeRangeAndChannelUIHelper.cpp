#include "stdafx.h"
#include "TimeRangeAndChannelUiHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTimeRangeAndChannelUIHelper::CTimeRangeAndChannelUIHelper(CDateTimeCtrl& FromTimeCtrl,
        CDateTimeCtrl& ToTimeCtrl, CSmartComboBox& ChannelsCombo,
        UINT& FromMilli, UINT& ToMilli):
        m_ChannelsCombo(ChannelsCombo), 
        m_FromTimeCtrl(FromTimeCtrl),
        m_ToTimeCtrl(ToTimeCtrl),
        m_FromMilli(FromMilli), m_ToMilli(ToMilli)
{
}

void CTimeRangeAndChannelUIHelper::Init(const std :: vector<CChannelId>& Channels)
{
    m_Channels = Channels;
    m_FromTimeCtrl.SetFormat("dd-MM-yyyy HH:mm:ss");
    m_ToTimeCtrl.SetFormat("dd-MM-yyyy HH:mm:ss");

    for(unsigned int i = 0; i < m_Channels.size(); ++i)
        m_ChannelsCombo.AddItem(i, m_Channels[i].ToDisplayableString().c_str());

    m_ChannelsCombo.SetValue(0);
    CTime FromTime = CTime::GetCurrentTime() - CTimeSpan(0, 0, 5, 0);
    CTime ToTime = CTime::GetCurrentTime();
    m_FromTimeCtrl.SetTime(&FromTime);
    m_ToTimeCtrl.SetTime(&ToTime);
    m_FromMilli = 0;
    m_ToMilli = 0;
}

bool CTimeRangeAndChannelUIHelper::IsValid() const
{
    CTime FromTime;
    CTime ToTime;
    m_FromTimeCtrl.GetTime(FromTime);
    m_ToTimeCtrl.GetTime(ToTime);
    if((ToTime < FromTime) || 
        ((ToTime == FromTime) && (m_ToMilli <= m_FromMilli)))
    {
        AfxMessageBox( "From Time is same or after To Time");
        m_FromTimeCtrl.SetFocus();
        return false;
    }

    int SelectedItem = m_ChannelsCombo.GetValue();
    if(SelectedItem < 0 || (unsigned int)SelectedItem >= m_Channels.size())
    {
        AfxMessageBox( "Invalid channel");
        m_ChannelsCombo.SetFocus();
        return false;
    }
    return true;
}

CTimeStamp CTimeRangeAndChannelUIHelper::GetFromTime() const
{
    return ConvertTimeToTimeStamp(m_FromTimeCtrl, m_FromMilli);
}

CTimeStamp CTimeRangeAndChannelUIHelper::GetToTime()const
{
    return ConvertTimeToTimeStamp(m_ToTimeCtrl, m_ToMilli);
}

CChannelId CTimeRangeAndChannelUIHelper::GetChannel() const
{
    int SelectedItem = m_ChannelsCombo.GetValue();
    if(SelectedItem >= 0 && (unsigned int)SelectedItem < m_Channels.size())
    {
        return CChannelId(m_Channels[SelectedItem]);
    }
    return CChannelId(-1,-1);
}

CTimeStamp CTimeRangeAndChannelUIHelper::ConvertTimeToTimeStamp(CDateTimeCtrl& TimeCtrl,UINT Milli) const
{
    CTime Time;
    TimeCtrl.GetTime(Time);
    return CTimeStamp((time_t)Time.GetTime(), Milli);
}

