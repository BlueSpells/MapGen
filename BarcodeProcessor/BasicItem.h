#pragma once
#include "iitem.h"

enum EBasicItemType
{
	Elevator,
	Staircase,
	WC,
	CarGate,
	PayingMachine,
	//etc...
	EBasicItemType_MaxEnum = 32 // 5 bit, values [0..31]
};

// Roi: Todo - back to basics

class CBasicItem : public IItem
{
public:
	CBasicItem(void);
	~CBasicItem(void);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);

	EBasicItemType m_BasicItemType;
};
