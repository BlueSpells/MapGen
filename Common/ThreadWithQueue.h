// CThreadWithQueue.h: interface for the CThreadWithQueue class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SimpleThread.h"
#include "SafeQueue.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define THIS_FILE __FILE__
#endif


//////////////////////////////////////////////////////////////////////////
//ThreadWithRQueue

//////////////////////////////////////////////////////////////////////////
//Note - If you are going to use this class make sure you are inherit
// this class as the first inherited class.
// This is important, for instance, when casting the object(instance of the inherited class)
// so the compiler will see the correct virtual table.

class CThreadWithQueue: public CSimpleThread
{
public:
	CThreadWithQueue(const char* ThreadName, int QueueSize, bool LogAddRequestFailures = true);
	virtual ~CThreadWithQueue();

	int NumItemsInQueue();
	void SetQueueSize(int QueueSize);
    void CloseThread(bool ForceClosing = false); //override the SimpleThread implementation 
    void ClearQueue();

private:
    // Hide copy ctor and assignment operator
    CThreadWithQueue(const CThreadWithQueue &);
    CThreadWithQueue & operator=(const CThreadWithQueue &);

protected:
	//to be override by derived classes
	virtual void OnTimeout();
	virtual void OnThreadStart();
	virtual void OnThreadClose();

    class IExternalHandler
    {
    public:
        virtual ~IExternalHandler() {}
        virtual void Handle() = 0;
    };

    bool AddToQueue(class IExternalHandler* Handler);

#if 0
    template <class DataClass> 
    class TemplatedExternalHandler : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(const DataClass& Data);

        TemplatedExternalHandler(CThreadWithQueue* Thread, DataHandler Handler, const DataClass& Data) :
            m_Data(Data),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data);
        }
    private:
        DataClass			m_Data;
        CThreadWithQueue*	m_Thread;
        DataHandler			m_Handler;
    };

    class ExternalHandlerWithNoData : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)();
        ExternalHandlerWithNoData(CThreadWithQueue* Thread, DataHandler Handler) :
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))();
        }
    private:
        CThreadWithQueue*	m_Thread;
        DataHandler			m_Handler;
    };

#define ADD_HANDLER_TO_QUEUE(HandleFunc, DataClass, Data) \
    TemplatedExternalHandler<DataClass>* ExternalHandler\
        = new TemplatedExternalHandler<DataClass>(\
            this, \
            (TemplatedExternalHandler<DataClass>::DataHandler)HandleFunc,\
            Data); \
	AddToQueue(ExternalHandler)


#define ADD_HANDLER_TO_QUEUE_WITH_RETURN(HandleFunc, DataClass, Data, RetVal) \
    TemplatedExternalHandler<DataClass>* ExternalHandler\
        = new TemplatedExternalHandler<DataClass>(\
            this, \
            (TemplatedExternalHandler<DataClass>::DataHandler)HandleFunc,\
            Data); \
	RetVal = AddToQueue(ExternalHandler)

#define ADD_HANDLER_WITH_NO_DATA_TO_QUEUE(HandleFunc) \
    ExternalHandlerWithNoData* ExternalHandler\
        = new ExternalHandlerWithNoData(\
            this, \
            (ExternalHandlerWithNoData::DataHandler)HandleFunc); \
			AddToQueue(ExternalHandler)

#define ADD_HANDLER_WITH_NO_DATA_TO_QUEUE_WITH_RETURN(HandleFunc, RetVal) \
    ExternalHandlerWithNoData* ExternalHandler\
        = new ExternalHandlerWithNoData(\
            this, \
            (ExternalHandlerWithNoData::DataHandler)HandleFunc); \
    RetVal = AddToQueue(ExternalHandler)

