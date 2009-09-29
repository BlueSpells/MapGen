#include "StdAfx.h"
#include "ParkingMapHeader.h"
#include "BitLib.h"

CParkingMapHeader::CParkingMapHeader(void)
{
}

CParkingMapHeader::~CParkingMapHeader(void)
{
}


void CParkingMapHeader::Encode(SParkingGarageID m_GarageID, ETextLanguage m_TextLanguage, 
			SParkingGarageDimensions m_GarageDimensions, 
			SParkingSpaceDimensions	 m_ParkingSpaceDimensions,
			SFloorAndSection		 m_FloorAndSection,
			SPositionInParking		 m_CurrentPosition,
			bool					 m_IsTargetPosition,
			bool					 m_IsTargetDescription,
			SPositionInParking		 m_TargetPosition)
{
	size_t NumberOfBits	= BitSize(m_GarageID) 
		+ BitSize(m_TextLanguage) 
		+ BitSize(m_GarageDimensions)
		+ BitSize(m_ParkingSpaceDimensions) 
		+ BitSize(m_FloorAndSection)
		+ BitSize(m_CurrentPosition) 
		+ BitSize(m_IsTargetPosition)
		+ BitSize(m_IsTargetDescription) 
		+ ((m_IsTargetPosition) ? BitSize(m_TargetPosition) : 0)
		+ ((m_IsTargetDescription) ? BitSize(m_TargerDescription) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, m_GarageID);
	BitCopyAndContinue(BitPtr, m_TextLanguage);
	BitCopyAndContinue(BitPtr, m_GarageDimensions);
	BitCopyAndContinue(BitPtr, m_ParkingSpaceDimensions);
	BitCopyAndContinue(BitPtr, m_FloorAndSection);
	BitCopyAndContinue(BitPtr, m_CurrentPosition);
	BitCopyAndContinue(BitPtr, m_IsTargetPosition);
	BitCopyAndContinue(BitPtr, m_IsTargetDescription);
	if (m_IsTargetPosition)
		BitCopyAndContinue(BitPtr, m_TargetPosition);
	if (m_IsTargetDescription)
		BitCopyAndContinue(BitPtr, m_TargerDescription);
}

/*virtual*/ void CParkingMapHeader::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int * /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data;

	BitPasteAndContinue(BitPtr, m_GarageID);
	BitPasteAndContinue(BitPtr, m_TextLanguage);
	BitPasteAndContinue(BitPtr, m_GarageDimensions);
	BitPasteAndContinue(BitPtr, m_ParkingSpaceDimensions);
	BitPasteAndContinue(BitPtr, m_FloorAndSection);
	BitPasteAndContinue(BitPtr, m_CurrentPosition);
	BitPasteAndContinue(BitPtr, m_IsTargetPosition);
	BitPasteAndContinue(BitPtr, m_IsTargetDescription);
	if (m_IsTargetPosition)
		BitPasteAndContinue(BitPtr, m_TargetPosition);
	if (m_IsTargetDescription)
		BitPasteAndContinue(BitPtr, m_TargerDescription);


	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CParkingMapHeader::GetBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);

	AddItemToBitString(m_GarageID, BitPtr, ParsedString);
	AddItemToBitString(m_TextLanguage, BitPtr, ParsedString);
	AddItemToBitString(m_GarageDimensions, BitPtr, ParsedString);
	AddItemToBitString(m_ParkingSpaceDimensions, BitPtr, ParsedString);
	AddItemToBitString(m_FloorAndSection, BitPtr, ParsedString);
	AddItemToBitString(m_CurrentPosition, BitPtr, ParsedString);
	AddItemToBitString(m_IsTargetPosition, BitPtr, ParsedString);
	AddItemToBitString(m_IsTargetDescription, BitPtr, ParsedString);

	if (m_IsTargetPosition)
		AddItemToBitString(m_TargetPosition, BitPtr, ParsedString);
	if (m_IsTargetDescription)
		AddItemToBitString(m_TargerDescription, BitPtr, ParsedString);

	return ParsedString;
}
