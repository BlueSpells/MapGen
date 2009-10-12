#include "StdAfx.h"
#include "ParkingMapHeader.h"
#include "BitLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	size_t NumberOfBits	= BitSize(GarageID.A) + BitSize(GarageID.B) + BitSize(GarageID.C) + BitSize(GarageID.D)
		+ BitSize(TextLanguage) 
		+ BitSize(GarageDimensions.Height) + BitSize(GarageDimensions.Width)
		+ BitSize(ParkingSpaceDimensions.Height) + BitSize(ParkingSpaceDimensions.Width)
		+ BitSize(FloorAndSection.Floor) + BitSize(FloorAndSection.Section)
		+ BitSize(CurrentPosition.X) + BitSize(CurrentPosition.Y)
		+ BitSize(IsTargetPosition)
		+ BitSize(IsTargetDescription) 
		+ ((IsTargetPosition) ? BitSize(TargetPosition.X) + BitSize(TargetPosition.Y) : 0)
		/*+ ((IsTargetDescription) ? BitSize(TargetDescription) : 0)*/;
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, GarageID.A);
	BitCopyAndContinue(BitPtr, GarageID.B);
	BitCopyAndContinue(BitPtr, GarageID.C);
	BitCopyAndContinue(BitPtr, GarageID.D);
	BitCopyAndContinue(BitPtr, TextLanguage);
	BitCopyAndContinue(BitPtr, GarageDimensions.Height);
	BitCopyAndContinue(BitPtr, GarageDimensions.Width);
	BitCopyAndContinue(BitPtr, ParkingSpaceDimensions.Height);
	BitCopyAndContinue(BitPtr, ParkingSpaceDimensions.Width);
	BitCopyAndContinue(BitPtr, FloorAndSection.Floor);
	BitCopyAndContinue(BitPtr, FloorAndSection.Section);
	BitCopyAndContinue(BitPtr, CurrentPosition.X);
	BitCopyAndContinue(BitPtr, CurrentPosition.Y);
	BitCopyAndContinue(BitPtr, IsTargetPosition);
	BitCopyAndContinue(BitPtr, IsTargetDescription);
	if (IsTargetPosition)
	{
		BitCopyAndContinue(BitPtr, TargetPosition.X);
		BitCopyAndContinue(BitPtr, TargetPosition.Y);
	}
	if (IsTargetDescription)
		/*BitCopyAndContinue(BitPtr, TargetDescription)*/BitPtr;
}

/*virtual*/ void CParkingMapHeader::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int * /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data;

	BitPasteAndContinue(BitPtr, m_GarageID.A);
	BitPasteAndContinue(BitPtr, m_GarageID.B);
	BitPasteAndContinue(BitPtr, m_GarageID.C);
	BitPasteAndContinue(BitPtr, m_GarageID.D);
	BitPasteAndContinue(BitPtr, m_TextLanguage);
	BitPasteAndContinue(BitPtr, m_GarageDimensions.Height);
	BitPasteAndContinue(BitPtr, m_GarageDimensions.Width);
	BitPasteAndContinue(BitPtr, m_ParkingSpaceDimensions.Height);
	BitPasteAndContinue(BitPtr, m_ParkingSpaceDimensions.Width);
	BitPasteAndContinue(BitPtr, m_FloorAndSection.Floor);
	BitPasteAndContinue(BitPtr, m_FloorAndSection.Section);
	BitPasteAndContinue(BitPtr, m_CurrentPosition.X);
	BitPasteAndContinue(BitPtr, m_CurrentPosition.Y);
	BitPasteAndContinue(BitPtr, m_IsTargetPosition);
	BitPasteAndContinue(BitPtr, m_IsTargetDescription);
	if (m_IsTargetPosition)
	{	
		BitPasteAndContinue(BitPtr, m_TargetPosition.X);
		BitPasteAndContinue(BitPtr, m_TargetPosition.Y);
	}

	if (m_IsTargetDescription)
		/*BitPasteAndContinue(BitPtr, m_TargetDescription)*/BitPtr;


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
		/*AddItemToBitString(m_TargetDescription, BitPtr, ParsedString)*/BitPtr;

	return ParsedString;
}
