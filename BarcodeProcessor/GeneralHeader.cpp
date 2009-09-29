#include "StdAfx.h"
#include "GeneralHeader.h"
#include "BitLib.h"

CGeneralHeader::CGeneralHeader(void) : m_Signature(ConvertIntToInt32Bit(0)), 
	m_Version(ConvertIntToInt4Bit(0)), m_BarcodeType((EBarcodeType)0)
{
}

CGeneralHeader::~CGeneralHeader(void)
{
}


void CGeneralHeader::Encode(Int32Bit Signature, Int4Bit Version, EBarcodeType BarcodeType)
{
	size_t NumberOfBits	= BitSize(Signature) 
		+ BitSize(Version) 
		+ BitSize(BarcodeType);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, Signature);
	BitCopyAndContinue(BitPtr, Version);
	BitCopyAndContinue(BitPtr, BarcodeType);
}

/*virtual*/ void CGeneralHeader::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int * /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data;

	BitPasteAndContinue(BitPtr, m_Signature);
	BitPasteAndContinue(BitPtr, m_Version);
	BitPasteAndContinue(BitPtr, m_BarcodeType);
	
	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CGeneralHeader::GetBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);

	AddItemToBitString(m_Signature, BitPtr, ParsedString);
	AddItemToBitString(m_Version, BitPtr, ParsedString);
	AddItemToBitString(m_BarcodeType, BitPtr, ParsedString);

	return ParsedString;
}