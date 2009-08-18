#pragma once

#include "IItem.h"
#include <vector>
#include "IItemDecoder.h"
#include "ParkingMapHeader.h"
#include "ParkingDecoder.h"

class CBarcodeParkingMapDecoder
{
public:
	CBarcodeParkingMapDecoder(int Version);
	~CBarcodeParkingMapDecoder(void);

	bool Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);
	bool GetExtractedItemList(std::vector<IItem *> ItemList);
	bool GetExtractedHeader(CParkingMapHeader &Header);

	std::vector<IItemDecoder *> m_ListOfItemDecoders;
	std::vector<IItem *> m_ExtractedItemList;
	CParkingMapHeader m_Header;

private:
	std::vector<IItemDecoder *> Decoders;
	CParkingDecoder				*m_ParkingDecoder;
};
