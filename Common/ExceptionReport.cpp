#include "stdafx.h"

//#include <Winsock2.h>
//#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <imagehlp.h>
#include <time.h>
#include <sys/timeb.h>
#include <process.h>
#include <new.h>
#include <malloc.h>
#include "psapi.h"

#include "ExceptionReport.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////

static CExceptionReport TheExceptionReport;

/*static*/ CExceptionReport& CExceptionReport::GetTheExceptionReport()
{
    return TheExceptionReport;
}

////////////////////////////////////


#define gle (GetLastError())
#define lenof(a) (sizeof(a) / sizeof((a)[0]))
#define MAXNAMELEN 1024 // max name length for found symbols
#define IMGSYMLEN ( sizeof IMAGEHLP_SYMBOL )
#define TTBUFLEN 65536 // for a temp buffer

// SymCleanup()
typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );
tSC pSC = NULL;

// SymFunctionTableAccess()
typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD AddrBase );
tSFTA pSFTA = NULL;

// SymGetLineFromAddr()
typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD dwAddr,
	OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );
tSGLFA pSGLFA = NULL;

// SymGetModuleBase()
typedef DWORD (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD dwAddr );
tSGMB pSGMB = NULL;

// SymGetModuleInfo()
typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );
tSGMI pSGMI = NULL;

// SymGetOptions()
typedef DWORD (__stdcall *tSGO)( VOID );
tSGO pSGO = NULL;

// SymGetSymFromAddr()
typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD dwAddr,
	OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );
tSGSFA pSGSFA = NULL;

// SymInitialize()
typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
tSI pSI = NULL;

// SymLoadModule()
typedef DWORD (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
	IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll );
tSLM pSLM = NULL;

// SymSetOptions()
typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );
tSSO pSSO = NULL;

// StackWalk()
typedef BOOL (__stdcall *tSW)( DWORD MachineType, HANDLE hProcess,
	HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
	PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
	PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
	PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
	PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );
tSW pSW = NULL;

// UnDecorateSymbolName()
typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
	DWORD UndecoratedLength, DWORD Flags );
tUDSN pUDSN = NULL;


void ShowStack( HANDLE hThread, CONTEXT& c ); // dump a stack
DWORD __stdcall TargetThread( void *arg );



