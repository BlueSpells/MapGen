#pragma once
#include "ICommand.h"

const std::string ParkingCommand = "Parking";

class CParkingCommand : public IScriptCommand
{
public:
	CParkingCommand(void);
	~CParkingCommand(void);

	virtual ECommandResult OnCommandImplementation(std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType ElementType);
};
