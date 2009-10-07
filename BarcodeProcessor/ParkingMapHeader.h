#pragma once
#include "iheader.h"
#include "ItemHelper.h"

enum ETextLanguage
{
	Empty,		// One space needs to be empty. Doesn't take bits.
	English,
	Hebrew,
	ETextLanguage_MaxEnum
};
DefineEnumBitSize(ETextLanguage);

struct SParkingGarageID
{
	Int8Bit A;
	Int8Bit B;
	Int8Bit C;
	Int8Bit D;
	SParkingGarageID(int _A, int _B, int _C, int _D)
		: A(ConvertIntToInt8Bit(_A)), B(ConvertIntToInt8Bit(_B)), C(ConvertIntToInt8Bit(_C)), D(ConvertIntToInt8Bit(_D)) {}
	SParkingGarageID() {ZeroMemory(this, sizeof(*this));}
};
DefineStructBitSize4(SParkingGarageID, A, B, C, D);

struct SParkingGarageDimensions
{
	Int16Bit Width;
	Int16Bit Height;
	SParkingGarageDimensions(int _Width, int _Height)
		: Width(ConvertIntToInt16Bit(_Width)), Height(ConvertIntToInt16Bit(_Height)) {}
	SParkingGarageDimensions() {ZeroMemory(this, sizeof(*this));}
};
DefineStructBitSize2(SParkingGarageDimensions, Width, Height);

struct SParkingSpaceDimensions
{
	Int8Bit Width;
	Int8Bit Height;
	SParkingSpaceDimensions(int _Width, int _Height)
		: Width(ConvertIntToInt8Bit(_Width)), Height(ConvertIntToInt8Bit(_Height)) {}
	SParkingSpaceDimensions() {ZeroMemory(this, sizeof(*this));}
};
DefineStructBitSize2(SParkingSpaceDimensions, Width, Height);

struct SFloorAndSection
{
	Int4Bit Floor;
	Int4Bit Section;

	SFloorAndSection(int _Floor, int _Section)
		: Floor(ConvertIntToInt4Bit(_Floor)), Section(ConvertIntToInt4Bit(_Section)) {}
	SFloorAndSection() {ZeroMemory(this, sizeof(*this));}
};
DefineStructBitSize2(SFloorAndSection, Floor, Section);

struct SPositionInParking
{
	Int8Bit X;
	Int8Bit Y;
	SPositionInParking(int _X, int _Y)
		: X(ConvertIntToInt8Bit(_X)), Y(ConvertIntToInt8Bit(_Y)) {}
	SPositionInParking() {ZeroMemory(this, sizeof(*this));}
};
DefineStructBitSize2(SPositionInParking, X, Y);

class CParkingMapHeader : public IHeader
{
public:
	CParkingMapHeader(void);
	~CParkingMapHeader(void);

	SParkingGarageID m_GarageID;
	ETextLanguage m_TextLanguage;
	SParkingGarageDimensions m_GarageDimensions;
	SParkingSpaceDimensions	 m_ParkingSpaceDimensions;
	SFloorAndSection		 m_FloorAndSection;
	SPositionInParking		 m_CurrentPosition;
	bool					 m_IsTargetPosition;
	bool					 m_IsTargetDescription;
	SPositionInParking		 m_TargetPosition;
	char					 m_TargetDescription[MAX_STR_LEN];

	void Encode(SParkingGarageID GarageID, ETextLanguage TextLanguage, 
		SParkingGarageDimensions GarageDimensions, 
		SParkingSpaceDimensions	 ParkingSpaceDimensions,
		SFloorAndSection		 FloorAndSection,
		SPositionInParking		 CurrentPosition,
		bool					 IsTargetPosition,
		bool					 IsTargetDescription,
		SPositionInParking		 TargetPosition,
		char					 TargetDescription[MAX_STR_LEN]);

	virtual void		Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int * /*Context unnecessary*/);

	virtual	std::string	GetHeaderBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);

};
