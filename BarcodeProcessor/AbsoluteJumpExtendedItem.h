#pragma once
#include "iitem.h"
#include "ExtendedItem.h"


enum EAbsoluteJumpItemType
{
	Unused,
	HorizontalJump,
	VerticalJump,
	DiagonalJump,
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
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);



	// Decoded data:
	EAbsoluteJumpItemType		m_AbsoluteJumpItemType;
	Int16Bit				m_X; 
	Int16Bit				m_Y;
};
