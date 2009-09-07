// NewFilesNotifier.cpp: implementation of the CNewFilesNotifier class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <process.h>
#include "NewFilesNotifier.h"
#include <direct.h>
#include "LogEvent.h"
#include "Utils.h"
#include "ConfigVariables.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int DEFAULT_POLLING_INTERVAL = 300;

CNewFilesNotifier::CNewFilesNotifier(int NotficationBufferLen /*= DEF_NOTIFICATIONS_BUFFER_LEN*/) :
	m_Sink(NULL),
    m_AllFilesSink(NULL),
    m_FileNotChangedInterval(0),
    m_WatchSubDirectories(false),
    m_ReportExistingFiles(true),
    m_ReturnFullPath(false),
    m_PollingInterval(DEFAULT_POLLING_INTERVAL),
	m_hDir(INVALID_HANDLE_VALUE),
	m_hTerminationEvent(NULL),
	m_hCompletionPort(NULL),
    m_hThread(NULL),
    m_ConfigFileNotChangedInterval(NULL),
	m_NotficationBuffer(NULL),
	m_NotficationBufferLen(NotficationBufferLen),
    m_IsStarted(false)
{
}

CNewFilesNotifier::~CNewFilesNotifier()
{
	Close();

    delete m_ConfigFileNotChangedInterval;

//	Sleep(1000); // wait till the thread exits
}

void CNewFilesNotifier::Close()
{
	//close the thread
	if(m_hTerminationEvent)
	{
		SetEvent(m_hTerminationEvent);
        if (m_hThread != NULL)
            WaitForSingleObject(m_hThread, 2000);
		CloseHandle(m_hTerminationEvent);
		m_hTerminationEvent = NULL;
        m_hThread = NULL;
	}

	if(m_hDir != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDir);
		m_hDir = INVALID_HANDLE_VALUE;
	}

	if(m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	if(m_NotficationBuffer)
	{
		delete [] m_NotficationBuffer;
		m_NotficationBuffer = NULL;
	}

    m_IsStarted = false;
}


void CNewFilesNotifier::Advise(INewFilesNotifierEvents* Sink)
{
	m_Sink = Sink;
}

void CNewFilesNotifier::Advise( INewFilesVectorNotifierEvents* Sink )
{
   m_AllFilesSink = Sink;
}
// The new files notifier will read this parameter from config
// so it can be hotly refreshed
void CNewFilesNotifier::SetHotFileNotChangedInterval(
    const char* Section, const char* Key, int Default)
{
    m_ConfigFileNotChangedInterval = 
        new CConfigInt(Section, Key, Default);
}

