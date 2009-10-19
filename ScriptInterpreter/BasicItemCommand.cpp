#include "StdAfx.h"
#include "BasicItemCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/BasicItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	bool IsScaledValue = false;
	UScalingCoordinates X_Value, Y_Value;
	X_Value.Size16bits=ConvertIntToInt16Bit(0);
	Y_Value.Size16bits=ConvertIntToInt16Bit(0);
	CString IsScaledStr;
	CString ScalingStr;
	if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, BasicItemType, ParsedArguments, BasicItemValue))
		return CommandFailed;

	if (!((BasicItemValue == Door) || (BasicItemValue == CarGate_Left) || (BasicItemValue == CarGate_Right) || (BasicItemValue == CarGate_Down)))
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, IsScaled, ParsedArguments, IsScaledValue))
			return CommandFailed;
		IsScaledStr.Format(", IsScaled = %s", BooleanStr(IsScaledValue));

		if (BasicItemValue == Road)
		{
			if (IsScaledValue) 
			{
				if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, X_Size16bits, ParsedArguments, X_Value.Size16bits))
					return CommandFailed;
				if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, Y_Size16bits, ParsedArguments, Y_Value.Size16bits))
					return CommandFailed;
				ScalingStr.Format(", Scaling(16bit) = [X=%d, Y=%d]", X_Value.Size16bits, Y_Value.Size16bits);
			}
			else
			{
				if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, X_Size10bits, ParsedArguments, X_Value.Size10bits))
					return CommandFailed;
				if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, Y_Size10bits, ParsedArguments, Y_Value.Size10bits))
					return CommandFailed;
				ScalingStr.Format(", Scaling(10bit) = [X=%d, Y=%d]", X_Value.Size10bits, Y_Value.Size10bits);
			}
		}
		else if (IsScaledValue)
		{
			if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, X_Size6bits, ParsedArguments, X_Value.Size6bits))
				return CommandFailed;
			if (!ExtractAndInterperetArgumentValue(ContextLine, BasicItemCommand, Y_Size6bits, ParsedArguments, Y_Value.Size6bits))
				return CommandFailed;
			ScalingStr.Format(", Scaling(6bit) = [X=%d, Y=%d]", X_Value.Size6bits, Y_Value.Size6bits);
		}
	}

	CBasicItem *BasicItem = new CBasicItem;
	BasicItem->Encode(BasicItemValue, IsScaledValue, X_Value, Y_Value);

	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: BasicItemType = %s%s%s", 
		BasicItemCommand, EnumToString(BasicItemValue).c_str(), IsScaledStr, ScalingStr);

	Element = (void *)BasicItem;
	ElementType = AddItem;
	return CommandSucceeded;
}