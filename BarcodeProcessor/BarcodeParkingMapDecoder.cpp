#include "StdAfx.h"
#include "BarcodeParkingMapDecoder.h"
#include "Common/CollectionHelper.h"

#include "ItemsFactory.h"
#include "EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBarcodeParkingMapDecoder::CBarcodeParkingMapDecoder(int Version)
{
	EItemType LastItem = LastElementOfEnum(EItemType);
	switch (Version)
	{
	case 0:
		LastItem = ComplexStructure;
	case 1:
		for (EItemType ItemType = FirstElementOfEnum(EItemType); ItemType <= LastItem; IncrementEnumIterator(ItemType))
		{
			if (ItemType == ExtendedItem)
			{
				for (EExtendedItemType ExtendedItemType = FirstElementOfEnum(EExtendedItemType); ExtendedItemType <= LastElementOfEnum(EItemType); IncrementEnumIterator(ExtendedItemType))
				{
					m_ItemsAsDecoders.push_back(CItemsFactory::CreateItem(ExtendedItemType));
				}
			}
			else
				m_ItemsAsDecoders.push_back(CItemsFactory::CreateItem(ItemType));
		}

	default:
		ASSERT(false);
	}
}

CBarcodeParkingMapDecoder::~CBarcodeParkingMapDecoder(void)
{
	while (m_ItemsAsDecoders.size() > 0)
	{
		delete m_ItemsAsDecoders[0];
		RemoveValueFromVector(m_ItemsAsDecoders, m_ItemsAsDecoders[0]);
	}
}


bool CBarcodeParkingMapDecoder::Decode(IN const CBitPointer &Data, IN const int DataSize, IN OUT int &UsedBits)
{
	if (!DecodeHeader(Data, DataSize, UsedBits))
		return false;

	return DecodeBody(Data, DataSize, UsedBits);
}

bool CBarcodeParkingMapDecoder::GetExtractedItemList(std::vector<IItem *> &ItemList)
{
	return false;
}

bool CBarcodeParkingMapDecoder::GetExtractedHeader(CParkingMapHeader &/*Header*/)
{
	// Todo: Implement this !!
	return false;
}

IItem *CBarcodeParkingMapDecoder::CloneItem(int ItemAsDecoderIndex)
{
	if (m_ItemsAsDecoders[ItemAsDecoderIndex]->GetType() == ExtendedItem)
		return CItemsFactory::CreateItem(m_ItemsAsDecoders[ItemAsDecoderIndex]->GetType());
	else
		return CItemsFactory::CreateItem(((CExtendedItem *)m_ItemsAsDecoders[ItemAsDecoderIndex])->GetExtenededType());
}

bool   CBarcodeParkingMapDecoder::MoreItemsToDecode(IN const CBitPointer &/*DataPtr*/, IN const int &BitsLeft)
{
	// Luckily, all items are at least 8 bits long, and since there are no full empty bytes at the end,
	// as soon a we either reach the buffer's end or the last byte (any bit position within) we know we
	// just finished decoding.
	// Otherwise we would have to go through each one of the Items and ask it whether it can decode the next item
	// Or: better, decode will have a return value (true\false) which the Items will use in order to return
	// whether they have managed to decode their buffer or not (i.e. we have reached the end of buffer)

	return (BitsLeft >= MINIMUM_SIZE_OF_ITEM_BIT_BUFFER);
}

bool   CBarcodeParkingMapDecoder::DecodeHeader(IN const CBitPointer &Data, IN const int DataSize/*Bits*/, IN OUT int &UsedBits)
{
	CBitPointer DataPtr = Data + UsedBits;

	BitDecodeAndContinue((&m_Header), DataPtr);

	UsedBits = DataPtr - Data;
	return true;
}

bool   CBarcodeParkingMapDecoder::DecodeBody(IN const CBitPointer &Data, IN const int DataSize/*Bits*/, IN OUT int &UsedBits)
{
	CBitPointer DataPtr = Data + UsedBits;

	while (MoreItemsToDecode(DataPtr, DataSize - (DataPtr - Data)))
	{
		for (unsigned int iItemDecoder = 0; iItemDecoder <= m_ItemsAsDecoders.size(); iItemDecoder++)
		{
			if (iItemDecoder == m_ItemsAsDecoders.size())
			{
				// 				LogEvent(LE_ERROR, "Decoding of Item %d (at offset %d) failed! Item Type unrecognized",
				// 					iItem, CBitPointer.GetBitOffset());
				ASSERT(false);
				return false;
			}

			if (m_ItemsAsDecoders[iItemDecoder]->IsOfThisType(&DataPtr))
			{
				IItem *ItemClone = CloneItem(iItemDecoder);

				switch (ItemClone->GetType())
				{
				case ExtendedItem:
					switch (((CExtendedItem *)ItemClone)->GetExtenededType())
					{
					case SignedJump:
					case AbsoluteJump:
					case Text:
						break;
					case Extended8bit:
					case Extended16bit:
					case Extended24bit:
					case Extended32bit:
						Assert(false); // ToDo - implement in the future!!
						return false;
						break;
					case Reserved:
						// IN PURPOSE
					default:
						Assert(false); // ToDo - implement in the future!!
						return false;
					}

				case Parking:
				case Position:
				case Pavement:
					if (GetLastItemTypeDecoded() == Parking)
					{
						// Adjacent to parking
					}
				case BasicComponent:
					BitDecodeAndContinue(ItemClone, DataPtr);

				case ComplexStructure:

				case SubItem:
					// Break; - Intentionally!
				default:
					ASSERT(false);
					return false;
				}
			}

			break; // the for loop
		}
	}

	UsedBits = DataPtr - Data;
	return false;

}

IItem *CBarcodeParkingMapDecoder::GetLastItemDecoded()
{
	if (m_ExtractedItemList.size() >= 1)
		return m_ExtractedItemList[m_ExtractedItemList.size()-1];
	return NULL;
}

IItem *CBarcodeParkingMapDecoder::GetPrecedingLastItemDecoced()
{
	if (m_ExtractedItemList.size() >= 2)
		return m_ExtractedItemList[m_ExtractedItemList.size()-2];
	return NULL;
}

EItemType CBarcodeParkingMapDecoder::GetLastItemTypeDecoded()
{
	if (GetLastItemDecoded())
		return GetLastItemDecoded()->GetType();
	else
		return SubItem;
}

EItemType CBarcodeParkingMapDecoder::GetPrecedingLastItemTypeDecoced()
{
	if (GetPrecedingLastItemDecoced())
		return GetPrecedingLastItemDecoced()->GetType();
	else
		return SubItem;
}