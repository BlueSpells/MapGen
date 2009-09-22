#include "StdAfx.h"
#include "SimpleScriptReader.h"
#include "Common/Utils.h"

CSimpleScriptReader::CSimpleScriptReader(void)
{
}

CSimpleScriptReader::~CSimpleScriptReader(void)
{
}


/*virtual*/ bool CSimpleScriptReader::Init()
{
	LogEvent(LE_INFOHIGH, __FUNCTION__ ": ScriptReader initialized successfully");
	return true;
}

/*virtual*/ bool CSimpleScriptReader::OpenScriptFile(CString FileName)
{
	m_LineIndex = 0;

	if (!m_StreamFile.Open(FileName, CFile::modeRead))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Failed Opening file %s", FileName);
		return false;
	}

	LogEvent(LE_INFOHIGH, __FUNCTION__ ": File %s Opened Successfully", FileName);
	return true;
}

/*virtual*/ bool CSimpleScriptReader::CloseScriptFile()
{
	m_StreamFile.Close();

	LogEvent(LE_INFOHIGH, __FUNCTION__ ": File Closed");
	return true;
}

const char *SpaceDelimiter = " ";
const int MaxNumberOfArguments = 64;

/*virtual*/ bool CSimpleScriptReader::ReadLine(std::string &Command, std::vector<std::string> &ParametersList)
{
	CString LineFromFile;
	bool IsOK = m_StreamFile.ReadString(LineFromFile);

	if (!IsOK)
		return false;

	m_LineIndex++;

	CTokenParser Parser(LineFromFile);

	if (!Parser.MoreTokens())
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d doesn't contain any command!", m_LineIndex);
		return false;
	}

	Command = Parser.GetNextToken(SpaceDelimiter);

	ParametersList.clear();
	int i = 0;
	while (Parser.MoreTokens())
	{
		std::string argument = Parser.GetNextToken(SpaceDelimiter);
		ParametersList.push_back(argument);
		if (i > MaxNumberOfArguments)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d contains more than %d arguments!", m_LineIndex, MaxNumberOfArguments);
			return false;
		}
		i++;
	}
}