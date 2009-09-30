#pragma once
#include "Common/Utils.h"

#include "BarcodeProcessor/BasicItem.h"
#include "BarcodeProcessor/ParkingItem.h"
#include "BarcodeProcessor/PositionItem.h"
#include "BarcodeProcessor/SignedJumpExtendedItem.h"


#define EnumName(x) #x

#define DefineEnumToString5(EnumType, Val1, Val2, Val3, Val4, Val5)				\
	static std::string EnumToString(EnumType Value)									\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	RETURN_TYPE_STR(Val4);														\
	RETURN_TYPE_STR(Val5);														\
		default:																\
		char TmpStr[64];														\
		sprintf_s(TmpStr, "%s Unknown Value [%d]", EnumName(EnumType), Value);	\
		LogEvent(LE_WARNING, __FUNCTION__ ": %s", TmpStr);						\
		return TmpStr;															\
	};																			\
}\

#define DefineEnumToString4(EnumType, Val1, Val2, Val3, Val4)					\
static std::string EnumToString(EnumType Value)										\
{																				\
	switch(Value)																\
	{																			\
		RETURN_TYPE_STR(Val1);													\
		RETURN_TYPE_STR(Val2);													\
		RETURN_TYPE_STR(Val3);													\
		RETURN_TYPE_STR(Val4);													\
		default:																\
		char TmpStr[64];														\
		sprintf_s(TmpStr, "%s Unknown Value [%d]", EnumName(EnumType), Value);	\
		LogEvent(LE_WARNING, __FUNCTION__ ": %s", TmpStr);						\
		return TmpStr;															\
	};																			\
}\

#define DefineEnumToString3(EnumType, Val1, Val2, Val3)							\
static std::string EnumToString(EnumType Value)										\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
		default:																\
		char TmpStr[64];														\
		sprintf_s(TmpStr, "%s Unknown Value [%d]", EnumName(EnumType), Value);	\
		LogEvent(LE_WARNING, __FUNCTION__ ": %s", TmpStr);						\
		return TmpStr;															\
	};																			\
}\

DefineEnumToString4(EParkingAngle, Perpendicular, Parallel, Right45Degrees, Left45Degrees);
DefineEnumToString4(EParkingType, Regular, Handicap, Bikes, BigParking);
DefineEnumToString4(EParkingOrientation, Degrees0, Degrees90, Degrees180, Degrees270);
DefineEnumToString5(EBasicItemType, Elevator, Staircase, WC, CarGate, PayingMachine);
DefineEnumToString4(EPositionItemType, CR, ForwardHorizontalJump, ForwardVerticalJump, ForwardDiagonalJump);
DefineEnumToString4(SPeriodicBetweenPoles::EPolesType, SPeriodicBetweenPoles::MarksOnRoad, SPeriodicBetweenPoles::Circular, SPeriodicBetweenPoles::Squared, SPeriodicBetweenPoles::Rectangular);
DefineEnumToString4(SStructureShape::EAngleType, SStructureShape::None, SStructureShape::AngleUpTo90, SStructureShape::AngleUpTo360, SStructureShape::Circular);
DefineEnumToString4(ESignedJumpItemType, CarriageDir, SignedHorizontalJump, SignedVerticalJump, SignedDiagonalJump);

