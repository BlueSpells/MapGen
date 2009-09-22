// AppResources.h : main header file for the AppResources DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH (stdafx.h should include 'afxwin.h')
#endif

#include "resource.h"		// main symbols


// CAppResourcesApp
// See AppResources.cpp for the implementation of this class
//

class CAppResourcesApp : public CWinApp
{
public:
	CAppResourcesApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
