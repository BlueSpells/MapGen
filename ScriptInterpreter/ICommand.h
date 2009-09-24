#pragma once

#include <string>
#include <vector>

enum ECommandResult
{
	CommandUnknown,
	CommandSucceeded,
	CommandFailed
};

enum EElementType
{
	AddItem,
	AddHeader
};

class IScriptCommand
{
public:
	IScriptCommand(std::string CommandWord) : m_CommandWord(CommandWord) {}
	~IScriptCommand(void) {}

	ECommandResult OnCommand(IN int ContextLine, IN std::string CommandWord, IN std::vector<std::string> CommandArguments, OUT void *&Element, OUT EElementType &ElementType)
	{
		if (m_CommandWord == CommandWord)
			return OnCommandImplementation(ContextLine, CommandWord, CommandArguments, Element, ElementType);

		LogEvent(LE_WARNING, __FUNCTION__ ": Problem with Dictionary! Command %s returned CommandUnknown", CommandWord.c_str());
		return CommandUnknown;
	}

protected:
	virtual ECommandResult OnCommandImplementation(IN int ContextLine, IN std::string CommandWord, IN std::vector<std::string> CommandArguments, OUT void *&Element, OUT EElementType &ElementType) = 0;

private:
	std::string m_CommandWord;
};
