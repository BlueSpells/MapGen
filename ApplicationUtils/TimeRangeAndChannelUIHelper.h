#pragma once

#include "SmartComboBox.h"
#include "Common/TimeStamp.h"
//#include "Common/ChannelId.h"
#include <afxdtctl.h>
#include <vector>

class CTimeRangeAndChannelUIHelper
{
public:
    CTimeRangeAndChannelUIHelper(CDateTimeCtrl& FromTimeCtrl,
        CDateTimeCtrl& ToTimeCtrl, CSmartComboBox& ChannelsCombo,
        UINT& FromMilli, UINT& ToMilli);

    void Init(const std :: vector<CChannelId>& Channels);
    bool IsValid() const;
    CTimeStamp GetFromTime() const;
    CTimeStamp GetToTime()const;
    CChannelId GetChannel() const;

private:
    CTimeStamp ConvertTimeToTimeStamp(CDateTimeCtrl& TimeCtrl,UINT Milli) const;

private:
    CDateTimeCtrl& m_FromTimeCtrl;
    CDateTimeCtrl& m_ToTimeCtrl;
    CSmartComboBox& m_ChannelsCombo;
    std :: vector<CChannelId> m_Channels;
    UINT&	m_FromMilli;
    UINT&	m_ToMilli;
};
