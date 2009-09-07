// NewFilesNotifier.h: interface for the CNewFilesNotifier class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWFILESNOTIFIER_H__3EB503B8_0627_4033_B1DC_1089ED31836A__INCLUDED_)
#define AFX_NEWFILESNOTIFIER_H__3EB503B8_0627_4033_B1DC_1089ED31836A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786)
#include <string>
#include <map>
#include <vector>

class INewFilesNotifierEvents
{
public:
	//notify on new file
	virtual void OnNewFile(const char* NewFileName) = 0;
};

class INewFilesVectorNotifierEvents
{
public:
    //notify on new files
    virtual void OnNewFiles(const std::vector<std::string>& NewFilesVector) = 0;
};


class CNewFilesNotifier 
{
public:
    enum{ DEF_NOTIFICATIONS_BUFFER_LEN = 102400 };

	CNewFilesNotifier(int NotficationBufferLen = DEF_NOTIFICATIONS_BUFFER_LEN);
	virtual ~CNewFilesNotifier();

	void Advise(INewFilesNotifierEvents* Sink);
    void Advise(INewFilesVectorNotifierEvents* Sink);

    // The new files notifier will read this parameter from configuration
    // so it can be hotly changed
    void SetHotFileNotChangedInterval(
        const char* Section, const char* Key, int Default);
	
	//start the thread
	bool Start(const char* DirectoryToWatch, 
		       int  FileNotChangedInterval = 0, /*milliseconds - 0-means use default*/
               bool WatchSubDirectories = false,
               bool ReportExisitingFiles = true,
               int  PollingInterval = 0, /*0 means default interval time*/ 
			   bool ReturnFullPath = false);

	//close the thread
	void Close();

    bool IsStarted() { return m_IsStarted; }

private:
	void NotifyToClient(const char* FileName);

    BOOL ReadDirectoryChanges();
    void ReadFilesInDirectory(const std::string& Directory);

	typedef DWORD FILE_UPDATE_TIME; //using the GetTickCount()
	
	bool UpdateFileInMap(const char* FileName);    
	void UpdateDirectoryChanges();

    static UINT WINAPI MonitorDirectoryThreadFunction(void* Data);
	
	UINT MonitorDirectoryLoop();
	
private:

	INewFilesNotifierEvents*	m_Sink;
    INewFilesVectorNotifierEvents* m_AllFilesSink;
	std::string					m_Directory;
	int							m_FileNotChangedInterval; //milliseconds
    bool                        m_WatchSubDirectories;
    bool                        m_ReportExistingFiles;
	bool                        m_ReturnFullPath;
    int                         m_PollingInterval;
	HANDLE						m_hDir;
	HANDLE						m_hTerminationEvent;
	HANDLE						m_hCompletionPort;
    HANDLE                      m_hThread;
	OVERLAPPED					m_Overlapped;

    class CConfigInt*           m_ConfigFileNotChangedInterval;

	BYTE*						m_NotficationBuffer;
	int							m_NotficationBufferLen;
	typedef std::map<std::string,  FILE_UPDATE_TIME> FilesUpdateTimeMap;
	typedef FilesUpdateTimeMap::iterator FilesUpdateTimeMapIterator;
	FilesUpdateTimeMap 			m_FilesMap;

    bool                        m_IsStarted;


};

#endif // !defined(AFX_NEWFILESNOTIFIER_H__3EB503B8_0627_4033_B1DC_1089ED31836A__INCLUDED_)
