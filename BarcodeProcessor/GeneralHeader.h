#pragma once
#include "iheader.h"
#include "ItemHelper.h"

#define SIGNATURE 0xDeadBabe
#define LAST_VERSION 0

enum EBarcodeType
{
	ParkingMap,
	EBarcodeType_MaxEnum = 16 // Values [0..15], 4 bits.
};
DefineEnumBitSize(EBarcodeType);

class CGeneralHeader : public IHeader
{
public:
	CGeneralHeader(void);
	~CGeneralHeader(void);

	void Encode(Int32Bit Signature = (Int32Bit)SIGNATURE, 
				Int4Bit Version = (Int4Bit)LAST_VERSION, 
				EBarcodeType BarcodeType = ParkingMap);

	virtual void Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);

	Int32Bit m_Signature;
	Int4Bit  m_Version;
	EBarcodeType  m_BarcodeType;
};