bool CNewFilesNotifier::Start(const char* DirectoryToWatch,
							 int FileNotChangedInterval, /*milliseconds*/
                             bool WatchSubDirectories, 
                             bool ReportExisitingFiles, 
                             int  PollingInterval,
							 bool ReturnFullPath)
{
    LogEvent(LE_INFOHIGH, "CNewFilesNotifier::Start: Directory [%s]", DirectoryToWatch);
    
	m_FileNotChangedInterval = FileNotChangedInterval;
	m_Directory = DirectoryToWatch;
    m_WatchSubDirectories = WatchSubDirectories;
    m_ReportExistingFiles = ReportExisitingFiles;
	m_ReturnFullPath      = ReturnFullPath;
    if(PollingInterval != 0)
        m_PollingInterval = PollingInterval;
    
	m_NotficationBuffer = new BYTE[m_NotficationBufferLen];
	m_Overlapped.hEvent = 0;
	
	//concatenate '\' sign to the directory if it is missing 
	if(!m_Directory.empty() && m_Directory[m_Directory.length() - 1] != '\\')
		m_Directory += "\\"; 
	
	//check if the directory is valid
	m_hDir = CreateFile(
		m_Directory.c_str(),                // pointer to the file name
		FILE_LIST_DIRECTORY,                // access (read/write) mode
		FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE, // share mode
		NULL,                               // security descriptor
		OPEN_EXISTING,                      // how to create
		FILE_FLAG_BACKUP_SEMANTICS| FILE_FLAG_OVERLAPPED,         // file attributes
		NULL                                // file with attributes to copy
		);
	
	if (m_hDir == INVALID_HANDLE_VALUE)
	{
		LogGetLastError("CNewFilesNotifier::Start: open directory problem:");
		LogEvent(LE_ERROR, "CNewFilesNotifier::Start: open directory problem %s",m_Directory.c_str());
		return false;
	}

	m_hCompletionPort = CreateIoCompletionPort(m_hDir, NULL, (DWORD) m_NotficationBuffer, 0);
	if (m_hCompletionPort == NULL) 
	{
		LogGetLastError("CNewFilesNotifier::Start: CreateIoCompletionPort failed");
		Close();
		return false;
	}

	if(!ReadDirectoryChanges())
		return false;
	
	m_hTerminationEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_hTerminationEvent == NULL)
	{
		LogEvent(LE_ERROR, "CNewFilesNotifier::Start: CreateEvent failed");
		return false;
	}

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, 
		MonitorDirectoryThreadFunction, this, 0, NULL);
    if (m_hThread == NULL)
	{
		LogEvent(LE_ERROR, "CNewFilesNotifier::Start: _beginthreadex failed");
		CloseHandle(m_hTerminationEvent);
		return false;
	}

    m_IsStarted = true;
	return true;
}

BOOL CNewFilesNotifier::ReadDirectoryChanges()
{
    DWORD BytesReturned = 0;
	DWORD Filter = FILE_NOTIFY_CHANGE_SIZE | 
		FILE_NOTIFY_CHANGE_LAST_WRITE |	
		FILE_NOTIFY_CHANGE_LAST_ACCESS |
		FILE_NOTIFY_CHANGE_CREATION |
        FILE_NOTIFY_CHANGE_FILE_NAME |
 		FILE_NOTIFY_CHANGE_ATTRIBUTES;  //Added in order to detect ftp remote rename operation.


    BOOL Ret = ReadDirectoryChangesW (
		m_hDir, m_NotficationBuffer, m_NotficationBufferLen, m_WatchSubDirectories, Filter,
		&BytesReturned, &m_Overlapped, NULL);
    if(!Ret)
    {
        LogGetLastError("CNewFilesNotifier::ReadDirectoryChanges: ReadDirectoryChangesW failed");
    }
    return Ret;
}


UINT WINAPI CNewFilesNotifier::MonitorDirectoryThreadFunction(void* Data)
{
	CNewFilesNotifier* NewFilesNotifier = (CNewFilesNotifier*) Data;
	return NewFilesNotifier->MonitorDirectoryLoop();
}
	
	
UINT CNewFilesNotifier::MonitorDirectoryLoop()
{
	
	//read the starting content of directory
    if(m_ReportExistingFiles)
	    ReadFilesInDirectory(m_Directory);

	while(WaitForSingleObject(m_hTerminationEvent, m_PollingInterval) != WAIT_OBJECT_0)
	{
		//get information regarding directory changes
		UpdateDirectoryChanges();

        if (m_ConfigFileNotChangedInterval)
            m_FileNotChangedInterval = *m_ConfigFileNotChangedInterval;

		//check the files update map to see if 
		//files are completed
        
		FilesUpdateTimeMapIterator Iter = m_FilesMap.begin();
		FilesUpdateTimeMapIterator End = m_FilesMap.end();
        std::vector<std::string> NewFilesVector;
        DWORD TempTime = GetTickCount();
		for(;Iter != End; ++Iter)
		{
			FILE_UPDATE_TIME& time = Iter->second;
			if(TempTime - time > (unsigned int)m_FileNotChangedInterval)
			{
				std::string FileName  = Iter->first;
				LogEvent(LE_INFOLOW, "CNewFilesNotifier::MonitorDirectoryLoop, file '%s' time = %ld is timed-out (current time %ld)", 
					FileName.c_str(), time, TempTime);
				NotifyToClient(FileName.c_str());
                NewFilesVector.push_back( FileName );

// 				if(m_FilesMap.size() == 1)
// 				{
// 					m_FilesMap.clear();
// 					break;
// 				}
// 				else
// 				{
// 					m_FilesMap.erase(Iter);
// 					Iter = m_FilesMap.upper_bound(FileName); //maybe upper_bound
// 				}
			}
		}

        if( m_AllFilesSink != NULL && NewFilesVector.size() > 0)
            m_AllFilesSink->OnNewFiles( NewFilesVector );
        //Instead doing erase inside a loop, we erase map entries, after notifying to the user.
        for(unsigned int i=0 ; i < NewFilesVector.size() ; i++)
        {
            m_FilesMap.erase( NewFilesVector[i]);
        }
	}

	return 0;
}

