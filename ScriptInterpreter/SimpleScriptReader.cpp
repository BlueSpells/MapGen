#include "StdAfx.h"
#include "SimpleScriptReader.h"
#include "ScriptSyntaxDefinitions.h"
#include "Common/Utils.h"
#include "CommandHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimpleScriptReader::CSimpleScriptReader(void)
{
}

CSimpleScriptReader::~CSimpleScriptReader(void)
{
}


/*virtual*/ bool CSimpleScriptReader::Init()
{
	LogEvent(LE_INFOLOW, __FUNCTION__ ": ScriptReader initialized successfully");
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

	LogEvent(LE_DEBUG, __FUNCTION__ ": File Closed");
	return true;
}

std::string CSimpleScriptReader::CleanTabsAndSpaces(std::string &Argument)
{
	std::string CleanedArgument;
	
	int BracketsCounter = 0;

	for (unsigned int i = 0; i < Argument.size(); i++)
	{
		if (Argument[i] == ListBegin[0])
			BracketsCounter++;

		if (Argument[i] == ListEnd[0])
			BracketsCounter--;

		if ((Argument[i] != ' ' && Argument[i] != '\t') || BracketsCounter > 0)
			CleanedArgument += Argument[i];
	}

	Argument = CleanedArgument;
	return Argument;
}

std::string CSimpleScriptReader::CleanComment(std::string &Argument)
{
	std::string CleanedArgument;

	for (unsigned int i = 0; i < Argument.size(); i++)
	{
		if (Argument[i] == CommentMark[0])
			break;
	
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

		// Read line from file
		CString LineFromFile;
		BOOL IsOK = m_StreamFile.ReadString(LineFromFile);

		if (IsOK == FALSE)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Failed to read line #%d. perhaps ; is missing?", m_LineIndex);
			return false;
		}

		// deal with complete-line comment
		if (LineFromFile[0] == CommentMark[0])
		{
			LogEvent(LE_DEBUG, __FUNCTION__ ": Line #%d contains a comment: %s. Moving to next line..", m_LineIndex, LineFromFile);
			FirstLineOfCommand = m_LineIndex + 1;
			iLineInCommand = 0;
			continue;
		}

		// remove comment within line
		LineFromFile = CleanComment(std::string(LineFromFile)).c_str();

		// Clean all tabs and spaces
#pragma warning(push)
#pragma warning (disable:4239)
		if (CleanTabsAndSpaces(std::string(LineFromFile)).size() == 0)
		{
			LogEvent(LE_DEBUG, __FUNCTION__ ": Line #%d is empty. Moving to next line..", m_LineIndex);
			FirstLineOfCommand = m_LineIndex + 1;
			iLineInCommand = 0;
			continue;
		}
#pragma warning(pop)

		CTokenParser Parser(LineFromFile);

		if (!Parser.MoreTokens())
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d doesn't contain any command!", m_LineIndex);
			return false;
		}

		if (iLineInCommand == 1) // first line of command contains the command itself
		{
#pragma warning(push)
#pragma warning (disable:4239)
			Command = CleanTabsAndSpaces(Parser.GetNextToken(CommandDelimiter));
#pragma warning(pop)
			if (Command == EndOfScript)
			{
				LogEvent(LE_INFOLOW, __FUNCTION__ ": Reached end of script (line #%d)", m_LineIndex);
				return true;
			}
		}

		int iArgumentsPerLine = 0;
		while (Parser.MoreTokens())
		{
			iArgumentsPerLine++;
#pragma warning(push)
#pragma warning (disable:4239)
			std::string argument = CleanTabsAndSpaces(Parser.GetNextToken(ArgumentsDelimiter));
#pragma warning(pop)
			if (argument.find(CommandDelimiter) != std::string::npos)
			{
				if (iArgumentsPerLine == 1)
					LogEvent(LE_ERROR, __FUNCTION__ ": Command separator was found in an line which should not have contained it. Have you forgotten ';' in line %d?", m_LineIndex-1);
				else
					LogEvent(LE_ERROR, __FUNCTION__ ": Command separator should not be used within a parameter! was found in argument %d of line %d", iArgumentsPerLine, m_LineIndex);
				return false;
			}

			if (argument.size() > 0)
				ParametersList.push_back(argument);

			if (iArgumentsPerLine > MaxNumberOfArgumentsPerLine)
			{
				LogEvent(LE_ERROR, __FUNCTION__ ": Line #%d contains more than %d arguments!", m_LineIndex, MaxNumberOfArgumentsPerLine);
				return false;
			}
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