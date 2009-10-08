#include "StdAfx.h"
#include "SignedJumpCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/SignedJumpExtendedItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

CSignedJumpCommand::CSignedJumpCommand(void) : IScriptCommand(SignedJumpCommand)
{
}

CSignedJumpCommand::~CSignedJumpCommand(void)
{
}

/*virtual*/ ECommandResult CSignedJumpCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == SignedJumpCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	ESignedJumpItemType SignedJumpItemTypeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, SignedJumpCommand, SignedJumpItemType, ParsedArguments, SignedJumpItemTypeValue))
		return CommandFailed;

	bool IsHorizontal = (SignedJumpItemTypeValue == SignedHorizontalJump || SignedJumpItemTypeValue == SignedDiagonalJump);
	bool IsVertical = (SignedJumpItemTypeValue == SignedVerticalJump || SignedJumpItemTypeValue == SignedDiagonalJump);

	SignedInt8Bit dXValue = ConvertIntToSignedInt8Bit(0);
	CString dXStr;
	if (IsHorizontal) 
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, SignedJumpCommand, Signed_dX, ParsedArguments, dXValue))
			return CommandFailed;
		dXStr.Format(", dX = %c%d", ((dXValue.sign) ? '+' : '-'), dXValue.value);
	}

	SignedInt8Bit dYValue = ConvertIntToSignedInt8Bit(0);
	CString dYStr;
	if (IsVertical) 
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, SignedJumpCommand, Signed_dY, ParsedArguments, dYValue))
			return CommandFailed;
		dYStr.Format(", dY = %c%d", ((dYValue.sign) ? '+' : '-'), dYValue.value);
	}


	CSignedJumpExtendedItem *SignedJumpExtendedItem = new CSignedJumpExtendedItem;
	SignedJumpExtendedItem->Encode(SignedJumpItemTypeValue, dXValue, dYValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: SignedJumpItemType = %s%s%s)", 
		SignedJumpCommand, EnumToString(SignedJumpItemTypeValue).c_str(), dXStr, dYStr);

	Element = (void *)SignedJumpExtendedItem;
	ElementType = AddItem;
	return CommandSucceeded;
}