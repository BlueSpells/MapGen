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
						+ BitSize(TextureType) 
						+ ((TextureType == SolidFill) ? BitSize(FillType) : 0)
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
	BitCopyAndContinue(BitPtr, ShapeType);
	BitCopyAndContinue(BitPtr, TextureType);
	if (TextureType == SolidFill)
		BitCopyAndContinue(BitPtr, FillType);
	BitCopyAndContinue(BitPtr, IsAdjacentToParking);
	BitCopyAndContinue(BitPtr, SpecialVertexCurvature);
	if (IsAdjacentToParking)
	{
		BitCopyAndContinue(BitPtr, SizeOrSide.Side);
		BitCopyAndContinue(BitPtr, ShortenVertexCoordinate);
	}
	else
		BitCopyAndContinue(BitPtr, SizeOrSide.Size);


	if (IsAdjacentToParking || SizeOrSide.Size)
	{ //6 bit
		for (unsigned int i =0; i < ListOfVertices.size(); i++)
		{
			BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size6bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size6bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].CurvatureType);
		}
	}
	else
	{ //8 bit
		for (unsigned int i =0; i < ListOfVertices.size(); i++)
		{
			BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size8bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size8bits);
			BitCopyAndContinue(BitPtr, ListOfVertices[i].CurvatureType);
		}
	}
}


/*virtual*/ void CPavementItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/)
{
	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

	BitPasteAndContinue(BitPtr, m_ShapeType);
	BitPasteAndContinue(BitPtr, m_TextureType);
	if (m_TextureType == SolidFill)
		BitPasteAndContinue(BitPtr, m_FillType);
	BitPasteAndContinue(BitPtr, m_IsAdjacentToParking);
	BitCopyAndContinue(BitPtr, m_SpecialVertexCurvature);
	if (m_IsAdjacentToParking)
	{
		BitCopyAndContinue(BitPtr, m_SizeOrSide.Side);
		BitCopyAndContinue(BitPtr, m_ShortenVertexCoordinate);
	}
	else
		BitCopyAndContinue(BitPtr, m_SizeOrSide.Size);



	int ListOfVerticesSize = int(m_ShapeType) + 2 - int(m_IsAdjacentToParking);

	if (m_IsAdjacentToParking || m_SizeOrSide.Size)
	{ //6 bit
		for (int i =0; i < ListOfVerticesSize; i++)
		{
			SVertexParameters FullVertex;
			BitPasteAndContinue(BitPtr, FullVertex.X.Size6bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size6bits);
			BitPasteAndContinue(BitPtr, FullVertex.CurvatureType);
			m_ListOfVertices.push_back(FullVertex);
		}
	}
	else  
	{ //8 bit
		for (int i = 0; i < ListOfVerticesSize; i++)
		{
			SVertexParameters FullVertex;
			BitPasteAndContinue(BitPtr, FullVertex.X.Size8bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size8bits);
			BitPasteAndContinue(BitPtr, FullVertex.CurvatureType);
			m_ListOfVertices.push_back(FullVertex);
		}
	}

	UsedBits = (int)(BitPtr - Data);
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
