#include "stdafx.h"
#include "RequestIdGenerator.h"
#include "Common/CriticalSection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CRequestIdGenerator& CRequestIdGenerator::GetInstance()
{
	static CRequestIdGenerator TheSingleInstance;
	return TheSingleInstance;
}

UINT CRequestIdGenerator::GetNextId()
{
	CCriticalSectionLocker Lock(m_Lock);
    if(m_NextId == 0 || m_NextId > m_MaxValue)
        m_NextId = 1;
	return m_NextId++;
}

CRequestIdGenerator::CRequestIdGenerator(UINT MaxValue /*= UINT_MAX*/)
: m_NextId(1), m_MaxValue(MaxValue)
{
}
