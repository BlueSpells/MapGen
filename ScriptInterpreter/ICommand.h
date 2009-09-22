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

	ECommandResult OnCommand(std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
	{
		if (m_CommandWord == CommandWord)
			return OnCommandImplementation(CommandWord, CommandArguments, Element, ElementType);

		return CommandUnknown;
	}

protected:
	virtual ECommandResult OnCommandImplementation(std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType) = 0;

private:
	std::string m_CommandWord;
};
