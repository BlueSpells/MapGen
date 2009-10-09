#include "StdAfx.h"
#include "PavementItem.h"
#include "BitLib.h"


CPavementItem::CPavementItem(void) : IItem(Pavement),
	m_ShapeType((EShapeType)0),
	m_TextureType((ETextureType)0),
	m_FillType(ConvertIntToInt4Bit(0)),
	m_IsAdjacentToParking(0),
	m_SpecialVertexCurvature((ECurvatureType)0),
	m_ShortenVertexCoordinate(ConvertIntToInt6Bit(0))
{
	ZeroMemory(&m_SizeOrSide, sizeof m_SizeOrSide);
}

CPavementItem::~CPavementItem(void)
{
}


void CPavementItem::Encode(	EShapeType ShapeType, ETextureType TextureType, Int4Bit FillType,
			bool IsAdjacentToParking, USizeOrSide SizeOrSide, std::vector<SVertexParameters> ListOfVertices,
			ECurvatureType SpecialVertexCurvature, Int6Bit ShortenVertexCoordinate)
{
	size_t NumberOfBits	= BitSize(ShapeType) 
						+ ((!IsAdjacentToParking) ? BitSize(TextureType) : 0 )
						+ (((TextureType == SolidFill) && (!IsAdjacentToParking)) ? BitSize(FillType) : 0)
						+ BitSize(IsAdjacentToParking)
						+ ((IsAdjacentToParking) ? BitSize(SizeOrSide.Side) : BitSize(SizeOrSide.Size))
						+ BitSize(SpecialVertexCurvature)
						+ ((IsAdjacentToParking) ? BitSize(ShortenVertexCoordinate) : 0);


	if (IsAdjacentToParking || SizeOrSide.Size)
	{	//6 bit
		NumberOfBits += ListOfVertices.size() * (  BitSize(ListOfVertices[0].X.Size6bits)
												 + BitSize(ListOfVertices[0].Y.Size6bits) 
												 + BitSize(ListOfVertices[0].CurvatureType));
	}
	else  
	{	// 8 bit
		NumberOfBits += ListOfVertices.size() * (  BitSize(ListOfVertices[0].X.Size8bits)
												 + BitSize(ListOfVertices[0].Y.Size8bits) 
												 + BitSize(ListOfVertices[0].CurvatureType));
	}
	
	IncreaseBitBufferSize(NumberOfBits);

	CBitPointer BitPtr = AllocateBitBuffer();
	BitCopyAndContinue(BitPtr, IsAdjacentToParking);
	if (IsAdjacentToParking)
	{
		BitCopyAndContinue(BitPtr, SizeOrSide.Side);
	}
	else
	{
		BitCopyAndContinue(BitPtr, SizeOrSide.Size);
	}
	BitCopyAndContinue(BitPtr, ShapeType);
	if (!IsAdjacentToParking)
	{
		BitCopyAndContinue(BitPtr, TextureType);
		if (TextureType == SolidFill)
			BitCopyAndContinue(BitPtr, FillType);
	}
	BitCopyAndContinue(BitPtr, SpecialVertexCurvature);
	if (IsAdjacentToParking)
		BitCopyAndContinue(BitPtr, ShortenVertexCoordinate);


	for (unsigned int i =0; i < ListOfVertices.size(); i++)
	{
		BitCopyAndContinue(BitPtr, ListOfVertices[i].CurvatureType);
		if (IsAdjacentToParking || SizeOrSide.Size)
		{ //6 bit
			BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size6bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size6bits);
		}
		else
		{ //8 bit
			BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size8bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size8bits);
		}
	}
}


/*virtual*/ void CPavementItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_IsAdjacentToParking);
	if (m_IsAdjacentToParking)
	{
		BitPasteAndContinue(BitPtr, m_SizeOrSide.Side);
	}
	else
	{
		BitPasteAndContinue(BitPtr, m_SizeOrSide.Size);
	}
	BitPasteAndContinue(BitPtr, m_ShapeType);
	if (!m_IsAdjacentToParking)
	{
		BitPasteAndContinue(BitPtr, m_TextureType);
		if (m_TextureType == SolidFill)
			BitPasteAndContinue(BitPtr, m_FillType);
	}
	BitPasteAndContinue(BitPtr, m_SpecialVertexCurvature);
	if (m_IsAdjacentToParking)
		BitPasteAndContinue(BitPtr, m_ShortenVertexCoordinate);

	unsigned int ListOfVerticesSize;
	if ((m_ShapeType == Rect) && (m_SpecialVertexCurvature == Rectangular))
	{
		ListOfVerticesSize = 1;
	}
	else
	{	
		ListOfVerticesSize = (int)m_ShapeType + 2 - (int)m_IsAdjacentToParking;
	}
	for (unsigned int i =0; i < ListOfVerticesSize; i++)
	{
		SVertexParameters FullVertex;
		BitPasteAndContinue(BitPtr, FullVertex.CurvatureType);
		if (m_IsAdjacentToParking || m_SizeOrSide.Size)
		{ //6 bit
			BitPasteAndContinue(BitPtr, FullVertex.X.Size6bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size6bits);
		}
		else
		{ //8 bit
			BitPasteAndContinue(BitPtr, FullVertex.X.Size8bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size8bits);
		}
		m_ListOfVertices.push_back(FullVertex);
	}

	UsedBits = (int)(BitPtr - Data);
}


/*virtual*/	std::string	CPavementItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, NULL);
	
	AddItemToBitString(m_IsAdjacentToParking, BitPtr, ParsedString);
	if (m_IsAdjacentToParking)
	{
		AddItemToBitString(m_SizeOrSide.Side, BitPtr, ParsedString);
	}
	else
	{
		AddItemToBitString(m_SizeOrSide.Size, BitPtr, ParsedString);
	}
	AddItemToBitString(m_ShapeType, BitPtr, ParsedString);
	if (!m_IsAdjacentToParking)
	{
		AddItemToBitString(m_TextureType, BitPtr, ParsedString);
		if (m_TextureType == SolidFill)
			AddItemToBitString(m_FillType, BitPtr, ParsedString);
	}
	AddItemToBitString(m_SpecialVertexCurvature, BitPtr, ParsedString);
	if (m_IsAdjacentToParking)
		AddItemToBitString(m_ShortenVertexCoordinate, BitPtr, ParsedString);

	unsigned int ListOfVerticesSize;
	if ((m_ShapeType == Rect) && (m_SpecialVertexCurvature == Rectangular))
	{
		ListOfVerticesSize = 1;
	}
	else
	{	
		ListOfVerticesSize = (int)m_ShapeType + 2 - (int)m_IsAdjacentToParking;
	}
	for (unsigned int i =0; i < ListOfVerticesSize; i++)
	{
		SVertexParameters FullVertex;
		AddItemToBitString(FullVertex.CurvatureType, BitPtr, ParsedString);
		if (m_IsAdjacentToParking || m_SizeOrSide.Size)
		{ //6 bit
			AddItemToBitString(FullVertex.X.Size6bits, BitPtr, ParsedString);
			AddItemToBitString(FullVertex.Y.Size6bits, BitPtr, ParsedString);
		}
		else
		{ //8 bit
			AddItemToBitString(FullVertex.X.Size8bits, BitPtr, ParsedString);
			AddItemToBitString(FullVertex.Y.Size8bits, BitPtr, ParsedString);
		}
	}

	return ParsedString;
}

/*virtual*/ void CPavementItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 0;
}


/*virtual*/ bool CPavementItem::IsOfThisType(CBitPointer *Data)
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 0) return false;
	return true;
}
