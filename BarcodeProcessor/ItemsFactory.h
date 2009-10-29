#pragma once

#include "IItem.h"
#include "ExtendedItem.h"

class CItemsFactory
{
private:
	CItemsFactory(void);
	~CItemsFactory(void);

public:
	static IItem *CreateItem(EItemType ItemType);
	static IItem *CreateItem(EExtendedItemType ExtendedItemType);
};
