#include "stdafx.h"
#include "SmartPtr.h"
#include "LogEvent.h"
#include <string>
#include "Common/ThreadWithQueue.h"
#include "Common/Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//testing of the smart ptr template

class CDummy
{
public:
    CDummy(const char* Name): m_Name(Name)
    {
        LogEvent(LE_INFO, "CDummy[%s] CTor",
            m_Name.c_str());
    }

    ~CDummy()
    {
        LogEvent(LE_INFO, "CDummy[%s] DTor",
            m_Name.c_str());
    }

    std::string GetName() const
    {
        return m_Name;
    }
 
private:
    std::string m_Name;
};

typedef SmartPtr<CDummy> SmartDummy;
SmartDummy CreateDummy(const char* Name)
{
    SmartDummy Dummy(new CDummy(Name));
    Assert(Dummy.GetRefCount() == 1);
    return Dummy;
}

class CSmartPtrCreator: public CThreadWithQueue
{
public:
    CSmartPtrCreator(): 
      CThreadWithQueue("SmartPtrCreator", 10000),
      m_Counter(0)
    {
    }

    ~CSmartPtrCreator()
    {
        Close();
    }

    bool Init()
    {
        SetTimeout(100);
        return StartThread();
    }

    void Close()
    {
        CloseThread(true);
    }

    void DoSomethingWithSmartPtr()
    {
        char DummyName[20];
        sprintf_s(DummyName, "%d", m_Counter++);
        SmartDummy Dummy = CreateDummy(DummyName);
        AddHandlerToQueue(&CSmartPtrCreator::HandleSmartDummy, Dummy);

    }

    void DoSomethingWithSmartPtr(const SmartDummy& Dummy)
    {
        AddHandlerToQueue(&CSmartPtrCreator::HandleSmartDummy, Dummy);
    }

private:

    void HandleSmartDummy(const SmartDummy& Dummy)
    {
        LogEvent(LE_INFOLOW, "HandleSmartDummy(), Dummy %s",
            Dummy->GetName().c_str());
    }

private:
    int m_Counter;
};


void TestSmartPtr()
{
    SmartPtr<CDummy> SmartDummy1 = CreateDummy("First");
    Assert(SmartDummy1.GetRefCount() ==  1);

    SmartPtr<CDummy> SmartDummy2;
    Assert(SmartDummy2.IsNull());
    Assert(SmartDummy2.GetRefCount() ==  0);
    
    SmartDummy2 = SmartDummy1;
    Assert(SmartDummy1.GetRefCount() ==  2);
    Assert(SmartDummy2.GetRefCount() ==  2);
    Assert(SmartDummy1->GetName() == SmartDummy2->GetName());
    Assert(SmartDummy1->GetName() == std::string("First"));

    SmartDummy1 = CreateDummy("Second");
    Assert(SmartDummy1.GetRefCount() ==  1);
    Assert(SmartDummy2.GetRefCount() ==  1);
    Assert(SmartDummy1->GetName() != SmartDummy2->GetName());

    //First should be deleted
    SmartDummy2 = CreateDummy("Third");
    Assert(SmartDummy1.GetRefCount() ==  1);
    Assert(SmartDummy2.GetRefCount() ==  1);
    Assert(SmartDummy1->GetName() != SmartDummy2->GetName());

    //Second should be deleted
    SmartDummy1 = SmartDummy2;
    Assert(SmartDummy1.GetRefCount() ==  2);
    Assert(SmartDummy2.GetRefCount() ==  2);
    Assert(SmartDummy1->GetName() == SmartDummy2->GetName());
    Assert(SmartDummy1->GetName() == std::string("Third"));

    //checking assignment to NULL
    SmartDummy1 = NULL;
    Assert(SmartDummy1.IsNull());
    Assert(SmartDummy1.GetRefCount() ==  0);

    CSmartPtrCreator Creator;
    Assert(Creator.Init());
    for(int i = 0; i < 1000; i++)
    {
        Creator.DoSomethingWithSmartPtr();
    }
    WaitAndPumpMessages(5000);

    for(int i = 0; i < 1000; i++)
    {
        char DummyName[20];
        sprintf_s(DummyName, "%d", i);
        Creator.DoSomethingWithSmartPtr(CreateDummy(DummyName));
        WaitAndPumpMessages(0);
    }
    WaitAndPumpMessages(5000);

    Creator.Close();

    LogEvent(LE_INFOHIGH, "TestSmartPtr Exit, Expect the First and The Second to be destructed");
}



