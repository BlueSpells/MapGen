#include "StdAfx.h"
#include "HandlerVerifier.h"
#include "CriticalSection.h"
#include "CollectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHandlerVerifier::CHandlerVerifier()
{
	m_HandlerState = new HandlerState;
	m_HandlerState->IsValid = true;
	m_HandlerState->NumReferences = 1;
	m_IsOwner = true;
	m_Attached = true;
}

CHandlerVerifier::~CHandlerVerifier()
{
	Detach();
}

CHandlerVerifier::CHandlerVerifier(const CHandlerVerifier& HandlerVerifier)
{
	Attach(HandlerVerifier);
}

void CHandlerVerifier::Invalidate()
{
	Detach();
}

void CHandlerVerifier::operator=(const CHandlerVerifier& HandlerVerifier)
{
	Detach();
	Attach(HandlerVerifier);
}

bool CHandlerVerifier::operator==(const CHandlerVerifier& OtherHandlerVerifier)
{
    if(m_HandlerState == NULL)
        return false;
    return m_HandlerState == OtherHandlerVerifier.m_HandlerState;
}


void CHandlerVerifier::Attach(const CHandlerVerifier& HandlerVerifier)
{
	InterlockedIncrement(&HandlerVerifier.m_HandlerState->NumReferences);
	m_HandlerState = HandlerVerifier.m_HandlerState;
	m_IsOwner = false;
	m_Attached = true;
}

void CHandlerVerifier::Detach()
{
	if(m_HandlerState == NULL || !m_Attached)
		return;

	Lock();
	m_Attached = false;
	if(m_IsOwner)
		m_HandlerState->IsValid = false;
	Unlock();
	if(0 == InterlockedDecrement(&m_HandlerState->NumReferences))
	{
		delete m_HandlerState;
		m_HandlerState = NULL;
	}
}

bool CHandlerVerifier::Lock()
{
	m_HandlerState->Lock.Lock();
	return m_HandlerState->IsValid;
}

void CHandlerVerifier::Unlock()
{
	m_HandlerState->Lock.Unlock();
}

#ifdef _DEBUG
#undef new
#endif //_DEBUG

#ifdef _TEST
#include <map>
#include "LogEvent.h"
#include "Utils.h"
#include "ThreadWithQueue.h"
class CHandlerVerifierTester
{
public:
	CHandlerVerifierTester() : m_Thread("HandlerVerifierTester", 10000)
	{
		m_Thread.StartThread();
	}
	void Test()
	{
		while(true)
		{
			int Operation = rand() % 10;
			switch(Operation)
			{
			// 10% of the time we create new objects
			case 0:
				GenerateNewObject();
				break;
			// 20% of the time we delete existing objects
			case 1:
			case 2:
				DeleteExistingObject();
				break;
			// 70% of the time we generate requests
			default:
				GenerateNewRequest();
			}
			
			// Don't make the GUI vanish
//			Sleep(10);
		}
	}
private:
	void GenerateNewObject()
	{
		CCriticalSectionLocker Locker(m_Lock);
		Assert(m_Verifiers.size() == m_VerifiersID.size());
		int index = rand();
		int VerifierID = rand();
		CHandlerVerifier* Verifier = new CHandlerVerifier;
		if( ! InsertValueToMap(m_Verifiers, index, Verifier))
		{
			delete Verifier;
			return;
		}
		InsertValueToMap(m_VerifiersID, index, VerifierID);		
		printf("Generated a verifier. ID = %d. Index = %d.\n", VerifierID, index);
	}
	void DeleteExistingObject()
	{
		m_Lock.Lock();
		if(m_Verifiers.empty())
		{
			m_Lock.Unlock();
			return;
		}
		Assert(m_Verifiers.size() == m_VerifiersID.size());		
		int SizeIndex = rand() % m_VerifiersID.size();
		std::map<int, int>::iterator			   IDIter		= m_VerifiersID.begin();
		std::map<int, CHandlerVerifier*>::iterator VerifierIter = m_Verifiers.begin();
		for(int i = 0; i < SizeIndex; ++i)
		{
			++IDIter;
			++VerifierIter;
		}
		Assert(IDIter->first == VerifierIter->first);
		int index					= IDIter->first;
		int VerifierID				= IDIter->second;
		CHandlerVerifier* Verifier  = VerifierIter->second;
		m_Lock.Unlock();
		// the delete operation Locks() the verifier
		delete Verifier;
		m_Lock.Lock();
		m_VerifiersID.erase(IDIter);
		m_Verifiers.erase(VerifierIter);
		m_Lock.Unlock();		

		printf("Deleted	  a verifier. ID = %d. Index = %d.\n", VerifierID, index);
	}
	void GenerateNewRequest()
	{
		m_Lock.Lock();
		if(m_Verifiers.empty())
		{
			m_Lock.Unlock();
			return;
		}
		Assert(m_Verifiers.size() == m_VerifiersID.size());		
		int SizeIndex = rand() % m_VerifiersID.size();
		std::map<int, int>::iterator			   IDIter		= m_VerifiersID.begin();
		std::map<int, CHandlerVerifier*>::iterator VerifierIter = m_Verifiers.begin();
		for(int i = 0; i < SizeIndex; ++i)
		{
			++IDIter;
			++VerifierIter;
		}
		Assert(IDIter->first == VerifierIter->first);
		int index					= IDIter->first;
		int VerifierID				= IDIter->second;
		CHandlerVerifier* Verifier  = VerifierIter->second;
		m_Lock.Unlock();

		// AddRequest Lock() the verifier
        AddRequest(&m_Thread, this, *Verifier, &CHandlerVerifierTester::HandleRequest, index, VerifierID);
		printf("Added a request. ID = %d. Index = %d.\n", VerifierID, index);
	}

	void HandleRequest(const int& VerifierIndex, const int& VerifierID)
	{
		CCriticalSectionLocker Locker(m_Lock);
		int ExpectedVerifierID = m_VerifiersID[VerifierIndex];
        Assert(ExpectedVerifierID == VerifierID);
		printf("Handled a req .  verifier ID = %d. Expected ID = %d. Verifier index = %d.\n", VerifierID, ExpectedVerifierID, VerifierIndex);
	}
private:
	// It must be pointer since only the owner can invalidate the verifier
	CCriticalSection				 m_Lock;
	std::map<int, CHandlerVerifier*> m_Verifiers;
	std::map<int, int>				 m_VerifiersID;
	CThreadWithRequestsQueue		 m_Thread;
};

void TestHandlerVerifier()
{
	CHandlerVerifierTester Tester;
	Tester.Test();
}
#endif