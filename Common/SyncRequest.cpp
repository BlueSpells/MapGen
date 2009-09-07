#include "StdAfx.h"
#include "SyncRequest.h"
#include "LogEvent.h"
#include "ThreadWithQueue.h"
#include "SmartPtr.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// Test 

class CSyncRequestTester: public CThreadWithQueue
{
public:
    CSyncRequestTester():
      CThreadWithQueue("CSyncRequestTester", 1000)
    {
    }

    void Test(bool SimulateNoReply, int Input)
    {
        char Context[100];
        sprintf_s(Context, "No Reply = %d, Input = %d", SimulateNoReply, Input);
        SmartSyncTestReply Reply(new SyncTestReply(Context));
        TestData Data;
        Data.Input = Input;
        Data.SyncReply = Reply;
        Data.SimulateNoReply = SimulateNoReply;
        DWORD RequestStartTime = GetTickCount();
        OutputData ReplyData;
        AddHandlerToQueue(&CSyncRequestTester::HandleTest, Data);
        bool ReplyReturned = Reply->WaitForReply(WAIT_TIME, ReplyData);

        Assert(ReplyReturned == !SimulateNoReply);
        if(ReplyReturned)
        {
            Assert(ReplyData.Output == SimulateFunc(Input));
            LogEvent(LE_INFO, "CSyncRequestTester::Test() Success Test %s, took %d milli", 
                Context, GetTickCount() - RequestStartTime);
        }
    }

private:
    enum { WAIT_TIME = 1000 };

    struct OutputData
    {
        int Output;
        std::string OutputStr;
    };

    typedef CSyncRequest<OutputData> SyncTestReply;
    typedef SmartPtr<SyncTestReply> SmartSyncTestReply;

    struct TestData
    {
        bool SimulateNoReply;
        int Input;
        SmartSyncTestReply SyncReply;
    };

    // performed on the thread with queue
    void HandleTest(const TestData& Data)
    {
        if(Data.SimulateNoReply)
            Sleep(WAIT_TIME + 100);

        //simulate reply
        int ResultInt = SimulateFunc(Data.Input);
        char ResultStr[100];
        sprintf_s(ResultStr, " result: %d", ResultInt);
        OutputData Result = { ResultInt, ResultStr };

        Data.SyncReply->NotifyResult(Result);
    }

    int SimulateFunc(int Input)
    {
        enum { TEST_FACTOR = 100 };
        return Input * TEST_FACTOR;
    }
};

void TestSyncRequest()
{
    DWORD TestLength = 30 * 1000; //30 seconds
    CSyncRequestTester Tester;

    Assert(Tester.StartThread());

    DWORD Start = GetTickCount();

    while(GetTickCount() - Start  <  TestLength)
    {
        int Input = rand() % 1000;
        bool SimulateNoReply = (rand() % 10) == 0;  // 1/10 of the tests should simulate no reply
        Tester.Test(SimulateNoReply, Input);
        WaitAndPumpMessages(10);
    }

    Tester.CloseThread(true);
}