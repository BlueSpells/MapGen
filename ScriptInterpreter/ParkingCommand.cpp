#include "StdAfx.h"
#include "ParkingCommand.h"

CParkingCommand::CParkingCommand(void) : IScriptCommand(ParkingCommand)
{
}

CParkingCommand::~CParkingCommand(void)
{
}

/*virtual */ ECommandResult CParkingCommand::OnCommandImplementation(std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType ElementType)
{
	return CommandUnknown;
}