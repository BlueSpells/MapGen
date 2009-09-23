#include "StdAfx.h"
#include "SignedJumpExtendedItem.h"
#include "BitLib.h"

// Position units will be in physical decimeters (0.1m)
// The position is cyclic

CSignedJumpExtendedItem::CSignedJumpExtendedItem(void) : CExtendedItem(SignedJump),
	m_SignedJumpItemType((ESignedJumpItemType)0xFFFFFFFF),
	m_X(ConvertIntToSignedInt9Bit(0)),
	m_Y(ConvertIntToSignedInt9Bit(0))
{
}

CSignedJumpExtendedItem::~CSignedJumpExtendedItem(void)
{
}

void CSignedJumpExtendedItem::Encode(ESignedJumpItemType SignedJumpItemType, SignedInt9Bit X, SignedInt9Bit Y)
{
	bool IsHorizontal = (SignedJumpItemType == HorizontalJump || SignedJumpItemType == DiagonalJump);
	bool IsVertical = (SignedJumpItemType == VerticalJump || SignedJumpItemType == DiagonalJump);

	size_t NumberOfBits	= BitSize(SignedJumpItemType) 
		+ ((IsHorizontal) ? BitSize(X) : 0)
		+ ((IsVertical) ? BitSize(Y) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, SignedJumpItemType);
	if (IsHorizontal) 
		BitCopyAndContinue(BitPtr, X);
	if (IsVertical)	
		BitCopyAndContinue(BitPtr, Y);
}

/*virtual*/ void CSignedJumpExtendedItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_SignedJumpItemType);

	if (m_SignedJumpItemType == HorizontalJump || m_SignedJumpItemType == DiagonalJump)
		BitCopyAndContinue(BitPtr, m_X);
	if (m_SignedJumpItemType == VerticalJump || m_SignedJumpItemType == DiagonalJump)
		BitCopyAndContinue(BitPtr, m_Y);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/ void CSignedJumpExtendedItem::InsertItemType()
{
	CExtendedItem::InsertItemType();
	(GetBitBuffer())[0] = 0;
	(GetBitBuffer())[1] = 0;
	(GetBitBuffer())[2] = 0;
}

/*virtual*/ bool CSignedJumpExtendedItem::IsOfThisType(CBitPointer *Data)
{
	if (!CExtendedItem::IsOfThisType(Data))
		return false;
	if (*Data[0] != 0) return false;
	if (*Data[1] != 0) return false;
	if (*Data[2] != 0) return false;
	return true;
}