// miscellaneous toolhelp32 declarations; we cannot #include the header
// because not all systems may have it
#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE   0x00000008
#pragma pack( push, 8 )
typedef struct tagMODULEENTRY32
{
    DWORD   dwSize;
    DWORD   th32ModuleID;       // This module
    DWORD   th32ProcessID;      // owning process
    DWORD   GlblcntUsage;       // Global usage count on the module
    DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
    BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
    DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
    HMODULE hModule;            // The hModule of this module in th32ProcessID's context
    char    szModule[MAX_MODULE_NAME32 + 1];
    char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;
#pragma pack( pop )



static LONG __stdcall CrashHandlerExceptionFilter (EXCEPTION_POINTERS* ep)
{
	return TheExceptionReport.CrashHandler(ep);
}

// Memory Allocation Exception Handler
static int HandleProgramMemoryDepletion(size_t Len)
{
	char Message[1000];
	sprintf_s(Message, "Memory Allocation Failed  Len:%u", Len);

	TheExceptionReport.ExitApp(Message, NULL);

    exit(-1);

	//return 0;
}

////////////////////////////////////

int CExceptionReport::InitStackWalk()
{
	//printf ("\n!!! InitStackWalk\n");

	HINSTANCE hImagehlpDll = NULL;

	// we load imagehlp.dll dynamically because the NT4-version does not
	// offer all the functions that are in the NT5 lib
	//hImagehlpDll = LoadLibrary( "imagehlp.dll" );
    hImagehlpDll = LoadLibrary( "dbghelp.dll" );
	if ( hImagehlpDll == NULL )
	{
		//printf( "LoadLibrary (\"imagehlp.dll\") failure : Error = %lu\r\n", GetLastError() );
		return 1;
	}

	pSC = (tSC) GetProcAddress( hImagehlpDll, "SymCleanup" );
	pSFTA = (tSFTA) GetProcAddress( hImagehlpDll, "SymFunctionTableAccess" );
	pSGLFA = (tSGLFA) GetProcAddress( hImagehlpDll, "SymGetLineFromAddr" );
	pSGMB = (tSGMB) GetProcAddress( hImagehlpDll, "SymGetModuleBase" );
	pSGMI = (tSGMI) GetProcAddress( hImagehlpDll, "SymGetModuleInfo" );
	pSGO = (tSGO) GetProcAddress( hImagehlpDll, "SymGetOptions" );
	pSGSFA = (tSGSFA) GetProcAddress( hImagehlpDll, "SymGetSymFromAddr" );
	pSI = (tSI) GetProcAddress( hImagehlpDll, "SymInitialize" );
	pSSO = (tSSO) GetProcAddress( hImagehlpDll, "SymSetOptions" );
	pSW = (tSW) GetProcAddress( hImagehlpDll, "StackWalk" );
	pUDSN = (tUDSN) GetProcAddress( hImagehlpDll, "UnDecorateSymbolName" );
	pSLM = (tSLM) GetProcAddress( hImagehlpDll, "SymLoadModule" );

	if ( pSC == NULL || pSFTA == NULL || pSGMB == NULL || pSGMI == NULL ||
		pSGO == NULL || pSGSFA == NULL || pSI == NULL || pSSO == NULL ||
		pSW == NULL || pUDSN == NULL || pSLM == NULL )
	{
		puts( "GetProcAddress(): some required function not found." );
		FreeLibrary( hImagehlpDll );
		return 1;
	}

	return 0;
////////////////////////////////////////////////////////////////////
  // 02-12-19: Now we only support dbghelp.dll!
  //           To use it on NT you have to install the redistrubutable for DBGHELP.DLL
//  g_hImagehlpDll = LoadLibrary( _T("dbghelp.dll") );
//  if ( g_hImagehlpDll == NULL )
//  {
//    printf( "LoadLibrary( \"dbghelp.dll\" ): GetLastError = %lu\n", gle );
//    g_bInitialized = FALSE;
//    return 1;
//  }

}


CExceptionReport::CExceptionReport()
{
	//printf ("!!! CExceptionReport\n");

	m_OutFile = INVALID_HANDLE_VALUE;
	
	memset(m_LogName,0,1024);

	m_bFirst = TRUE;

	m_Modules = NULL;
	m_LastElement = NULL;
}



void CExceptionReport::enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid )
{
	//printf ("!!! enumAndLoadModuleSymbols\n");
	// fill in module list
	GetModuleList( pid, hProcess );

	while (m_Modules != NULL)
	{
//		char zzz[1000];
//		sprintf (zzz,"\r\n=== %s %s %d\r\n", m_Modules->imageName, 
//												m_Modules->moduleName,
//												m_Modules->baseAddress,
//												m_Modules->size);
//		Log (zzz);

		if ( pSLM(  hProcess, 
					0, 
					m_Modules->imageName, 
					m_Modules->moduleName, 
					m_Modules->baseAddress, 
					m_Modules->size ) == 0 )
		{
			//printf( "Error %lu loading symbols for \"%s\"\r\n",
			//	GetLastError(), m_Modules->moduleName);
		}

		m_Modules = m_Modules->Next;
	}

}

void CExceptionReport::OpenOutFile()
{
    m_OutFile=CreateFile(	m_LogName,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);
    
    // Append 
    SetFilePointer(m_OutFile,0,0,FILE_END);
    
    // print to the file (time etc...)	#############
    
    time_t StartTime;
    time(&StartTime);
    
    char TempString[1000];
    char TimeStr[64];
    VERIFY(ctime_s(TimeStr, sizeof TimeStr, &StartTime) == 0);
    sprintf_s(TempString, "\r\n\r\n============== %s ================\r\n", TimeStr);
    
    // Write the record
    Log(TempString);
}

