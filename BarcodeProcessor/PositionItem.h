#pragma once
#include "iitem.h"

// The position jump is restricted to FORWARD RELATIVE
// Position units will be in physical decimeters (0.1m)
// The relative position is cyclic

enum EPositionItemType
{
	CR,
	ForwardHorizontalJump,
	ForwardVerticalJump,
	ForwardDiagonalJump,
	EPositionItemType_MaxEnum
};
DefineEnumBitSize(EPositionItemType);


class CPositionItem : public IItem
{
public:
	CPositionItem(void);
	~CPositionItem(void);

	void Encode(	EPositionItemType PositionItemType, Int8Bit dX, Int8Bit dY);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);



	// Decoded data:
	EPositionItemType		m_PositionItemType;
	Int8Bit					m_dX; 
	Int8Bit					m_dY;
};
