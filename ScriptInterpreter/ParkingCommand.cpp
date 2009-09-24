#include "StdAfx.h"
#include "ParkingCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/ParkingItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

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

	EParkingOrientation	OrientationValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ParkingCommand, Orientation, ParsedArguments, OrientationValue))
		return CommandFailed;


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
	/*std::string UnionStr;
	if (!ExtractAndInterperetStructField(ContextLine, StructureShape, StructureShape_AngleType, StructureShapeVector, UnionStr))
		return CommandFailed;*/

	switch (StructureShapeValue.AngleType)
	{

	};

	CItemStructure ItemStructure;
	CParkingItem *ParkingItem = new CParkingItem;
//	ParkingItem->Encode(ParkingAngleValue, )
	
	/*
	bool					m_IsPeriodicBetweenPoles;
	SPeriodicBetweenPoles	m_PeriodicBetweenPoles;*/

	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: ParkingType = %s, AngleValue = %s, IsMirrorDoubledValue=%s, Orientation=%s, IsMultiplied=%s(Multiplies=%d), ...", 
		ParkingCommand, EnumToString(ParkingTypeValue).c_str(), EnumToString(ParkingAngleValue).c_str(), BooleanStr(IsMirrorDoubledValue), EnumToString(OrientationValue).c_str(), 
		BooleanStr(MultiplicityValue.IsMultiplied), ((MultiplicityValue.IsMultiplied) ? MultiplicityValue.Multiples : 0));


	Element = (void *)ParkingItem;
	ElementType = AddItem;
	return CommandSucceeded;
}