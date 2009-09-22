// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER
#define WINVER 0x0501
#endif

#define _USE_32BIT_TIME_T

#include <afx.h>
#include <afxwin.h>
#include <afxmt.h>
#include <afxtempl.h>

#include <afxdisp.h>        // MFC Automation classes
#include <afxsock.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