#else
    //////////////////////////////////////////////////////////////////////
    // No parameters

    class ExternalHandlerWithNoData : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)();
        ExternalHandlerWithNoData(CThreadWithQueue* Thread, DataHandler Handler) :
        m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))();
        }
    private:
        CThreadWithQueue* m_Thread;
        DataHandler m_Handler;
    };
    template<class Base> bool AddHandlerToQueue(void (Base::*InternalHandler)())
    {
        ExternalHandlerWithNoData* ExternalHandler = 
            new ExternalHandlerWithNoData(
            this, 
            (ExternalHandlerWithNoData::DataHandler)InternalHandler
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 1 Parameter

    template <class DataClass> class TemplatedExternalHandler : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(const DataClass& Data);

        TemplatedExternalHandler(CThreadWithQueue* Thread, DataHandler Handler, const DataClass& Data) :
        m_Data(Data),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data);
        }
    private:
        DataClass m_Data;
        CThreadWithQueue* m_Thread;
        DataHandler m_Handler;
    };

    template<class Base, class DataClass> bool AddHandlerToQueue(
        void (Base::*InternalHandler)(const DataClass& Data),
        const DataClass& Data)
    {
        TemplatedExternalHandler<DataClass>* ExternalHandler = 
            new TemplatedExternalHandler<DataClass>(
            this, 
            (TemplatedExternalHandler<DataClass>::DataHandler)InternalHandler,
            Data
            );

        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 2 Parameters

    template <class DataClass1, class DataClass2> 
    class TemplatedExternalHandler2 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2
            );

        TemplatedExternalHandler2(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2> 
    bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2
        ),
        const DataClass1& Data1,
        const DataClass2& Data2)
    {
        typedef TemplatedExternalHandler2<DataClass1, DataClass2> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 3 Parameters

    template <class DataClass1, class DataClass2, class DataClass3> 
    class TemplatedExternalHandler3 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3
            );

        TemplatedExternalHandler3(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Data3(Data3),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2, m_Data3);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        DataClass3   m_Data3;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2, class DataClass3> 
    bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2,
        const DataClass3& Data3
        ),
        const DataClass1& Data1,
        const DataClass2& Data2,
        const DataClass3& Data3)
    {
        typedef TemplatedExternalHandler3<DataClass1, DataClass2, DataClass3> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2, Data3
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 4 Parameters

    template <class DataClass1, class DataClass2, class DataClass3, class DataClass4> 
    class TemplatedExternalHandler4 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4
            );

        TemplatedExternalHandler4(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Data3(Data3),
            m_Data4(Data4),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2, m_Data3, m_Data4);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        DataClass3   m_Data3;
        DataClass4   m_Data4;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2, class DataClass3, class DataClass4> 
    bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4
        ),
        const DataClass1& Data1,
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4)
    {
        typedef TemplatedExternalHandler4<DataClass1, DataClass2, DataClass3, DataClass4> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2, Data3, Data4
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 5 Parameters

    template <class DataClass1, class DataClass2, class DataClass3, class DataClass4, class DataClass5> 
    class TemplatedExternalHandler5 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5
            );

        TemplatedExternalHandler5(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Data3(Data3),
            m_Data4(Data4),
            m_Data5(Data5),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2, m_Data3, m_Data4, m_Data5);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        DataClass3   m_Data3;
        DataClass4   m_Data4;
        DataClass5   m_Data5;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2, class DataClass3, class DataClass4, class DataClass5> 
    bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5
        ),
        const DataClass1& Data1,
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5)
    {
        typedef TemplatedExternalHandler5<DataClass1, DataClass2, DataClass3, DataClass4, DataClass5> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2, Data3, Data4, Data5
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 6 Parameters

    template <class DataClass1, class DataClass2, class DataClass3, 
    class DataClass4, class DataClass5, class DataClass6> 
    class TemplatedExternalHandler6 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5,
            const DataClass6& Data6
            );

        TemplatedExternalHandler6(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5,
            const DataClass6& Data6
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Data3(Data3),
            m_Data4(Data4),
            m_Data5(Data5),
            m_Data6(Data6),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2, m_Data3, m_Data4, m_Data5, m_Data6);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        DataClass3   m_Data3;
        DataClass4   m_Data4;
        DataClass5   m_Data5;
        DataClass6   m_Data6;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2, class DataClass3, 
    class DataClass4, class DataClass5, class DataClass6> 
        bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5,
        const DataClass6& Data6
        ),
        const DataClass1& Data1,
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5,
        const DataClass6& Data6)
    {
        typedef TemplatedExternalHandler6<DataClass1, DataClass2, DataClass3, 
            DataClass4, DataClass5, DataClass6> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2, Data3, Data4, Data5, Data6
            );
        return AddToQueue(ExternalHandler);
    }

    //////////////////////////////////////////////////////////////////////
    // 7 Parameters

    template <class DataClass1, class DataClass2, class DataClass3, class DataClass4, 
    class DataClass5, class DataClass6, class DataClass7> 
    class TemplatedExternalHandler7 : public IExternalHandler
    {
    public:
        typedef void (CThreadWithQueue::*DataHandler)(
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5,
            const DataClass6& Data6,
            const DataClass7& Data7
            );

        TemplatedExternalHandler7(
            CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const DataClass1& Data1,
            const DataClass2& Data2,
            const DataClass3& Data3,
            const DataClass4& Data4,
            const DataClass5& Data5,
            const DataClass6& Data6,
            const DataClass7& Data7
            ) :
        m_Data1(Data1),
            m_Data2(Data2),
            m_Data3(Data3),
            m_Data4(Data4),
            m_Data5(Data5),
            m_Data6(Data6),
            m_Data7(Data7),
            m_Thread(Thread),
            m_Handler(Handler)
        {
        }
        virtual void Handle()
        {
            (m_Thread->*(m_Handler))(m_Data1, m_Data2, m_Data3, m_Data4, m_Data5, m_Data6, m_Data7);
        }
    private:
        DataClass1   m_Data1;
        DataClass2   m_Data2;
        DataClass3   m_Data3;
        DataClass4   m_Data4;
        DataClass5   m_Data5;
        DataClass6   m_Data6;
        DataClass7   m_Data7;
        CThreadWithQueue* m_Thread;
        DataHandler  m_Handler;
    };

    template<class Base, class DataClass1, class DataClass2, class DataClass3, 
    class DataClass4, class DataClass5, class DataClass6, class DataClass7> 
        bool AddHandlerToQueue(
        void (Base::*InternalHandler)(
        const DataClass1& Data1, 
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5,
        const DataClass6& Data6,
        const DataClass7& Data7
        ),
        const DataClass1& Data1,
        const DataClass2& Data2,
        const DataClass3& Data3,
        const DataClass4& Data4,
        const DataClass5& Data5,
        const DataClass6& Data6,
        const DataClass7& Data7)
    {
        typedef TemplatedExternalHandler7<DataClass1, DataClass2, DataClass3, DataClass4, 
            DataClass5, DataClass6, DataClass7> ExternalDataHandler;
        ExternalDataHandler* ExternalHandler = new ExternalDataHandler(
            this, 
            (ExternalDataHandler::DataHandler)InternalHandler,
            Data1, Data2, Data3, Data4, Data5, Data6, Data7
            );
        return AddToQueue(ExternalHandler);
    }
