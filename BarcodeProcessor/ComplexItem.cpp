#include "StdAfx.h"
#include "ComplexItem.h"
#include "BitLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComplexItem::CComplexItem(void) : IItem(ComplexStructure),
	m_UID(ConvertIntToInt5Bit(0)), m_IsFirstDefinitionOfComplexItem(false), m_NumberOfObjectsInComplex(ConvertIntToInt5Bit(0)),
	m_IsVerticalMirror(false), m_IsHorizontalMirror(false), m_IsVerticalReplication(false), 
	m_IsHorizontalReplication(false), m_IsReplicationPartOfDefinition(false)
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


void CComplexItem::Encode(Int5Bit ComplexItemUID, Int5Bit NumberOfEncodedItems,
			bool IsVerticalMirror, bool IsHorizontalMirror, 
			bool IsVerticalReplication, bool IsHorizontalReplication, bool IsReplicationPartOfDefinition,
			SReplication *VerticalReplication /*= NULL*/, SReplication *HorizontalReplication /*= NULL*/)
{
	m_IsFirstDefinitionOfComplexItem = true;

//	Int5Bit UID = ConvertIntToInt5Bit(ComplexItemUID);
//	Int5Bit NumberOfObjectsInComplex = ConvertIntToInt5Bit(NumberOfEncodedItems);

	size_t NumberOfBits	= BitSize(ComplexItemUID) 
		+ BitSize(NumberOfEncodedItems) 
		+ BitSize(IsVerticalMirror)
		+ BitSize(IsHorizontalMirror)
		+ BitSize(IsVerticalReplication)
		+ BitSize(IsHorizontalReplication)
		+ BitSize(IsReplicationPartOfDefinition)
		+ ((IsVerticalReplication) ? BitSize(*VerticalReplication) : 0)
		+ ((IsHorizontalReplication) ? BitSize(*HorizontalReplication) : 0);

	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, ComplexItemUID);
	BitCopyAndContinue(BitPtr, NumberOfEncodedItems);
	BitCopyAndContinue(BitPtr, IsVerticalMirror);
	BitCopyAndContinue(BitPtr, IsHorizontalMirror);
	BitCopyAndContinue(BitPtr, IsVerticalReplication);
	BitCopyAndContinue(BitPtr, IsHorizontalReplication);
	BitCopyAndContinue(BitPtr, IsReplicationPartOfDefinition);
	if (IsVerticalReplication)
	{
		BitCopyAndContinue(BitPtr, VerticalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, VerticalReplication->TimesToReplicate);
	}
	if (IsHorizontalReplication)
	{
		BitCopyAndContinue(BitPtr, HorizontalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, HorizontalReplication->TimesToReplicate);
	}
}


void CComplexItem::Encode(Int5Bit ComplexItemUID, std::vector<IItem *> ListOfEncodedItems,
			bool IsVerticalMirror, bool IsHorizontalMirror, 
			bool IsVerticalReplication, bool IsHorizontalReplication, bool IsReplicationPartOfDefinition,
			SReplication *VerticalReplication/* = NULL*/, SReplication *HorizontalReplication/* = NULL*/)
{
	size_t TotalNumberOfBitsInItems = 0;
	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		TotalNumberOfBitsInItems += ListOfEncodedItems[i]->GetBitBufferSize();
	}
	IncreaseBitBufferSize(TotalNumberOfBitsInItems);


	Int5Bit NumberOfItems = ConvertIntToInt5Bit(ListOfEncodedItems.size());
	Encode(ComplexItemUID, NumberOfItems, IsVerticalMirror, IsHorizontalMirror, 
		IsVerticalReplication, IsHorizontalReplication, IsReplicationPartOfDefinition,
		VerticalReplication, HorizontalReplication);


	CBitPointer BitPtr = GetBitBuffer() + GetBitBufferSize() - TotalNumberOfBitsInItems;
	for (unsigned int i = 0; i < ListOfEncodedItems.size(); i++)
	{
		BitCopyItemAndContinue(BitPtr, *ListOfEncodedItems[i]);
	}
}

