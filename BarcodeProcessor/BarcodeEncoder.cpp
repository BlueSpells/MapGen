#include "StdAfx.h"
#include "BarcodeEncoder.h"
#include "ItemHelper.h"
#include "BitLib.h"

#define BARCODE_MAX_SIZE_IN_BITS BARCODE_MAX_SIZE * 8

CBarcodeEncoder::CBarcodeEncoder(void)
{
	m_BarcodeBitPointer.Set((void *)m_BarcodeData,0);
	ZeroMemory(m_BarcodeData, BARCODE_MAX_SIZE);
}

CBarcodeEncoder::~CBarcodeEncoder(void)
{
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CBarcodeEncoder::CreateHeaders(std::vector<IHeader *>HeaderList)
{
	// Can call this function only once, and for empty m_BarcodeBitPointer buffer!!
	if (m_BarcodeBitPointer.GetBitOffset() > 0)
		return false;

	bool isok = true;
	for (unsigned int i = 0; i < HeaderList.size(); i++)
		isok = isok && AddHeader(HeaderList[i]);

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


void CBarcodeEncoder::CompleteBarcoAndGetBuffer(BYTE *Data, int &DataSize)
{
	// No Need to complete barco as ZeroMemory did all the job. Just need to copy the appropriate amount

	Data = m_BarcodeData;
	DataSize = (int)ceil(((float)m_BarcodeBitPointer.GetBitOffset()) / 8);
}