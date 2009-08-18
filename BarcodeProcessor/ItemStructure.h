#pragma once
#include "BitPointer.h"
#include "IItem.h"

struct SStructureShape
{
	enum EAngleType
	{
		None,
		AngleUpTo90,
		AngleUpTo360,
		Circular,
		EAngleType_MaxEnum
	}AngleType;

	union SShape
	{
		enum ESmallAngle
		{
			ESmallAngle_MaxEnum = 64
		} SmallAngle;
		enum EBigAngle
		{
			EBigAngle_MaxEnum = 256
		} BigAngle;
		struct SRadiusCoordinates
		{
			enum ERadiusCoordinateValue
			{
				ERadiusCoordinateValue_MaxEnum = 256
			} X, Y;
		} RadiusCoordinates;
	}Shape;

	SStructureShape() {ZeroMemory(this, sizeof(*this));}
	SStructureShape(EAngleType _AngleType, SStructureShape::SShape _Shape)
		: AngleType(_AngleType), Shape(_Shape) {}
};
DefineEnumBitSize(SStructureShape::EAngleType);
DefineEnumBitSize(SStructureShape::SShape::ESmallAngle);
DefineEnumBitSize(SStructureShape::SShape::EBigAngle);
DefineEnumBitSize(SStructureShape::SShape::SRadiusCoordinates::ERadiusCoordinateValue);
DefineStructBitSize2(SStructureShape::SShape::SRadiusCoordinates, X, Y);
DefineUnionStructBitSize3(SStructureShape, AngleType, Shape, SmallAngle, BigAngle, RadiusCoordinates);

struct SMultiplicity
{
	bool IsMultiplied;

	enum EMultiplied
	{
		EMultiplied_MaxEnum = 256
	}Multiples;

	SMultiplicity() {ZeroMemory(this, sizeof(*this));}
	SMultiplicity(bool _IsMultiplied, int _Multiples = 0) 
		: IsMultiplied(_IsMultiplied), Multiples((EMultiplied)_Multiples) {}
};
DefineEnumBitSize(SMultiplicity::EMultiplied);
DefineStructWithBoolBitSize2(SMultiplicity, IsMultiplied, Multiples);

class CItemStructure : public IItem
{
public:

	CItemStructure(void);
	~CItemStructure(void);

	void Encode(SStructureShape StructureShape, SMultiplicity Multiplicity);

	virtual void	InsertItemType() {}
	virtual bool	IsOfThisType(CBitPointer* /*Data*/) {return false;}
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits);

	// Decoded data:
	SStructureShape m_StructureShape;
	SMultiplicity	m_Multiplicity;
};
