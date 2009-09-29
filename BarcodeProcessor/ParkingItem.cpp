#include "StdAfx.h"
#include "ParkingItem.h"
#include "BitPointer.h"
#include "BitLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CParkingItem::CParkingItem(void) : IItem(Parking),
	m_ParkingAngle((EParkingAngle)0),
	m_ParkingType((EParkingType)0),
	m_IsMirrorDoubled(0),
	m_Orientation((EParkingOrientation)0),
	m_IsPeriodicBetweenPoles(0)
{
}

CParkingItem::~CParkingItem(void)
{
}



void CParkingItem::Encode(EParkingAngle ParkingAngle, EParkingType ParkingType, 
						  bool IsMirrorDoubled, EParkingOrientation Orientation,
						  CItemStructure ItemStructure,
						  bool IsPeriodicBetweenPoles, SPeriodicBetweenPoles *PeriodicBetweenPoles /*= NULL*/)
{
	size_t NumberOfBits	= BitSize(ParkingAngle) 
						+ BitSize(ParkingType) 
						+ BitSize(IsMirrorDoubled)
						+ BitSize(Orientation)
						+ BitSize(IsPeriodicBetweenPoles)
						+ ((IsPeriodicBetweenPoles) ? BitSize(*PeriodicBetweenPoles) : 0)
						+ ItemStructure.GetBitBufferSize();
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, ParkingAngle);
	BitCopyAndContinue(BitPtr, ParkingType);
	BitCopyAndContinue(BitPtr, IsMirrorDoubled);
	BitCopyAndContinue(BitPtr, Orientation);
	BitCopyAndContinue(BitPtr, IsPeriodicBetweenPoles);
	if (IsPeriodicBetweenPoles)
	{
		BitCopyAndContinue(BitPtr, PeriodicBetweenPoles->NumberOfParkingPlaces);
		BitCopyAndContinue(BitPtr, PeriodicBetweenPoles->PolesType);
	}

	BitCopyItemAndContinue(BitPtr, ItemStructure);
}


/*virtual*/ void CParkingItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());


	BitPasteAndContinue(BitPtr, m_ParkingAngle);
	BitPasteAndContinue(BitPtr, m_ParkingType);
	BitPasteAndContinue(BitPtr, m_IsMirrorDoubled);
	BitPasteAndContinue(BitPtr, m_Orientation);
	BitPasteAndContinue(BitPtr, m_IsPeriodicBetweenPoles);
	if (m_IsPeriodicBetweenPoles)
	{
		BitPasteAndContinue(BitPtr, m_PeriodicBetweenPoles.NumberOfParkingPlaces);
		BitPasteAndContinue(BitPtr, m_PeriodicBetweenPoles.PolesType);
	}

	int BitsUsedByItemStructure = 0;
	m_ItemStructure.Decode(BitPtr, BitsUsedByItemStructure, NULL);
	BitPtr += BitsUsedByItemStructure; // IMPORTANT even if there are no more decodings after the last for UsedBits calculations

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/ void CParkingItem::InsertItemType()
{
	(GetBitBuffer())[0] = 0;
}

/*virtual*/ bool CParkingItem::IsOfThisType(CBitPointer *Data)
{
	return (*Data[0] == 0);
}


/*virtual*/	std::string	CParkingItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	AddItemToBitString(m_ParkingAngle, BitPtr, ParsedString);
	AddItemToBitString(m_ParkingType, BitPtr, ParsedString);
	AddItemToBitString(m_IsMirrorDoubled, BitPtr, ParsedString);
	AddItemToBitString(m_Orientation, BitPtr, ParsedString);
	AddItemToBitString(m_IsPeriodicBetweenPoles, BitPtr, ParsedString);
	if (m_IsPeriodicBetweenPoles)
	{
		AddItemToBitString(m_PeriodicBetweenPoles.NumberOfParkingPlaces, BitPtr, ParsedString);
		AddItemToBitString(m_PeriodicBetweenPoles.PolesType, BitPtr, ParsedString);
	}

	return m_ItemStructure.GetItemBitBufferParsedString(ParsedString, BitPtr);
}