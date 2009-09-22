#pragma once

//////////////////////////////////////////////////////////////////////////
// CResourcedGuiApplication
//
//Serves as infrastructure that will allow fast and easy development 
//of applications. (due to the fact that each project will have a distinct executable). 
//This base class that will provide the following capabilities: 
//  o	Tracing to the screen
//  o	Logging to files
//  o	Ini File support 
//  o	About Box
//  o	Menus
//
//It will read the following resources from the AppResources DLL:
//  o	Main Window Dialog
//  o	About Box.
//  o	DAL Default Icon
//
//It will display the 'Application Name' and 'Application version' in the about box.



#include "GuiApplication.h"
#include "TraceTabBaseDlg.h"
#include "..\Common/MenuManager.h"

class CResourcedGuiApplication : public CGuiApplication, 
    public IMenuEventHandler,
    public ITraceDlgEvents
{
public:
    CResourcedGuiApplication(const char* AppTitle, 
        const char* AppVersion,
        int IconResourceId = -1,
        bool AllowMultipleApplication = true,
        bool EditConfig = true);

    virtual ~CResourcedGuiApplication();

    const CString GetCaption();
    void GetCaption(CString& Caption);
    void SetCaption(const char * Caption);
    void SetIdToCaption(int Id, const char * IdStr = " Id: ");
    void SetIdToCaption(const char * Id, const char * IdStr = " Id: ");
    void ModifyTabStyle(DWORD dwRemove, DWORD dwAdd);
    bool SetAffinity();
    bool SetProcessPriority();
    void RememberTab();
    void AddProcessIdToCaption();

private:
    // Hide copy ctor and assignment operator
    CResourcedGuiApplication(const CResourcedGuiApplication &);
    CResourcedGuiApplication & operator=(const CResourcedGuiApplication &);

protected:
    //add dialog to be displayed in the Tab control.
    bool AddDialog(CDialog& DlgObject, int DialogId, const char* DialogTitle);

    //define if the application will display 'Close confirmation' dialog 
    //on application close
    void SetConfirmCloseApplication(bool ConfirmClose); 

    bool AddMenuItem(int MenuId, const char* MenuName, const char* SubMenuName);

    //call this function to terminate the application, this should be the last call
    //after all cleanups
    void CloseApplication();

    //call this function to start the application closing sequence. This function 
    //will generate the application cleanup.
    void PostCloseRequest();
 
    //////////////////////////////////////////////////////////////////////////
    //to be override by specific Application
    virtual void AddDialogs();

    //override this function to perform application startup initializations
    virtual bool PerformInitalization();

    //basic implementation calls the CloseApplication. 
    //Derived class should eventually call the CloseApplication after 
    //completing cleanup.
    virtual bool PerformCleanup();

    // Implemented CGuiApplication methods
    virtual bool Init();
    virtual void Close();

    //override this function to perform menu commands tasks
    enum
    {
        IDM_ABOUT = 0x0010,
    };
    virtual void OnMenuSelected(int Id);

private:
    // ITraceDlgEvents
    virtual void OnTraceDlgClose();
    virtual void OnSystemShutdown();

    void DisplayAboutBox();
    bool ConfirmClose();
    void ShutDown();

private:
    class CResourcedMainDlg* m_MainDlg;
    HINSTANCE           m_ResourceDLL;
    HINSTANCE           m_ExeResourceHandle;
    HMODULE             m_ResourceModuleHandle;
    std::string         m_AppVersion;
    int                 m_IconId;
    bool                m_ConfirmCloseApplication;
    bool                m_EditConfig;
    bool                m_RememberTab;
};
