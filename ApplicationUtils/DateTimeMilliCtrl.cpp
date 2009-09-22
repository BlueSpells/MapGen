#include "StdAfx.h"
#include "DateTimeMilliCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CDateTimeMilliCtrl::CDateTimeMilliCtrl(int MilliDlgID/*, CWnd* Owner*/)
: m_MilliCtrlID(MilliDlgID)
{
	// TODO: try to attach a CEdit to the milli Ctrl
//	m_MilliCtrl.AttachControlSite(Owner, MilliDlgID);
}

//BOOL CDateTimeMilliCtrl::SetFormat(LPCTSTR pstrFormat)
//{
////	m_MilliCtrl.AttachControlSite(GetOwner(), m_MilliCtrlID);
////	m_MilliCtrl.SetLimitText(3);
//	return CDateTimeCtrl::SetFormat(pstrFormat);
//}
//
//void CDateTimeMilliCtrl::DoDataExchange(CDataExchange* pDX)
//{
//	CDateTimeCtrl::DoDataExchange(pDX);
//	DDX_Control(pDX, m_MilliCtrlID, m_MilliCtrl);
//}

CDateTimeMilliCtrl::~CDateTimeMilliCtrl()
{
}

void CDateTimeMilliCtrl::SetTime(const CTimeStamp& TimeStamp)
{
	CTime Time = TimeStamp.GetSeconds();
	CDateTimeCtrl::SetTime(&Time);
	::SetDlgItemInt(GetOwner()->GetSafeHwnd(), m_MilliCtrlID, TimeStamp.GetMilli(), false);
}

void CDateTimeMilliCtrl::SetTime(const Api1DateTime& Time)
{
	CDateTimeCtrl::SetTime(Time.DateTime);
	::SetDlgItemInt(GetOwner()->GetSafeHwnd(), m_MilliCtrlID, Time.Milliseconds, false);
}

CTimeStamp CDateTimeMilliCtrl::GetTime() const
{
	CTime Time;
	CDateTimeCtrl::GetTime(Time);
	int Milli = ::GetDlgItemInt(GetOwner()->GetSafeHwnd(), m_MilliCtrlID, NULL, false);
	
	return CTimeStamp((time_t)Time.GetTime(), Milli);
}

Api1DateTime CDateTimeMilliCtrl::GetLoggerTime() const
{		
	Api1DateTime CtrlTime;	
	CTime Time;
	CDateTimeCtrl::GetTime(Time);
	CtrlTime.DateTime = (time_t)Time.GetTime();
	CtrlTime.Milliseconds = ::GetDlgItemInt(GetOwner()->GetSafeHwnd(), m_MilliCtrlID, NULL, false);
	return CtrlTime;
}

std::string CDateTimeMilliCtrl::ToString() const
{
	return GetTime().ToString();
}

bool CDateTimeMilliCtrl::FromString(const std::string& TimeString)
{
	CTimeStamp Time;
	if(false == Time.FromString(TimeString.c_str()))
		return false;
	SetTime(Time);
	return true;
}
