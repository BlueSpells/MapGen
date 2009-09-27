#include "StdAfx.h"
#include "ParkingScriptInterpreter.h"
#include "CommandsDictionary.h"
#include "ParkingCommand.h"
#include "BasicItemCommand.h"
#include "PositionCommand.h"
#include "SignedJumpCommand.h"
#include "ScriptSyntaxDefinitions.h"

CParkingScriptInterpreter::CParkingScriptInterpreter(IScriptReader *ScriptReader) : CScriptInterpreter(ScriptReader)
{
	AddCommandToCommandsDictionary(ParkingCommand, CParkingCommand);
	AddCommandToCommandsDictionary(BasicItemCommand, CBasicItemCommand);
	AddCommandToCommandsDictionary(PositionCommand, CPositionCommand);
	AddCommandToCommandsDictionary(SignedJumpCommand, CSignedJumpCommand);
}

CParkingScriptInterpreter::~CParkingScriptInterpreter(void)
{
}
