// ExceptionReport.h: interface for the CEtsiConverter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EXCEPTIONREPORT_H__INCLUDED_)
#define EXCEPTIONREPORT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExceptionReport
{
public:
	// Constructor
	CExceptionReport();

	LONG CrashHandler (EXCEPTION_POINTERS* ExcepPtr);

	void enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid );

	void Init(const char * FileName);
	void ExitApp(char *ExitReason,CONTEXT *pContext);

    static CExceptionReport& GetTheExceptionReport();

private:
    // Hide copy ctor and assignment operator
    CExceptionReport(const CExceptionReport &);
    CExceptionReport & operator=(const CExceptionReport &);

private:
    void OpenOutFile();
	HANDLE m_OutFile;

    struct  ModuleEntry
    {
	    char imageName[250];
	    char moduleName[250];
	    DWORD baseAddress;
	    DWORD size;

	    ModuleEntry * Next;
    };

	char m_LogName[1024];

	BOOL m_bFirst;

	int InitStackWalk();

	void ShowStack(HANDLE hThread, CONTEXT& Context);

	BOOL GetModuleList(DWORD pid, HANDLE hProcess );

	BOOL GetModuleListTH32(DWORD pid);

	ModuleEntry * m_Modules;
	ModuleEntry * m_LastElement;

	void Log(const char * Buf);

//	tSC pSC;
//	tSFTA pSFTA;
//	tSGLFA pSGLFA;
//	tSGMB pSGMB;
//	tSGMI pSGMI;
//	tSGO pSGO;
//	tSGSFA pSGSFA;
//	tSI pSI;
//	tSLM pSLM;
//	tSSO pSSO;
//	tSW pSW;
//	tUDSN pUDSN;

	void CheckHeapStatus();
};

#endif // !defined(EXCEPTIONREPORT_H__INCLUDED_)
