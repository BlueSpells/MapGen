// GuiApplication.h: interface for the GuiApplication class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <afxmt.h>

#include "..\Common/LogEvent.h"

class CGuiApplication : public CWinApp
{
public:
	CGuiApplication(const char* ApplicationName, bool AllowMultipleInstances = true);
    const char* GetApplicationName();
    const char* GetIniFileName();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuiApp)
	public:
	virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGuiApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
protected:
    // Should be called by derived classes
    void SetTraceDlg(class CTraceDlg* TraceDlg);
	static void GuiLogOutput(ELogSeverity Severity, const char* Text);
    static ELogSeverity GetGuiLogOutputSeverity();

    // Should be implemented by derived class
    virtual bool Init() = 0;
    virtual void Close() {}
private:
	std::string m_ApplicationName;
    std::string	m_IniFileName;

    bool m_AllowMultipleInstances; //use the mutex or not
    CMutex* m_AppMutex;
};

