#include "StdAfx.h"
#include "ParkingMapHeader.h"
#include "BitLib.h"

CParkingMapHeader::CParkingMapHeader(void)
{
}

CParkingMapHeader::~CParkingMapHeader(void)
{
}


void CParkingMapHeader::Encode(SParkingGarageID GarageID, ETextLanguage TextLanguage, 
			SParkingGarageDimensions GarageDimensions, 
			SParkingSpaceDimensions	 ParkingSpaceDimensions,
			SFloorAndSection		 FloorAndSection,
			SPositionInParking		 CurrentPosition,
			bool					 IsTargetPosition,
			bool					 IsTargetDescription,
			SPositionInParking		 TargetPosition,
			char					 TargetDescription[MAX_STR_LEN])
{
	size_t NumberOfBits	= BitSize(GarageID) 
		+ BitSize(TextLanguage) 
		+ BitSize(GarageDimensions)
		+ BitSize(ParkingSpaceDimensions) 
		+ BitSize(FloorAndSection)
		+ BitSize(CurrentPosition) 
		+ BitSize(IsTargetPosition)
		+ BitSize(IsTargetDescription) 
		+ ((IsTargetPosition) ? BitSize(TargetPosition) : 0)
		/*+ ((IsTargetDescription) ? BitSize(TargetDescription) : 0)*/;
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, GarageID);
	BitCopyAndContinue(BitPtr, TextLanguage);
	BitCopyAndContinue(BitPtr, GarageDimensions);
	BitCopyAndContinue(BitPtr, ParkingSpaceDimensions);
	BitCopyAndContinue(BitPtr, FloorAndSection);
	BitCopyAndContinue(BitPtr, CurrentPosition);
	BitCopyAndContinue(BitPtr, IsTargetPosition);
	BitCopyAndContinue(BitPtr, IsTargetDescription);
	if (IsTargetPosition)
		BitCopyAndContinue(BitPtr, TargetPosition);
	if (IsTargetDescription)
		/*BitCopyAndContinue(BitPtr, TargetDescription)*/;
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
		/*BitPasteAndContinue(BitPtr, m_TargetDescription)*/;


	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CParkingMapHeader::GetHeaderBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);

	AddItemToBitString(m_GarageID.A, BitPtr, ParsedString);
	AddItemToBitString(m_GarageID.B, BitPtr, ParsedString);
	AddItemToBitString(m_GarageID.C, BitPtr, ParsedString);
	AddItemToBitString(m_GarageID.D, BitPtr, ParsedString);

	AddItemToBitString(m_TextLanguage, BitPtr, ParsedString);
	AddItemToBitString(m_GarageDimensions.Height, BitPtr, ParsedString);
	AddItemToBitString(m_GarageDimensions.Width, BitPtr, ParsedString);
	AddItemToBitString(m_ParkingSpaceDimensions.Height, BitPtr, ParsedString);
	AddItemToBitString(m_ParkingSpaceDimensions.Width, BitPtr, ParsedString);
	AddItemToBitString(m_FloorAndSection.Floor, BitPtr, ParsedString);
	AddItemToBitString(m_FloorAndSection.Section, BitPtr, ParsedString);
	AddItemToBitString(m_CurrentPosition.X, BitPtr, ParsedString);
	AddItemToBitString(m_CurrentPosition.Y, BitPtr, ParsedString);
	AddItemToBitString(m_IsTargetPosition, BitPtr, ParsedString);
	AddItemToBitString(m_IsTargetDescription, BitPtr, ParsedString);

	if (m_IsTargetPosition)
	{
		AddItemToBitString(m_TargetPosition.X, BitPtr, ParsedString);
		AddItemToBitString(m_TargetPosition.Y, BitPtr, ParsedString);
	}
	if (m_IsTargetDescription)
		/*AddItemToBitString(m_TargetDescription, BitPtr, ParsedString)*/;

	return ParsedString;
}
