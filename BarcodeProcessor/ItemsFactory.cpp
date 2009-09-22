#include "StdAfx.h"
#include "ItemsFactory.h"
#include "PositionItem.h"
#include "PavementItem.h"
#include "ParkingItem.h"
#include "BasicItem.h"
#include "ComplexItem.h"

CItemsFactory::CItemsFactory(void)
{
}

CItemsFactory::~CItemsFactory(void)
{
}

/*static*/ IItem *CItemsFactory::CreateItem(EItemType ItemType)
{
	IItem *NewItem = NULL;
	
	switch (ItemType)
	{
	case Parking:
		NewItem = new CParkingItem;
		break;
	case Position:
		NewItem = new CPositionItem;
		break;
	case Pavement:
		NewItem = new CPavementItem;
		break;
	case BasicComponent:
		NewItem = new CBasicItem;
		break;
	case ComplexStructure:
		NewItem = new CComplexItem;
		break;
	// TODO : add other item types
	default:
		ASSERT(false);
	};

	return NewItem;
}