#endif

protected:
	virtual void LoopFunction(); 
    void HandleInputMessage();
    void HandleTimeout();
    void LogThreadPerformance();


protected:
	SafeQueue<IExternalHandler*> m_Queue;
    DWORD			m_LastTicksOnTimeout;

    // The m_LogAddToQueueFailures flag will prevent adding logs 
    //the Trace thread should avoid printing LogEvent
    bool m_LogAddToQueueFailures; 

    //Thread Performance logging variables
    bool m_ThreadPerformanceInitialized;
    int m_ThreadPerformanceLogFrequency; //in seconds
    ELogSeverity m_ThreadPerformanceLogLevel;
    LONG volatile m_NumAddToQueueFailures;
    LONG volatile m_NumRequestsAdded;
    DWORD m_NumRequestsHandled;
    DWORD m_LastLogTime;
    enum { PERFORMANCE_LOG_FREQUENCY = 60 };
};

//////////////////////////////////////////////////////////////////////////
//CThreadWithRequestsQueue
#include "HandlerVerifier.h"

class CThreadRequest
{
public:
	CThreadRequest(const CHandlerVerifier& HandlerVerifier)
		: m_HandlerVerifier(HandlerVerifier) {}
    virtual ~CThreadRequest() {}
	virtual void HandleRequest() = 0;
	virtual bool IsCompleted() { return true; }
	virtual bool LockRequest() { return m_HandlerVerifier.Lock(); }
	virtual void UnlockRequest() { m_HandlerVerifier.Unlock(); }

