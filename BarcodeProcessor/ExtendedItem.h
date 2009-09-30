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
	EExtendedItems  GetExtenededType() {return m_ExtendedItemType;}


#pragma warning(push)
#pragma warning (disable:4239)
#pragma warning (disable:4172)
	CBitPointer&	GetExtendedItemContentBuffer() {return IItem::GetItemContentBuffer()+BitSize(m_ExtendedItemType);}
#pragma warning(pop)

	virtual CBitPointer&	AllocateBitBuffer()	{IItem::AllocateBitBuffer(); return GetExtendedItemContentBuffer();}


	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
		{AddItemToBitString(m_ExtendedItemType, BitPtr, ParsedString); return GetExtendedItemBitBufferParsedString(ParsedString, BitPtr);}

	virtual	std::string	GetExtendedItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr) = 0;

private:
	// Decoded data:
	EExtendedItems m_ExtendedItemType;
};
