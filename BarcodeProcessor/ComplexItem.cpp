#include "StdAfx.h"
#include "ComplexItem.h"
#include "BitLib.h"

CComplexItem::CComplexItem(void) : IItem(ComplexStructure)
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
}


/*virtual*/ bool CComplexItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 1) return false;
	if (*Data[3] != 1) return false;
	return true;
}

void CComplexItem::Encode(std::vector<IItem *> ListOfEncodedItems, int _ComplexItemUID, int _NumberOfItems, 
			bool IsVerticalMirror, bool ISHorizontalMirror, bool IsVeritcalReplication, bool IsHotizontalReplication,
			SReplication *VeritcalReplication /*= NULL*/, SReplication *HotizontalReplication /*= NULL*/)
{
	EComplexItemUID ComplexItemUID = (EComplexItemUID)_ComplexItemUID;
	ENumberOfItemsInComplexStructure NumberOfItems = (ENumberOfItemsInComplexStructure)_NumberOfItems;

	size_t TotalNumberOfBitsInItems = 0;
	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		TotalNumberOfBitsInItems += ListOfEncodedItems[i]->GetBitBufferSize();
	}

	size_t NumberOfBits	= BitSize(ComplexItemUID) 
		+ BitSize(NumberOfItems) 
		+ BitSize(IsVerticalMirror)
		+ BitSize(ISHorizontalMirror)
		+ BitSize(IsVeritcalReplication)
		+ BitSize(IsHotizontalReplication)
		+ ((IsVeritcalReplication) ? BitSize(*VeritcalReplication) : 0)
		+ ((IsHotizontalReplication) ? BitSize(*HotizontalReplication) : 0)
		+ TotalNumberOfBitsInItems;
	
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, ComplexItemUID);
	BitCopyAndContinue(BitPtr, NumberOfItems);
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

	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		BitCopyItemAndContinue(BitPtr, *ListOfEncodedItems[i]);
	}
}

void CComplexItem::Encode(int _ComplexItemUID)
{
	EComplexItemUID ComplexItemUID = (EComplexItemUID)_ComplexItemUID;

	size_t NumberOfBits	= BitSize(ComplexItemUID);

	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, ComplexItemUID);
}