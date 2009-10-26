#pragma once
#include "IItem.h"
#include "IHeader.h"
#include "BitPointer.h"
#include <vector>

#define BARCODE_MAX_SIZE 4096

class CBarcodeEncoder
{
public:
	CBarcodeEncoder(void);
	~CBarcodeEncoder(void);

	bool BuildBarcode(std::vector<IHeader *>HeaderList, std::vector<IItem *>ItemList);
	void CompleteBarcodeAndGetBuffer(BYTE *&Data, int &DataSize);

	CBitPointer	m_BarcodeBitPointer;
	BYTE		m_BarcodeData[BARCODE_MAX_SIZE];
	int			m_BitCount;

private:
	bool AddHeaders(std::vector<IHeader *>HeaderList);
	bool AddItems(std::vector<IItem *>ItemList);
	bool AddHeader(IHeader *Header);
	bool AddItem(IItem *Item);
};
