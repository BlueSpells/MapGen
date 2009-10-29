#pragma once

#include "IItem.h"
#include <vector>
#include "ParkingMapHeader.h"

#define MINIMUM_SIZE_OF_ITEM_BIT_BUFFER	8 //bits

class CBarcodeParkingMapDecoder
{
public:
	// version 1 = All PARKING TYPES.
	// version 0 = TEMPORARY VERSION. current implemented item types
	CBarcodeParkingMapDecoder(int Version);
	~CBarcodeParkingMapDecoder(void);

	bool Decode(IN const CBitPointer &Data, IN const int DataSize/*Bits*/, IN OUT int &UsedBits);
	bool GetExtractedItemList(std::vector<IItem *> &ItemList);
	bool GetExtractedHeader(CParkingMapHeader &Header);

private:
	IItem *CloneItem(int ItemAsDecoderIndex);
	bool   MoreItemsToDecode(IN const CBitPointer &DataPtr, IN const int &BitsLeft);
	bool   DecodeHeader(IN const CBitPointer &Data, IN const int DataSize/*Bits*/, IN OUT int &UsedBits);
	bool   DecodeBody(IN const CBitPointer &Data, IN const int DataSize/*Bits*/, IN OUT int &UsedBits);

	IItem *GetLastItemDecoded();
	IItem *GetPrecedingLastItemDecoced();
	EItemType GetLastItemTypeDecoded();
	EItemType GetPrecedingLastItemTypeDecoced();

	std::vector<IItem *>	m_ItemsAsDecoders;

	std::vector<IItem *> m_ExtractedItemList;
	CParkingMapHeader m_Header;
};
