#pragma once
#include "scriptinterpreter.h"

class CParkingScriptInterpreter : public CScriptInterpreter
{
public:
	CParkingScriptInterpreter(IScriptReader *ScriptReader);
	~CParkingScriptInterpreter(void);
};
