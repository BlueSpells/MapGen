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

	union UShape
	{
		Int6Bit SmallAngle;
		Int8Bit BigAngle;
		struct SRadiusCoordinates
		{
			Int8Bit X, Y;
		} RadiusCoordinates;
	}Shape;

	SStructureShape() {ZeroMemory(this, sizeof(*this));}
	SStructureShape(EAngleType _AngleType, SStructureShape::UShape _Shape)
		: AngleType(_AngleType), Shape(_Shape) {}
};
DefineEnumBitSize(SStructureShape::EAngleType);
DefineStructBitSize2(SStructureShape::UShape::SRadiusCoordinates, X, Y);
DefineUnionStructBitSize3(SStructureShape, AngleType, Shape, SmallAngle, BigAngle, RadiusCoordinates);

struct SMultiplicity
{
	bool IsMultiplied;
	Int6Bit Multiples;

	SMultiplicity() {ZeroMemory(this, sizeof(*this));}
	SMultiplicity(bool _IsMultiplied, int _Multiples = 0) 
		: IsMultiplied(_IsMultiplied), Multiples(ConvertIntToInt6Bit(_Multiples)) {}
};
DefineStructWithBoolBitSize2(SMultiplicity, IsMultiplied, Multiples);

class CItemStructure : public IItem
{
public:

	CItemStructure(void);
	~CItemStructure(void);

	void Encode(SStructureShape StructureShape, SMultiplicity Multiplicity);

	virtual void	InsertItemType() {}
	virtual bool	IsOfThisType(CBitPointer* /*Data*/) {return false;}
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);

	// Decoded data:
	SStructureShape m_StructureShape;
	SMultiplicity	m_Multiplicity;
};