    //helper that will be used to prevent extra locks in case of thread that send a request to itself
    bool HasSameHandler(const CHandlerVerifier& HandlerVerifier) { return  m_HandlerVerifier == HandlerVerifier; }
private:
	// The HandlerVerifier can be copied with no concern
	// since the request is not its owner
	CHandlerVerifier m_HandlerVerifier;
};

class IRequestManager
{
public:
	virtual bool AddRequest(CThreadRequest* Request, bool DeleteRequest = true) = 0;
};

class CThreadWithRequestsQueue: public CThreadWithQueue, public IRequestManager
{
public:
    CThreadWithRequestsQueue(const char* ThreadName, int QueueSize, bool LogAddRequestFailures = true);
	~CThreadWithRequestsQueue();
	virtual bool AddRequest(CThreadRequest* Request, bool DeleteRequest = true);
    
private:
	struct RequestData
	{
		CThreadRequest* Request;
		bool			DeleteRequest;
	};
    void HandleThreadRequest(const RequestData& Data);

    //specialization of TemplatedExternalHandler - used in order to delete
    //requests on thread completion
    typedef void (CThreadWithRequestsQueue::*DataHandler)(const RequestData& Data);
    class ThreadRequestHandler: public TemplatedExternalHandler<RequestData>
    {
    public:

        ThreadRequestHandler(CThreadWithQueue* Thread, 
            DataHandler Handler, 
            const RequestData& Data):
                TemplatedExternalHandler<RequestData>(Thread, Handler, Data),
                m_RequestData(Data),
                m_Handled(false)
        {
        }

        virtual void Handle()
        {
            TemplatedExternalHandler<RequestData>::Handle();
            m_Handled = true;
        }


        virtual ~ThreadRequestHandler()
        {
            if(!m_Handled && m_RequestData.DeleteRequest)
            {
                delete m_RequestData.Request;
                m_RequestData.Request = NULL;
            }
        }

	private:
        RequestData m_RequestData;
        bool m_Handled;
    };

protected:
	// Used for adding thread requests
	CHandlerVerifier m_HandlerVerifier;
private:
	// When implementing don't copy m_HandlerVerifier
	CThreadWithRequestsQueue(const CThreadWithRequestsQueue&);
	void operator=(const CThreadWithRequestsQueue&);
};

//////////////////////////////////////////////////////////////////////////
/// Templated Request adders

template<class Handler>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(),
				bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequestNoParams<Handler>(_Handler, HandlerVerifier, HandleMethod);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(const P1&),
				P1 _P1,
				bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest1Param<Handler, P1>
		(_Handler, HandlerVerifier, HandleMethod, _P1);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(const P1&, const P2&),
				P1 _P1,
				P2 _P2,
				bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest2Params<Handler, P1, P2>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(const P1&, const P2&, const P3&),
				P1 _P1,
                P2 _P2,
				P3 _P3,
                bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest3Params<Handler, P1, P2, P3>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&),
				P1 _P1,
				P2 _P2,
				P3 _P3,
				P4 _P4,
				bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest4Params<Handler, P1, P2, P3, P4>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