//go over the current files
void CNewFilesNotifier::ReadFilesInDirectory(const std::string& Directory)
{
	WIN32_FIND_DATA FindData;

	std::string FindFileCriteria = Directory + "*.*";
	
	HANDLE hFind = FindFirstFile(FindFileCriteria.c_str() , &FindData);
	
	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
            char FullPathName[MAX_PATH];
            StrCopy(FullPathName, Directory.c_str(), Directory.size()+1);
            strcat_s(FullPathName, FindData.cFileName);

			//skip sub directories
			if(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(m_WatchSubDirectories && 
                   strcmp(FindData.cFileName,".")!=0 &&                    
                   strcmp(FindData.cFileName,"..")!=0)
                {
                    strcat_s(FullPathName, "\\");
                    ReadFilesInDirectory(FullPathName);
                }
                continue;
            }				           
            
			if(!m_ReturnFullPath)
			{
				char* RelativePathName = &FullPathName[m_Directory.size()];
				UpdateFileInMap(RelativePathName);
			}
			else
				UpdateFileInMap(FullPathName);
					
		}while(FindNextFile(hFind, &FindData));
		
		FindClose(hFind);
	}
}

bool CNewFilesNotifier::UpdateFileInMap(const char* FileName)
{
	FILE_UPDATE_TIME time = GetTickCount();

	//check if file already exist
	FilesUpdateTimeMapIterator Iter = m_FilesMap.find(FileName);
	if(Iter == m_FilesMap.end())
	{
		LogEvent(LE_INFOLOW, "CNewFilesNotifier::UpdateFileInMap, insert file '%s' time = %ld", 
			FileName, time);
		//file does not exist, so add the file to the map
		FilesUpdateTimeMap::value_type Value(FileName, time);
		std::pair<FilesUpdateTimeMap::iterator, bool> Result = m_FilesMap.insert(Value);
		return Result.second;
	}
	else
	{
		//update time if file already exist
		LogEvent(LE_INFOLOW, "CNewFilesNotifier::UpdateFileInMap, update file '%s' time = %ld", 
			FileName, time);
		Iter->second = time;
	}
	return true;
}

void CNewFilesNotifier::NotifyToClient(const char* FileName)
{
	//notify on new file
	if(m_Sink)
		m_Sink->OnNewFile(FileName);
}

