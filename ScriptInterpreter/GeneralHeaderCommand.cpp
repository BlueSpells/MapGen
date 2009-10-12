#include "StdAfx.h"
#include "GeneralHeaderCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/GeneralHeader.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGeneralHeaderCommand::CGeneralHeaderCommand(void) : IScriptCommand(GeneralHeaderCommand)
{
}

CGeneralHeaderCommand::~CGeneralHeaderCommand(void)
{
}

/*virtual*/ ECommandResult CGeneralHeaderCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == GeneralHeaderCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	Int32Bit SignatureValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, GeneralHeaderCommand, Signature, ParsedArguments, SignatureValue))
		return CommandFailed;

	Int4Bit VersionValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, GeneralHeaderCommand, Version, ParsedArguments, VersionValue))
		return CommandFailed;

	EBarcodeType BarcodeTypeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, GeneralHeaderCommand, BarcodeType, ParsedArguments, BarcodeTypeValue))
		return CommandFailed;

	
	CGeneralHeader *GeneralHeader = new CGeneralHeader;
	GeneralHeader->Encode(SignatureValue, VersionValue, BarcodeTypeValue);

	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: Signature = %d, Version = %d, BarcodeType = %s)", 
		GeneralHeaderCommand, SignatureValue, VersionValue, EnumToString(BarcodeTypeValue).c_str());

	Element = (void *)GeneralHeader;
	ElementType = AddHeader;
	return CommandSucceeded;
}