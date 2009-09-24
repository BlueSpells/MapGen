#pragma once
#include "Common/Utils.h"

#define EnumName(x) #x

#define DefineEnumToString4(EnumType, Val1, Val2, Val3, Val4)					\
std::string EnumToString(EnumType Value)										\
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
	return "";																	\
}\

#define DefineEnumToString3(EnumType, Val1, Val2, Val3)							\
std::string EnumToString(EnumType Value)										\
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
	return "";																	\
}\

DefineEnumToString4(EParkingAngle, Perpendicular, Parallel, Right45Degrees, Left45Degrees);
DefineEnumToString4(EParkingType, Regular, Handicap, Bikes, BigParking);
DefineEnumToString4(EParkingOrientation, Degrees0, Degrees90, Degrees180, Degrees270);