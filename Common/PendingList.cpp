#include "stdafx.h"
#include "PendingList.h"
#include "LogEvent.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _TEST

static inline int Random(int Max)
{
    return (rand() % 11335577) % Max;
}

struct TestPendingItem
{
    int OriginalRequestId;
    int ExtraData;
};

class CPendingListTester
{
public:
    CPendingListTester() : m_TimeoutInSeconds(0), m_CurrentRequestId(0)
    {
    }

    void SetTimeout(time_t TimeoutInSeconds)
    {
        m_TimeoutInSeconds = TimeoutInSeconds;
    }

    inline int ExtraDataFromOriginalRequestid(int OriginalRequestId)
    {
        return OriginalRequestId % 3 == 0;
    }


    bool AddRequest(int OriginalRequestId, int& RequestId)
    {
        int ExtraData = ExtraDataFromOriginalRequestid(OriginalRequestId);
        TestPendingItem Item = {OriginalRequestId, ExtraData};
        if (!m_PendingList.AddRequest( Item, CTimePeriod(m_TimeoutInSeconds, 0), RequestId))
            return false;

        AddRequest2(RequestId, OriginalRequestId);
        return true;
    }

    bool CheckData( int OriginalRequestId, int ActualExtraData)
    {
        int ExtraData = ExtraDataFromOriginalRequestid(OriginalRequestId);
        Assert(ExtraData == ActualExtraData);
        return ExtraData == ActualExtraData;
    }

    bool FindRequest(int RequestId, int& OriginalRequestId)
    {
        TestPendingItem Item;
        bool Result = m_PendingList.FindRequest(RequestId, Item);
        OriginalRequestId = Item.OriginalRequestId;
        int OriginalExtraData = Item.ExtraData;

        int OriginalRequestId2;
        bool Result2 = FindRequest2(RequestId, OriginalRequestId2);
        Assert(Result == Result2);
        if (!Result)
            return false;

        Assert(OriginalRequestId2 == OriginalRequestId);
        return CheckData(OriginalRequestId, OriginalExtraData);
    }

    void StartOutdated()
    {
        m_PendingList.StartOutdated();
    }

    bool NextOutdatedRequest(int& OriginalRequestId)
    {
        TestPendingItem Item;
        int RequestId;

        if (!m_PendingList.NextOutdatedRequest(RequestId, Item ))
            return false;
        OriginalRequestId = Item.OriginalRequestId;

        int OriginalRequestId2;
        FindRequest2(RequestId, OriginalRequestId2);
        Assert(OriginalRequestId2 == OriginalRequestId);
        return true;
    }

    bool CheckListForOutdatedRequest()
    {
        RequestIterator Iter = m_Requests.begin();
        RequestIterator End = m_Requests.end();

        time_t CurrentTime = time(NULL);
        for (; Iter != End; ++Iter)
        {
            Request& CurrentRequest = *Iter;
            time_t TimeoutWithSpare = m_TimeoutInSeconds + 1;
            if (CurrentTime - CurrentRequest.RequestTime > TimeoutWithSpare)
                return false;
        }
        return true;
    }

    int GetRandomRequestId()
    {
        int Count = m_Requests.size();
        if (Count == 0)
            return 0;

        int Index = Random(Count);
        return m_Requests[Index].RequestId;
    }

    int GetCount() { return m_Requests.size(); }
private:
    struct Request
    {
        int RequestId;
        int OriginalRequestId;
        time_t RequestTime;
    };
    typedef std::vector<Request> RequestVector;
    typedef RequestVector::iterator RequestIterator;

    void AddRequest2(int RequestId, int OriginalRequestId)
    {
        Request NewRequest = { RequestId, OriginalRequestId, time(NULL) };

        m_Requests.push_back(NewRequest);
    }

