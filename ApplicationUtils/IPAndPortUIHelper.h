#pragma once
#include <afxcmn.h>

class CIPAndPortUIHelper
{
public:
	// TODO: try to use controls IDs instead of control objects.
	//			think how to map the controls events
	//		 try to use GetDlgItemInt instead of GetWindowText.
	//		 try to verify the controls format. See remarked code in the constructor.
	CIPAndPortUIHelper(CIPAddressCtrl& IPCtrl, CEdit& PortCtrl);
	~CIPAndPortUIHelper();
	bool Set(const CString& IP, int port);
	bool Get(CString& IP, int& port);
	bool EnableWindow(bool Enable);
	void ClearAddress();
private:
	CIPAddressCtrl& m_IPCtrl;
	CEdit& m_PortCtrl;
};
