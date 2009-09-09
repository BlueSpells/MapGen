#pragma once
#include "iitem.h"

// The position jump is restricted to FORWARD RELATIVE
// Position units will be in physical decimeters (0.1m)
// The relative position is cyclic

enum EPositionItemType
{
	CR,
	HorizontalJump,
	VerticalJump,
	DiagonalJump,
	EPositionItemType_MaxEnum
};
DefineEnumBitSize(EPositionItemType);


class CPositionItem : public IItem
{
public:
	CPositionItem(void);
	~CPositionItem(void);

	void Encode(	EPositionItemType PositionItemType, Int8Bit X, Int8Bit Y);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);


	// Decoded data:
	EPositionItemType		m_PositionItemType;
	Int8Bit					m_X; 
	Int8Bit					m_Y;
};
