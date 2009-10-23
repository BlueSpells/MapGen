#pragma once

class CAppWindow
{
public:
		CAppWindow();
		~CAppWindow();

		HRESULT CreateControls( HINSTANCE hInstance );
		HRESULT Run();

private:
		HWND    m_hwnd;
		HWND	m_stillButtonHwnd;
		WCHAR   *m_wzStatusString;
		CGraphManager *m_pGraphManager;
		bool	m_ChildDialogRunning;

		static LRESULT CALLBACK AppWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); 
		LRESULT HandleWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	  HRESULT HandleCommand( WPARAM wParam );
		HRESULT RepaintWindow();
		HRESULT UpdateNotification( Message *pMessage );
};

