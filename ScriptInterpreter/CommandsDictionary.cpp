#include "StdAfx.h"
#include "CommandsDictionary.h"

#include "Common/collectionhelper.h"
#include "Common/LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCommandsDictionary::CCommandsDictionary(void)
{
}

CCommandsDictionary::~CCommandsDictionary(void)
{
	std::vector<IScriptCommand *> CommandsToDelete;
	std::vector<std::string> CommandsToRemove;

	std::map<std::string, IScriptCommand *>::iterator Iter = m_CommandsDictionary.begin();
	for (Iter; Iter != m_CommandsDictionary.end() ; Iter++)
	{
		CommandsToRemove.push_back(Iter->first);
		CommandsToDelete.push_back(Iter->second);
	}

	for (unsigned int i = 0; i < CommandsToRemove.size(); i++)
	{
		delete CommandsToDelete[i];
		RemoveValueFromMap(m_CommandsDictionary, CommandsToRemove[i]);
	}
}


/*static*/ CCommandsDictionary* CCommandsDictionary::m_This = NULL;

/*static*/ CCommandsDictionary* CCommandsDictionary::GetCommandsDictionary()
{
	if (m_This == NULL)
		m_This = new CCommandsDictionary;

	return m_This;
}

/*static*/ void CCommandsDictionary::DestroyCommandsDictionary()
{
	Assert(m_This != NULL);
	if (m_This != NULL)
		delete m_This;

	m_This = NULL;
}


bool CCommandsDictionary::AddCommand(std::string CommandWord, IScriptCommand* CommandObject)
{
	if (!InsertValueToMap(m_CommandsDictionary, CommandWord, CommandObject))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Failed to add command word %s to dictionary", CommandWord.c_str());
		return false;
	}

	return true;
}

IScriptCommand *CCommandsDictionary::GetCommand(std::string CommandWord)
{
	IScriptCommand *CommandObject = NULL;
	if (!GetValueFromMap(m_CommandsDictionary, CommandWord, CommandObject))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Failed to extract command for command word %s", CommandWord.c_str());
		return NULL;
	}

	return CommandObject;
}