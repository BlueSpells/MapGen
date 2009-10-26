#pragma once
#include "struct.h"

typedef enum 
{
  COMMAND_BUILDGRAPH,
  COMMAND_RUNGRAPH,
  COMMAND_SHUTDOWN,
  COMMAND_STILLIMAGE,
  COMMAND_NOCOMMAND
} GRAPHCOMMANDS;

class CGraphManager
{
public:
  CGraphManager();
  ~CGraphManager();

  HRESULT Init();
  HRESULT BuildCaptureGraph();
  HRESULT RunCaptureGraph();
  HRESULT CaptureStillImage();
  HRESULT ShutDown();
  HRESULT RegisterNotificationWindow( HWND hwnd );

  HRESULT SetCameraResolution(DWORD dwId);
  int	  GetFileCounter() {return m_Counter;}
  CString BuildFileName() {CString str; str.Format(L"\\test%d.jpg", m_Counter); return str;}

private:

  static DWORD WINAPI ThreadProc( LPVOID lpParameter );
  HRESULT CreateCaptureGraphInternal();
  HRESULT RunCaptureGraphInternal();
  HRESULT CaptureStillImageInternal();
  HRESULT NotifyMessage( DSHOW_MESSAGE message, const WCHAR *wzText );
  HRESULT ProcessCommand();
  HRESULT ProcessDShowEvent();
  HRESULT GetFirstCameraDriver( WCHAR *wzName );

  HANDLE          m_handle[2];
  HWND            m_hwnd;
  DWORD           m_dwThreadId;
  HANDLE          m_hThread;
  HANDLE          m_hCommandCompleted;
  GRAPHCOMMANDS   m_currentCommand;
  BOOL            m_fGraphBuilt;

  CComPtr<ICaptureGraphBuilder2>  m_pCaptureGraphBuilder;
  CComPtr<IBaseFilter>			m_pVideoCaptureFilter;
  CComPtr<IImageSinkFilter>	    m_pImageSinkFilter;

  int m_Counter;
}; 