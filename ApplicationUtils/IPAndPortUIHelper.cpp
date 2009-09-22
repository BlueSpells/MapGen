#include "StdAfx.h"
#include "IPAndPortUIHelper.h"
#include "common/Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CIPAndPortUIHelper::CIPAndPortUIHelper(CIPAddressCtrl& IPCtrl,
									   CEdit& PortCtrl)
									   : m_IPCtrl(IPCtrl)
									   , m_PortCtrl(PortCtrl)
{
}

CIPAndPortUIHelper::~CIPAndPortUIHelper()
{
}

bool CIPAndPortUIHelper::Set(const CString& IP, int Port)
{
	int Field0, Field1, Field2, Field3;
	CString PortString;
	
	if(IP == "")
		return false;

    std::string StandardIp;
    if (!GetStandardHostIp(IP, StandardIp))
        return false;

	if(4 != sscanf_s(StandardIp.c_str() ,"%d.%d.%d.%d", &Field0, &Field1, &Field2, &Field3))
        return false;

	PortString.Format("%d", Port);

	m_IPCtrl.SetAddress((BYTE)Field0, (BYTE)Field1, (BYTE)Field2, (BYTE)Field3);
	m_PortCtrl.SetWindowText(PortString);
	return true;
//	return TRUE == SetDlgItemInt(m_PortCtrl.GetSafeHwnd(), m_PortCtrl.GetDlgCtrlID(), Port, FALSE);
}

bool CIPAndPortUIHelper::Get(CString& IP, int& Port)
{
	if(m_IPCtrl.IsBlank())
		return false;

	BYTE Field0, Field1, Field2, Field3;
	CString PortString;

//	Port = m_PortCtrl.GetDlgItemInt(m_PortCtrl.GetDlgCtrlID());
	m_PortCtrl.GetWindowText(PortString);
	if( 1 != sscanf_s(PortString, "%d", &Port))
		return false;

	m_IPCtrl.GetAddress(Field0, Field1, Field2, Field3);
	IP.Format("%d.%d.%d.%d", Field0, Field1, Field2, Field3);
	return true;
}

bool CIPAndPortUIHelper::EnableWindow(bool Enable)
{
	m_IPCtrl.EnableWindow(Enable);
	// both controls has the same state. It is enough to return only one.
	return TRUE == m_PortCtrl.EnableWindow(Enable);
}

void CIPAndPortUIHelper::ClearAddress()
{
	m_IPCtrl.ClearAddress();
	m_PortCtrl.SetWindowText("");
}