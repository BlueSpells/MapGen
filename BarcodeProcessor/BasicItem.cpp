#include "StdAfx.h"
#include "BasicItem.h"
#include "BitPointer.h"
#include "BitLib.h"

// Another change

CBasicItem::CBasicItem(void) : IItem(BasicComponent),
	m_BasicItemType((EBasicItemType)0xFFFFFFFF)
{
}

CBasicItem::~CBasicItem(void)
{
}


void CBasicItem::Encode(EBasicItemType BasicItem)
{
	size_t NumberOfBits	= BitSize(BasicItem);
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, BasicItem);
}


/*virtual*/ void CBasicItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_BasicItemType);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/ void CBasicItem::InsertItemType()
{
	(GetBitBuffer())[0] = 0;
}

/*virtual*/ bool CBasicItem::IsOfThisType(CBitPointer *Data)
{
	return (*Data[0] == 0);
}