    bool FindRequest2(int RequestId, int& OriginalRequestId)
    {
        RequestIterator Iter = m_Requests.begin();
        RequestIterator End = m_Requests.end();

        for (; Iter != End; ++Iter)
        {
            Request& CurrentRequest = *Iter;
            if (CurrentRequest.RequestId == RequestId)
            {
                OriginalRequestId = CurrentRequest.OriginalRequestId;
                m_Requests.erase(Iter);
                return true;
            }
        }
        return false;
    }


    CPendingList<TestPendingItem> m_PendingList;
    RequestVector m_Requests;
    time_t           m_TimeoutInSeconds;
    int           m_CurrentRequestId;
};

#include "Utils.h"
static void AutomaticTest()
{
    const time_t Timeout = 10;
    CPendingListTester Tester;

    Tester.SetTimeout(Timeout/*Seconds*/);

    // Check that:
    // (1) Does not receive outdated non-existant
    // (2) Find whatever is there
    // (3) Does not find whatever is not there
    // (4) Does not leave outdated
    const int Count = 1000000;
    for (int i = 0; i < Count; ++i)
    {
        //Splash messages to the screen every 'Count / 1000' iterations
        if(i % (Count / 1000) == 0)
            WaitAndPumpMessages(10);

        enum
        {
            ADD_REQUEST = 0,
            FIND_REQUEST = 1,
            MAX_OPERATION = 2
        };
        int Operation = Random(MAX_OPERATION);
        switch (Operation)
        {
        case ADD_REQUEST:
            {
                int RequestId, OriginalRequestId = Random(1000);

                Assert(Tester.AddRequest(OriginalRequestId, RequestId));
                break;
            }
        case FIND_REQUEST:
            {
                int RequestId = Tester.GetRandomRequestId();
                bool RandomRequestId = Random(1000) == 999;
                if (RandomRequestId) // from time to time take a random request id
                    RequestId = Random(32000);

                int OriginalRequestId;
                bool Result = Tester.FindRequest(RequestId, OriginalRequestId);
                Assert(Result || RandomRequestId || RequestId == 0);
                break;
            }
        default:
            Assert(false);
            break;
        }

        // Clear outdated and test
        if (i % (Count / 100) == 0)
        {
            LogEvent(LE_INFOHIGH, "Tester: #%d/%d [%03d], Size is %d", 
                i, Count, i * 100 / Count, Tester.GetCount());
            Tester.StartOutdated();
            for (;;)
            {
                int OriginalRequestId;
                if (!Tester.NextOutdatedRequest(OriginalRequestId))
                    break;
            }

            Assert(Tester.CheckListForOutdatedRequest());
        }
    }
}


static void FunctionalTest()
{
    CPendingListTester Tester;

    int OriginalRequestId; 
    Assert(!Tester.FindRequest(1, OriginalRequestId));

    int RequestId1, RequestId2;
    Assert(Tester.AddRequest(1, RequestId1));
    Assert(Tester.AddRequest(2, RequestId2));

    Assert(Tester.FindRequest(RequestId1, OriginalRequestId) && OriginalRequestId == 1);
    Assert(!Tester.FindRequest(RequestId1, OriginalRequestId));

    Assert(Tester.FindRequest(RequestId2, OriginalRequestId) && OriginalRequestId == 2);
    WaitAndPumpMessages(1000);

    // Test timeout
    Tester.SetTimeout(2/*Seconds*/);

    Assert(Tester.AddRequest(1, RequestId1));
    WaitAndPumpMessages(3000);
    Assert(Tester.AddRequest(2, RequestId2));

    Tester.StartOutdated();
    Assert(Tester.NextOutdatedRequest(OriginalRequestId) && OriginalRequestId == 1);
    Assert(!Tester.NextOutdatedRequest(OriginalRequestId));

    WaitAndPumpMessages(3000);
    Tester.StartOutdated();
    Assert(Tester.NextOutdatedRequest(OriginalRequestId) && OriginalRequestId == 2);
}

void TestPendingList()
{
    FunctionalTest();
    AutomaticTest();
}

#endif
