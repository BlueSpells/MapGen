#pragma once
#include "iitem.h"
#include "ExtendedItem.h"

// Position units will be in physical decimeters (0.1m)
// The position is cyclic

enum ESignedJumpItemType
{
	CarriageDir,
	SignedHorizontalJump,
	SignedVerticalJump,
	SignedDiagonalJump,
	ESignedJumpItemType_MaxEnum
};
DefineEnumBitSize(ESignedJumpItemType);


class CSignedJumpExtendedItem : public CExtendedItem
{
public:
	CSignedJumpExtendedItem(void);
	~CSignedJumpExtendedItem(void);

	void Encode(	ESignedJumpItemType SignedJumpItemType, SignedInt8Bit dX, SignedInt8Bit dY);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);



	// Decoded data:
	ESignedJumpItemType		m_SignedJumpItemType;
	SignedInt8Bit			m_dX; 
	SignedInt8Bit			m_dY;
};
