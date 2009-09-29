#include "StdAfx.h"
#include "ComplexItem.h"
#include "BitLib.h"

CComplexItem::CComplexItem(void) : IItem(ComplexStructure),
	m_UID(ConvertIntToInt5Bit(0)), m_IsFirstDefinitionOfComplexItem(false), m_NumberOfObjectsInComplex(ConvertIntToInt5Bit(0)),
	m_IsVerticalMirror(false), m_IsHorizontalMirror(false), m_IsVeritcalReplication(false), 
	m_IsHotizontalReplication(false), m_IsReplicationPartOfDefinition(false)
{
}

CComplexItem::~CComplexItem(void)
{
}


/*virtual*/ void CComplexItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 1;
	(GetBitBuffer())[3] = 1;
	(GetBitBuffer())[4] = 0;
}


/*virtual*/ bool CComplexItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 1) return false;
	if (*Data[3] != 1) return false;
	if (*Data[4] != 0) return false;
	return true;
}

void CComplexItem::Encode(int ComplexItemUID, std::vector<IItem *> ListOfEncodedItems,
			bool IsVerticalMirror, bool ISHorizontalMirror, 
			bool IsVeritcalReplication, bool IsHotizontalReplication, bool IsReplicationPartOfDefinition,
			SReplication *VeritcalReplication/* = NULL*/, SReplication *HotizontalReplication/* = NULL*/)
{
	m_IsFirstDefinitionOfComplexItem = true;

	Int5Bit UID = ConvertIntToInt5Bit(ComplexItemUID);
	Int5Bit NumberOfItems = ConvertIntToInt5Bit(ListOfEncodedItems.size());

	size_t TotalNumberOfBitsInItems = 0;
	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		TotalNumberOfBitsInItems += ListOfEncodedItems[i]->GetBitBufferSize();
	}

	size_t NumberOfBits	= BitSize(UID) 
		+ BitSize(NumberOfItems) 
		+ BitSize(IsVerticalMirror)
		+ BitSize(ISHorizontalMirror)
		+ BitSize(IsVeritcalReplication)
		+ BitSize(IsHotizontalReplication)
		+ BitSize(IsReplicationPartOfDefinition)
		+ ((IsVeritcalReplication) ? BitSize(*VeritcalReplication) : 0)
		+ ((IsHotizontalReplication) ? BitSize(*HotizontalReplication) : 0)
		+ TotalNumberOfBitsInItems;
	
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, UID);
	BitCopyAndContinue(BitPtr, NumberOfItems);
	BitCopyAndContinue(BitPtr, IsVerticalMirror);
	BitCopyAndContinue(BitPtr, ISHorizontalMirror);
	BitCopyAndContinue(BitPtr, IsVeritcalReplication);
	BitCopyAndContinue(BitPtr, IsHotizontalReplication);
	BitCopyAndContinue(BitPtr, IsReplicationPartOfDefinition);
	if (IsVeritcalReplication)
	{
		BitCopyAndContinue(BitPtr, VeritcalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, VeritcalReplication->TimesToReplicate);
	}
	if (IsHotizontalReplication)
	{
		BitCopyAndContinue(BitPtr, HotizontalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, HotizontalReplication->TimesToReplicate);
	}

	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		BitCopyItemAndContinue(BitPtr, *ListOfEncodedItems[i]);
	}
}

void CComplexItem::Encode(int ComplexItemUID, bool IsVerticalMirror, bool ISHorizontalMirror, 
						  bool IsVeritcalReplication, bool IsHotizontalReplication,
						  SReplication *VeritcalReplication/* = NULL*/, SReplication *HotizontalReplication /*= NULL*/)
{
	Int5Bit UID = ConvertIntToInt5Bit(ComplexItemUID);
		
	size_t NumberOfBits	= BitSize(UID)
		+ BitSize(IsVerticalMirror)
		+ BitSize(ISHorizontalMirror)
		+ BitSize(IsVeritcalReplication)
		+ BitSize(IsHotizontalReplication)
		+ ((IsVeritcalReplication) ? BitSize(*VeritcalReplication) : 0)
		+ ((IsHotizontalReplication) ? BitSize(*HotizontalReplication) : 0);

	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, UID);
	BitCopyAndContinue(BitPtr, IsVerticalMirror);
	BitCopyAndContinue(BitPtr, ISHorizontalMirror);
	BitCopyAndContinue(BitPtr, IsVeritcalReplication);
	BitCopyAndContinue(BitPtr, IsHotizontalReplication);
	if (IsVeritcalReplication)
	{
		BitCopyAndContinue(BitPtr, VeritcalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, VeritcalReplication->TimesToReplicate);
	}
	if (IsHotizontalReplication)
	{
		BitCopyAndContinue(BitPtr, HotizontalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, HotizontalReplication->TimesToReplicate);
	}

}


/*virtual */void CComplexItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context)
{
	if (UsedBits == 0)	// first phase
		DecodePhase1(Data, UsedBits);
	else				// second phase
		DecodePhase2(Data, UsedBits, Context);
}

/*virtual*/ void CComplexItem::DecodePhase1(IN const CBitPointer &Data, IN OUT int &UsedBits)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_UID);

	UsedBits = (int)(BitPtr - Data);
}

/*virtual*/ void CComplexItem::DecodePhase2(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context)
{
	bool IsFirstTimeDefinition = (*Context == 0);

	int BitsOffset = UsedBits;
	CBitPointer BitPtr = Data + BitsOffset;

	if (IsFirstTimeDefinition)
		BitPasteAndContinue(BitPtr, m_NumberOfObjectsInComplex);


	BitPasteAndContinue(BitPtr, m_IsVerticalMirror);
	BitPasteAndContinue(BitPtr, m_IsHorizontalMirror);
	BitPasteAndContinue(BitPtr, m_IsVeritcalReplication);
	BitPasteAndContinue(BitPtr, m_IsHotizontalReplication);

	if (IsFirstTimeDefinition)
		BitPasteAndContinue(BitPtr, m_IsReplicationPartOfDefinition);


	if (m_IsVeritcalReplication)
	{
		BitPasteAndContinue(BitPtr, m_VeritcalReplication.GapBetweenReplicas);
		BitPasteAndContinue(BitPtr, m_VeritcalReplication.TimesToReplicate);
	}
	if (m_IsHotizontalReplication)
	{
		BitPasteAndContinue(BitPtr, m_HotizontalReplication.GapBetweenReplicas);
		BitPasteAndContinue(BitPtr, m_HotizontalReplication.TimesToReplicate);
	}

	//  Complex item will NOT decode its own items, but will let the ParkingMapDecoder to do it
	//	for it and later call AddDecodedItemToList, because otherwise the Complex Item would need to know
	//  all other item types, which is not really good design. Alternately, it could ask the Decoder to give it
	//  the appropriate item and take if from there, but since there's Decoder intervention anyway, it can 
	//  simply decode all the items itself and add them to the complex.
	// 	if (IsFirstTimeDefinition)
	//		for (unsigned int i = 0; i < (int)m_NumberOfObjectsInComplex; i++)
	// 		{}

	UsedBits = (int)(BitPtr - Data);
}

/*virtual */void CComplexItem::AddDecodedItemToList(IItem *ItemWithinComplex)
{
	m_ListOfEncodedItems.push_back(ItemWithinComplex);
	ASSERT(m_ListOfEncodedItems.size() <= (unsigned int)m_NumberOfObjectsInComplex);
}