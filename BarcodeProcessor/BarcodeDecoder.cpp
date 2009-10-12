#include "StdAfx.h"
#include "BarcodeDecoder.h"
#include "GeneralHeader.h"
#include "BarcodeParkingMapDecoder.h"

CBarcodeDecoder::CBarcodeDecoder(void)
{
}

CBarcodeDecoder::~CBarcodeDecoder(void)
{
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CBarcodeDecoder::DecodeBuffer(IN BYTE *Data, IN int DataSize, 
								   OUT std::vector<IHeader *> HeaderList, OUT std::vector<IItem *> ItemList)
{
	m_BitPointer.Set(Data);

	CGeneralHeader *GeneralHeader = new CGeneralHeader;
	BitDecodeAndContinue(GeneralHeader, m_BitPointer);
	HeaderList.push_back(GeneralHeader);

	// ToDo: Check Signature, and parse accordingly
	if ((int)GeneralHeader->m_Signature != SIGNATURE)
	{
		// Do non-guide me decoding:
		// decode text. if it's a url, go to. otherwise, simply show text.
		return;
	}

	// Todo: Check barcode type, and parse accordingly
	switch (GeneralHeader->m_BarcodeType)
	{
	case ParkingMap:
		{
			CBarcodeParkingMapDecoder MapDecoder(GeneralHeader->m_Version);
			int RemainingBitsToDecode = DataSize*8-m_BitPointer.GetBitOffset();
			MapDecoder.Decode(m_BitPointer, RemainingBitsToDecode);
			CParkingMapHeader *Header = new CParkingMapHeader;
			MapDecoder.GetExtractedHeader(*Header);
			HeaderList.push_back(Header);
			MapDecoder.GetExtractedItemList(ItemList);
		}
		break;
	default:
		ASSERT(false);
	};
}