class P5>
    bool AddRequest(IRequestManager* Thread,
    Handler* _Handler,
    const CHandlerVerifier& HandlerVerifier,
    void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
    const P5&),
    P1 _P1,
    P2 _P2,
    P3 _P3,
    P4 _P4,
    P5 _P5,
    bool DeleteRequest = true)
{
    CThreadRequest* Request = new TemplatedRequest5Params<Handler, P1, P2, P3, P4, P5>
        (_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5);
    return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6>
	bool AddRequest(IRequestManager* Thread,
	Handler* _Handler,
	const CHandlerVerifier& HandlerVerifier,
	void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
	const P5&, const P6&),
	P1 _P1,
	P2 _P2,
	P3 _P3,
	P4 _P4,
	P5 _P5,
	P6 _P6,
	bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest6Params<Handler, P1, P2, P3, P4, P5, P6>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6, class P7>
	bool AddRequest(IRequestManager* Thread,
	Handler* _Handler,
	const CHandlerVerifier& HandlerVerifier,
	void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
	const P5&, const P6&, const P7&),
	P1 _P1,
	P2 _P2,
	P3 _P3,
	P4 _P4,
	P5 _P5,
	P6 _P6,
	P7 _P7,
	bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest7Params<Handler, P1, P2, P3, P4, P5, P6, P7>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
	class P5, class P6, class P7, class P8>
bool AddRequest(IRequestManager* Thread,
				Handler* _Handler,
				const CHandlerVerifier& HandlerVerifier,
				void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
					const P5&, const P6&, const P7&, const P8&),
				P1 _P1,
				P2 _P2,
				P3 _P3,
				P4 _P4,
				P5 _P5,
				P6 _P6,
				P7 _P7,
				P8 _P8,
				bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest8Params<Handler, P1, P2, P3, P4, P5, P6, P7, P8>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6, class P7, class P8, class P9>
	bool AddRequest(IRequestManager* Thread,
	Handler* _Handler,
	const CHandlerVerifier& HandlerVerifier,
	void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
	const P5&, const P6&, const P7&, const P8&, const P9&),
	P1 _P1,
	P2 _P2,
	P3 _P3,
	P4 _P4,
	P5 _P5,
	P6 _P6,
	P7 _P7,
	P8 _P8,
	P9 _P9,
	bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest9Params<Handler, P1, P2, P3, P4, P5, P6, P7, P8, P9>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9);
	return Thread->AddRequest(Request, DeleteRequest);
}


template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6, class P7, class P8, class P9, class P10>
    bool AddRequest(IRequestManager* Thread,
    Handler* _Handler,
    const CHandlerVerifier& HandlerVerifier,
    void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
    const P5&, const P6&, const P7&, const P8&, const P9&, const P10&),
    P1 _P1,
    P2 _P2,
    P3 _P3,
    P4 _P4,
    P5 _P5,
    P6 _P6,
    P7 _P7,
    P8 _P8,
    P9 _P9,
    P10 _P10,
    bool DeleteRequest = true)
{
    CThreadRequest* Request = new TemplatedRequest10Params<Handler, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>
        (_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9, _P10);
    return Thread->AddRequest(Request, DeleteRequest);
}


template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6, class P7, class P8, class P9, class P10, class P11>
	bool AddRequest(IRequestManager* Thread,
	Handler* _Handler,
	const CHandlerVerifier& HandlerVerifier,
	void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
	const P5&, const P6&, const P7&, const P8&, const P9&, const P10&,
	const P11&),
	P1 _P1,
	P2 _P2,
	P3 _P3,
	P4 _P4,
	P5 _P5,
	P6 _P6,
	P7 _P7,
	P8 _P8,
	P9 _P9,
	P10 _P10,
	P11 _P11,
	bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest11Params<Handler, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9, _P10, _P11);
	return Thread->AddRequest(Request, DeleteRequest);
}

template<class Handler, class P1, class P2, class P3, class P4,
class P5, class P6, class P7, class P8, class P9, class P10, class P11, class P12>
	bool AddRequest(IRequestManager* Thread,
	Handler* _Handler,
	const CHandlerVerifier& HandlerVerifier,
	void (Handler::*HandleMethod)(const P1&, const P2&, const P3&, const P4&,
	const P5&, const P6&, const P7&, const P8&, const P9&, const P10&,
	const P11&, const P12&),
	P1 _P1,
	P2 _P2,
	P3 _P3,
	P4 _P4,
	P5 _P5,
	P6 _P6,
	P7 _P7,
	P8 _P8,
	P9 _P9,
	P10 _P10,
	P11 _P11,
	P12 _P12,
	bool DeleteRequest = true)
{
	CThreadRequest* Request = new TemplatedRequest12Params<Handler, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>
		(_Handler, HandlerVerifier, HandleMethod, _P1, _P2, _P3, _P4, _P5, _P6, _P7, _P8, _P9, _P10, _P11, _P12);
	return Thread->AddRequest(Request, DeleteRequest);
}
//////////////////////////////////////////////////////////////////////////
///			Templated Request Handlers