void CComplexItem::Encode(Int5Bit ComplexItemUID, bool IsVerticalMirror, bool IsHorizontalMirror, 
						  bool IsVerticalReplication, bool IsHorizontalReplication,
						  SReplication *VerticalReplication/* = NULL*/, SReplication *HorizontalReplication /*= NULL*/)
{
//	Int5Bit UID = ConvertIntToInt5Bit(ComplexItemUID);
		
	size_t NumberOfBits	= BitSize(ComplexItemUID)
		+ BitSize(IsVerticalMirror)
		+ BitSize(IsHorizontalMirror)
		+ BitSize(IsVerticalReplication)
		+ BitSize(IsHorizontalReplication)
		+ ((IsVerticalReplication) ? BitSize(*VerticalReplication) : 0)
		+ ((IsHorizontalReplication) ? BitSize(*HorizontalReplication) : 0);

	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, ComplexItemUID);
	BitCopyAndContinue(BitPtr, IsVerticalMirror);
	BitCopyAndContinue(BitPtr, IsHorizontalMirror);
	BitCopyAndContinue(BitPtr, IsVerticalReplication);
	BitCopyAndContinue(BitPtr, IsHorizontalReplication);
	if (IsVerticalReplication)
	{
		BitCopyAndContinue(BitPtr, VerticalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, VerticalReplication->TimesToReplicate);
	}
	if (IsHorizontalReplication)
	{
		BitCopyAndContinue(BitPtr, HorizontalReplication->GapBetweenReplicas);
		BitCopyAndContinue(BitPtr, HorizontalReplication->TimesToReplicate);
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
	BitPasteAndContinue(BitPtr, m_IsVerticalReplication);
	BitPasteAndContinue(BitPtr, m_IsHorizontalReplication);

	if (IsFirstTimeDefinition)
		BitPasteAndContinue(BitPtr, m_IsReplicationPartOfDefinition);


	if (m_IsVerticalReplication)
	{
		BitPasteAndContinue(BitPtr, m_VerticalReplication.GapBetweenReplicas);
		BitPasteAndContinue(BitPtr, m_VerticalReplication.TimesToReplicate);
	}
	if (m_IsHorizontalReplication)
	{
		BitPasteAndContinue(BitPtr, m_HorizontalReplication.GapBetweenReplicas);
		BitPasteAndContinue(BitPtr, m_HorizontalReplication.TimesToReplicate);
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

/*virtual*/	std::string	CComplexItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);

	int IsFirstTimeFlag = (m_IsFirstDefinitionOfComplexItem) ? 0 : 1;
	Decode(GetBitBuffer(), UsedBits, &IsFirstTimeFlag);

	// The above is the ComplexItem's version of Decode. 
	

	// From here should start the regular GetItemBitBufferParsedString implementation

	AddItemToBitString(m_UID, BitPtr, ParsedString);

	if (m_IsFirstDefinitionOfComplexItem)
		AddItemToBitString(m_NumberOfObjectsInComplex, BitPtr, ParsedString);

	AddItemToBitString(m_IsVerticalMirror, BitPtr, ParsedString);
	AddItemToBitString(m_IsHorizontalMirror, BitPtr, ParsedString);
	AddItemToBitString(m_IsVerticalReplication, BitPtr, ParsedString);
	AddItemToBitString(m_IsHorizontalReplication, BitPtr, ParsedString);
	
	if (m_IsFirstDefinitionOfComplexItem)
		AddItemToBitString(m_IsReplicationPartOfDefinition, BitPtr, ParsedString);

	if (m_IsVerticalReplication)
	{
		AddItemToBitString(m_VerticalReplication.GapBetweenReplicas, BitPtr, ParsedString);
		AddItemToBitString(m_VerticalReplication.TimesToReplicate, BitPtr, ParsedString);
	}
	if (m_IsHorizontalReplication)
	{
		AddItemToBitString(m_HorizontalReplication.GapBetweenReplicas, BitPtr, ParsedString);
		AddItemToBitString(m_HorizontalReplication.TimesToReplicate, BitPtr, ParsedString);
	}

	return ParsedString;
}