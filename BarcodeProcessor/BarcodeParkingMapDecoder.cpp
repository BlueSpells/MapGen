#include "StdAfx.h"
#include "BarcodeParkingMapDecoder.h"
#include "Common/CollectionHelper.h"

#include "ItemsFactory.h"

CBarcodeParkingMapDecoder::CBarcodeParkingMapDecoder(int Version)
{
	EItemType LastItem = (EItemType)((int)EItemType_MaxEnum - 1);
	switch (Version)
	{
	case 0:
		LastItem = ComplexStructure;
	case 1:
		for (EItemType ItemType = Parking; ItemType <= LastItem; (*((int *)&ItemType))++)
		{
			Decoders.push_back(CItemsFactory::CreateItem(ItemType));
		}
	default:
		ASSERT(false);
	}
}

CBarcodeParkingMapDecoder::~CBarcodeParkingMapDecoder(void)
{
	while (Decoders.size() > 0)
	{
		delete Decoders[0];
		RemoveValueFromVector(Decoders, Decoders[0]);
	}
}


bool CBarcodeParkingMapDecoder::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits)
{
	/*while ()*/
	return false;
}

bool CBarcodeParkingMapDecoder::GetExtractedItemList(std::vector<IItem *> ItemList)
{
	return false;
}

bool CBarcodeParkingMapDecoder::GetExtractedHeader(CParkingMapHeader &Header)
{
	return false;
}