#include "StdAfx.h"
#include "SignedJumpExtendedItem.h"
#include "BitLib.h"

// Position units will be in physical decimeters (0.1m)
// The position is cyclic

CSignedJumpExtendedItem::CSignedJumpExtendedItem(void) : CExtendedItem(SignedJump),
	m_SignedJumpItemType((ESignedJumpItemType)0xFFFFFFFF),
	m_dX(ConvertIntToSignedInt8Bit(0)),
	m_dY(ConvertIntToSignedInt8Bit(0))
{
}

CSignedJumpExtendedItem::~CSignedJumpExtendedItem(void)
{
}

void CSignedJumpExtendedItem::Encode(ESignedJumpItemType SignedJumpItemType, SignedInt8Bit dX, SignedInt8Bit dY)
{
	bool IsHorizontal = (SignedJumpItemType == SignedHorizontalJump || SignedJumpItemType == SignedDiagonalJump);
	bool IsVertical = (SignedJumpItemType == SignedVerticalJump || SignedJumpItemType == SignedDiagonalJump);

	size_t NumberOfBits	= BitSize(SignedJumpItemType) 
		+ ((IsHorizontal) ? BitSize(dX) : 0)
		+ ((IsVertical) ? BitSize(dY) : 0);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, SignedJumpItemType);
	if (IsHorizontal) 
		BitCopyAndContinue(BitPtr, dX);
	if (IsVertical)	
		BitCopyAndContinue(BitPtr, dY);
}

/*virtual*/ void CSignedJumpExtendedItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_SignedJumpItemType);

	if (m_SignedJumpItemType == SignedHorizontalJump || m_SignedJumpItemType == SignedDiagonalJump)
		BitCopyAndContinue(BitPtr, m_dX);
	if (m_SignedJumpItemType == SignedVerticalJump || m_SignedJumpItemType == SignedDiagonalJump)
		BitCopyAndContinue(BitPtr, m_dY);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/	std::string	CSignedJumpExtendedItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	AddItemToBitString(m_SignedJumpItemType, BitPtr, ParsedString);

	if (m_SignedJumpItemType == SignedHorizontalJump || m_SignedJumpItemType == SignedDiagonalJump)
		AddItemToBitString(m_dX, BitPtr, ParsedString);
	if (m_SignedJumpItemType == SignedVerticalJump || m_SignedJumpItemType == SignedDiagonalJump)
		AddItemToBitString(m_dY, BitPtr, ParsedString);

	return ParsedString;
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

