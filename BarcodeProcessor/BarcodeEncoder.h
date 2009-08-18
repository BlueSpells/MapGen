#pragma once
#include "IItem.h"
#include "IHeader.h"
#include "BitPointer.h"
#include <vector>

#define BARCODE_MAX_SIZE 2048

class CBarcodeEncoder
{
public:
	CBarcodeEncoder(void);
	~CBarcodeEncoder(void);

	bool CreateHeaders(std::vector<IHeader *>HeaderList);
	bool AddItem(IItem *Item);
	void CompleteBarcoAndGetBuffer(BYTE *Data, int &DataSize);

	CBitPointer	m_BarcodeBitPointer;
	BYTE		m_BarcodeData[BARCODE_MAX_SIZE];

private:
	bool AddHeader(IHeader *Header);
};
