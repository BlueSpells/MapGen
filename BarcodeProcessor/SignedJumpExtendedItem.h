#pragma once
#include "iitem.h"
#include "ExtendedItem.h"

// Position units will be in physical decimeters (0.1m)
// The position is cyclic

enum ESignedJumpItemType
{
	CarriageDir,
	HorizontalJump,
	VerticalJump,
	DiagonalJump,
	ESignedJumpItemType_MaxEnum
};
DefineEnumBitSize(ESignedJumpItemType);


class CSignedJumpExtendedItem : public CExtendedItem
{
public:
	CSignedJumpExtendedItem(void);
	~CSignedJumpExtendedItem(void);

	void Encode(	ESignedJumpItemType PositionItemType, Int9Bit X, Int9Bit Y);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);



	// Decoded data:
	ESignedJumpItemType		m_SignedJumpItemType;
	Int9Bit					m_X; 
	Int9Bit					m_Y;
};
