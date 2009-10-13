#include "StdAfx.h"
#include "ParkingCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/ParkingItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CParkingCommand::CParkingCommand(void) : IScriptCommand(ParkingCommand)
{
}

CParkingCommand::~CParkingCommand(void)
{
}

/*virtual*/ ECommandResult CParkingCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == ParkingCommand);
	
	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	EParkingAngle ParkingAngleValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, ParkingAngle, ParsedArguments, ParkingAngleValue))
		return CommandFailed;

	EParkingType ParkingTypeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, ParkingType, ParsedArguments, ParkingTypeValue))
		return CommandFailed;

	bool IsMirrorDoubledValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, IsMirrorDoubled, ParsedArguments, IsMirrorDoubledValue))
		return CommandFailed;

	bool IsWallBetweenMirrorsValue = false;
	CString IsWallBetweenMirrorsStr;
	if (IsMirrorDoubledValue)
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, IsWallBetweenMirrors, ParsedArguments, IsWallBetweenMirrorsValue))
			return CommandFailed;
		IsWallBetweenMirrorsStr.Format(", IsWallBetweenMirrors = %s,", BooleanStr(IsWallBetweenMirrorsValue));
	}

	EParkingOrientation	OrientationValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, Orientation, ParsedArguments, OrientationValue))
		return CommandFailed;


	bool IsPeriodicBetweenPolesValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, IsPeriodicBetweenPoles, ParsedArguments, IsPeriodicBetweenPolesValue))
		return CommandFailed;

	SPeriodicBetweenPoles PeriodicBetweenPolesValue;
	if (IsPeriodicBetweenPolesValue)
	{
		// First parse the struct itself
		std::vector<std::string> PeriodicBetweenPolesVector;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, PeriodicBetweenPoles, ParsedArguments, PeriodicBetweenPolesVector))
			return CommandFailed;

		// then parse its fields
		if (!ExtractAndInterperetStructField(ContextLine, PeriodicBetweenPoles, PeriodicBetweenPoles_NumberOfParkingPlaces, PeriodicBetweenPolesVector, PeriodicBetweenPolesValue.NumberOfParkingPlaces))
			return CommandFailed;

		if (!ExtractAndInterperetStructField(ContextLine, PeriodicBetweenPoles, PeriodicBetweenPoles_PolesType, PeriodicBetweenPolesVector, PeriodicBetweenPolesValue.PolesType))
			return CommandFailed;
	}


	SMultiplicity MultiplicityValue;
	
	// First parse the struct itself
	std::vector<std::string> MultiplicityStructVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, Multiplicity, ParsedArguments, MultiplicityStructVector))
		return CommandFailed;

	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, Multiplicity, Multiplicity_IsMultiplied, MultiplicityStructVector, MultiplicityValue.IsMultiplied))
		return CommandFailed;
	if (MultiplicityValue.IsMultiplied)
	{
		if (!ExtractAndInterperetStructField(ContextLine, Multiplicity, Multiplicity_Multiples, MultiplicityStructVector, MultiplicityValue.Multiples))
			return CommandFailed;
	}

	SStructureShape StructureShapeValue;

	// First parse the struct itself
	std::vector<std::string> StructureShapeVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, StructureShape, ParsedArguments, StructureShapeVector))
		return CommandFailed;

	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, StructureShape, StructureShape_AngleType, StructureShapeVector, StructureShapeValue.AngleType))
		return CommandFailed;
	std::string UnionStr;
	if (!ExtractAndInterperetStructField(ContextLine, StructureShape, StructureShape_Shape, StructureShapeVector, UnionStr))
		return CommandFailed;

	switch (StructureShapeValue.AngleType)
	{
	case SStructureShape::None:
		break;
	case SStructureShape::AngleUpTo90:
		if (!ExtractAndInterperetUnionField(ContextLine, StructureShape_Shape, StructureShape_Shape_SmallAngle, UnionStr, StructureShapeValue.Shape.SmallAngle))
			return CommandFailed;
		break;
	case SStructureShape::AngleUpTo360:
		if (!ExtractAndInterperetUnionField(ContextLine, StructureShape_Shape, StructureShape_Shape_BigAngle, UnionStr, StructureShapeValue.Shape.BigAngle))
			return CommandFailed;
		break;
	case SStructureShape::Circular:
	{
		std::vector<std::string> RadiusCoordinatesVector;
		if (!ExtractAndInterperetUnionField(ContextLine, StructureShape_Shape, StructureShape_Shape_RadiusCoordinates, UnionStr, RadiusCoordinatesVector))
			return CommandFailed;

		if (!ExtractAndInterperetStructField(ContextLine, StructureShape_Shape_RadiusCoordinates, StructureShape_Shape_RadiusCoordinates_X, RadiusCoordinatesVector, StructureShapeValue.Shape.RadiusCoordinates.X))
			return CommandFailed;
		if (!ExtractAndInterperetStructField(ContextLine, StructureShape_Shape_RadiusCoordinates, StructureShape_Shape_RadiusCoordinates_Y, RadiusCoordinatesVector, StructureShapeValue.Shape.RadiusCoordinates.Y))
			return CommandFailed;
		break;
	}
	default:
		Assert(false);
	};

	CItemStructure ItemStructure;
	ItemStructure.Encode(StructureShapeValue, MultiplicityValue);

	CParkingItem *ParkingItem = new CParkingItem;
	ParkingItem->Encode(ParkingAngleValue, ParkingTypeValue, IsMirrorDoubledValue, IsWallBetweenMirrorsValue, OrientationValue, ItemStructure, IsPeriodicBetweenPolesValue, ((IsPeriodicBetweenPolesValue) ? &PeriodicBetweenPolesValue : NULL));
	
	CString PeriodicBetweenPolesStatus;
	if (IsPeriodicBetweenPolesValue)
	{
		PeriodicBetweenPolesStatus.Format(", NumberOfParkingPlaces=%d, PolesType=%s",
			PeriodicBetweenPolesValue.NumberOfParkingPlaces, EnumToString(PeriodicBetweenPolesValue.PolesType).c_str());
	}

	CString StructureShapeStatus;

	switch (StructureShapeValue.AngleType)
	{
	case SStructureShape::None:
		break;
	case SStructureShape::AngleUpTo90:
		StructureShapeStatus.Format(", Angle=%d", StructureShapeValue.Shape.SmallAngle);
		break;
	case SStructureShape::AngleUpTo360:
		StructureShapeStatus.Format(", Angle=%d", StructureShapeValue.Shape.BigAngle);
		break;
	case SStructureShape::Circular:
		StructureShapeStatus.Format(", CircleCenterCoordinates=(%d,%d)", 
		StructureShapeValue.Shape.RadiusCoordinates.X,
		StructureShapeValue.Shape.RadiusCoordinates.Y);
		break;
	default:
		Assert(false);
	};

	LogEvent(LE_INFO, __FUNCTION__ ": [Line #%d]: %s Command Parsed Successfully: ParkingType = %s, AngleValue = %s, IsMirrorDoubledValue=%s%s, Orientation=%s, IsPeriodicBetweenPoles=%s%s, IsMultiplied=%s(Multiplies=%d), StructureShapeValue.AngleType=%s%s.", 
		ContextLine, ParkingCommand, EnumToString(ParkingTypeValue).c_str(), EnumToString(ParkingAngleValue).c_str(), BooleanStr(IsMirrorDoubledValue), IsWallBetweenMirrorsStr, EnumToString(OrientationValue).c_str(), 
		BooleanStr(IsPeriodicBetweenPolesValue), PeriodicBetweenPolesStatus, 
		BooleanStr(MultiplicityValue.IsMultiplied), ((MultiplicityValue.IsMultiplied) ? MultiplicityValue.Multiples : 0),
		EnumToString(StructureShapeValue.AngleType).c_str(), StructureShapeStatus);

	Element = (void *)ParkingItem;
	ElementType = AddItem;
	return CommandSucceeded;
}