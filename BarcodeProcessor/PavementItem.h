#pragma once
#include "iitem.h"
#include <vector>

enum EShapeType
{
	Wall,
	Right_Triangle,
	Triangle,
	Rect,
	Quadrangle,
	Pentagon,
	EShapeType_MaxEnum
};
DefineEnumBitSize(EShapeType);

enum ETextureType
{
	Texture_Pavement,
	Texture_Building,
	Texture_NoParking,
	SolidFill,
	ETextureType_MaxEnum
};
DefineEnumBitSize(ETextureType);

enum ECurvatureType
{
	Linear,
	Convex,
	Concave,
	Rectangular,
	ECurvatureType_MaxEnum
};
DefineEnumBitSize(ECurvatureType);

struct SCurvatureParameters
{
	ECurvatureType Type;
	bool IsCircular;

	SCurvatureParameters() {ZeroMemory(this, sizeof(*this));}
	SCurvatureParameters(	ECurvatureType _Type, bool _IsCircular)
		: Type(_Type), IsCircular(_IsCircular) {}
};


struct SVertexParameters
{
	ECurvatureType CurvatureType;
	bool IsCircular;
		
	union UVertexCoordinates
	{
		Int8Bit Size8bits;
		Int6Bit Size6bits;
	}X, Y;

	SVertexParameters() {ZeroMemory(this, sizeof(*this));}
	SVertexParameters(	ECurvatureType _CurvatureType,
						bool _IsCircular,
						SVertexParameters::UVertexCoordinates _X,
						SVertexParameters::UVertexCoordinates _Y)
		: CurvatureType(_CurvatureType), IsCircular(_IsCircular), X(_X), Y(_Y) {}
};

union USizeOrSide
{
	bool Side;
	bool ReducedSize;
};

class CPavementItem : public IItem
{
public:
	CPavementItem(void);
	~CPavementItem(void);

	void Encode(	EShapeType ShapeType, ETextureType TextureType, Int4Bit FillType,
					bool IsAdjacentToParking, USizeOrSide SizeOrSide, std::vector<SVertexParameters> ListOfVertices,
					SCurvatureParameters SpecialVertexCurvature, Int6Bit ShortenVertexCoordinate);


	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context);

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);


	// Decoded data:
	EShapeType						m_ShapeType;
//	Int2Bit							m_ShapeID;
	ETextureType					m_TextureType;
	Int4Bit							m_FillType;
	bool							m_IsAdjacentToParking;
	USizeOrSide						m_SizeOrSide;
	std::vector<SVertexParameters>	m_ListOfVertices;
	SCurvatureParameters			m_SpecialVertexCurvature;
	Int6Bit							m_ShortenVertexCoordinate;
};
