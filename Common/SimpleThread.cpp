// CSimpleThread.cpp: implementation for the CSimpleThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SimpleThread.h"
#include <process.h>
#include "LogEvent.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimpleThread::CSimpleThread(const char* ThreadName): 
	m_ThreadHandle(NULL),
	m_ThreadIsActive(false),
	m_TimeoutInMilli(DEFAULT_TIMEOUT),
	m_ThreadName(ThreadName),
    m_ThreadId(0),
    m_Exit(false)
{
	m_CloseEvent.ResetEvent();
    m_InitiatedTimeoutEvent.ResetEvent();
}

CSimpleThread::~CSimpleThread()
{
	CloseThread(true);
}

bool CSimpleThread::StartThread()
{
    Assert(m_ThreadHandle == NULL);
    m_CloseEvent.ResetEvent();
    m_InitiatedTimeoutEvent.ResetEvent();
	m_ThreadHandle = CreateThread(NULL, 0, CSimpleThread::ThreadFunc, this, 0, &m_ThreadId);

    SetThreadName(m_ThreadId, m_ThreadName.c_str());

	LogEvent(m_ThreadHandle? LE_INFOHIGH : LE_ERROR, "CSimpleThread::StartThread() %s(Id: %d) %s", 
		m_ThreadName.c_str(), m_ThreadId, m_ThreadHandle? "Started Successfully" : "Failed to start");
	m_ThreadIsActive = m_ThreadHandle != NULL;
	return m_ThreadIsActive;
}

void CSimpleThread::CloseThread(bool ForceClosing, DWORD WaitForCloseMilli)
{
    if(!m_ThreadIsActive)
        return;

    LogEvent(LE_INFOHIGH, "CSimpleThread::CloseThread(%s)", m_ThreadName.c_str());

    SignalExit();
    Assert(GetCurrentThreadId() != m_ThreadId);
	m_CloseEvent.SetEvent();

    if(ForceClosing == true)
    {
        DWORD WaitResult = WaitForSingleObject(*this, WaitForCloseMilli);
        if(WaitResult != WAIT_OBJECT_0)
        {
            //Wait was not completed
            if(m_ThreadIsActive)
            {
                LogEvent(LE_ERROR, "CSimpleThread::CloseThread(%s): Terminate Thread by force",
                    m_ThreadName.c_str());
                if(::TerminateThread(m_ThreadHandle, 0))
                    m_ThreadIsActive = false;
                else
                    LogEvent(LE_WARNING, "CSimpleThread::CloseThread(%s): Terminate Thread Failed",
                    m_ThreadName.c_str());
            }
            else
                LogEvent(LE_INFO, "CSimpleThread::CloseThread() Thread %s was successfully closed", 
                m_ThreadName.c_str());
        }
    }

    if(!m_ThreadIsActive)
    {
        CloseHandle(m_ThreadHandle);
        m_ThreadHandle = NULL;
    }
}

void CSimpleThread::SetTimeout(DWORD MilliSeconds)
{
    bool NewTimeoutIsShorter = MilliSeconds < m_TimeoutInMilli;
	m_TimeoutInMilli = MilliSeconds;
    if (NewTimeoutIsShorter)
    { // when setting a shorter timeout, need to stop the Wait(with old timeout) in the loop function.
     // This to prevent a situation where the old timeout is 1000 milli and the
     // new timeout is 10 milli, but the client will have to wait maybe a 1000 milli
     // before the 10 milli timeout will be updated.
        m_InitiatedTimeoutEvent.SetEvent();
    }
}

bool CSimpleThread::SetThreadPriority(int Priority)
{
    LogEvent(LE_INFO, "SimpleThread(%s)::SetThreadPriority : priority = %d", 
        m_ThreadName.c_str(), Priority);
	if(!m_ThreadIsActive)
	{
		LogEvent(LE_ERROR, "SimpleThread(%s)::SetThreadPriority : attempt to set thread priority for inactive thread.",
            m_ThreadName.c_str());
		return false;
	}
	bool Success = (::SetThreadPriority(m_ThreadHandle, Priority) == TRUE);
	if(!Success)
		LogEvent(LE_ERROR, "SimpleThread(%s)::SetThreadPriority failed with error %s.",
			m_ThreadName.c_str(), GetWin32Error(GetLastError()).c_str());
	
	return Success;
}



