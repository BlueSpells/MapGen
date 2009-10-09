#pragma once
#include "iitem.h"
#include "ItemHelper.h"
#include "ItemStructure.h"

enum EParkingAngle
{
	Perpendicular,
	Parallel,
	Right45Degrees,
	Left45Degrees,
	EParkingAngle_MaxEnum
};
DefineEnumBitSize(EParkingAngle);

enum EParkingType
{
	Regular,
	Handicap,
	Bikes,
	BigParking,
	EParkingType_MaxEnum
};
DefineEnumBitSize(EParkingType);

enum EParkingOrientation
{
	Degrees0,
	Degrees90,
	Degrees180,
	Degrees270,
	EParkingOrientation_MaxEnum
};
DefineEnumBitSize(EParkingOrientation);

struct SPeriodicBetweenPoles
{
	Int4Bit NumberOfParkingPlaces;
	enum EPolesType
	{
		MarksOnRoad,
		Circular,
		Squared,
		Wall,
		EPolesType_MaxEnum
	}PolesType;

	SPeriodicBetweenPoles(int _NumberOfParkingPlaces, EPolesType _PolesType) 
		: NumberOfParkingPlaces(ConvertIntToInt4Bit(_NumberOfParkingPlaces)), PolesType(_PolesType) {}
	SPeriodicBetweenPoles() {ZeroMemory(this, sizeof(*this));}
};
DefineEnumBitSize(SPeriodicBetweenPoles::EPolesType);
DefineStructBitSize2(SPeriodicBetweenPoles, NumberOfParkingPlaces, PolesType);

class CParkingItem : public IItem
{
public:
	CParkingItem(void);
	~CParkingItem(void);

	void Encode(	EParkingAngle ParkingAngle, EParkingType ParkingType, 
					bool IsMirrorDoubled, bool IsWallBetweenMirrors, EParkingOrientation Orientation,
					CItemStructure ItemStructure,
					bool IsPeriodicBetweenPoles, SPeriodicBetweenPoles *PeriodicBetweenPoles = NULL);

	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int* /*Context unnecessary*/);

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);


	// Decoded data:
	EParkingAngle			m_ParkingAngle;
	EParkingType			m_ParkingType; 
	bool					m_IsMirrorDoubled;
	bool					m_IsWallBetweenMirrors;
	EParkingOrientation		m_Orientation;
	CItemStructure			m_ItemStructure;
	bool					m_IsPeriodicBetweenPoles;
	SPeriodicBetweenPoles	m_PeriodicBetweenPoles;
};
