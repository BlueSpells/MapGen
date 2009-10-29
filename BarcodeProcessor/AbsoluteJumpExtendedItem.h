#pragma once
#include "iitem.h"
#include "ExtendedItem.h"


enum EAbsoluteJumpItemType
{
	Unused,
	AbsoluteHorizontalJump,
	AbsoluteVerticalJump,
	AbsoluteDiagonalJump,
	EAbsoluteJumpItemType_MaxEnum
};
DefineEnumBitSize(EAbsoluteJumpItemType);


class CAbsoluteJumpExtendedItem : public CExtendedItem
{
public:
	CAbsoluteJumpExtendedItem(void);
	~CAbsoluteJumpExtendedItem(void);

	void Encode(	EAbsoluteJumpItemType PositionItemType, Int16Bit X, Int16Bit Y);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType (CBitPointer *Data) const;
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);

	virtual	std::string	GetExtendedItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);

	// Decoded data:
	EAbsoluteJumpItemType		m_AbsoluteJumpItemType;
	Int16Bit					m_X; 
	Int16Bit					m_Y;
};
