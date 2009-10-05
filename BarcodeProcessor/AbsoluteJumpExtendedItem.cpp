#include "StdAfx.h"
#include "AbsoluteJumpExtendedItem.h"
#include "BitLib.h"


CAbsoluteJumpExtendedItem::CAbsoluteJumpExtendedItem(void) : CExtendedItem(AbsoluteJump),
	m_AbsoluteJumpItemType((EAbsoluteJumpItemType)0xFFFFFFFF),
	m_X(ConvertIntToInt16Bit(0)),
	m_Y(ConvertIntToInt16Bit(0))
{
}

CAbsoluteJumpExtendedItem::~CAbsoluteJumpExtendedItem(void)
{
}

void CAbsoluteJumpExtendedItem::Encode(EAbsoluteJumpItemType AbsoluteJumpItemType, Int16Bit X, Int16Bit Y)
{
	bool IsHorizontal = (AbsoluteJumpItemType == AbsoluteHorizontalJump || AbsoluteJumpItemType == AbsoluteDiagonalJump);
	bool IsVertical = (AbsoluteJumpItemType == AbsoluteVerticalJump || AbsoluteJumpItemType == AbsoluteDiagonalJump);

	size_t NumberOfBits	= BitSize(AbsoluteJumpItemType) 
		+ ((IsHorizontal) ? BitSize(X) : 0)
		+ ((IsVertical) ? BitSize(Y) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, AbsoluteJumpItemType);
	if (IsHorizontal) 
		BitCopyAndContinue(BitPtr, X);
	if (IsVertical)	
		BitCopyAndContinue(BitPtr, Y);
}

/*virtual*/ void CAbsoluteJumpExtendedItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType()) + BitSize(GetExtenededType());

	BitPasteAndContinue(BitPtr, m_AbsoluteJumpItemType);

	if (m_AbsoluteJumpItemType == AbsoluteHorizontalJump || m_AbsoluteJumpItemType == AbsoluteDiagonalJump)
		BitPasteAndContinue(BitPtr, m_X);
	if (m_AbsoluteJumpItemType == AbsoluteVerticalJump || m_AbsoluteJumpItemType == AbsoluteDiagonalJump)
		BitPasteAndContinue(BitPtr, m_Y);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CAbsoluteJumpExtendedItem::GetExtendedItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	AddItemToBitString(m_AbsoluteJumpItemType, BitPtr, ParsedString);

	if (m_AbsoluteJumpItemType == AbsoluteHorizontalJump || m_AbsoluteJumpItemType == AbsoluteDiagonalJump)
		AddItemToBitString(m_X, BitPtr, ParsedString);
	if (m_AbsoluteJumpItemType == AbsoluteVerticalJump || m_AbsoluteJumpItemType == AbsoluteDiagonalJump)
		AddItemToBitString(m_Y, BitPtr, ParsedString);

	return ParsedString;
}

/*virtual*/ void CAbsoluteJumpExtendedItem::InsertItemType()
{
	CExtendedItem::InsertItemType();
	(GetItemContentBuffer())[0] = 0;
	(GetItemContentBuffer())[1] = 0;
	(GetItemContentBuffer())[2] = 1;
}

/*virtual*/ bool CAbsoluteJumpExtendedItem::IsOfThisType(CBitPointer *Data)
{
	if (!CExtendedItem::IsOfThisType(Data))
		return false;
	if (*Data[0] != 0) return false;
	if (*Data[1] != 0) return false;
	if (*Data[2] != 1) return false;
	return true;
}

