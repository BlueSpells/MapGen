#pragma once
#include "iitem.h"

enum EBasicItemType
{
	Elevator,
	Staircase_H,
	Staircase_V,
	WC,
	CarGate_Right,
	CarGate_Left,
	CarGate_Up,
	CarGate_Down,
	PayingMachine,
	Door,
	Sign,
	Column_Sqr,
	Column_Cir,
	Road,
	//etc...
	EBasicItemType_MaxEnum = 16 // 4 bit, values [0..15]
};
DefineEnumBitSize(EBasicItemType);

union UScalingCoordinates
{
	Int16Bit Size16bits;
	Int10Bit Size10bits;
	Int6Bit Size6bits;
};


class CBasicItem : public IItem
{
public:
	CBasicItem(void);
	~CBasicItem(void);

	void Encode(	EBasicItemType BasicItemType, bool IsScaled, UScalingCoordinates X, UScalingCoordinates Y);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int * /*Context unnecessary*/);

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);

	// Decoded data:
	EBasicItemType					m_BasicItemType;
	bool							m_IsScaled;
	UScalingCoordinates				m_X;
	UScalingCoordinates				m_Y;
};
