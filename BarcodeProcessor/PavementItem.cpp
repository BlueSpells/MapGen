#include "StdAfx.h"
#include "PavementItem.h"
#include "BitLib.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPavementItem::CPavementItem(void) : IItem(Pavement),
	m_ShapeType((EShapeType)0),
//	m_ShapeID(ConvertIntToInt2Bit(0)),
	m_TextureType((ETextureType)0),
	m_FillType(ConvertIntToInt4Bit(0)),
	m_IsAdjacentToParking(0),
	m_ShortenVertexCoordinate(ConvertIntToInt6Bit(0))
{
	ZeroMemory(&m_SizeOrSide, sizeof m_SizeOrSide);
}

CPavementItem::~CPavementItem(void)
{
}


void CPavementItem::Encode(	EShapeType ShapeType, ETextureType TextureType, Int4Bit FillType,
						   bool IsAdjacentToParking, USizeOrSide SizeOrSide, std::vector<SVertexParameters> ListOfVertices,
						   SCurvatureParameters SpecialVertexCurvature, Int6Bit ShortenVertexCoordinate)
{

// Note, this is a hack!:
// Normally Encode does not touch local members, but it does fill up the following
// member for the sake of GetItemBitBufferParsedString accurate interpretation.
	m_IsAdjacentToParking = IsAdjacentToParking; 

	Int2Bit ShapeID;
	bool ListOfVerticesSizeCheck = false;  //Checking if number of given vertexes is correct
	switch (ShapeType)
	{
		case Wall:
			ShapeID = ConvertIntToInt2Bit(0);
			if ((IsAdjacentToParking && (ListOfVertices.size() == 0)) || (!IsAdjacentToParking && (ListOfVertices.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Right_Triangle:
			ShapeID = ConvertIntToInt2Bit(((IsAdjacentToParking) ? 1 : 0));
			if ((IsAdjacentToParking && (ListOfVertices.size() == 0)) || (!IsAdjacentToParking && (ListOfVertices.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Triangle:
			ShapeID = ConvertIntToInt2Bit(1);
			if ((IsAdjacentToParking && (ListOfVertices.size() == 0)) || (!IsAdjacentToParking && (ListOfVertices.size() == 2)))
				ListOfVerticesSizeCheck = true;
			break;
		case Rect:
			ShapeID = ConvertIntToInt2Bit(((IsAdjacentToParking) ? 1 : 0));
			if ((IsAdjacentToParking && (ListOfVertices.size() == 0)) || (!IsAdjacentToParking && (ListOfVertices.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Quadrangle:
			ShapeID = ConvertIntToInt2Bit(2);
			if ((IsAdjacentToParking && (ListOfVertices.size() == 2)) || (!IsAdjacentToParking && (ListOfVertices.size() == 3)))
				ListOfVerticesSizeCheck = true;
			break;
		case Pentagon:
			ShapeID = ConvertIntToInt2Bit(3);
			if ((IsAdjacentToParking && (ListOfVertices.size() == 3)) || (!IsAdjacentToParking && (ListOfVertices.size() == 4)))
				ListOfVerticesSizeCheck = true;
			break;
		default:
			ShapeID = ConvertIntToInt2Bit(0);
	}


	if (ListOfVerticesSizeCheck)
	{
		size_t NumberOfBits	= BitSize(ShapeID)
							+ ((!(ShapeType == Wall) && IsAdjacentToParking) ? BitSize(ShortenVertexCoordinate) : 0)
							+ ((!IsAdjacentToParking) ? BitSize(TextureType) : 0 )
							+ (((TextureType == SolidFill) && (!IsAdjacentToParking)) ? BitSize(FillType) : 0)
	//						+ BitSize(IsAdjacentToParking)
							+ ((IsAdjacentToParking) ? 0 : BitSize(SizeOrSide.ReducedSize));

		if (!((ShapeType == Wall) && IsAdjacentToParking))
		{
			NumberOfBits += BitSize(SpecialVertexCurvature.Type);
			if ((SpecialVertexCurvature.Type == Convex) || (SpecialVertexCurvature.Type == Concave))
				NumberOfBits += BitSize(SpecialVertexCurvature.IsCircular);
		}

		if (IsAdjacentToParking && (!(ShapeType == Wall)))
			NumberOfBits += BitSize(SizeOrSide.Side);
		
		if ((ShapeType == Quadrangle) || (ShapeType == Pentagon) || (! IsAdjacentToParking))
		{
			NumberOfBits += BitSize(ListOfVertices[0].CurvatureType);
			if ((ListOfVertices[0].CurvatureType == Convex) || (ListOfVertices[0].CurvatureType == Concave))
				NumberOfBits += BitSize(ListOfVertices[0].IsCircular);

			if (IsAdjacentToParking || SizeOrSide.ReducedSize)
			{	//6 bit
				NumberOfBits += ListOfVertices.size() * (  BitSize(ListOfVertices[0].X.Size6bits)
														 + BitSize(ListOfVertices[0].Y.Size6bits));
			}
			else  
			{	// 8 bit
				NumberOfBits += ListOfVertices.size() * (  BitSize(ListOfVertices[0].X.Size8bits)
														 + BitSize(ListOfVertices[0].Y.Size8bits));
			}
		}
		


		IncreaseBitBufferSize(NumberOfBits);

		CBitPointer BitPtr = AllocateBitBuffer();
	//	BitCopyAndContinue(BitPtr, IsAdjacentToParking);

		BitCopyAndContinue(BitPtr, ShapeID);

		if (!((ShapeType == Wall) && IsAdjacentToParking))
		{
			BitCopyAndContinue(BitPtr, SpecialVertexCurvature.Type);
			if ((SpecialVertexCurvature.Type == Convex) || (SpecialVertexCurvature.Type == Concave))
				BitCopyAndContinue(BitPtr, SpecialVertexCurvature.IsCircular);
		}

		if ((!(ShapeType == Wall) && IsAdjacentToParking))
			BitCopyAndContinue(BitPtr, ShortenVertexCoordinate);
		
		if (IsAdjacentToParking)
		{
			if (!(ShapeType == Wall))
				BitCopyAndContinue(BitPtr, SizeOrSide.Side);
		}
		else
			BitCopyAndContinue(BitPtr, SizeOrSide.ReducedSize);


		for (unsigned int i =0; i < ListOfVertices.size(); i++)
		{
			if (IsAdjacentToParking || SizeOrSide.ReducedSize)
			{ //6 bit
				BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size6bits);
				BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size6bits);
			}
			else
			{ //8 bit
				BitCopyAndContinue(BitPtr, ListOfVertices[i].X.Size8bits);
				BitCopyAndContinue(BitPtr, ListOfVertices[i].Y.Size8bits);
			}

			BitCopyAndContinue(BitPtr, ListOfVertices[i].CurvatureType);
			if ((ListOfVertices[i].CurvatureType == Convex) || (ListOfVertices[i].CurvatureType == Concave))
				BitCopyAndContinue(BitPtr, ListOfVertices[i].IsCircular);
		}

		if (!IsAdjacentToParking && (ShapeType != Wall))
		{
			BitCopyAndContinue(BitPtr, TextureType);
			if (TextureType == SolidFill)
				BitCopyAndContinue(BitPtr, FillType);
		}
	}
}


/*virtual*/ void CPavementItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context)
{
	bool m_IsAdjacentToParking = *(bool *)Context;

	Int2Bit m_ShapeID;

	int BitsToRead = UsedBits; BitsToRead;
	CBitPointer BitPtr = Data + BitSize(GetType());

//	BitPasteAndContinue(BitPtr, m_IsAdjacentToParking);


	BitPasteAndContinue(BitPtr, m_ShapeID);
	int m_ShapeID_int = m_ShapeID;

	if (!((m_ShapeID_int == 0) && m_IsAdjacentToParking))
	{
		BitPasteAndContinue(BitPtr, m_SpecialVertexCurvature.Type);
		if ((m_SpecialVertexCurvature.Type == Convex) || (m_SpecialVertexCurvature.Type == Concave))
			BitPasteAndContinue(BitPtr, m_SpecialVertexCurvature.IsCircular);
	}

	if ((!(m_ShapeID_int == 0) && m_IsAdjacentToParking))
		BitPasteAndContinue(BitPtr, m_ShortenVertexCoordinate);


	if (m_IsAdjacentToParking)
	{
		if (!(m_ShapeID_int == 0))
			BitPasteAndContinue(BitPtr, m_SizeOrSide.Side);
	}
	else
		BitPasteAndContinue(BitPtr, m_SizeOrSide.ReducedSize);


	unsigned int ListOfVerticesSize;
	if (m_IsAdjacentToParking)
		ListOfVerticesSize = ((m_ShapeID_int < 2) ? 0 : m_ShapeID_int);
	else
		ListOfVerticesSize = m_ShapeID_int + 1;

	for (unsigned int i =0; i < ListOfVerticesSize; i++)
	{
		SVertexParameters FullVertex;

		if (m_IsAdjacentToParking || m_SizeOrSide.ReducedSize)
		{ //6 bit
			BitPasteAndContinue(BitPtr, FullVertex.X.Size6bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size6bits);
		}
		else
		{ //8 bit
			BitPasteAndContinue(BitPtr, FullVertex.X.Size8bits);
			BitPasteAndContinue(BitPtr, FullVertex.Y.Size8bits);
		}

		BitPasteAndContinue(BitPtr, FullVertex.CurvatureType);
		if ((FullVertex.CurvatureType == Convex) || (FullVertex.CurvatureType == Concave))
			BitPasteAndContinue(BitPtr, FullVertex.IsCircular);

		m_ListOfVertices.push_back(FullVertex);
	}

	switch (m_ShapeID_int)
	{
	case 0:
		if (m_IsAdjacentToParking)
			m_ShapeType = Wall;
		else
			if (m_SpecialVertexCurvature.Type != m_ListOfVertices[0].CurvatureType)
				m_ShapeType = Right_Triangle;
			else
				((m_SpecialVertexCurvature.Type == Rectangular) ? m_ShapeType = Rect : m_ShapeType = Wall);
		break;
	case 1:
		if (!m_IsAdjacentToParking)
			m_ShapeType = Triangle;
		else
			if (m_SpecialVertexCurvature.Type == Rectangular) 
				m_ShapeType = Rect;
			else 
				m_ShapeType = Triangle;
		break;
	case 2:
		m_ShapeType = Quadrangle;
		break;
	case 3:
		m_ShapeType = Pentagon;
		break;
	default:
		m_ShapeType = Wall;
	}

	if (!m_IsAdjacentToParking && (m_ShapeType != Wall))
	{
		BitPasteAndContinue(BitPtr, m_TextureType);
		if (m_TextureType == SolidFill)
			BitPasteAndContinue(BitPtr, m_FillType);
	}

	UsedBits = (int)(BitPtr - Data);
}


/*virtual*/	std::string	CPavementItem::GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr)
{
	int UsedBits = 0;
	Decode(GetBitBuffer(), UsedBits, (int*)&m_IsAdjacentToParking);
	
//	AddItemToBitString(m_IsAdjacentToParking, BitPtr, ParsedString);

	Int2Bit m_ShapeID;
	switch (m_ShapeType)
	{
	case Wall:
		m_ShapeID = ConvertIntToInt2Bit(0);
		break;
	case Right_Triangle:
		m_ShapeID = ConvertIntToInt2Bit(((m_IsAdjacentToParking) ? 1 : 0));
		break;
	case Triangle:
		m_ShapeID = ConvertIntToInt2Bit(1);
		break;
	case Rect:
		m_ShapeID = ConvertIntToInt2Bit(((m_IsAdjacentToParking) ? 1 : 0));
		break;
	case Quadrangle:
		m_ShapeID = ConvertIntToInt2Bit(2);
		break;
	case Pentagon:
		m_ShapeID = ConvertIntToInt2Bit(3);
		break;
	default:
		m_ShapeID = ConvertIntToInt2Bit(0);
	}

	
	AddItemToBitString(m_ShapeID, BitPtr, ParsedString);

	if (!((m_ShapeType == Wall) && m_IsAdjacentToParking))
	{
		AddItemToBitString(m_SpecialVertexCurvature.Type, BitPtr, ParsedString);
		if ((m_SpecialVertexCurvature.Type == Convex) || (m_SpecialVertexCurvature.Type == Concave))
			AddItemToBitString(m_SpecialVertexCurvature.IsCircular, BitPtr, ParsedString);
	}

	if ((!(m_ShapeType == Wall) && m_IsAdjacentToParking))
		AddItemToBitString(m_ShortenVertexCoordinate, BitPtr, ParsedString);


	if (m_IsAdjacentToParking)
	{
		if (!(m_ShapeType == Wall))
			AddItemToBitString(m_SizeOrSide.Side, BitPtr, ParsedString);
	}
	else
		AddItemToBitString(m_SizeOrSide.ReducedSize, BitPtr, ParsedString);


	unsigned int ListOfVerticesSize;
	int m_ShapeID_int = m_ShapeID;
	if (m_IsAdjacentToParking)
		ListOfVerticesSize = ((m_ShapeID_int < 2) ? 0 : m_ShapeID_int);
	else
		ListOfVerticesSize = m_ShapeID_int + 1;

	for (unsigned int i =0; i < ListOfVerticesSize; i++)
	{
		if (m_IsAdjacentToParking || m_SizeOrSide.ReducedSize)
		{ //6 bit
			AddItemToBitString(m_ListOfVertices[i].X.Size6bits, BitPtr, ParsedString);
			AddItemToBitString(m_ListOfVertices[i].Y.Size6bits, BitPtr, ParsedString);
		}
		else
		{ //8 bit
			AddItemToBitString(m_ListOfVertices[i].X.Size8bits, BitPtr, ParsedString);
			AddItemToBitString(m_ListOfVertices[i].Y.Size8bits, BitPtr, ParsedString);
		}

		AddItemToBitString(m_ListOfVertices[i].CurvatureType, BitPtr, ParsedString);
		if ((m_ListOfVertices[i].CurvatureType == Convex) || (m_ListOfVertices[i].CurvatureType == Concave))
			AddItemToBitString(m_ListOfVertices[i].IsCircular, BitPtr, ParsedString);
	}

	if (!m_IsAdjacentToParking && (m_ShapeType != Wall))
	{
		AddItemToBitString(m_TextureType, BitPtr, ParsedString);
		if (m_TextureType == SolidFill)
			AddItemToBitString(m_FillType, BitPtr, ParsedString);
	}

	return ParsedString;
}

/*virtual*/ void CPavementItem::InsertItemType()
{
	(GetBitBuffer())[0] = 1;
	(GetBitBuffer())[1] = 1;
	(GetBitBuffer())[2] = 0;
}


/*virtual*/ bool CPavementItem::IsOfThisType(CBitPointer *Data) const
{
	if (*Data[0] != 1) return false;
	if (*Data[1] != 1) return false;
	if (*Data[2] != 0) return false;
	return true;
}
