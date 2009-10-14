#pragma once
#include "iscriptreader.h"

class CSimpleScriptReader :	public IScriptReader
{
public:
	CSimpleScriptReader(void);
	~CSimpleScriptReader(void);

	virtual bool Init();
	virtual bool OpenScriptFile(CString FileName);
	virtual bool CloseScriptFile();
	virtual bool ReadLine(std::string &Command, std::vector<std::string> &ParametersList);

private:
	std::string CleanTabsAndSpaces(std::string &Argument);
	std::string CleanComment(std::string &Argument);
	CStdioFile m_StreamFile;
};
