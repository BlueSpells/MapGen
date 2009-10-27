#pragma once

typedef enum
{
    MESSAGE_INFO,
    MESSAGE_ERROR,
    MESSAGE_ENDRECORDING,
    MESSAGE_FILECAPTURED,
	MESSAGE_FILEREADYTODECODE
} DSHOW_MESSAGE;


typedef struct
{
    DSHOW_MESSAGE   dwMessage;
    WCHAR*  wzMessage;
} Message; 

const TCHAR MyPics[] = L"\\My Documents\\My Pictures\\";

static void GetFullPathToFile(LPTSTR pszFullPath, LPCTSTR pszFilename)
{
	// Find the path to the current executable
	
	GetModuleFileName(GetModuleHandle(NULL), pszFullPath, MAX_PATH);
	//wcscpy(pszFullPath, MyPics);

	// Strip off the exe filename and replace it with
	// the path provided by our second parameter.
	wcscpy(wcsrchr(pszFullPath, '\\') /*+ 1*/, pszFilename);
}