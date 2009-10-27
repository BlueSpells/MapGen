#include "StdAfx.h"
#include "ParkingMapHeaderCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/ParkingMapHeader.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CParkingMapHeaderCommand::CParkingMapHeaderCommand(void) : IScriptCommand(ParkingMapHeaderCommand)
{
}

CParkingMapHeaderCommand::~CParkingMapHeaderCommand(void)
{
}

/*virtual*/ ECommandResult CParkingMapHeaderCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == ParkingMapHeaderCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}


	ETextLanguage TextLanguageValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, TextLanguage, ParsedArguments, TextLanguageValue))
		return CommandFailed;

	SParkingGarageID ParkingGarageID_Value;
	// First parse the struct itself
	std::vector<std::string> ParkingGarageID_Vector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, ParkingGarageID, ParsedArguments, ParkingGarageID_Vector))
		return CommandFailed;
	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageID, ParkingGarageID_A, ParkingGarageID_Vector, ParkingGarageID_Value.A))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageID, ParkingGarageID_B, ParkingGarageID_Vector, ParkingGarageID_Value.B))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageID, ParkingGarageID_C, ParkingGarageID_Vector, ParkingGarageID_Value.C))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageID, ParkingGarageID_D, ParkingGarageID_Vector, ParkingGarageID_Value.D))
		return CommandFailed;

	SParkingGarageDimensions ParkingGarageDimensionsValue;
	// First parse the struct itself
	std::vector<std::string> ParkingGarageDimensionsVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, ParkingGarageDimensions, ParsedArguments, ParkingGarageDimensionsVector))
		return CommandFailed;
	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageDimensions, ParkingGarageDimensions_Width, ParkingGarageDimensionsVector, ParkingGarageDimensionsValue.Width))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, ParkingGarageDimensions, ParkingGarageDimensions_Height, ParkingGarageDimensionsVector, ParkingGarageDimensionsValue.Height))
		return CommandFailed;

	SParkingSpaceDimensions ParkingSpaceDimensionsValue;
	// First parse the struct itself
	std::vector<std::string> ParkingSpaceDimensionsVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, ParkingSpaceDimensions, ParsedArguments, ParkingSpaceDimensionsVector))
		return CommandFailed;
	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, ParkingSpaceDimensions, ParkingSpaceDimensions_Width, ParkingSpaceDimensionsVector, ParkingSpaceDimensionsValue.Width))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, ParkingSpaceDimensions, ParkingSpaceDimensions_Height, ParkingSpaceDimensionsVector, ParkingSpaceDimensionsValue.Height))
		return CommandFailed;

	SFloorAndSection FloorAndSectionValue;
	// First parse the struct itself
	std::vector<std::string> FloorAndSectionVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, FloorAndSection, ParsedArguments, FloorAndSectionVector))
		return CommandFailed;
	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, FloorAndSection, FloorAndSection_Floor, FloorAndSectionVector, FloorAndSectionValue.Floor))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, FloorAndSection, FloorAndSection_Section, FloorAndSectionVector, FloorAndSectionValue.Section))
		return CommandFailed;

	SPositionInParking CurrentPositionValue;
	// First parse the struct itself
	std::vector<std::string> CurrentPositionVector;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, CurrentPosition, ParsedArguments, CurrentPositionVector))
		return CommandFailed;
	// then parse its fields
	if (!ExtractAndInterperetStructField(ContextLine, CurrentPosition, CurrentPosition_X, CurrentPositionVector, CurrentPositionValue.X))
		return CommandFailed;
	if (!ExtractAndInterperetStructField(ContextLine, CurrentPosition, CurrentPosition_Y, CurrentPositionVector, CurrentPositionValue.Y))
		return CommandFailed;

	
	bool IsTargetPositionValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, IsTargetPosition, ParsedArguments, IsTargetPositionValue))
		return CommandFailed;

	bool IsTargetDescriptionValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, IsTargetDescription, ParsedArguments, IsTargetDescriptionValue))
		return CommandFailed;

	SPositionInParking TargetPositionValue;
	TargetPositionValue.X = ConvertIntToInt8Bit(0);
	TargetPositionValue.Y = ConvertIntToInt8Bit(0);
	if (IsTargetPositionValue)
	{
		// First parse the struct itself
		std::vector<std::string> TargetPositionVector;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingMapHeaderCommand, TargetPosition, ParsedArguments, TargetPositionVector))
			return CommandFailed;
		// then parse its fields
		if (!ExtractAndInterperetStructField(ContextLine, TargetPosition, TargetPosition_X, TargetPositionVector, TargetPositionValue.X))
			return CommandFailed;
		if (!ExtractAndInterperetStructField(ContextLine, TargetPosition, TargetPosition_Y, TargetPositionVector, TargetPositionValue.Y))
			return CommandFailed;
	}
	
	CString TargetDescriptionStr;
	char TargetDescriptionValue[] = {' '};
	if (IsTargetDescriptionValue)
	{
//		TargetDescriptionValue = 
		TargetDescriptionStr.Format(", TargetDescription = %s", TargetDescriptionValue);
	}

	CString TargetPositionStr;
	if (IsTargetPositionValue)
		TargetPositionStr.Format(", TargetPosition=(%d,%d)", TargetPositionValue.X, TargetPositionValue.Y);
	


	CParkingMapHeader *ParkingMapHeader = new CParkingMapHeader;
	ParkingMapHeader->Encode(ParkingGarageID_Value, TextLanguageValue, ParkingGarageDimensionsValue, ParkingSpaceDimensionsValue,
							FloorAndSectionValue, CurrentPositionValue, IsTargetPositionValue, IsTargetDescriptionValue, 
							TargetPositionValue, TargetDescriptionValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: GarageID = %d %d %d %d, Language = %s, GarageDimensions = [%d %d], ParkingSpaceDimensions = [%d %d], Floor = %d, Section = %d, CurrentPosition = (%d %d), IsTargetPositionStatus = %s, IsTargetDescriptionStatus = %s%s%s)", 
		ParkingMapHeaderCommand, ParkingGarageID_Value.A, ParkingGarageID_Value.B, 
		ParkingGarageID_Value.C, ParkingGarageID_Value.D, EnumToString(TextLanguageValue).c_str(),
		ParkingGarageDimensionsValue.Height, ParkingGarageDimensionsValue.Width,
		ParkingSpaceDimensionsValue.Height, ParkingSpaceDimensionsValue.Width,
		FloorAndSectionValue.Floor, FloorAndSectionValue.Section,
		CurrentPositionValue.X, CurrentPositionValue.Y,
		BooleanStr(IsTargetPositionValue), BooleanStr(IsTargetDescriptionValue), 
		TargetPositionStr, TargetDescriptionStr);


	Element = (void *)ParkingMapHeader;
	ElementType = AddHeaderCommand;
	return CommandSucceeded;
}