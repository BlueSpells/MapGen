#pragma once

#include <map>
#include "TimeStamp.h"
#include "LogEvent.h"
#include "Common/CriticalSection.h"

template<typename PendingItem>
class CPendingList
{
public:
    CPendingList(): m_CurrentRequestId(0), m_LastGeneratedId(0)
    {
    }

    void ClearAll()
    {
        CCriticalSectionLocker Locker(m_PendingMapLock);
        m_CurrentRequestId = 0;
        m_LastGeneratedId = 0;
        m_Pending.clear();
    }

    bool AddRequest(const PendingItem& Item, const CTimePeriod& ItemTimeout, int& AllocatedRequestId)
    {
        CCriticalSectionLocker Locker(m_PendingMapLock);

        AllocatedRequestId = ++m_LastGeneratedId;
        PendingRequest Request = {Item, CTimeStamp::Now(), ItemTimeout};

        PendingMap::value_type Value(AllocatedRequestId, Request);
        std::pair<PendingIterator, bool> Result = m_Pending.insert(Value);
        if (!Result.second)
        {
            LogEvent(LE_ERROR, "CPendingList::AddRequest: error insert (%d)", AllocatedRequestId);
            return false;
        }
        return true;
    }

    // Return false if no such request
    bool FindRequest(int RequestId, PendingItem& Item, bool RemoveRequest = true, bool UpdateRequestTime = false)
    {
        CCriticalSectionLocker Locker(m_PendingMapLock);

        PendingIterator Iter = m_Pending.find(RequestId);
        if (Iter == m_Pending.end())
        {
            LogEvent(LE_ERROR, "CPendingList::FindRequest: (%d) not found", RequestId);
            return false;
        }

        PendingRequest& Request = Iter->second;
        Item = Request.Item;
        if(RemoveRequest)
            m_Pending.erase(Iter);
        else if(UpdateRequestTime)
            Request.RequestTime = CTimeStamp::Now();
        return true;
    }

    // Return false if no such request
    bool RemoveRequest(int RequestId)
    {
        PendingItem Item;
        return FindRequest(RequestId, Item);
    }

    // These are used to walk over list and retrieve outdated requests
    void StartOutdated()
    {
        CCriticalSectionLocker Locker(m_PendingMapLock);
        m_CurrentRequestId = 0;
    }

    bool NextOutdatedRequest(
        int& RequestId,
        PendingItem& Item)
    {
        CCriticalSectionLocker Locker(m_PendingMapLock);
        PendingIterator End = m_Pending.end();

        CTimeStamp CurrentTime = CTimeStamp::Now();
        for (;; ++m_CurrentRequestId)
        {
            PendingIterator Iter = m_Pending.lower_bound(m_CurrentRequestId);
            if (Iter == End)
                break;

            PendingRequest& Request = Iter->second;
            if (CurrentTime - Request.RequestTime > Request.RequestTimeout)
            {
                RequestId = Iter->first;
                return FindRequest(RequestId, Item);
            }
        }
        return false; // Not found
    }

private:
    struct PendingRequest
    {
        PendingItem Item;
        CTimeStamp  RequestTime;
        CTimePeriod RequestTimeout;
    };

    typedef std::map<int/*RequestId*/, PendingRequest> PendingMap;
    typedef typename PendingMap::iterator PendingIterator;
    PendingMap  m_Pending;
    int         m_CurrentRequestId;  // Used to iterate over all requests
    int         m_LastGeneratedId;
    CCriticalSection m_PendingMapLock;

};