void CExceptionReport::Init(const char * FileName)
{	
//	printf ("!!! Init\n");
	strncpy_s(m_LogName, FileName, _TRUNCATE);

	// Set the Memory Allocaiton Exception Handler
	_set_new_handler(HandleProgramMemoryDepletion);
	_set_new_mode(1);

	SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
}

static void GetTimeString(char* TimeString, const int Size)
{
    memset(TimeString, 0, Size);

    _timeb TimeB;
    VERIFY(_ftime_s(&TimeB) == 0);
    time_t Seconds = TimeB.time;
    int    Millis  = TimeB.millitm;

//    struct tm* Tm = localtime(&Seconds);
    struct tm Tm;
    VERIFY(localtime_s(&Tm, &Seconds) == 0);
    strftime(TimeString, Size, "%a %b %d, %Y %H:%M:%S.", &Tm);

    char MilliString[16];
    memset(MilliString, 0, 16);

    _stprintf_s(MilliString, sizeof MilliString, "%03u", Millis);
    strcat_s(TimeString, Size, MilliString);
}

void CExceptionReport::ExitApp(char *ExitReason,CONTEXT *pContext)
{
	//printf ("!!! ExitApp\n");
	// Handle recursive exeption....
	if (m_bFirst == FALSE)
	{
		return;
	}

    OpenOutFile();

	m_bFirst = FALSE;

	char TempBuf[1000];

    const int TimeBufSize = 128;
    char TimeString[TimeBufSize + 1];
    GetTimeString(TimeString, TimeBufSize);

	sprintf_s(TempBuf, "\r\nApplication failure at: %s\r\nFailure reason: %s\r\n\r\n",
			TimeString, ExitReason);

	Log(TempBuf);

	//	Duplicate the handle of the proccess
	HANDLE hThread;
	if (DuplicateHandle(GetCurrentProcess(),
						GetCurrentThread(),
						GetCurrentProcess(),
						&hThread,
						0,
						false,
						DUPLICATE_SAME_ACCESS) != 0)
	{
		if (pContext != NULL)
		{
			ShowStack( hThread, *pContext );
		}

		CloseHandle(hThread);
	}
}


void CExceptionReport::ShowStack( HANDLE hThread, CONTEXT& c )
{
	//printf ("\n!!! ShowStack\n");
	InitStackWalk();

	// normally, call ImageNtHeader() and use machine info from PE header
	DWORD imageType = IMAGE_FILE_MACHINE_I386;
	HANDLE hProcess = GetCurrentProcess(); // hProcess normally comes from outside
	int frameNum; // counts walked frames
	DWORD offsetFromSymbol; // tells us how far from the symbol we were
	DWORD symOptions; // symbol handler settings
	IMAGEHLP_SYMBOL *pSym = (IMAGEHLP_SYMBOL *) malloc( IMGSYMLEN + MAXNAMELEN );
	char undName[MAXNAMELEN]; // undecorated name
	char undFullName[MAXNAMELEN]; // undecorated name with all shenanigans
	IMAGEHLP_MODULE Module;
	IMAGEHLP_LINE Line;
	char symSearchPath[10000];
	char /**tt = 0,*/ *p;

	char Buf[10000];

	STACKFRAME s; // in/out stackframe
	memset( &s, '\0', sizeof s );

	// NOTE: normally, the exe directory and the current directory should be taken
	// from the target process. The current dir would be gotten through injection
	// of a remote thread; the exe fir through either ToolHelp32 or PSAPI.

	char * const tt = new char[TTBUFLEN]; // this is a _sample_. you can do the error checking yourself.

	// build symbol search path from:
	symSearchPath[0]=0;

	// current directory
	if ( GetCurrentDirectory( TTBUFLEN, tt ) )
	{
		strcat_s(symSearchPath, tt);
		strcat_s(symSearchPath, ";");
	}

	// dir with executable
	if ( GetModuleFileName( 0, tt, TTBUFLEN ) )
	{
		for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )
		{
			// locate the rightmost path separator
			if ( *p == '\\' || *p == '/' || *p == ':' )
				break;
		}
		// if we found one, p is pointing at it; if not, tt only contains
		// an exe name (no path), and p points before its first byte
		if ( p != tt ) // path sep found?
		{
			if ( *p == ':' ) // we leave colons in place
				++ p;
			*p = '\0'; // eliminate the exe name and last path sep
			strcat_s(symSearchPath, tt);
			strcat_s(symSearchPath, ";");
		}
	}
	// environment variable _NT_SYMBOL_PATH
	if ( GetEnvironmentVariable( "_NT_SYMBOL_PATH", tt, TTBUFLEN ) )
	{
		strcat_s(symSearchPath, tt);
		strcat_s(symSearchPath, ";");
	}

	// environment variable _NT_ALTERNATE_SYMBOL_PATH
	if ( GetEnvironmentVariable( "_NT_ALTERNATE_SYMBOL_PATH", tt, TTBUFLEN ) )
	{
		strcat_s(symSearchPath, tt);
		strcat_s(symSearchPath, ";");
	}

	// environment variable SYSTEMROOT
	if ( GetEnvironmentVariable( "SYSTEMROOT", tt, TTBUFLEN ) )
	{
		strcat_s(symSearchPath, tt);
		strcat_s(symSearchPath, ";");
	}


	if ( strlen(symSearchPath) > 0 ) // if we added anything, we have a trailing semicolon
	{
		symSearchPath[strlen(symSearchPath) - 1] = 0;
	}

	sprintf_s(Buf, "symbols path: %s\r\n\r\n", symSearchPath);

	Log(Buf);
	//printf("%s",Buf);


	// why oh why does SymInitialize() want a writeable string?
    strncpy_s(tt, TTBUFLEN, symSearchPath, _TRUNCATE);

	// init symbol handler stuff (SymInitialize())
