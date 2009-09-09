#include "StdAfx.h"
#include "PositionItem.h"
#include "BitPointer.h"
#include "BitLib.h"

// The position jump is restricted to FORWARD RELATIVE
// Position units will be in physical decimeters (0.1m)
// The relative position is cyclic

CPositionItem::CPositionItem(void) : IItem(Position),
	m_PositionItemType((EPositionItemType)0xFFFFFFFF),
	m_X(ConvertIntToInt8Bit(0)),
	m_Y(ConvertIntToInt8Bit(0))
{
}

CPositionItem::~CPositionItem(void)
{
}

void CPositionItem::Encode(EPositionItemType PositionItemType, Int8Bit X, Int8Bit Y)
{
	bool IsHorizontal = (PositionItemType == HorizontalJump || PositionItemType == DiagonalJump);
	bool IsVertical = (PositionItemType == VerticalJump || PositionItemType == DiagonalJump);

	size_t NumberOfBits	= BitSize(PositionItemType) 
		+ ((IsHorizontal) ? BitSize(X) : 0)
		+ ((IsVertical) ? BitSize(Y) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, PositionItemType);
	if (IsHorizontal)
	{
		BitCopyAndContinue(BitPtr, X);
	}
	if (IsVertical)
	{
		BitCopyAndContinue(BitPtr, Y);
	}
}

/*virtual*/ void CPositionItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits)
{

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_PositionItemType);

	if ((m_PositionItemType == HorizontalJump || m_PositionItemType == DiagonalJump))
	{
		BitCopyAndContinue(BitPtr, m_X);
	}
	if (m_PositionItemType == VerticalJump || m_PositionItemType == DiagonalJump)
	{
		BitCopyAndContinue(BitPtr, m_Y);
	}

	UsedBits = (int)(BitPtr - Data);
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

