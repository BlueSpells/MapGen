#include "StdAfx.h"
#include "SimpleScriptReader.h"
#include "ScriptSyntaxDefinitions.h"
#include "Common/Utils.h"
//#include "CommandHelper.h"

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

std::string CSimpleScriptReader::CleanTabsAndSpaces(std::string &Argument)
{
	std::string CleanedArgument;
	
	for (unsigned int i = 0; i < Argument.size(); i++)
	{
		if (Argument[i] != ' ' && Argument[i] != '\t')
			CleanedArgument += Argument[i];
	}

	Argument = CleanedArgument;
	return Argument;
}

/*virtual*/ bool CSimpleScriptReader::ReadLine(std::string &Command, std::vector<std::string> &ParametersList)
{
	ParametersList.clear();

	int FirstLineOfCommand = m_LineIndex + 1;

	int iLineInCommand = 0;
	bool HasReachedEndOfLine = false;
	do 
	{
		m_LineIndex++;
		iLineInCommand++;

		CString LineFromFile;
		bool IsOK = m_StreamFile.ReadString(LineFromFile);

		if (!IsOK)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Failed to read line #%d. perhaps ; is missing?", m_LineIndex);
			return false;
		}

		CTokenParser Parser(LineFromFile);

		if (!Parser.MoreTokens())
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d doesn't contain any command!", m_LineIndex);
			return false;
		}

		if (iLineInCommand == 1) // first line of command contains the command itself
			Command = CleanTabsAndSpaces(Parser.GetNextToken(CommandDelimiter));

		ParametersList.clear();
		int iArgumentsPerLine = 0;
		while (Parser.MoreTokens())
		{
			std::string argument = CleanTabsAndSpaces(Parser.GetNextToken(ArgumentsDelimiter));
			ParametersList.push_back(argument);
			if (iArgumentsPerLine > MaxNumberOfArgumentsPerLine)
			{
				LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d contains more than %d arguments!", m_LineIndex, MaxNumberOfArgumentsPerLine);
				return false;
			}
			iArgumentsPerLine++;
		}

		if ((ParametersList[ParametersList.size()-1])[ParametersList[ParametersList.size()-1].size()-1] == CommandEnd[0])
		{
			HasReachedEndOfLine = true;
			CleanCharacter(ParametersList[ParametersList.size()-1], CommandEnd);
		}

		if (iLineInCommand > MaxNumberOfLinesPerCommand)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Command %s in line %d is spread over more than %d lines!", Command, FirstLineOfCommand, MaxNumberOfLinesPerCommand);
			return false;
		}
	} while(!HasReachedEndOfLine);
	
	return HasReachedEndOfLine;
}