bool CSimpleThread::SetThreadPriority(const char* Priority)
{
    int PriorityValue = ThreadPriorityStr2Value(Priority);
	return SetThreadPriority(PriorityValue);
}

bool CSimpleThread::SleepUntilClose(UINT MilliSeconds)
{
    DWORD Ret = WaitForSingleObject(m_CloseEvent, MilliSeconds);
    return Ret == WAIT_TIMEOUT;
}

void CSimpleThread::LoopFunction()
{
    // define the wait handles
    const int HandlesNum = 2;
    HANDLE WaitHandles[HandlesNum];
    WaitHandles[0] = m_CloseEvent;
    WaitHandles[1] = m_InitiatedTimeoutEvent;
    const int TerminateThread  = WAIT_OBJECT_0;     
    const int InitiatedTimeout = WAIT_OBJECT_0 + 1;  
    const int Timeout          = WAIT_TIMEOUT;
    m_Exit = false;

	while (!m_Exit)
	{
        int Result = WaitForMultipleObjects(HandlesNum, WaitHandles, FALSE, m_TimeoutInMilli);
        switch(Result)
        {
        case TerminateThread:
            return; // This will terminate the thread
        case InitiatedTimeout:
        case Timeout:
            OnTimeout();
            break;
        default:
            LogEvent(LE_ERROR, "CSimpleThread::LoopFunction: WaitForMultipleObjects FAILED %d, Exit thread %s",
                Result, m_ThreadName.c_str());
            return; //exit
        }
	}
}

void CSimpleThread::SignalExit()
{
    if (IsThreadActive())
        m_Exit = true;
}


DWORD CSimpleThread::ThreadFunc(void* Params)
{
	CSimpleThread* ThreadInstance =  (CSimpleThread*) Params;
    ThreadInstance->OnThreadStart();
	ThreadInstance->LoopFunction();
    ThreadInstance->HandleThreadClose();
    //::ExitThread(0);
	return 0;
}

void CSimpleThread::HandleThreadClose()
{
    LogEvent(LE_INFOLOW, "CSimpleThread(%s)::HandleThreadClose", m_ThreadName.c_str());
	OnThreadClose();
	m_ThreadIsActive = false;
   
    m_ThreadId = 0;
}

//////////////////////////////////////////////////////////////////////////////
#ifdef 	_TEST

class DummySimpleThread: public CSimpleThread
{
public:
	DummySimpleThread(): CSimpleThread("Dummy thread")
	{
	}
	
	void Init(const char* /* ThreadName */, int TimeoutMilli)
	{
		SetTimeout(TimeoutMilli);
	}
private:
	virtual void OnStart()
	{
		LogEvent(LE_INFOHIGH, "DummySimpleThread On Start");
	}

	virtual void OnTimeout()
	{
		LogEvent(LE_INFOHIGH, "DummySimpleThread On Timeout");
	}

	virtual void OnClose()
	{
		LogEvent(LE_INFOHIGH, "DummySimpleThread On Close");
	}
};

class SimpleThreadTester
{
public:
	void Init()
	{
		m_Thread.Init("xxx", 100);
	}

	void Start()
	{
		m_Thread.StartThread();
	}

	void Close()
	{
		m_Thread.CloseThread();
	}
	
private:
	DummySimpleThread m_Thread;
};

#include "Utils.h"
#include <vector>
void TestSimpleThread()
{
	SimpleThreadTester Tester;
	Tester.Init();
	Tester.Start();
	WaitAndPumpMessages(10000);
	Tester.Close();
	WaitAndPumpMessages(3000);


	enum { NUM_THREADS = 100};

	std::vector<SimpleThreadTester*> Testers;
	Testers.resize(NUM_THREADS);
	for(unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		Testers[i] = new SimpleThreadTester();
		Testers[i]->Init();
		WaitAndPumpMessages(20);
	}

	for(unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		Testers[i]->Start();
		WaitAndPumpMessages(20);
	}

	WaitAndPumpMessages(5000);
	for(unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		Testers[i]->Close();
		WaitAndPumpMessages(20);
	}

	WaitAndPumpMessages(3000);
	for(unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		delete Testers[i];
		WaitAndPumpMessages(20);
	}
}


#endif	_TEST


