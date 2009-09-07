#pragma once

/************************************************************************/
/* A singleton class that generates consecutive integers. It can be shared
	between different parts of the DAL that are required to access other
	DAL classes with a unique request ID. Originally, the purpose of this
	class was to share a unique request ID between the DalAPI messages
	and the ExpImpMgr request IDs.										*/
/************************************************************************/
class CRequestIdGenerator
{
public:
	static CRequestIdGenerator& GetInstance();
	UINT GetNextId();
    CRequestIdGenerator(UINT MaxValue = UINT_MAX);
private:
	CRequestIdGenerator(const CRequestIdGenerator&);
private:
	UINT m_NextId;
	CCriticalSection m_Lock;
    UINT m_MaxValue;
};