template<class RequestHandler>
class TemplatedRequestNoParams : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)();
	TemplatedRequestNoParams(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
	{}
	virtual ~TemplatedRequestNoParams() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)();
	}
private:
	RequestHandler*	 m_RequestHandler;
	HandleMethod	 m_HandleMethod;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class RequestHandler, class P1Class>
class TemplatedRequest1Param : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&);
	TemplatedRequest1Param(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
	{}
	virtual ~TemplatedRequest1Param() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class RequestHandler, class P1Class, class P2Class>
class TemplatedRequest2Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&);
	TemplatedRequest2Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
	{}
	virtual ~TemplatedRequest2Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
#pragma warning (push)
#pragma warning (disable: 4121)
	P2Class m_P2;
#pragma warning (pop)
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class RequestHandler, class P1Class, class P2Class, class P3Class>
class TemplatedRequest3Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&);
	TemplatedRequest3Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
	{}
	virtual ~TemplatedRequest3Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class RequestHandler, class P1Class, class P2Class,
	class P3Class, class P4Class>
class TemplatedRequest4Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&);
	TemplatedRequest4Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
	{}
	virtual ~TemplatedRequest4Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class>
class TemplatedRequest5Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&, const P5Class&);
	TemplatedRequest5Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
	{}
	virtual ~TemplatedRequest5Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
};

template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class, class P6Class>
class TemplatedRequest6Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
		const P5Class&, const P6Class&);
	TemplatedRequest6Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
	{}
	virtual ~TemplatedRequest6Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5, m_P6);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
};

template<class RequestHandler, class P1Class, class P2Class,
	class P3Class, class P4Class, class P5Class, class P6Class,
		class P7Class>
class TemplatedRequest7Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
			const P5Class&, const P6Class&, const P7Class&);
	TemplatedRequest7Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6,
		P7Class P7)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
		, m_P7(P7)
	{}
	virtual ~TemplatedRequest7Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5, m_P6, m_P7);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
	P7Class m_P7;
};

template<class RequestHandler, class P1Class, class P2Class,
	class P3Class, class P4Class, class P5Class, class P6Class,
		class P7Class, class P8Class>
class TemplatedRequest8Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
			const P5Class&, const P6Class&, const P7Class&, const P8Class&);
	TemplatedRequest8Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6,
		P7Class P7,
		P8Class P8)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
		, m_P7(P7)
		, m_P8(P8)
	{}
	virtual ~TemplatedRequest8Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5,
					m_P6, m_P7, m_P8);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
	P7Class m_P7;
	P8Class m_P8;
};

template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class, class P6Class,
class P7Class, class P8Class, class P9Class>
class TemplatedRequest9Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
		const P5Class&, const P6Class&, const P7Class&,
		const P8Class&, const P9Class&);
	TemplatedRequest9Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6,
		P7Class P7,
		P8Class P8,
		P9Class P9)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
		, m_P7(P7)
		, m_P8(P8)
		, m_P9(P9)
	{}
	virtual ~TemplatedRequest9Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5,
					m_P6, m_P7, m_P8, m_P9);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
	P7Class m_P7;
	P8Class m_P8;
	P9Class m_P9;
};


