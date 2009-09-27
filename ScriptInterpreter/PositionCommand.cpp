#include "StdAfx.h"
#include "PositionCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/PositionItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

CPositionCommand::CPositionCommand(void) : IScriptCommand(PositionCommand)
{
}

CPositionCommand::~CPositionCommand(void)
{
}

/*virtual*/ ECommandResult CPositionCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == PositionCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	EPositionItemType PositionItemTypeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, PositionItemType, ParsedArguments, PositionItemTypeValue))
		return CommandFailed;

	bool IsHorizontal = (PositionItemTypeValue == ForwardHorizontalJump || PositionItemTypeValue == ForwardDiagonalJump);
	bool IsVertical = (PositionItemTypeValue == ForwardVerticalJump || PositionItemTypeValue == ForwardDiagonalJump);

	Int8Bit dXValue = ConvertIntToInt8Bit(0);	
	if (IsHorizontal) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, dX, ParsedArguments, dXValue))
			return CommandFailed;
	
	Int8Bit dYValue = ConvertIntToInt8Bit(0);
	if (IsVertical) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, dY, ParsedArguments, dYValue))
			return CommandFailed;
	

	CPositionItem *PositionItem = new CPositionItem;
	PositionItem->Encode(PositionItemTypeValue, dXValue, dYValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: PositionItemType = %s  [dX,dY]=[%d,%d]), ...", 
		PositionCommand, EnumToString(PositionItemTypeValue).c_str(), dXValue, dYValue);

	Element = (void *)PositionItem;
	ElementType = AddItem;
	return CommandSucceeded;
}