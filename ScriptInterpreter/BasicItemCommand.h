#pragma once
#include "ICommand.h"
#include "CommandsDictionary.h"

class CBasicItemCommand : public IScriptCommand
{
public:
	CBasicItemCommand(void);
	~CBasicItemCommand(void);

protected:
	virtual ECommandResult OnCommandImplementation(IN int ContextLine, IN std::string CommandWord, IN std::vector<std::string> CommandArguments, OUT void *&Element, OUT EElementType &ElementType);
};