template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class, class P6Class,
class P7Class, class P8Class, class P9Class, class P10Class>
class TemplatedRequest10Params : public CThreadRequest
{
public:
    typedef void (RequestHandler::*HandleMethod)(const P1Class&,
        const P2Class&, const P3Class&, const P4Class&,
        const P5Class&, const P6Class&, const P7Class&,
        const P8Class&, const P9Class&, const P10Class&);
    TemplatedRequest10Params(RequestHandler* Handler,
        const CHandlerVerifier& HandlerVerifier,
        HandleMethod Method,
        P1Class P1,
        P2Class P2,
        P3Class P3,
        P4Class P4,
        P5Class P5,
        P6Class P6,
        P7Class P7,
        P8Class P8,
        P9Class P9,
        P10Class P10)
        : CThreadRequest(HandlerVerifier)
        , m_RequestHandler(Handler)
        , m_HandleMethod(Method)
        , m_P1(P1)
        , m_P2(P2)
        , m_P3(P3)
        , m_P4(P4)
        , m_P5(P5)
        , m_P6(P6)
        , m_P7(P7)
        , m_P8(P8)
        , m_P9(P9)
        , m_P10(P10)
    {}
    virtual ~TemplatedRequest10Params() {}
    virtual void HandleRequest()
    {
        (m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5,
            m_P6, m_P7, m_P8, m_P9, m_P10);
    }
private:
    RequestHandler*  m_RequestHandler;
    HandleMethod	 m_HandleMethod;
    P1Class m_P1;
    P2Class m_P2;
    P3Class m_P3;
    P4Class m_P4;
    P5Class m_P5;
    P6Class m_P6;
    P7Class m_P7;
    P8Class m_P8;
    P9Class m_P9;
    P10Class m_P10;
};


template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class, class P6Class,
class P7Class, class P8Class, class P9Class, class P10Class,
class P11Class>
class TemplatedRequest11Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
		const P5Class&, const P6Class&, const P7Class&,
		const P8Class&, const P9Class&, const P10Class&,
		const P11Class&);
	TemplatedRequest11Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6,
		P7Class P7,
		P8Class P8,
		P9Class P9,
		P10Class P10,
		P11Class P11)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
		, m_P7(P7)
		, m_P8(P8)
		, m_P9(P9)
		, m_P10(P10)
		, m_P11(P11)
	{}
	virtual ~TemplatedRequest11Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5,
				m_P6, m_P7, m_P8, m_P9, m_P10, m_P11);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
	P7Class m_P7;
	P8Class m_P8;
	P9Class m_P9;
	P10Class m_P10;
	P11Class m_P11;
};

template<class RequestHandler, class P1Class, class P2Class,
class P3Class, class P4Class, class P5Class, class P6Class,
class P7Class, class P8Class, class P9Class, class P10Class,
class P11Class, class P12Class>
class TemplatedRequest12Params : public CThreadRequest
{
public:
	typedef void (RequestHandler::*HandleMethod)(const P1Class&,
		const P2Class&, const P3Class&, const P4Class&,
		const P5Class&, const P6Class&, const P7Class&,
		const P8Class&, const P9Class&, const P10Class&,
		const P11Class&, const P12Class&);
	TemplatedRequest12Params(RequestHandler* Handler,
		const CHandlerVerifier& HandlerVerifier,
		HandleMethod Method,
		P1Class P1,
		P2Class P2,
		P3Class P3,
		P4Class P4,
		P5Class P5,
		P6Class P6,
		P7Class P7,
		P8Class P8,
		P9Class P9,
		P10Class P10,
		P11Class P11,
		P12Class P12)
		: CThreadRequest(HandlerVerifier)
		, m_RequestHandler(Handler)
		, m_HandleMethod(Method)
		, m_P1(P1)
		, m_P2(P2)
		, m_P3(P3)
		, m_P4(P4)
		, m_P5(P5)
		, m_P6(P6)
		, m_P7(P7)
		, m_P8(P8)
		, m_P9(P9)
		, m_P10(P10)
		, m_P11(P11)
		, m_P12(P12)
	{}
	virtual ~TemplatedRequest12Params() {}
	virtual void HandleRequest()
	{
		(m_RequestHandler->*m_HandleMethod)(m_P1, m_P2, m_P3, m_P4, m_P5,
				m_P6, m_P7, m_P8, m_P9, m_P10, m_P11, m_P12);
	}
private:
	RequestHandler*  m_RequestHandler;
	HandleMethod	 m_HandleMethod;
	P1Class m_P1;
	P2Class m_P2;
	P3Class m_P3;
	P4Class m_P4;
	P5Class m_P5;
	P6Class m_P6;
	P7Class m_P7;
	P8Class m_P8;
	P9Class m_P9;
	P10Class m_P10;
	P11Class m_P11;
	P12Class m_P12;
};


#ifdef _DEBUG
#define THIS_FILE __FILE__
#undef new
#endif