//	if ( ! pSI( hProcess, tt, false ) )
//	{
//		printf( "SymInitialize(): gle = %lu\r\n", gle );
//		goto cleanup;
//	}

	if ( pSI( hProcess, tt, false ) )
	{
		// SymGetOptions()
		symOptions = pSGO();
		symOptions |= SYMOPT_LOAD_LINES;
		symOptions &= ~SYMOPT_UNDNAME;
		pSSO( symOptions ); // SymSetOptions()

		// Enumerate modules and tell imagehlp.dll about them.
		// On NT, this is not necessary, but it won't hurt.
		enumAndLoadModuleSymbols( hProcess, GetCurrentProcessId() );

		// init STACKFRAME for first call
		// Notes: AddrModeFlat is just an assumption. I hate VDM debugging.
		// Notes: will have to be #ifdef-ed for Alphas; MIPSes are dead anyway,
		// and good riddance.
		s.AddrPC.Offset = c.Eip;
		s.AddrPC.Mode = AddrModeFlat;
		s.AddrStack.Offset		= c.Esp;
		s.AddrStack.Mode			= AddrModeFlat;
		s.AddrFrame.Offset = c.Ebp;
		s.AddrFrame.Mode = AddrModeFlat;

		memset( pSym, '\0', IMGSYMLEN + MAXNAMELEN );
		pSym->SizeOfStruct = IMGSYMLEN;
		pSym->MaxNameLength = MAXNAMELEN;

		memset( &Line, '\0', sizeof Line );
		Line.SizeOfStruct = sizeof Line;

		memset( &Module, '\0', sizeof Module );
		Module.SizeOfStruct = sizeof Module;

		offsetFromSymbol = 0;

		sprintf_s(Buf, "\r\n--# FV EIP----- RetAddr- FramePtr StackPtr Symbol\r\n" );

		Log(Buf);
		//printf("%s",Buf);


		for ( frameNum = 0; frameNum<50; ++ frameNum )
		{
			// get next stack frame (StackWalk(), SymFunctionTableAccess(), SymGetModuleBase())
			// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
			// assume that either you are done, or that the stack is so hosed that the next
			// deeper frame could not be found.
			if ( ! pSW( imageType, hProcess, hThread, &s, &c, NULL,
				pSFTA, pSGMB, NULL ) )
				break;

			// display its contents
			sprintf_s(Buf, "\r\n%3d %c%c %08lx %08lx %08lx %08lx ",
				frameNum, s.Far? 'F': '.', s.Virtual? 'V': '.',
				s.AddrPC.Offset, s.AddrReturn.Offset,
				s.AddrFrame.Offset, s.AddrStack.Offset );

			Log(Buf);
			//printf("%s",Buf);

			if ( s.AddrPC.Offset == 0 )
			{
				sprintf_s(Buf, "(-nosymbols- PC == 0)\r\n" );

				Log(Buf);
				//printf("%s",Buf);
			}
			else
			{ // we seem to have a valid PC
				// show procedure info (SymGetSymFromAddr())
				if ( ! pSGSFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, pSym ) )
				{
//					if ( gle != 487 )
//						printf( "SymGetSymFromAddr(): gle = %lu\r\n", gle );
				}
				else
				{
					// UnDecorateSymbolName()
					pUDSN( pSym->Name, undName, MAXNAMELEN, UNDNAME_NAME_ONLY );
					pUDSN( pSym->Name, undFullName, MAXNAMELEN, UNDNAME_COMPLETE );
					sprintf_s(Buf, "%s", undName );

					Log(Buf);
					//printf("%s",Buf);

					if ( offsetFromSymbol != 0 )
					{
						sprintf_s(Buf, " %+ld bytes", (long) offsetFromSymbol );

						Log(Buf);
						//printf("%s",Buf);
					}
					sprintf_s(Buf, "\r\n    Sig:  %s\r\n    Decl: %s\r\n", pSym->Name, undFullName  );

					Log(Buf);
					//printf("%s",Buf);
				}

				// show line number info, NT5.0-method (SymGetLineFromAddr())
				if ( pSGLFA != NULL )
				{ // yes, we have SymGetLineFromAddr()
					if ( ! pSGLFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, &Line ) )
					{
//						if ( gle != 487 )
//							printf( "SymGetLineFromAddr(): gle = %lu\r\n", gle );
					}
					else
					{
						sprintf_s(Buf, "    Line: %s(%lu) %+ld bytes\r\n",
							Line.FileName, Line.LineNumber, offsetFromSymbol );

						Log(Buf);
						//printf("%s",Buf);
					}
				} // yes, we have SymGetLineFromAddr()

				// show module info (SymGetModuleInfo())
				if ( ! pSGMI( hProcess, s.AddrPC.Offset, &Module ) )
				{
					//printf( "SymGetModuleInfo): gle = %lu\r\n", gle );
				}
				else
				{ // got module info OK
					char ty[80];
					switch ( Module.SymType )
					{
					case SymNone:
                        strncpy_s(ty, "-nosymbols-", _TRUNCATE);
						break;
					case SymCoff:
						strncpy_s(ty, "COFF", _TRUNCATE);
						break;
					case SymCv:
						strncpy_s(ty, "CV", _TRUNCATE);
						break;
					case SymPdb:
						strncpy_s(ty, "PDB", _TRUNCATE);
						break;
					case SymExport:
						strncpy_s(ty, "-exported-", _TRUNCATE);
						break;
					case SymDeferred:
						strncpy_s(ty, "-deferred-", _TRUNCATE);
						break;
					case SymSym:
						strncpy_s(ty, "SYM", _TRUNCATE);
						break;
					default:
						sprintf_s(ty, "symtype=%ld", (long) Module.SymType);
						break;
					}

					sprintf_s(Buf, "    Mod:  %s[%s], base: %08lxh\r\n",
						Module.ModuleName, Module.ImageName, Module.BaseOfImage);
					Log(Buf);
					//printf("%s",Buf);

					sprintf_s(Buf, "    Sym:  type: %s, file: %s\r\n",
						ty, Module.LoadedImageName);
					Log(Buf);
					//printf("%s",Buf);

				} // got module info OK
			} // we seem to have a valid PC

			// no return address means no deeper stackframe
			if ( s.AddrReturn.Offset == 0 )
			{
				// avoid misunderstandings in the printf() following the loop
				SetLastError( 0 );
				break;
			}

		} // for ( frameNum )

