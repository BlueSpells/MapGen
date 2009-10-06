#include "StdAfx.h"
#include "ComplexItemCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/ComplexItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

CComplexItemCommand::CComplexItemCommand(void) : IScriptCommand(ComplexItemCommand)
{
}

CComplexItemCommand::~CComplexItemCommand(void)
{
}

/*virtual*/ ECommandResult CComplexItemCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == ComplexItemCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	int UID_Value;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, UID, ParsedArguments, UID_Value))
		return CommandFailed;

	bool IsVerticalMirrorValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsVerticalMirror, ParsedArguments, IsVerticalMirrorValue))
		return CommandFailed;
	bool IsHorizontalMirrorValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsHorizontalMirror, ParsedArguments, IsHorizontalMirrorValue))
		return CommandFailed;
	bool IsVerticalReplicationValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsVerticalReplication, ParsedArguments, IsVerticalReplicationValue))
		return CommandFailed;
	bool IsHorizontalReplicationValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsHorizontalReplication, ParsedArguments, IsHorizontalReplicationValue))
		return CommandFailed;

	SReplication VerticalReplicationValue;
	VerticalReplicationValue.GapBetweenReplicas = ConvertIntToInt6Bit(0);
	VerticalReplicationValue.TimesToReplicate = ConvertIntToInt6Bit(0);
	if (IsVerticalReplicationValue)
	{
		// First parse the struct itself
		std::vector<std::string> VerticalReplicationVector;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, VerticalReplication, ParsedArguments, VerticalReplicationVector))
			return CommandFailed;
		// then parse its fields
		if (!ExtractAndInterperetStructField(ContextLine, VerticalReplication, VerticalReplication_GapBetweenReplicas, VerticalReplicationVector, VerticalReplicationValue.GapBetweenReplicas))
			return CommandFailed;
		if (!ExtractAndInterperetStructField(ContextLine, VerticalReplication, VerticalReplication_TimesToReplicate, VerticalReplicationVector, VerticalReplicationValue.TimesToReplicate))
			return CommandFailed;
	}

	SReplication HorizontalReplicationValue;
	HorizontalReplicationValue.GapBetweenReplicas = ConvertIntToInt6Bit(0);
	HorizontalReplicationValue.TimesToReplicate = ConvertIntToInt6Bit(0);
	if (IsHorizontalReplicationValue)
	{
		// First parse the struct itself
		std::vector<std::string> HorizontalReplicationVector;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, HorizontalReplication, ParsedArguments, HorizontalReplicationVector))
			return CommandFailed;
		// then parse its fields
		if (!ExtractAndInterperetStructField(ContextLine, HorizontalReplication, HorizontalReplication_GapBetweenReplicas, HorizontalReplicationVector, HorizontalReplicationValue.GapBetweenReplicas))
			return CommandFailed;
		if (!ExtractAndInterperetStructField(ContextLine, HorizontalReplication, HorizontalReplication_TimesToReplicate, HorizontalReplicationVector, HorizontalReplicationValue.TimesToReplicate))
			return CommandFailed;
	}

	CComplexItem *ComplexItem = new CComplexItem;
	std::vector<IItem *> ListOfEncodedItemsValue;
	std::vector<std::string> ListOfItemsVector;
	if(!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, ListOfEncodedItems, ParsedArguments, ListOfItemsVector))
	{
		ComplexItem->Encode(UID_Value, IsVerticalMirrorValue, IsHorizontalMirrorValue, IsVerticalReplicationValue,
							IsHorizontalReplicationValue, ((IsVerticalReplicationValue) ? &VerticalReplicationValue : NULL),
							((IsHorizontalReplicationValue) ? &HorizontalReplicationValue : NULL));

		LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: UID = %d, IsVerticalMirror = %s, IsHorizontalMirror = %s, IsVerticalReplication = %s, IsHorizontalReplication = %s, VerticalReplication = [%d %d], HorizontalReplication = [%d %d])", 
			ComplexItemCommand, UID_Value, BooleanStr(IsVerticalMirrorValue), BooleanStr(IsHorizontalMirrorValue), BooleanStr(IsVerticalReplicationValue), BooleanStr(IsHorizontalReplicationValue), 
			VerticalReplicationValue.TimesToReplicate, VerticalReplicationValue.GapBetweenReplicas, HorizontalReplicationValue.TimesToReplicate, HorizontalReplicationValue.GapBetweenReplicas );

	}
	else
	{		
		bool IsReplicationPartOfDefinitionValue;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsReplicationPartOfDefinition, ParsedArguments, IsReplicationPartOfDefinitionValue))
			return CommandFailed;
		Int5Bit NumberOfObjectsInComplexValue;
		if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, NumberOfObjectsInComplex, ParsedArguments, NumberOfObjectsInComplexValue))
			return CommandFailed;

		for (int i = 0; i < NumberOfObjectsInComplexValue; i++)
		{
			IItem *ItemWithinComplex = NULL;
			if(!ExtractAndInterperetListItem(ContextLine, ListOfEncodedItems, ListOfEncodedItems_Item, ListOfItemsVector, ItemWithinComplex, i))
				return CommandFailed;
			ListOfEncodedItemsValue.push_back(ItemWithinComplex);
		}
		
		ComplexItem->Encode(UID_Value, ListOfEncodedItemsValue, IsVerticalMirrorValue, IsHorizontalMirrorValue, 
							IsVerticalReplicationValue,	IsHorizontalReplicationValue, IsReplicationPartOfDefinitionValue, 
							((IsVerticalReplicationValue) ? &VerticalReplicationValue : NULL),
							((IsHorizontalReplicationValue) ? &HorizontalReplicationValue : NULL));

		LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: UID = %d, NumberOfObjectsInComplex = %d, IsReplicationPartOfDefinition = %s, IsVerticalMirror = %s, IsHorizontalMirror = %s, IsVerticalReplication = %s, IsHorizontalReplication = %s, VerticalReplication = [%d %d], HorizontalReplication = [%d %d])", 
					ComplexItemCommand, UID_Value, NumberOfObjectsInComplexValue, BooleanStr(IsReplicationPartOfDefinitionValue), BooleanStr(IsVerticalMirrorValue), BooleanStr(IsHorizontalMirrorValue), BooleanStr(IsVerticalReplicationValue), BooleanStr(IsHorizontalReplicationValue), 
					VerticalReplicationValue.TimesToReplicate, VerticalReplicationValue.GapBetweenReplicas, HorizontalReplicationValue.TimesToReplicate, HorizontalReplicationValue.GapBetweenReplicas );
	}




//	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: BasicItemType = %s)", 
//		BasicItemCommand, EnumToString(BasicItemValue).c_str());

	Element = (void *)ComplexItem;
	ElementType = AddItem;
	return CommandSucceeded;
}