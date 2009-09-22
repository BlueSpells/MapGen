#include "StdAfx.h"
#include "ScriptInterpreter.h"
#include "IScriptReader.h"

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

bool CScriptInterpreter::InterperetFile(std::string FileName, std::vector<IHeader *> &HeaderList, std::vector<IItem *> &ItemsList)
{
	if (!m_ScriptReader->OpenScriptFile(FileName.c_str()))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Failed to open script file %s", FileName.c_str());
		return false;
	}


	std::string Command;
	std::vector<std::string> Arguments;
	while (m_ScriptReader->ReadLine(Command, Arguments))
	{
		
	}

	if (m_ScriptReader->GetLineIndex() == 0)
	{
		LogEvent(LE_WARNING, __FUNCTION__ ": 0 lines read from file! Is file empty?");
		return false;
	}

	m_ScriptReader->CloseScriptFile();

	return true;
}