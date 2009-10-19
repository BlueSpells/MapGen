// WindowsMobileDemo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef SMARTPHONE2003_UI_MODEL
#include "resourcesp.h"
#endif

// CWindowsMobileDemoApp:
// See WindowsMobileDemo.cpp for the implementation of this class
//

class CWindowsMobileDemoApp : public CWinApp
{
public:
	CWindowsMobileDemoApp();
	
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWindowsMobileDemoApp theApp;
