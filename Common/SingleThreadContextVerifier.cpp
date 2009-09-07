#include "StdAfx.h"
#include "SingleThreadContextVerifier.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CSingleThreadContextVerifier::CSingleThreadContextVerifier(): m_ThreadId(0)
{
}

void CSingleThreadContextVerifier::Reset()
{
    m_ThreadId = 0;
}

bool CSingleThreadContextVerifier::VerifyThreadContext()
{
    if(m_ThreadId == 0)
        m_ThreadId = GetCurrentThreadId();
    Assert(m_ThreadId == GetCurrentThreadId());
    return m_ThreadId == GetCurrentThreadId();
}