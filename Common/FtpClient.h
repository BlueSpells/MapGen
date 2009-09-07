// FtpClient.h: interface for the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////
#include <afxinet.h>
#if !defined(AFX_FTPCLIENT_H__1DC59F03_7752_4062_AFE2_C623D86A8255__INCLUDED_)
#define AFX_FTPCLIENT_H__1DC59F03_7752_4062_AFE2_C623D86A8255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFtpConnection;
class CInternetSession;

class CFtpClient  
{
public:

    CFtpClient();
	virtual ~CFtpClient();

    bool Open(
        const char* FtpSite,
		const char* User, 
		const char* Password);
    
	bool SendFile(
		const char* LocalFileName, 
		const char* DestinationFileName);

	bool SendFileInSegments(
		const char* LocalFileName, 
		const char* DestinationFileName);

	bool SendData(
		const BYTE* Data,
		int			DataLen,
		int			ChunkSize,
		const char* DestinationFileName);

	bool RemoteRename(
		const char* ExistinfFileName, 
		const char* NewFileName);

    void Close();

private:

    bool ReOpen();


    CInternetSession m_Session;
    CFtpConnection* m_FtpConnection;
    char m_FtpSite[64];
    char m_User[16];
    char m_Password[16];
    
};

#endif // !defined(AFX_FTPCLIENT_H__1DC59F03_7752_4062_AFE2_C623D86A8255__INCLUDED_)
