#pragma once
#include <vector>
#include "IHeader.h"
#include "IItem.h"

class CBarcodeDecoder
{
public:
	CBarcodeDecoder(void);
	~CBarcodeDecoder(void);

	void DecodeBuffer(IN BYTE *Data, IN int DataSize/*Bytes*/, 
		OUT std::vector<IHeader *> HeaderList, OUT std::vector<IItem *> ItemList);

	CBitPointer m_BitPointer;
};
