#pragma once

#include <vector>
#include <string>

class IScriptReader
{
public:
	IScriptReader(void) : m_LineIndex(-1) {};
	~IScriptReader(void) {};

	virtual bool Init() = 0;
	virtual bool OpenScriptFile(CString FileName) = 0;
	virtual bool CloseScriptFile() = 0;
	virtual bool ReadLine(std::string &Command, std::vector<std::string> &ParametersList) = 0;
	bool GetLineIndex() {return m_LineIndex;}

protected:
	int m_LineIndex;
};
