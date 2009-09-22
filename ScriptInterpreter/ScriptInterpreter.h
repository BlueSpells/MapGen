#pragma once

#include <vector>
class IItem;
class IHeader;
class IScriptReader;

class CScriptInterpreter
{
public:
	CScriptInterpreter(IScriptReader *ScriptReader);
	~CScriptInterpreter(void);

	bool Init();
	bool InterperetFile(std::string FileName, std::vector<IHeader *> &HeaderList, std::vector<IItem *> &ItemsList);

protected:
	IScriptReader *m_ScriptReader;
};
