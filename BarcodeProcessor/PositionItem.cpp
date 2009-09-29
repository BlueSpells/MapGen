#include "StdAfx.h"
#include "PositionItem.h"
#include "BitLib.h"

// The position jump is restricted to FORWARD RELATIVE
// Position units will be in physical decimeters (0.1m)
// The relative position is cyclic

CPositionItem::CPositionItem(void) : IItem(Position),
	m_PositionItemType((EPositionItemType)0xFFFFFFFF),
	m_dX(ConvertIntToInt8Bit(0)),
	m_dY(ConvertIntToInt8Bit(0))
{
}

CPositionItem::~CPositionItem(void)
{
}

void CPositionItem::Encode(EPositionItemType PositionItemType, Int8Bit dX, Int8Bit dY)
{
	bool IsHorizontal = (PositionItemType == ForwardHorizontalJump || PositionItemType == ForwardDiagonalJump);
	bool IsVertical = (PositionItemType == ForwardVerticalJump || PositionItemType == ForwardDiagonalJump);

	size_t NumberOfBits	= BitSize(PositionItemType) 
		+ ((IsHorizontal) ? BitSize(dX) : 0)
		+ ((IsVertical) ? BitSize(dY) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, PositionItemType);
	if (IsHorizontal) 
		BitCopyAndContinue(BitPtr, dX);
	if (IsVertical)	
		BitCopyAndContinue(BitPtr, dY);
}

/*virtual*/ void CPositionItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_PositionItemType);

	if (m_PositionItemType == ForwardHorizontalJump || m_PositionItemType == ForwardDiagonalJump)
		BitPasteAndContinue(BitPtr, m_dX);
	if (m_PositionItemType == ForwardVerticalJump || m_PositionItemType == ForwardDiagonalJump)
		BitPasteAndContinue(BitPtr, m_dY);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CPositionItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	AddItemToBitString(m_PositionItemType, BitPtr, ParsedString);

	if (m_PositionItemType == ForwardHorizontalJump || m_PositionItemType == ForwardDiagonalJump)
		AddItemToBitString(m_dX, BitPtr, ParsedString);
	if (m_PositionItemType == ForwardVerticalJump || m_PositionItemType == ForwardDiagonalJump)
		AddItemToBitString(m_dY, BitPtr, ParsedString);

	return ParsedString;
}

/*virtual*/ void CPositionItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 0;
}

/*virtual*/ bool CPositionItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 0) return false;
	return true;
}

