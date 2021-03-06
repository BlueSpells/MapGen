#include "StdAfx.h"
#include "ScriptInterpreter.h"
#include "IScriptReader.h"
#include "CommandsDictionary.h"
#include "ScriptSyntaxDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScriptInterpreter::CScriptInterpreter(IScriptReader *ScriptReader) : m_ScriptReader(ScriptReader)
{
}

CScriptInterpreter::~CScriptInterpreter(void)
{
}

bool CScriptInterpreter::Init()
{
	return m_ScriptReader->Init();	
}


#define CloseFileAndReturnValue(val) m_ScriptReader->CloseScriptFile(); return val;

bool CScriptInterpreter::InterperetFile(std::string FileName, std::vector<IHeader *> &HeaderList, std::vector<IItem *> &ItemsList)
{
	HeaderList.clear();
	ItemsList.clear();

	if (!m_ScriptReader->OpenScriptFile(FileName.c_str()))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Failed to open script file %s", FileName.c_str());
		return false;
	}


	std::string Command;
	std::vector<std::string> Arguments;
	void *Element = NULL;
	EElementType ElementType;
	while (m_ScriptReader->ReadLine(Command, Arguments))
	{
		if (Command == EndOfScript)
			break; // End reached!

		IScriptCommand *CommandObject = CCommandsDictionary::GetCommandsDictionary()->GetCommand(Command);
		if (!CommandObject)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Unknown command %s. Could not locate command in commands dictionary", m_ScriptReader->GetLineIndex(), Command.c_str());
			CloseFileAndReturnValue(false);
		}
		
		ECommandResult Result = CommandObject->OnCommand(m_ScriptReader->GetLineIndex(), Command, Arguments, Element, ElementType);
		switch (Result) 
		{
		case CommandUnknown:
			LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Command %s was found in dictionary, yet returned CommandUnknown!!", m_ScriptReader->GetLineIndex(), Command.c_str());
			CloseFileAndReturnValue(false);

		case CommandSucceeded:
			//LogEvent(LE_INFO, __FUNCTION__ ": [Line #%d]: Command %s Parsed Successfully.", m_ScriptReader->GetLineIndex(), Command.c_str());
			break;

		case CommandFailed:
			LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Command %s failed. Something is wrong with arguments?", m_ScriptReader->GetLineIndex(), Command.c_str());
			CloseFileAndReturnValue(false);

		default:
			ASSERT(false);
			CloseFileAndReturnValue(false);
		};

		// if we got here - command was parsed successfully!
		switch (ElementType)
		{
		case AddItemCommand:
			ItemsList.push_back((IItem *)Element);
			break;
		case AddHeaderCommand:
			HeaderList.push_back((IHeader *)Element);
			break;
		default:
			ASSERT(false);
			CloseFileAndReturnValue(false);
		};

		LogEvent(LE_INFO, __FUNCTION__ ": [Line #%d]: Command %s Parsed Successfully.  { #%s = %d }", 
			m_ScriptReader->GetLineIndex(), Command.c_str(), 
			(ElementType == AddItemCommand) ? "Item" : "Header",
			(ElementType == AddItemCommand) ? ItemsList.size() : HeaderList.size());
	}

	if (m_ScriptReader->GetLineIndex() == 0)
	{
		LogEvent(LE_WARNING, __FUNCTION__ ": 0 lines read from file! Is file empty?");
		CloseFileAndReturnValue(false);
	}

	CloseFileAndReturnValue(true);
}