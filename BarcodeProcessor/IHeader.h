#pragma once
#include "BitPointer.h"

class IHeader
{
public:
	IHeader() : m_NumberOfBits(0) {}
	~IHeader(void) {}

	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits) = 0;

	CBitPointer&	GetBitBuffer()								{return m_BitBuffer;}
	void			IncreaseBitBufferSize(size_t NumberOfBits)	{m_NumberOfBits += NumberOfBits;}
	size_t			GetBitBufferSize()							{return m_NumberOfBits;}
	CBitPointer&	AllocateBitBuffer() 
	{m_BitBuffer = new CBitPointer[m_NumberOfBits]; return m_BitBuffer;}

private:
	CBitPointer m_BitBuffer;
	size_t m_NumberOfBits;
};