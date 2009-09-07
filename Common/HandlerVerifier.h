#pragma once

class CHandlerVerifier
{
public:
	CHandlerVerifier();
	~CHandlerVerifier();
	CHandlerVerifier(const CHandlerVerifier& HandlerVerifier);
	void operator=(const CHandlerVerifier& HandlerVerifier);

    //will return if the 'this' and the Other verifier are copies of the same original verifier
    bool operator==(const CHandlerVerifier& HandlerVerifier);
	bool Lock();
	void Unlock();

	//Invalidate: explicitly invalidate the verifier handler, should be called
	//by the container of the verifier on the destructor in case that the container
	//contains objects that perform requests. This will prevent destruction of the executing
	//object while the verifier being valid.
	void Invalidate();  

private:
	void Attach(const CHandlerVerifier& HandlerVerifier);
	void Detach();

private:
	struct HandlerState
	{
		CCriticalSection Lock;
		bool IsValid;
		volatile LONG NumReferences;
	};

	HandlerState*	m_HandlerState;
	bool			m_IsOwner;
	bool			m_Attached;
};