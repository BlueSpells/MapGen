#pragma once
#include "Common/TimeStamp.h"
#include "GenericDalApi/DalApi1CommonStruct.h"
#include <afxdtctl.h>

/************************************************************************/
/* This class extends CDateTimeCtrl to handle date and time with
	milliseconds. To use this class add a CDateTimeCtrl and a CEdit to
	your dialog drawing. Attach a control variable to the CDateTimeCtrl
	that you've added with type CDateTimeMilliCtrl. In the member
	initialization list of your dialog, initialize the control variable
	with the ID of the control associated with the milliseconds in your
	dialog.																*/
/************************************************************************/

class CDateTimeMilliCtrl : public CDateTimeCtrl
{
public:
	CDateTimeMilliCtrl(int MilliCtrlID/*, CWnd* Owner*/);
	~CDateTimeMilliCtrl();
	void SetTime(const CTimeStamp& Time);
	void SetTime(const Api1DateTime& Time);
	CTimeStamp GetTime() const;
	Api1DateTime GetLoggerTime() const;
	std::string ToString() const;
	bool FromString(const std::string& TimeString);

//	BOOL SetFormat(LPCTSTR pstrFormat);
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	int		m_MilliCtrlID;
//	CEdit	m_MilliCtrl;		
};
