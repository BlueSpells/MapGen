#pragma once

#include "IItem.h"

class CItemsFactory
{
private:
	CItemsFactory(void);
	~CItemsFactory(void);

public:
	static IItem *CreateItem(EItemType ItemType);
};
