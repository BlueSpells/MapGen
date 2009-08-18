#include "StdAfx.h"
#include "BarcodeParkingMapDecoder.h"

CBarcodeParkingMapDecoder::CBarcodeParkingMapDecoder(int Version)
{
	// ToDo
	/*Version;
	m_ParkingDecoder = new CParkingDecoder(Version);
	std::vector<IItemDecoder *> Decoders.push_back(m_ParkingDecoder);
	*/

}

CBarcodeParkingMapDecoder::~CBarcodeParkingMapDecoder(void)
{
}


bool CBarcodeParkingMapDecoder::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits)
{
	/*while ()*/
	return false;
}

bool CBarcodeParkingMapDecoder::GetExtractedItemList(std::vector<IItem *> ItemList)
{
	return false;
}

bool CBarcodeParkingMapDecoder::GetExtractedHeader(CParkingMapHeader &Header)
{
	return false;
}