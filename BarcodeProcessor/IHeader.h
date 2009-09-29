#pragma once
#include "IAbstractBarcodeElement.h"

class IHeader : public IAbstractBarcodeElement
{
public:
	IHeader() {}
	~IHeader(void) {}

	virtual	std::string	GetBitBufferParsedString()
	{std::string Str; CBitPointer BitPtr = GetBitBuffer(); return GetHeaderBitBufferParsedString(Str, BitPtr);}

	virtual	std::string	GetHeaderBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr) = 0;
};