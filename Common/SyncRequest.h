#pragma once
#include "afxmt.h"
#include <string>
#include "LogEvent.h"
//#include "DalCore/CommonStructs.h"

//////////////////////////////////////////////////////////////////////////
// Helper class that provides synchronization for requests
// that are being performed on another thread  

template<typename RetValType>
class CSyncRequest
{
public:
    CSyncRequest(const char* ContextStr): m_ContextStr(ContextStr)
    {
    }

    void NotifyResult(const RetValType& ReturnValue)
    {
        m_Result = ReturnValue;
        m_Event.SetEvent();
    }

    bool WaitForReply(DWORD NumMilli, RetValType& Result)
    {
        LogEvent(LE_INFOLOW, "CSyncRequest(%s)::WaitForReply, Start Wait",
            m_ContextStr.c_str());
        if(WaitForSingleObject(m_Event, NumMilli) == WAIT_OBJECT_0)
        {
            Result = m_Result;
            return true;
        }
        LogEvent(LE_ERROR, "CSyncRequest(%s)::WaitForReply, Failed to receive reply",
            m_ContextStr.c_str());
        return false;
    }

protected:
    CEvent m_Event;
    RetValType m_Result;
    std::string m_ContextStr;
};