void CNewFilesNotifier::UpdateDirectoryChanges()
{
	
    DWORD BytesReturned = 0;
	LPOVERLAPPED pOverlapped;
	char* pBuffer;
		
	if(GetQueuedCompletionStatus(m_hCompletionPort, 
		&BytesReturned, 
		(LPDWORD) &pBuffer,
		&pOverlapped, 
		0))
	{
		//parse received data
		LogEvent(LE_INFOLOW, "CNewFilesNotifier::UpdateDirectoryChanges, GetQueuedCompletionStatus %d bytes", 
			BytesReturned);
		FILE_NOTIFY_INFORMATION* pInfo = (FILE_NOTIFY_INFORMATION*) pBuffer;
		while(true)
		{
			char FileName[DEF_NOTIFICATIONS_BUFFER_LEN / 2];
			memset(FileName, 0, sizeof FileName);
            			
			/*int NumBytes = */WideCharToMultiByte(CP_ACP, 0,
				pInfo->FileName, pInfo->FileNameLength / 2, 
				FileName, sizeof(FileName), NULL, NULL);

            char FullPathName[MAX_PATH];
            StrCopy(FullPathName, m_Directory.c_str(), m_Directory.size()+1);
            strcat_s(FullPathName, FileName);
           
            DWORD FileAttr = GetFileAttributes(FullPathName);
            //if(FileAttr == 0xFFFFFFFF)                           
            //    LogEvent(LE_ERROR, "CNewFilesNotifier::NotifyToClient: GetFileAttributes failed, error - %d", GetLastError());            
            
            bool IsDir =  (FileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;                       
            if(FileAttr!=0xFFFFFFFF && !IsDir)
            {
                LogEvent(LE_INFOLOW, 
                    "CNewFilesNotifier::UpdateDirectoryChanges, file %s changed", FullPathName);
			    if((pInfo->Action == FILE_ACTION_ADDED) || (pInfo->Action == FILE_ACTION_MODIFIED))
				{
					if(m_ReturnFullPath)
						UpdateFileInMap(FullPathName);
					else
						UpdateFileInMap(FileName);
				}
            }			

			DWORD NextOffset = pInfo->NextEntryOffset;
			memset(pInfo, 0, sizeof(DWORD)*3 + (pInfo->FileNameLength));
			
			if (NextOffset == 0)
				//last file notify info
				break;
			else
				pInfo = (PFILE_NOTIFY_INFORMATION)(((char *)pInfo) + NextOffset);
		}
		ReadDirectoryChanges();		
    }
}



///////////////////////////////////////////////////////////////
#ifdef _TEST

class CNewFilesNotifierTester: public INewFilesNotifierEvents
{
public:
	CNewFilesNotifierTester()
	{
		m_Notifier.Advise(this);
	}

	bool Start(const char* Directory, 
               int FileNotChangedTime,
               bool WatchSubDir, 
               bool ReportExistingFiles,
               int  PollingInterval,
			   bool ReturnFullPath)
	{
		return m_Notifier.Start(Directory, FileNotChangedTime, WatchSubDir, ReportExistingFiles, PollingInterval,ReturnFullPath);
	}

	void Close()
	{
		m_Notifier.Close();
	}

	//INewFilesNotifierEvents implementation
	virtual void OnNewFile(const char* NewFileName)
	{
        LogEvent(LE_INFOHIGH, "CNewFilesNotifierTester::OnNewFile: receive new file: '%s'",
			NewFileName);
	}
	
private:
	CNewFilesNotifier m_Notifier;
};


void TestNewFilesNotifier()
{
	CNewFilesNotifierTester Tester;

	enum 
	{ 
		FileNotChangedTime = 500 //milli
	};

	//const char* None_Existing_Directory = "c:\\xx\\yy\\ddd";
	const char* Existing_Directory = "\\\\Srv02\\Users\\Dima\\Tmp";
	
	//test invalid directory
	//Assert(!Tester.Start(None_Existing_Directory, FileNotChangedTime));
	//Tester.Close();

    bool bWatchSubDir = true;
    bool bReportExisitingFiles = true;
    int PollingInterval = 50;
    bool bReturnFullPath = true;

	Assert(Tester.Start(
        Existing_Directory, 
        FileNotChangedTime,
        bWatchSubDir,
        bReportExisitingFiles,
        PollingInterval, 
        bReturnFullPath)
    );
    
	//start deliver new files and update existing files in the  'Existing_Directory'
	//WaitAndPumpMessages(50000);
    Sleep(900000);
	LogEvent(LE_INFOHIGH, "TestNewFilesNotifier: Tester closes");
	Tester.Close();
	LogEvent(LE_INFOHIGH, "TestNewFilesNotifier: Test Completed");
}

	
#endif _TEST