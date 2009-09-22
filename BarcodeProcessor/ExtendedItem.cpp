#include "StdAfx.h"
#include "ExtendedItem.h"
#include "BitLib.h"


//CExtendedItem::CExtendedItem(EExtendedItems ItemType) : IItem(ExtendedItem),
//	m_ExtendedItemType((EExtendedItems)0xFFFFFFFF)
//{
//}

CExtendedItem::~CExtendedItem(void)
{
}

/*virtual*/ void CExtendedItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 1;
	(GetBitBuffer())[3] = 1;
	(GetBitBuffer())[4] = 1;
}


/*virtual*/ bool CExtendedItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 1) return false;
	if (*Data[3] != 1) return false;
	if (*Data[4] != 1) return false;
	return true;
}

