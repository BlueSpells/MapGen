#pragma once
#include "iitem.h"

enum EExtendedItems
{
	SignedJump,
	AbsoluteJump,
	Text,
	Reserved,
	Extended8bit,
	Extended16bit,
	Extended24bit,
	Extended32bit,
	EExtendedItems_MaxEnum
};
DefineEnumBitSize(EExtendedItems);

class CExtendedItem : public IItem
{
public:

	CExtendedItem(EExtendedItems ItemType) : IItem(ExtendedItem) 
		{m_ExtendedItemType = ItemType; IncreaseBitBufferSize(BitSize(ItemType));}

	~CExtendedItem(void);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);

	// Decoded data:
	EExtendedItems m_ExtendedItemType;
};
