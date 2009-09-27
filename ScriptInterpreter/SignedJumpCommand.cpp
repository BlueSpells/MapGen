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
	if (IsHorizontal) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, SignedJumpCommand, Signed_dX, ParsedArguments, dXValue))
			return CommandFailed;

	SignedInt8Bit dYValue = ConvertIntToSignedInt8Bit(0);
	if (IsVertical) 
		if (!ExtractAndInterperetArgumentValue(ContextLine, SignedJumpCommand, Signed_dY, ParsedArguments, dYValue))
			return CommandFailed;


	CSignedJumpExtendedItem *SignedJumpExtendedItem = new CSignedJumpExtendedItem;
	SignedJumpExtendedItem->Encode(SignedJumpItemTypeValue, dXValue, dYValue);


	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: SignedJumpItemType = %s  [dX,dY]=[%d,%d]), ...", 
		SignedJumpCommand, EnumToString(SignedJumpItemTypeValue).c_str(), dXValue, dYValue);

	Element = (void *)SignedJumpCommand;
	ElementType = AddItem;
	return CommandSucceeded;
}