//		if ( gle != 0 )
//			printf( "\r\nStackWalk(): gle = %lu\r\n", gle );	
	}
//	else
//		printf( "SymInitialize(): gle = %lu\r\n", gle );

	ResumeThread( hThread );
	// de-init symbol handler etc. (SymCleanup())
	pSC( hProcess );
	free( pSym );
	delete [] tt;
}


LONG CExceptionReport::CrashHandler (EXCEPTION_POINTERS* ep)
{
	//printf ("\n!!! CrashHandler\n");
	char errMsg[512];
	char perrMsg[512];

	switch (ep->ExceptionRecord->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			strncpy_s(errMsg, "EXCEPTION_ACCESS_VIOLATION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_DATATYPE_MISALIGNMENT:
			strncpy_s(errMsg, "EXCEPTION_DATATYPE_MISALIGNMENT\r\n", _TRUNCATE);
			break;

		case EXCEPTION_BREAKPOINT:
			strncpy_s(errMsg, "EXCEPTION_BREAKPOINT\r\n", _TRUNCATE);
			break;

		case EXCEPTION_SINGLE_STEP:
			strncpy_s(errMsg, "EXCEPTION_SINGLE_STEP\r\n", _TRUNCATE);
			break;

		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			strncpy_s(errMsg, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_DENORMAL_OPERAND:
			strncpy_s(errMsg, "EXCEPTION_FLT_DENORMAL_OPERAND\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			strncpy_s(errMsg, "EXCEPTION_FLT_DIVIDE_BY_ZERO\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_INEXACT_RESULT:
			strncpy_s(errMsg, "EXCEPTION_FLT_INEXACT_RESULT\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_INVALID_OPERATION:
			strncpy_s(errMsg, "EXCEPTION_FLT_INVALID_OPERATION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_OVERFLOW:
			strncpy_s(errMsg, "EXCEPTION_FLT_OVERFLOW\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_STACK_CHECK:
			strncpy_s(errMsg, "EXCEPTION_FLT_STACK_CHECK\r\n", _TRUNCATE);
			break;

		case EXCEPTION_FLT_UNDERFLOW:
			strncpy_s(errMsg, "EXCEPTION_FLT_UNDERFLOW\r\n", _TRUNCATE);
			break;

		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			strncpy_s(errMsg, "EXCEPTION_INT_DIVIDE_BY_ZERO\r\n", _TRUNCATE);
			break;

		case EXCEPTION_INT_OVERFLOW:
			strncpy_s(errMsg, "EXCEPTION_INT_OVERFLOW\r\n", _TRUNCATE);
			break;

		case EXCEPTION_PRIV_INSTRUCTION:
			strncpy_s(errMsg, "EXCEPTION_PRIV_INSTRUCTION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_IN_PAGE_ERROR:
			strncpy_s(errMsg, "EXCEPTION_IN_PAGE_ERROR\r\n", _TRUNCATE);
			break;

		case EXCEPTION_ILLEGAL_INSTRUCTION:
			strncpy_s(errMsg, "EXCEPTION_ILLEGAL_INSTRUCTION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			strncpy_s(errMsg, "EXCEPTION_NONCONTINUABLE_EXCEPTION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_STACK_OVERFLOW:
			strncpy_s(errMsg, "EXCEPTION_STACK_OVERFLOW\r\n", _TRUNCATE);
			break;

		case EXCEPTION_INVALID_DISPOSITION:
			strncpy_s(errMsg, "EXCEPTION_INVALID_DISPOSITION\r\n", _TRUNCATE);
			break;

		case EXCEPTION_GUARD_PAGE:
			strncpy_s(errMsg, "EXCEPTION_GUARD_PAGE\r\n", _TRUNCATE);
			break;

		case EXCEPTION_INVALID_HANDLE:
			strncpy_s(errMsg, "EXCEPTION_INVALID_HANDLE\r\n", _TRUNCATE);
			break;

		default:
			sprintf_s(errMsg, "Unknown Exception: %x\r\n", ep->ExceptionRecord->ExceptionCode);
			break;

	}

	sprintf_s(perrMsg, "RunTime Exception: %s", errMsg);

//	ExceptionMessage(perrMsg,ep->ContextRecord);
	ExitApp(errMsg, ep->ContextRecord);

//	return EXCEPTION_EXECUTE_HANDLER;
    return EXCEPTION_CONTINUE_SEARCH;
}

void CExceptionReport::Log(const char *String)
{
//	printf ("\n!!! Log\n");
	unsigned long WriteSize;

	// Write the record
	if (WriteFile(m_OutFile,String,strlen(String),&WriteSize,NULL)==FALSE)
	{
		CloseHandle(m_OutFile);
		return;
	}
}


BOOL CExceptionReport::GetModuleList( DWORD pid, HANDLE /*hProcess*/ )
{
	//printf ("\n!!! GetModuleList\n");
	// try toolhelp32 first
	if ( GetModuleListTH32( pid ) )
	{
		return TRUE;
	}

	// nope? try psapi, then
//	return GetModuleListPSAPI( pid, hProcess );
	//printf ("GetModuleListPSAPI");
	return FALSE;
}


BOOL CExceptionReport::GetModuleListTH32( DWORD pid )
{
	//printf ("\n!!! GetModuleListTH32\n");
	char Buf[10000];

	// CreateToolhelp32Snapshot()
	typedef HANDLE (__stdcall *tCT32S)( DWORD dwFlags, DWORD th32ProcessID );
	// Module32First()
	typedef BOOL (__stdcall *tM32F)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );
	// Module32Next()
	typedef BOOL (__stdcall *tM32N)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );

	// I think the DLL is called tlhelp32.dll on Win9X, so we try both
	const char *dllname[] = { "kernel32.dll", "tlhelp32.dll" };
	HINSTANCE hToolhelp = 0;
	tCT32S pCT32S = 0;
	tM32F pM32F   = 0;
	tM32N pM32N   = 0;

	HANDLE hSnap     = (HANDLE)-1;
	MODULEENTRY32 me = { sizeof me };
	bool keepGoing;
	ModuleEntry * CurrModule;
	int i;

	for ( i = 0; i < lenof( dllname ); ++ i )
	{
		hToolhelp = LoadLibrary( dllname[i] );
		if ( hToolhelp == 0 )
			continue;

		pCT32S = (tCT32S) GetProcAddress( hToolhelp, "CreateToolhelp32Snapshot" );
		pM32F = (tM32F) GetProcAddress( hToolhelp, "Module32First" );
		pM32N = (tM32N) GetProcAddress( hToolhelp, "Module32Next" );
		if ( pCT32S != 0 && pM32F != 0 && pM32N != 0 )
			break; // found the functions!

		FreeLibrary( hToolhelp );
		hToolhelp = 0;
	}

	if ( hToolhelp == 0 ) // nothing found?
		return false;

	hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
	if ( hSnap == (HANDLE) -1 )
		return false;

	keepGoing = !!pM32F( hSnap, &me );
	while ( keepGoing )
	{

		CurrModule = new ModuleEntry;

//		Log ("!!!\r\n");
		// here, we have a filled-in MODULEENTRY32
		sprintf_s(Buf, "%08lXh %6lu %-15.15s %s\r\n", me.modBaseAddr, me.modBaseSize, me.szModule, me.szExePath );
		Log(Buf);
		//printf("%s",Buf);

		strncpy_s(CurrModule->imageName, me.szExePath, _TRUNCATE);
		strncpy_s(CurrModule->imageName, me.szModule, _TRUNCATE);
		CurrModule->baseAddress = (DWORD) me.modBaseAddr;
		CurrModule->size = me.modBaseSize;
		CurrModule->Next = NULL;

		if (m_LastElement == NULL)
		{
			m_Modules = CurrModule;
		}
		else
		{
			m_LastElement->Next = CurrModule;
		}

		m_LastElement = CurrModule;

		
		keepGoing = !!pM32N( hSnap, &me );
	}

	CloseHandle( hSnap );

	FreeLibrary( hToolhelp );

//	return modules.size() != 0;
	if (m_Modules == NULL)
		return FALSE;
	return TRUE;
}

void CExceptionReport::CheckHeapStatus()
{
	//printf ("\n!!! CheckHeapStatus\n");
	char Buf[100];
	int  heapstatus;

	sprintf_s(Buf, "Heap status:\r\n" );
	Log(Buf);
	//printf("%s",Buf);

	// Check heap status
	heapstatus = _heapchk();
	switch( heapstatus )
	{
		case _HEAPOK:
			sprintf_s(Buf, "  OK - heap is fine\r\n" );
			Log(Buf);
			//printf("%s",Buf);
			break;
		case _HEAPEMPTY:
			sprintf_s(Buf, "  OK - heap is empty\r\n" );
			Log(Buf);
			//printf("%s",Buf);
			break;
		case _HEAPBADBEGIN:
			sprintf_s(Buf, "  ERROR - bad start of heap\r\n" );
			Log(Buf);
			//printf("%s",Buf);
			break;
		case _HEAPBADNODE:
			sprintf_s(Buf, "  ERROR - bad node in heap\r\n" );
			Log(Buf);
			//printf("%s",Buf);
			break;
	}

	return;
}


/*
bool fillModuleListPSAPI( ModuleList& modules, DWORD pid, HANDLE hProcess )
{
	// EnumProcessModules()
	typedef BOOL (__stdcall *tEPM)( HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
	// GetModuleFileNameEx()
	typedef DWORD (__stdcall *tGMFNE)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
	// GetModuleBaseName() -- redundant, as GMFNE() has the same prototype, but who cares?
	typedef DWORD (__stdcall *tGMBN)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
	// GetModuleInformation()
	typedef BOOL (__stdcall *tGMI)( HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );

	HINSTANCE hPsapi;
	tEPM pEPM;
	tGMFNE pGMFNE;
	tGMBN pGMBN;
	tGMI pGMI;

	int i;
	ModuleEntry e;
	DWORD cbNeeded;
	MODULEINFO mi;
	HMODULE *hMods = 0;
	char *tt = 0;

	hPsapi = LoadLibrary( "psapi.dll" );
	if ( hPsapi == 0 )
		return false;

	modules.clear();

	pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );
	pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );
	pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );
	pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );
	if ( pEPM == 0 || pGMFNE == 0 || pGMBN == 0 || pGMI == 0 )
	{
		// yuck. Some API is missing.
		FreeLibrary( hPsapi );
		return false;
	}

	hMods = new HMODULE[TTBUFLEN / sizeof HMODULE];
	tt = new char[TTBUFLEN];
	// not that this is a sample. Which means I can get away with
	// not checking for errors, but you cannot. :)

	if ( ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) )
	{
		printf( "EPM failed, gle = %lu\r\n", gle );
		goto cleanup;
	}

	if ( cbNeeded > TTBUFLEN )
	{
		printf( "More than %lu module handles. Huh?\r\n", lenof( hMods ) );
		goto cleanup;
	}

	for ( i = 0; i < cbNeeded / sizeof hMods[0]; ++ i )
	{
		// for each module, get:
		// base address, size
		pGMI( hProcess, hMods[i], &mi, sizeof mi );
		e.baseAddress = (DWORD) mi.lpBaseOfDll;
		e.size = mi.SizeOfImage;
		// image file name
		tt[0] = '\0';
		pGMFNE( hProcess, hMods[i], tt, TTBUFLEN );
		e.imageName = tt;
		// module name
		tt[0] = '\0';
		pGMBN( hProcess, hMods[i], tt, TTBUFLEN );
		e.moduleName = tt;
		printf( "%08lXh %6lu %-15.15s %s\r\n", e.baseAddress,
			e.size, e.moduleName.c_str(), e.imageName.c_str() );

		modules.push_back( e );
	}

cleanup:
	if ( hPsapi )
		FreeLibrary( hPsapi );
	delete [] tt;
	delete [] hMods;

	return modules.size() != 0;
}
*/

