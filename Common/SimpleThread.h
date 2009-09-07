// SimpleThread.h: interface for the SimpleThread class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <afxmt.h>
#include <string>

class CSimpleThread
{
public:
	CSimpleThread(const char* Name);
	virtual ~CSimpleThread();

	bool StartThread();
	void CloseThread(bool ForceClosing = false, DWORD WaitForCloseMilli = DEFAULT_WAIT_FOR_THREAD_CLOSE);
	virtual void SetTimeout(DWORD MilliSeconds);
	bool SetThreadPriority(int Priority);
	bool SetThreadPriority(const char* Priority);
    virtual operator HANDLE() const { return (HANDLE) m_ThreadHandle; }
    DWORD GetThreadId() const { return m_ThreadId; }
    bool IsThreadActive() const { return m_ThreadIsActive; }
    void SignalExit();

    enum 
    { 
        DEFAULT_TIMEOUT = 1000,
        DEFAULT_WAIT_FOR_THREAD_CLOSE = 10000,
    };

private:
    // Hide copy ctor and assignment operator
    CSimpleThread(const CSimpleThread &);
    CSimpleThread & operator=(const CSimpleThread &);

protected:
	//to be implemented by derived classes
	virtual void OnThreadStart() {};
	virtual void OnThreadClose() {};
	virtual void OnTimeout() {};	//work should be performed in the OnTimeout method that
									//will be called every m_TimeoutInMilli
protected:
	virtual void LoopFunction(); //can be override
	void HandleThreadClose();
    bool SleepUntilClose(UINT MilliSeconds);
	HANDLE			m_ThreadHandle;
	CEvent			m_CloseEvent;
    CEvent          m_InitiatedTimeoutEvent; // when this event occur force a timeout.
    DWORD			m_TimeoutInMilli;
	bool			m_ThreadIsActive;
	std::string		m_ThreadName;
    DWORD           m_ThreadId;
    bool            m_Exit;

private:
	static DWORD WINAPI ThreadFunc(void* Params);

};

