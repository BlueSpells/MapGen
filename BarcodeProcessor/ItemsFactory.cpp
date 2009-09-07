#include "StdAfx.h"
#include "ItemsFactory.h"

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
		/*NewItem = new CParkingItem;*/ // TODO
		break;
	case Pavement:
		/*NewItem = new CParkingItem;*/ // TODO
		break;
	case BasicComponent:
		NewItem = new CBasicItem;
		break;
	case ComplexStructure:
		NewItem = new CComplexItem;
		break;
	default:
		ASSERT(false);
	};

	return NewItem;
}