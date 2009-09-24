#include "StdAfx.h"
#include "ParkingScriptInterpreter.h"
#include "CommandsDictionary.h"
#include "ParkingCommand.h"
#include "ScriptSyntaxDefinitions.h"

CParkingScriptInterpreter::CParkingScriptInterpreter(IScriptReader *ScriptReader) : CScriptInterpreter(ScriptReader)
{
	AddCommandToCommandsDictionary(ParkingCommand, CParkingCommand);
}

CParkingScriptInterpreter::~CParkingScriptInterpreter(void)
{
}
