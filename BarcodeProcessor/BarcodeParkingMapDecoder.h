#pragma once

#include "IItem.h"
#include <vector>
#include "IItemDecoder.h"
#include "ParkingMapHeader.h"
#include "ParkingDecoder.h"



class CBarcodeParkingMapDecoder
{
public:
	// version 1 = All PARKING TYPES.
	// version 0 = TEMPORARY VERSION. current implemented item types
	CBarcodeParkingMapDecoder(int Version);
	~CBarcodeParkingMapDecoder(void);

	bool Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);
	bool GetExtractedItemList(std::vector<IItem *> ItemList);
	bool GetExtractedHeader(CParkingMapHeader &Header);

	std::vector<IItem *> m_ExtractedItemList;
	CParkingMapHeader m_Header;

private:
	std::vector<IItem *> Decoders;
	CParkingDecoder		 *m_ParkingDecoder;
};
