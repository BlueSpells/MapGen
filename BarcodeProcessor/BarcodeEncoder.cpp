#include "StdAfx.h"
#include "BarcodeEncoder.h"
#include "ItemHelper.h"
#include "BitLib.h"

#define BARCODE_MAX_SIZE_IN_BITS BARCODE_MAX_SIZE * 8

CBarcodeEncoder::CBarcodeEncoder(void) : m_BitCount(0)
{
	m_BarcodeBitPointer.Set((void *)m_BarcodeData,0);
	ZeroMemory(m_BarcodeData, BARCODE_MAX_SIZE);
}

CBarcodeEncoder::~CBarcodeEncoder(void)
{
	//delete m_BarcodeBitPointer;
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CBarcodeEncoder::AddHeaders(std::vector<IHeader *>HeaderList)
{
	// Can call this function only once, and for empty m_BarcodeBitPointer buffer!!
	if (m_BarcodeBitPointer.GetBitOffset() > 0)
		return false;

	bool isok = true;
	for (unsigned int i = 0; i < HeaderList.size(); i++)
		isok = isok && AddHeader(HeaderList[i]);

	return isok;
}

bool CBarcodeEncoder::AddItems(std::vector<IItem *>ItemList)
{
	bool isok = true;
	for (unsigned int i = 0; i < ItemList.size(); i++)
		isok = isok && AddItem(ItemList[i]);

	return isok;
}

bool CBarcodeEncoder::AddHeader(IHeader *Header)
{
	if (Header->GetBitBufferSize() == 0 || (int)Header->GetBitBufferSize() > BARCODE_MAX_SIZE_IN_BITS - m_BarcodeBitPointer.GetBitOffset())
		return false;

	BitCopyItemAndContinue(m_BarcodeBitPointer, *Header);
	return true;
}

bool CBarcodeEncoder::AddItem(IItem *Item)
{
	if (Item->GetBitBufferSize() == 0 || (int)Item->GetBitBufferSize() > BARCODE_MAX_SIZE_IN_BITS - m_BarcodeBitPointer.GetBitOffset())
		return false;

	BitCopyItemAndContinue(m_BarcodeBitPointer, *Item);
	return true;
}


void CBarcodeEncoder::CompleteBarcodeAndGetBuffer(BYTE *&Data, int &DataSize)
{
	// No Need to complete barcode as ZeroMemory did all the job. Just need to copy the appropriate amount

	Data = m_BarcodeData;
	DataSize = (int)ceil(((float)m_BitCount) / 8);
	
	ASSERT(DataSize == (BYTE *)m_BarcodeBitPointer.GetAddress() - m_BarcodeData
				+ (int)ceil(((float)m_BarcodeBitPointer.GetBitOffset()) / 8));
}

bool CBarcodeEncoder::BuildBarcode(std::vector<IHeader *>HeaderList, std::vector<IItem *>ItemList)
{
	m_BitCount = 0;

	// Calculate size
	for (unsigned int i = 0; i < HeaderList.size(); i++)
		m_BitCount += HeaderList[i]->GetBitBufferSize();

	for (unsigned int i = 0; i < ItemList.size(); i++)
		m_BitCount += ItemList[i]->GetBitBufferSize();


// 	// Allocate Buffer (Deallocate if needed)
// 	if (m_BarcodeBitPointer.GetBitOffset() > 0)
// 		delete m_BarcodeBitPointer;
// 
// 	m_BarcodeBitPointer = new CBitPointer[BitCount]; 
// 	bitzero(m_BarcodeBitPointer, BitCount);
	m_BarcodeBitPointer.Set((void *)m_BarcodeData, 0);


	// Build buffer
	bool IsOk	= AddHeaders(HeaderList);
	IsOk		= IsOk && AddItems(ItemList);

	return IsOk;
}