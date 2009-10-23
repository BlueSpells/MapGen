//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
#include "stdafx.h"
#include "graphmanager.h"

CGraphManager::CGraphManager()
{
  m_fGraphBuilt = FALSE;
  ZeroMemory( m_handle, sizeof( m_handle ));
  m_hwnd = 0;
  m_dwThreadId = 0;
  m_hThread = NULL;
  m_hCommandCompleted = NULL;
  m_currentCommand = COMMAND_NOCOMMAND;
}


CGraphManager::~CGraphManager()
{
  if( m_handle[0] )
  {
    CloseHandle( m_handle[0] );
  }

  if( m_handle[1] )
  {
    CloseHandle( m_handle[1] );
  }
}


HRESULT
CGraphManager::Init()
{
  HRESULT hr = S_OK;

  // Create the event that will signal the thread for commands
  m_handle[0] = CreateEvent( NULL, FALSE, FALSE, NULL );
  if( m_handle[0] == NULL )
  {
    ERR( HRESULT_FROM_WIN32( GetLastError() ));
  }
  m_handle[1] = 0;

  // Create the event to sync on to wait for the command to be executed
  m_hCommandCompleted = CreateEvent( NULL, FALSE, FALSE, NULL );
  if( m_hCommandCompleted == NULL )
  {
    ERR( HRESULT_FROM_WIN32( GetLastError() ));
  }

  // CCreate the thread that will run the filtergraph. 
  // The filtergraph is runing on a background thread to prevent any window message
  // reentrancy issue. 
  m_hThread = CreateThread( NULL, 0, CGraphManager::ThreadProc, this, 0, &m_dwThreadId );
  if( m_hThread == NULL )
  {
    ERR( HRESULT_FROM_WIN32( GetLastError() ));
  }


Cleanup:
  return hr;
}


HRESULT
CGraphManager::BuildCaptureGraph()
{
  // The Graph is built on a separate thread to 
  // prevent reentrancy issues. 
  m_currentCommand = COMMAND_BUILDGRAPH;
  SetEvent( m_handle[0] );
  WaitForSingleObject( m_hCommandCompleted, INFINITE );

  return S_OK;
}


HRESULT
CGraphManager::RunCaptureGraph()
{
  // Unlike the other operations, running the graph
  // has to happen from the UI thread. 
  return RunCaptureGraphInternal();
}

HRESULT 
CGraphManager::CaptureStillImage()
{
  m_currentCommand = COMMAND_STILLIMAGE;
  SetEvent( m_handle[0] );
  WaitForSingleObject( m_hCommandCompleted, INFINITE );

  return S_OK;
}


HRESULT
CGraphManager::ShutDown()
{
  m_currentCommand = COMMAND_SHUTDOWN;
  SetEvent( m_handle[0] );
  WaitForSingleObject( m_hThread, INFINITE );

  return S_OK;
}


HRESULT
CGraphManager::RegisterNotificationWindow( HWND hwnd )
{
  m_hwnd = hwnd;
  return S_OK;
}


DWORD WINAPI 
CGraphManager::ThreadProc( LPVOID lpParameter )
{
  HRESULT       hr = S_OK;
  DWORD         dwReturnValue;
  CGraphManager *pThis = (CGraphManager*) lpParameter;
  GRAPHCOMMANDS command = COMMAND_NOCOMMAND;

  if( pThis == NULL )
  {
    return 0;
  }

  while(( command != COMMAND_SHUTDOWN ) && ( hr != S_FALSE ))
  {
    dwReturnValue = WaitForMultipleObjects( 2, pThis->m_handle, FALSE, INFINITE );
    switch( dwReturnValue )
    {
    case WAIT_OBJECT_0:
      command = pThis->m_currentCommand;
      pThis->ProcessCommand();
      break;

    case WAIT_OBJECT_0 + 1:
      pThis->ProcessDShowEvent();
      break;

    default:
      break;
    }
  };

  return 0;
}


HRESULT
CGraphManager::ProcessCommand()
{
  HRESULT hr = S_OK;

  switch( m_currentCommand )
  {
  case COMMAND_BUILDGRAPH:
    hr = CreateCaptureGraphInternal();
    SetEvent( m_hCommandCompleted );
    break;

  case COMMAND_RUNGRAPH:
    hr = RunCaptureGraphInternal();
    SetEvent( m_hCommandCompleted );
    break;

  case COMMAND_STILLIMAGE:
    hr = CaptureStillImageInternal();
    SetEvent( m_hCommandCompleted );
    break;

  case COMMAND_SHUTDOWN:
    hr = S_FALSE;
    break;

  default:
    break;
  }

  return hr;
}


