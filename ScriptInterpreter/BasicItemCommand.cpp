#include "StdAfx.h"
#include "BasicItemCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/BasicItem.h"
#include "ScriptSyntaxDefinitions.h"

CBasicItemCommand::CBasicItemCommand(void) : IScriptCommand(BasicItemCommand)
{
}

CBasicItemCommand::~CBasicItemCommand(void)
{
}

/*virtual*/ ECommandResult CBasicItemCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == BasicItemCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	EBasicItemType BasicItemValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, BasicItemType, ParsedArguments, BasicItemValue))
		return CommandFailed;

	CBasicItem *BasicItem = new CBasicItem;
	BasicItem->Encode(BasicItemValue);


	Element = (void *)BasicItem;
	ElementType = AddItem;
	return CommandSucceeded;
}