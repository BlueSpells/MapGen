#include "StdAfx.h"
#include "ParkingScriptInterpreter.h"
#include "CommandsDictionary.h"
#include "ParkingCommand.h"
#include "BasicItemCommand.h"
#include "PositionCommand.h"
#include "SignedJumpCommand.h"
#include "AbsoluteJumpCommand.h"
#include "ComplexItemCommand.h"
#include "PavementCommand.h"
#include "GeneralHeaderCommand.h"
#include "ParkingMapHeaderCommand.h"
#include "ScriptSyntaxDefinitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CParkingScriptInterpreter::CParkingScriptInterpreter(IScriptReader *ScriptReader) : CScriptInterpreter(ScriptReader)
{
	AddCommandToCommandsDictionary(ParkingCommand, CParkingCommand);
	AddCommandToCommandsDictionary(BasicItemCommand, CBasicItemCommand);
	AddCommandToCommandsDictionary(PositionCommand, CPositionCommand);
	AddCommandToCommandsDictionary(SignedJumpCommand, CSignedJumpCommand);
	AddCommandToCommandsDictionary(AbsoluteJumpCommand, CAbsoluteJumpCommand);
	AddCommandToCommandsDictionary(PavementCommand, CPavementCommand);
	AddCommandToCommandsDictionary(ComplexItemCommand, CComplexItemCommand);
	AddCommandToCommandsDictionary(GeneralHeaderCommand, CGeneralHeaderCommand);
	AddCommandToCommandsDictionary(ParkingMapHeaderCommand, CParkingMapHeaderCommand);
	
}

CParkingScriptInterpreter::~CParkingScriptInterpreter(void)
{
	CCommandsDictionary::DestroyCommandsDictionary();
}
