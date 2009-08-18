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

	EBasicItemType m_BasicItemType;
};
