#pragma once

#include "ICommand.h"
#include <string>
#include <map>

class CCommandsDictionary
{
// Singleton
public:
	static CCommandsDictionary* GetCommandsDictionary();
	static void					DestroyCommandsDictionary();
private:
	static CCommandsDictionary* m_This;
	CCommandsDictionary(void);
	~CCommandsDictionary(void);

public:
	bool AddCommand(std::string CommandWord, IScriptCommand *CommandObject);
	IScriptCommand *GetCommand(std::string CommandWord);
// Members
private:
	std::map<std::string, IScriptCommand *> m_CommandsDictionary;
	
};


#define AddCommandToCommandsDictionary(CommandWord, CommandClassName)								\
IScriptCommand *Command##CommandClassName = new CommandClassName;									\
CCommandsDictionary::GetCommandsDictionary()->AddCommand(CommandWord, Command##CommandClassName );	\

// class CommandClassName##Registrar																\
// {																								\
// public:																							\
// 	CommandClassName##Registrar()																\
// 	{																							\
// 		IScriptCommand *Command = new CommandClassName;											\
// 		CCommandsDictionary::GetCommandsDictionary()->AddCommand(CommandWord, Command);			\
// 	}																							\
// };																								\
// CommandClassName##Registrar RegistrarInstance;													
