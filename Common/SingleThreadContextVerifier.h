#pragma once

// A simple debugging class that enable client objects to verify 
// that several entry points (functions) are being called from one thread only.
class CSingleThreadContextVerifier
{
public:
    CSingleThreadContextVerifier();

    bool VerifyThreadContext();
    void Reset();

private:
    DWORD m_ThreadId;
};
