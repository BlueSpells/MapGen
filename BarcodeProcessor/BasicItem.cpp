#include "StdAfx.h"
#include "BasicItem.h"
#include "BitLib.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBasicItem::CBasicItem(void) : IItem(BasicComponent),
	m_BasicItemType((EBasicItemType)0xFFFFFFFF),
	m_IsScaled(0)
{
	ZeroMemory(&m_X, sizeof m_X);
	ZeroMemory(&m_Y, sizeof m_Y);
}

CBasicItem::~CBasicItem(void)
{
}


void CBasicItem::Encode(EBasicItemType BasicItemType, bool IsScaled, UScalingCoordinates X, UScalingCoordinates Y)
{
	size_t NumberOfBits	=	BitSize(BasicItemType);
	if (!((BasicItemType == Door) || (BasicItemType == CarGate_Left) || (BasicItemType == CarGate_Right) || (BasicItemType == CarGate_Down)))
	{
		NumberOfBits += BitSize(IsScaled);
		if (BasicItemType == Road)
		{
			if (IsScaled)
			{
				NumberOfBits += BitSize(X.Size16bits) + BitSize(Y.Size16bits);
			}
			else
			{
				NumberOfBits += BitSize(X.Size10bits) + BitSize(Y.Size10bits);
			}
		}
		else if (IsScaled)
		{
			NumberOfBits +=  BitSize(X.Size6bits) + BitSize(Y.Size6bits);
		}
	}
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, BasicItemType);

	if (!((BasicItemType == Door) || (BasicItemType == CarGate_Left) || (BasicItemType == CarGate_Right) || (BasicItemType == CarGate_Down)))
	{
		BitCopyAndContinue(BitPtr, IsScaled);
		if (BasicItemType == Road)
		{
			if (IsScaled) 
			{
				BitCopyAndContinue(BitPtr, X.Size16bits);
				BitCopyAndContinue(BitPtr, Y.Size16bits);
			}
			else
			{
				BitCopyAndContinue(BitPtr, X.Size10bits);
				BitCopyAndContinue(BitPtr, Y.Size10bits);
			}
		}
		else if (IsScaled)
		{
			BitCopyAndContinue(BitPtr, X.Size6bits);
			BitCopyAndContinue(BitPtr, Y.Size6bits);
		}
	}
}


/*virtual*/ void CBasicItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_BasicItemType);
	if (!((m_BasicItemType == Door) || (m_BasicItemType == CarGate_Left) || (m_BasicItemType == CarGate_Right) || (m_BasicItemType == CarGate_Down)))
	{
		BitPasteAndContinue(BitPtr, m_IsScaled);
		if (m_BasicItemType == Road)
		{
			if (m_IsScaled) 
			{
				BitPasteAndContinue(BitPtr, m_X.Size16bits);
				BitPasteAndContinue(BitPtr, m_Y.Size16bits);
			}
			else
			{
				BitPasteAndContinue(BitPtr, m_X.Size10bits);
				BitPasteAndContinue(BitPtr, m_Y.Size10bits);
			}
		}
		else if (m_IsScaled)
		{
			BitPasteAndContinue(BitPtr, m_X.Size6bits);
			BitPasteAndContinue(BitPtr, m_Y.Size6bits);
		}
	}
	UsedBits = (int)(BitPtr - Data);
}


/*virtual*/	std::string	CBasicItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	AddItemToBitString(m_BasicItemType, BitPtr, ParsedString);

	if (!((m_BasicItemType == Door) || (m_BasicItemType == CarGate_Left) || (m_BasicItemType == CarGate_Right) || (m_BasicItemType == CarGate_Down)))
	{
		AddItemToBitString(m_IsScaled, BitPtr, ParsedString);
		if (m_BasicItemType == Road)
		{
			if (m_IsScaled) 
			{
				AddItemToBitString(m_X.Size16bits, BitPtr, ParsedString);
				AddItemToBitString(m_Y.Size16bits, BitPtr, ParsedString);
			}
			else
			{
				AddItemToBitString(m_X.Size10bits, BitPtr, ParsedString);
				AddItemToBitString(m_Y.Size10bits, BitPtr, ParsedString);
			}
		}
		else if (m_IsScaled)
		{
			AddItemToBitString(m_X.Size6bits, BitPtr, ParsedString);
			AddItemToBitString(m_Y.Size6bits, BitPtr, ParsedString);
		}
	}

	return ParsedString;
}


/*virtual*/ void CBasicItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 1;
	(GetBitBuffer())[3] = 0;
}

/*virtual*/ bool CBasicItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 1) return false;
	if (*Data[3] != 0) return false;
	return true;
}

