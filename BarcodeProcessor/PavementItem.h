#pragma once
#include "iitem.h"
#include <vector>

enum EShapeType
{
	Triangle,
	Rect,
	Pentagon,
	Hexagon,
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

struct SVertexParameters
{
	ECurvatureType CurvatureType;
		
	union UVertexCoordinates
	{
		Int8Bit Size8bits;
		Int6Bit Size6bits;
	}X, Y;

	SVertexParameters() {ZeroMemory(this, sizeof(*this));}
	SVertexParameters(	ECurvatureType _CurvatureType, 
						SVertexParameters::UVertexCoordinates _X,
						SVertexParameters::UVertexCoordinates _Y)
		: CurvatureType(_CurvatureType), X(_X), Y(_Y) {}
};

union USizeOrSide
{
	bool Side;
	bool Size;
};

class CPavementItem : public IItem
{
public:
	CPavementItem(void);
	~CPavementItem(void);

	void Encode(	EShapeType ShapeType, ETextureType TextureType, Int4Bit FillType,
					bool IsAdjacentToParking, USizeOrSide SizeOrSide, std::vector<SVertexParameters> ListOfVertices,
					ECurvatureType SpecialVertexCurvature, Int6Bit ShortenVertexCoordinate);


	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);



	// Decoded data:
	EShapeType						m_ShapeType;
	ETextureType					m_TextureType;
	Int4Bit							m_FillType;
	bool							m_IsAdjacentToParking;
	USizeOrSide						m_SizeOrSide;
	std::vector<SVertexParameters>	m_ListOfVertices;
	ECurvatureType					m_SpecialVertexCurvature;
	Int6Bit							m_ShortenVertexCoordinate;
};