HRESULT
CGraphManager::ProcessDShowEvent()
{
  HRESULT hr = S_OK;
  long    lEventCode, lParam1, lParam2;

  CComPtr<IMediaEvent> pMediaEvent;
  CComPtr<IGraphBuilder> pFilterGraph;
  CComPtr<IMediaControl> pMediaControl;

  if( m_pCaptureGraphBuilder == NULL )
  {
    ERR( E_FAIL );
  }

  CHK( m_pCaptureGraphBuilder->GetFiltergraph( &pFilterGraph ));
  CHK( pFilterGraph->QueryInterface( &pMediaEvent ));
  CHK( pMediaEvent->GetEvent( &lEventCode, &lParam1, &lParam2, 0 ));

  if( lEventCode == EC_CAP_FILE_COMPLETED )
  {
    NotifyMessage( MESSAGE_FILECAPTURED, L"File captured ..." );
  }


  CHK( pMediaEvent->FreeEventParams( lEventCode, lParam1, lParam2 ));

Cleanup:
  return S_OK;
}


HRESULT 
CGraphManager::GetFirstCameraDriver( WCHAR *pwzName )
{
  HRESULT hr = S_OK;
  HANDLE	handle = NULL;
  DEVMGR_DEVICE_INFORMATION di;
  GUID guidCamera = { 0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A, 0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86 };
  // Note about the above: The driver material doesn't ship as part of the SDK. This GUID is hardcoded
  // here to be able to enumerate the camera drivers and pass the name of the driver to the video capture filter

  if( pwzName == NULL )
  {
    return E_POINTER;
  }

  di.dwSize = sizeof(di);

  handle = FindFirstDevice( DeviceSearchByGuid, &guidCamera, &di );
  if(( handle == NULL ) || ( di.hDevice == NULL ))
  {
    ERR( HRESULT_FROM_WIN32( GetLastError() ));
  }

  StringCchCopy( pwzName, MAX_PATH, di.szLegacyName );

Cleanup:
  FindClose( handle );
  return hr;
}

HRESULT
CGraphManager::CreateCaptureGraphInternal()
{
  HRESULT hr = S_OK;
  WCHAR	      wzDeviceName[ MAX_PATH + 1 ];

  CPropertyBag  PropBag;
  CComVariant   varCamName;
  OAEVENT       oaEvent;

  CComPtr<IMediaEvent>            pMediaEvent;
  CComPtr<IGraphBuilder>          pFilterGraph;
  CComPtr<IVideoWindow>           pVidWin;
  CComPtr<IBaseFilter>			      pImageSinkFilter;
  CComPtr<IPersistPropertyBag>    pPropertyBag;

  CHK( m_pCaptureGraphBuilder.CoCreateInstance( CLSID_CaptureGraphBuilder ));
  CHK( CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_IGraphBuilder, (void **)&pFilterGraph));
  CHK( m_pCaptureGraphBuilder->SetFiltergraph( pFilterGraph ));

  CHK( pFilterGraph->QueryInterface(IID_IVideoWindow,(void **)&pVidWin));

  CoCreateInstance(CLSID_VideoCapture, NULL, CLSCTX_INPROC,IID_IBaseFilter, (void**)&m_pVideoCaptureFilter);
  m_pVideoCaptureFilter->QueryInterface( &pPropertyBag );

  CHK( GetFirstCameraDriver(wzDeviceName));
  varCamName = wzDeviceName;   

  PropBag.Write( L"VCapName", &varCamName );
  pPropertyBag->Load( &PropBag, NULL );

  SetCameraResolution(4);
  // Everything succeeded, the video capture filter is added to the filtergraph
  CHK( pFilterGraph->AddFilter( m_pVideoCaptureFilter, L"Video Capture Filter Source" ));

  //
  // Create the still image filter, and connect it to the video capture filter
  //
  CHK( pImageSinkFilter.CoCreateInstance( CLSID_IMGSinkFilter ));
  CHK( pFilterGraph->AddFilter( pImageSinkFilter, L"Still image filter" ));
  CHK( m_pCaptureGraphBuilder->RenderStream( &PIN_CATEGORY_STILL, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, pImageSinkFilter ));
  CHK( pImageSinkFilter.QueryInterface( &m_pImageSinkFilter ));

  CHK( m_pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, m_pVideoCaptureFilter, NULL, NULL));  
  CHK( pVidWin->put_Owner((OAHWND)m_hwnd));
  CHK( pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS));

  long pWidth, pHeight;
  CHK( pVidWin->get_Width(&pWidth));
  CHK( pVidWin->get_Height(&pHeight));
  CHK( pVidWin->SetWindowPosition(30, 120, pWidth, pHeight));
  
  CHK( pVidWin->put_AutoShow(OATRUE));
  CHK( pVidWin->put_Visible(OATRUE));
  CHK( pVidWin->put_WindowState(SW_SHOW));

  // Let's get the handle for DShow events. The main loop will listen to both notifications from 
  // the UI thread and for DShow notifications
  //
  CHK( pFilterGraph->QueryInterface( IID_IMediaEvent, (void**) &pMediaEvent ));
  CHK( pMediaEvent->GetEventHandle( &oaEvent ));
  m_handle[1] = (HANDLE) oaEvent;

  m_fGraphBuilt = TRUE;
  NotifyMessage( MESSAGE_ERROR, L"Builing the graph failed" );

