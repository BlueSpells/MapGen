#pragma once

#include "BitPointer.h"
#include "ItemHelper.h"
#include "BitLib.h"

class IAbstractBarcodeElement
{
public:
	IAbstractBarcodeElement() : m_NumberOfBits(0) {}
	~IAbstractBarcodeElement(void) {}

	virtual void		Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context /*when necessary*/) = 0;
	/*virtual void		Encode();*/ //- Todo in the future?
	
	// Note, GetBitBufferParsedString assumes Item has just been Encoded!!
	// and performs Decode on its own buffer in order to parse it correctly.
	virtual	std::string	GetBitBufferParsedString() = 0;

	CBitPointer&	GetBitBuffer()								{return m_BitBuffer;}
	void			IncreaseBitBufferSize(size_t NumberOfBits)	{m_NumberOfBits += NumberOfBits;}
	size_t			GetBitBufferSize()							{return m_NumberOfBits;}
	std::string		GetBitBufferRawString()						{return BinaryString(m_BitBuffer, m_NumberOfBits);};

#pragma warning(push)
#pragma warning (disable:4239)
#pragma warning (disable:4172)
	virtual CBitPointer&	AllocateBitBuffer()							
	{m_BitBuffer = new CBitPointer[m_NumberOfBits]; bitzero(m_BitBuffer, m_NumberOfBits); return m_BitBuffer;}
#pragma warning(pop)

private:
	CBitPointer m_BitBuffer;
	size_t m_NumberOfBits;
};
