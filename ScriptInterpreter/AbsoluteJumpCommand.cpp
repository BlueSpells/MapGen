#include "StdAfx.h"
#include "AbsoluteJumpCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/AbsoluteJumpExtendedItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	CString XStr;
	if (IsHorizontal) 
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, AbsoluteJumpCommand, Abs_X, ParsedArguments, XValue))
			return CommandFailed;
		XStr.Format(", dX = %d", XValue);
	}

	Int16Bit YValue = ConvertIntToInt16Bit(0);
	CString YStr;
	if (IsVertical) 
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, AbsoluteJumpCommand, Abs_Y, ParsedArguments, YValue))
			return CommandFailed;
		YStr.Format(", dY = %d", YValue);
	}


	CAbsoluteJumpExtendedItem *AbsoluteJumpExtendedItem = new CAbsoluteJumpExtendedItem;
	AbsoluteJumpExtendedItem->Encode(AbsoluteJumpItemTypeValue, XValue, YValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: SignedJumpItemType = %s%s%s)", 
		AbsoluteJumpCommand, EnumToString(AbsoluteJumpItemTypeValue).c_str(), XStr, YStr);

	Element = (void *)AbsoluteJumpExtendedItem;
	ElementType = AddItem;
	return CommandSucceeded;
}