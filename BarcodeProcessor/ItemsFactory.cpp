#include "StdAfx.h"
#include "ItemsFactory.h"
#include "PositionItem.h"
#include "PavementItem.h"
#include "ParkingItem.h"
#include "BasicItem.h"
#include "ComplexItem.h"
#include "SignedJumpExtendedItem.h"
#include "AbsoluteJumpExtendedItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	case ExtendedItem:
		// BREAK; - in purpose !! Extended items should not be created here, but by using the EExtenedType directly!!
	default:
		ASSERT(false);
	};

	return NewItem;
}

/*static*/ IItem *CItemsFactory::CreateItem(EExtendedItemType ExtendedItemType)
{
	IItem *NewItem = NULL;

	switch (ExtendedItemType)
	{
	case SignedJump:
		NewItem = new CSignedJumpExtendedItem;
		break;
	case AbsoluteJump:
		NewItem = new CAbsoluteJumpExtendedItem;
		break;
		// TODO : add other item types
			/*Text,
			Reserved,
			Extended8bit,
			Extended16bit,
			Extended24bit,
			Extended32bit,*/
	case ExtendedItem:
		// BREAK; - in purpose !! Extended items should not be created here, but by using the EExtenedType directly!!
	default:
		ASSERT(false);
	};

	return NewItem;
}