Cleanup:
  if( FAILED( hr ))
  {
    NotifyMessage( MESSAGE_ERROR, L"Builing the graph failed" );
  }
  return hr;
}

HRESULT
CGraphManager::RunCaptureGraphInternal()
{
  HRESULT hr = S_OK;

  CComPtr<IGraphBuilder> pGraphBuilder;
  CComPtr<IMediaControl> pMediaControl;

  // Let's make sure that the graph has been initialized
  if(( m_pCaptureGraphBuilder == NULL ) || ( m_fGraphBuilt == FALSE ))
  {
    ERR( E_FAIL );
  }

  // Retrieve the filtergraph off the capture graph builder
  CHK( m_pCaptureGraphBuilder->GetFiltergraph( &pGraphBuilder ));

  // Get the media control interface, and run the graph
  CHK( pGraphBuilder->QueryInterface( &pMediaControl ));
  CHK( pMediaControl->Run());

  CHK( NotifyMessage( MESSAGE_INFO, L"The Graph is running" ));

Cleanup:
  if( FAILED( hr ))
  {
    NotifyMessage( MESSAGE_ERROR, L"Runing the capture graph failed" );
  }
  return hr;
}

HRESULT
CGraphManager::CaptureStillImageInternal()
{
  static int Counter = 0;
  CString str;
  HRESULT hr = S_OK;
  CComPtr<IFileSinkFilter2> pFileSink;
  CComPtr<IUnknown>		 pUnkCaptureFilter;
  CComPtr<IPin>			 pStillPin;
  CComPtr<IAMVideoControl> pVideoControl;

  if(( m_pCaptureGraphBuilder == NULL ) || ( m_fGraphBuilt == FALSE ))
  {
    ERR( E_FAIL );
  }

  CHK( m_pImageSinkFilter.QueryInterface( &pFileSink ));
  CHK( pFileSink->SetMode(AM_FILE_OVERWRITE));
  str.Format(L"\\test%d.jpg", ++Counter);
  CHK( pFileSink->SetFileName( str, NULL ));

  CHK( m_pVideoCaptureFilter.QueryInterface( &pUnkCaptureFilter ));
  CHK( m_pCaptureGraphBuilder->FindPin( pUnkCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, &MEDIATYPE_Video, FALSE, 0, &pStillPin ));
  CHK( m_pVideoCaptureFilter.QueryInterface( &pVideoControl ));
  CHK( pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger ));

Cleanup:
  if( FAILED( hr ))
  {
    NotifyMessage( MESSAGE_ERROR, L"Capturing a still image failed" );
  }
  return hr;
}


HRESULT
CGraphManager::NotifyMessage( DSHOW_MESSAGE message, WCHAR *wzText )
{
  HRESULT hr = S_OK;
  Message *pMessage;
  WCHAR   *wzString;

  if(( wzText == NULL ) || ( *wzText == NULL ))
  {
    ERR( E_POINTER );
  }

  if( m_hwnd == NULL )
  {
    return S_FALSE;
  }

  pMessage = (Message*) LocalAlloc( LMEM_ZEROINIT, sizeof( Message ));
  if( pMessage == NULL )
  {
    ERR( E_OUTOFMEMORY );
  }

  wzString = (WCHAR*) LocalAlloc( LMEM_ZEROINIT, ( wcslen( wzText ) + 1 ) * sizeof( WCHAR ));
  if( wzString == NULL )
  {
    ERR( E_OUTOFMEMORY );
  }

  StringCchCopy( wzString, wcslen( wzText ) + 1, wzText );

  pMessage->wzMessage = wzString;
  pMessage->dwMessage = message;

  PostMessage( m_hwnd, WM_USER, 0, (LPARAM) pMessage );


Cleanup:
  return hr;
}


// Sets up the capture filter to capture pictures at the specified 
// resolution.  the resolution is specified using an id.
HRESULT 
CGraphManager::SetCameraResolution(DWORD dwId)
{
	HRESULT hr = S_OK;

	// get the IAMStreamConfig interface so that we can set the resolution
	CComPtr<IAMStreamConfig>        pConfig;
	hr = m_pCaptureGraphBuilder->FindInterface(
		&PIN_CATEGORY_STILL,
		&MEDIATYPE_Video,
		m_pVideoCaptureFilter,
		IID_IAMStreamConfig,
		(void**)&pConfig);

	int iCount = 0;
	int iSize = 0;
	// get the number of different resolutions possible
	hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

	if (SUCCEEDED(hr) && 
		iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS) && 
		(int)dwId < iCount)
	{
		VIDEO_STREAM_CONFIG_CAPS scc;
		AM_MEDIA_TYPE *pmtConfig;
		// make sure we can set the capture format to the resolution we want
		hr = pConfig->GetStreamCaps(dwId, &pmtConfig, (BYTE*)&scc);
		if (SUCCEEDED(hr))
		{
			// That resolution is available, now we set the capture format to the resolution we want.
			pConfig->SetFormat(pmtConfig);

			DeleteMediaType(pmtConfig);
		}
	}
	else
	{
		hr = E_FAIL;
	}


	return hr;
}
