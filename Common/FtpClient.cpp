// FtpClient.cpp: implementation of the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <AFXINET.H>
#include "FtpClient.h"
#include "LogEvent.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpClient::CFtpClient() : m_Session("FEIS FtpClient"), m_FtpConnection(0)
{   
}

CFtpClient::~CFtpClient()
{
    Close();
}


bool CFtpClient::Open(
        const char* FtpSite,
		const char* User, 
		const char* Password)
{
    StrCopy(m_FtpSite, FtpSite, sizeof m_FtpSite);
    StrCopy(m_User, User, sizeof m_User);
    StrCopy(m_Password, Password, sizeof m_Password);
    
    Close();

    // Return a connection to ftp.microsoft.com. Default
	// parameters mean that we'll try with username = ANONYMOUS
	// and password set to the machine name @ domain name
    try
    {
        m_FtpConnection = m_Session.GetFtpConnection(FtpSite, User, Password);
    }
    catch(...)
    {
        LogEvent(LE_ERROR, "CFtpClient::Open: Error in GetFtpConnection");
    }
    return (m_FtpConnection!=NULL);
}




void CFtpClient::Close()
{
    if (m_FtpConnection != NULL)
    {
		m_FtpConnection->Close();
	    delete m_FtpConnection;
        m_FtpConnection = NULL;
    }    
}

bool CFtpClient::SendFile(
						const char* LocalFileName, 
						const char* DestinationFileName)
{
    if(NULL == m_FtpConnection)
        return false;
    
    bool FileCopied = false;	
	try
	{
	    FileCopied = m_FtpConnection->PutFile(LocalFileName, DestinationFileName) != 0;
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		LogEvent(LE_ERROR, "CFtpClient::SendFile: ERROR!  %s", sz);
		pEx->Delete();
        ReOpen();
	}
	return FileCopied;
}

bool CFtpClient::SendFileInSegments(
						 const char* LocalFileName, 
						 const char* DestinationFileName)
{
    if(NULL == m_FtpConnection)
        return false;

	bool FileCopied = false;    
	try
	{
        //load data from local file
		CFile File;
		if(File.Open(LocalFileName, CFile::modeRead  | CFile::typeBinary))
		{
			//use a file find object to enumerate files
			CInternetFile* iFile = m_FtpConnection->OpenFile(DestinationFileName, GENERIC_WRITE);
			if(iFile)
			{
				while(true)
				{
					BYTE Buff[1000];
					int ActualRead = File.Read(Buff, sizeof Buff);
					if(ActualRead == 0)
						break;
					iFile->Write(Buff, ActualRead);
					Sleep(10);
				}
				iFile->Flush();
				iFile->Close();
				FileCopied = true;
			}
		}
		File.Close();
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		LogEvent(LE_ERROR, "CFtpClient::SendFile: ERROR!  %s", sz);
		pEx->Delete();
        ReOpen();
	}
	
	return FileCopied;
}


bool CFtpClient::SendData(
						const BYTE* Data,
						int			DataLen,
						int			ChunkSize,
						const char* DestinationFileName)
{
	if(NULL == m_FtpConnection)
        return false;

	bool DataSent = false;	
	try
	{
		//use a file find object to enumerate files
		CInternetFile* iFile = m_FtpConnection->OpenFile(DestinationFileName,
			GENERIC_WRITE);
		if(iFile)
		{
			int Sent = 0;
			while(DataLen - Sent > 0)
			{
				int ChunkToSend = (DataLen - Sent >= ChunkSize) ? ChunkSize : (DataLen - Sent);
				iFile->Write(Data + Sent, ChunkToSend);
				Sent += ChunkToSend;
				Sleep(10);
			}
			iFile->Flush();
			iFile->Close();
			DataSent = true;
			delete iFile;
		}
		
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		LogEvent(LE_ERROR, "CFtpClient::SendFile: ERROR!  %s", sz);
		pEx->Delete();
	}
	catch (...)
	{
		LogEvent(LE_ERROR, "CFtpClient::SendFile: Unknown exception");
        ReOpen();
	}

	return DataSent;
}


bool CFtpClient::ReOpen()
{
    Close();    
    m_FtpConnection = m_Session.GetFtpConnection(m_FtpSite, m_User, m_Password);
    return (m_FtpConnection!=NULL);
}

bool CFtpClient::RemoteRename( const char* ExistinfFileName,  const char* NewFileName )
{
	if(NULL == m_FtpConnection)
		return false;

	bool FileRenamed = false;	
	try
	{
		FileRenamed = m_FtpConnection->Rename(ExistinfFileName, NewFileName) != 0;
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		LogEvent(LE_ERROR, "CFtpClient::RemoteRename: ERROR!  %s", sz);
		pEx->Delete();
		ReOpen();
	}
	return FileRenamed;
}
	

///////////////////////////////////////////////////////////
#ifdef _TEST

void TestFtpClient()
{
    CFtpClient FtpClient;
    FtpClient.Open("127.0.0.1", "aaa", "bbb");
    FtpClient.SendFile("c:\\1.txt", "1.txt");
	FtpClient.SendFileInSegments("c:\\1.txt", "2.txt");
    FtpClient.Close();
}

#endif _TEST

