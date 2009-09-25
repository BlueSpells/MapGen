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

	EPositionItemType PositionItemValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, PositionItemType, ParsedArguments, PositionItemValue))
		return CommandFailed;

	bool IsHorizontal = (PositionItemValue == HorizontalJump || PositionItemValue == DiagonalJump);
	bool IsVertical = (PositionItemValue == VerticalJump || PositionItemValue == DiagonalJump);

	Int8Bit XValue = ConvertIntToInt8Bit(0);	
	if (IsHorizontal) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, X, ParsedArguments, XValue))
			return CommandFailed;
	
	Int8Bit YValue = ConvertIntToInt8Bit(0);
	if (IsVertical) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, PositionCommand, Y, ParsedArguments, YValue))
			return CommandFailed;
	

	CPositionItem *PositionItem = new CPositionItem;
	PositionItem->Encode(PositionItemValue, XValue, YValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: PositionItemType = %s), ...", 
		PositionCommand, EnumToString(PositionItemValue).c_str());

	Element = (void *)PositionItem;
	ElementType = AddItem;
	return CommandSucceeded;
}