#include "StdAfx.h"
#include "AbsoluteJumpCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/AbsoluteJumpExtendedItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

CAbsoluteJumpCommand::CAbsoluteJumpCommand(void) : IScriptCommand(AbsoluteJumpCommand)
{
}

CAbsoluteJumpCommand::~CAbsoluteJumpCommand(void)
{
}

/*virtual*/ ECommandResult CAbsoluteJumpCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == AbsoluteJumpCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	EAbsoluteJumpItemType AbsoluteJumpItemTypeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, AbsoluteJumpCommand, AbsoluteJumpItemType, ParsedArguments, AbsoluteJumpItemTypeValue))
		return CommandFailed;

	bool IsHorizontal = (AbsoluteJumpItemTypeValue == AbsoluteHorizontalJump || AbsoluteJumpItemTypeValue == AbsoluteDiagonalJump);
	bool IsVertical = (AbsoluteJumpItemTypeValue == AbsoluteVerticalJump || AbsoluteJumpItemTypeValue == AbsoluteDiagonalJump);

	Int16Bit XValue = ConvertIntToInt16Bit(0);	
	if (IsHorizontal) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, AbsoluteJumpCommand, X, ParsedArguments, XValue))
			return CommandFailed;

	Int16Bit YValue = ConvertIntToInt16Bit(0);
	if (IsVertical) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, AbsoluteJumpCommand, Y, ParsedArguments, YValue))
			return CommandFailed;


	CAbsoluteJumpExtendedItem *AbsoluteJumpExtendedItem = new CAbsoluteJumpExtendedItem;
	AbsoluteJumpExtendedItem->Encode(AbsoluteJumpItemTypeValue, XValue, YValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: SignedJumpItemType = %s  [dX,dY]=[%d,%d])", 
		AbsoluteJumpCommand, EnumToString(AbsoluteJumpItemTypeValue).c_str(), XValue, YValue);

	Element = (void *)AbsoluteJumpExtendedItem;
	ElementType = AddItem;
	return CommandSucceeded;
}