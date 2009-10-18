#pragma once
#include "Common/Utils.h"

#include "BarcodeProcessor/BasicItem.h"
#include "BarcodeProcessor/ParkingItem.h"
#include "BarcodeProcessor/PositionItem.h"
#include "BarcodeProcessor/SignedJumpExtendedItem.h"
#include "BarcodeProcessor/AbsoluteJumpExtendedItem.h"
#include "BarcodeProcessor/PavementItem.h"
#include "BarcodeProcessor/ComplexItem.h"
#include "BarcodeProcessor/ParkingMapHeader.h"
#include "BarcodeProcessor/GeneralHeader.h"

#define HandleDefualtOfEnumToString(EnumType)								\
	default:																\
	char TmpStr[64];														\
	sprintf_s(TmpStr, "%s Unknown Value [%d]", EnumName(EnumType), Value);	\
	LogEvent(LE_WARNING, __FUNCTION__ ": %s", TmpStr);						\
	return TmpStr;															

#define EnumName(x) #x

#define DefineEnumToString14(EnumType, Val1, Val2, Val3, Val4, Val5, Val6, Val7, Val8, Val9, Val10, Val11, Val12, Val13, Val14)	\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	RETURN_TYPE_STR(Val4);														\
	RETURN_TYPE_STR(Val5);														\
	RETURN_TYPE_STR(Val6);														\
	RETURN_TYPE_STR(Val7);														\
	RETURN_TYPE_STR(Val8);														\
	RETURN_TYPE_STR(Val9);														\
	RETURN_TYPE_STR(Val10);														\
	RETURN_TYPE_STR(Val11);														\
	RETURN_TYPE_STR(Val12);														\
	RETURN_TYPE_STR(Val13);														\
	RETURN_TYPE_STR(Val14);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\

#define DefineEnumToString7(EnumType, Val1, Val2, Val3, Val4, Val5, Val6, Val7)	\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	RETURN_TYPE_STR(Val4);														\
	RETURN_TYPE_STR(Val5);														\
	RETURN_TYPE_STR(Val6);														\
	RETURN_TYPE_STR(Val7);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\

#define DefineEnumToString6(EnumType, Val1, Val2, Val3, Val4, Val5, Val6)	\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	RETURN_TYPE_STR(Val4);														\
	RETURN_TYPE_STR(Val5);														\
	RETURN_TYPE_STR(Val6);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\

#define DefineEnumToString5(EnumType, Val1, Val2, Val3, Val4, Val5)				\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	RETURN_TYPE_STR(Val4);														\
	RETURN_TYPE_STR(Val5);														\
	HandleDefualtOfEnumToString(EnumType)										\
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
		HandleDefualtOfEnumToString(EnumType)									\
	};																			\
}\

#define DefineEnumToString3(EnumType, Val1, Val2, Val3)							\
static std::string EnumToString(EnumType Value)									\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	RETURN_TYPE_STR(Val3);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\

#define DefineEnumToString2(EnumType, Val1, Val2)								\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	RETURN_TYPE_STR(Val2);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\


#define DefineEnumToString1(EnumType, Val1)										\
	static std::string EnumToString(EnumType Value)								\
{																				\
	switch(Value)																\
	{																			\
	RETURN_TYPE_STR(Val1);														\
	HandleDefualtOfEnumToString(EnumType)										\
	};																			\
}\

DefineEnumToString4(EParkingAngle, Perpendicular, Parallel, Right45Degrees, Left45Degrees);
DefineEnumToString4(EParkingType, Regular, Handicap, Bikes, BigParking);
DefineEnumToString4(EParkingOrientation, Degrees0, Degrees90, Degrees180, Degrees270);
DefineEnumToString14(EBasicItemType, Elevator, Staircase_H, Staircase_V, WC, CarGate_Right, CarGate_Left, CarGate_Up, CarGate_Down, PayingMachine, Door, Sign, Column_Sqr, Column_Cir, Road);
DefineEnumToString4(EPositionItemType, CR, ForwardHorizontalJump, ForwardVerticalJump, ForwardDiagonalJump);
DefineEnumToString4(SPeriodicBetweenPoles::EPolesType, SPeriodicBetweenPoles::Pavement, SPeriodicBetweenPoles::Circular, SPeriodicBetweenPoles::Squared, SPeriodicBetweenPoles::Wall);
DefineEnumToString4(SStructureShape::EAngleType, SStructureShape::None, SStructureShape::AngleUpTo90, SStructureShape::AngleUpTo360, SStructureShape::Circular);
DefineEnumToString4(ESignedJumpItemType, CarriageDir, SignedHorizontalJump, SignedVerticalJump, SignedDiagonalJump);
DefineEnumToString4(EAbsoluteJumpItemType, Unused, AbsoluteHorizontalJump, AbsoluteVerticalJump, AbsoluteDiagonalJump);
DefineEnumToString1(EBarcodeType, ParkingMap);
//DefineEnumToString3(ETextLanguage, Empty, English, Hebrew);
DefineEnumToString2(ETextLanguage, English, Hebrew);
DefineEnumToString6(EShapeType, Wall, Right_Triangle, Triangle, Rect, Quadrangle, Pentagon);
DefineEnumToString4(ETextureType, Texture_Pavement, Texture_Building, Texture_NoParking, SolidFill);
DefineEnumToString4(ECurvatureType, Linear, Convex, Concave, Rectangular);
DefineEnumToString7(EItemType, SubItem, Parking, Position, Pavement, BasicComponent, ComplexStructure, ExtendedItem);