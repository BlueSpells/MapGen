#include "StdAfx.h"
#include "BasicItem.h"
#include "BitLib.h"


CBasicItem::CBasicItem(void) : IItem(BasicComponent),
	m_BasicItemType((EBasicItemType)0xFFFFFFFF)
{
}

CBasicItem::~CBasicItem(void)
{
}


void CBasicItem::Encode(EBasicItemType BasicItemType)
{
	size_t NumberOfBits	= BitSize(BasicItemType);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, BasicItemType);
}


/*virtual*/ void CBasicItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int */*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_BasicItemType);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/ void CBasicItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 1;
	(GetBitBuffer())[3] = 0;
}

/*virtual*/ bool CBasicItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 1) return false;
	if (*Data[3] != 0) return false;
	return true;
}
