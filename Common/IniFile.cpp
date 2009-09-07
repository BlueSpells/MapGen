// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef  _UNICODE
#define __atoi64 _wtoi64
#else   /* ndef _UNICODE */
#define __atoi64 _atoi64
#endif  /* _UNICODE */

enum TBoolType
{
    BOOL_TYPE_0_1,              // Binary |
    BOOL_TYPE_YES_NO,           // Yes or No |
    BOOL_TYPE_TRUE_FALSE,       // True or False |
    BOOL_TYPE_ON_OFF,           // On or Off |
    MaxTBoolTypeCnt
};


static double _GetPrivateProfileDouble( 
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    double  dDefault,   // Return value if key name is not found.
    LPCTSTR lpFileName, // Address of initialization filename.
    bool& DefaultUsed
   )
{
#ifdef _WIN32_WCE
    CString buffer;
    buffer = AfxGetApp()->GetProfileString(lpAppName, lpKeyName, _T(""));
#else 
    TCHAR buffer[MAX_PATH];
    GetPrivateProfileString(lpAppName, lpKeyName, _T(""), buffer, sizeof(buffer), lpFileName );
#endif //_WIN32_WCE

    // Either we have a string to convert, or a default value:
    if ( _tcslen(buffer) == 0)
    {
        DefaultUsed = true;
        return dDefault;
    }
    else
    {
        DefaultUsed = false;
        return _tcstod( buffer, NULL);
    }
}

static BOOL _WritePrivateProfileDouble(
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    double  dValue,     // Value to add.
    LPCTSTR lpFileName  // Address of initialization filename.
   )
{
    BOOL ReturnValue = NULL;

#ifdef _WIN32_WCE
    CString buffer;
    buffer.Format(_T("%G"),dValue);
    ReturnValue = AfxGetApp()->WriteProfileString(lpAppName, lpKeyName, buffer);
#else
    TCHAR buffer[MAX_PATH];
    _stprintf_s(buffer,sizeof buffer,_T("%G"),dValue);
    ReturnValue = WritePrivateProfileString(lpAppName, lpKeyName, buffer, lpFileName);
#endif

    return ReturnValue;
}

static INT64 _GetPrivateProfileInt64( 
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    INT64   i64Default, // Return value if key name is not found.
    LPCTSTR lpFileName, // Address of initialization filename.
    bool& DefaultUsed
   )
{
#ifdef _WIN32_WCE
    CString buffer;
    buffer = AfxGetApp()->GetProfileString(lpAppName, lpKeyName, _T(""));
#else 
    TCHAR buffer[MAX_PATH];
    GetPrivateProfileString(lpAppName, lpKeyName, _T(""), buffer, sizeof(buffer), lpFileName );
#endif //_WIN32_WCE

    // Either we have a string to convert, or a default value:
    if ( _tcslen(buffer) == 0)
    {
        DefaultUsed = true;
        return i64Default;
    }
    else
    {
        DefaultUsed = false;
        return __atoi64(buffer);
    }
}

static BOOL _WritePrivateProfileInt64(
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    INT64   i64Value,       // Value to add.
    LPCTSTR lpFileName  // Address of initialization filename.
   )
{
    BOOL ReturnValue = NULL;

#ifdef _WIN32_WCE
    CString buffer;
    buffer.Format(_T("%I64d"),i64Value);
    ReturnValue = AfxGetApp()->WriteProfileString(lpAppName, lpKeyName, buffer);
#else
    TCHAR buffer[MAX_PATH];
    _stprintf_s(buffer,sizeof buffer,_T("%I64d"),i64Value);
    ReturnValue = WritePrivateProfileString(lpAppName, lpKeyName, buffer, lpFileName);
#endif

    return ReturnValue;
}

static int _GetPrivateProfileInt( 
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    int iDefault, // Return value if key name is not found.
    LPCTSTR lpFileName, // Address of initialization filename.
    bool& DefaultUsed
   )
{
#ifdef _WIN32_WCE
    CString buffer;
    buffer = AfxGetApp()->GetProfileString(lpAppName, lpKeyName, _T(""));
#else 
    TCHAR buffer[MAX_PATH];
    GetPrivateProfileString(lpAppName, lpKeyName, _T(""), buffer, sizeof(buffer), lpFileName );
#endif //_WIN32_WCE

    // Either we have a string to convert, or a default value:
    if ( _tcslen(buffer) == 0)
    {
        DefaultUsed = true;
        return iDefault;
    }
    else
    {
        DefaultUsed = false;
        return atoi(buffer);
    }
}

static BOOL _WritePrivateProfileInt( 
    LPCTSTR lpAppName,  // Address of section name.
    LPCTSTR lpKeyName,  // Address of key name.
    int     iValue,     // Value to add.
    LPCTSTR lpFileName  // Address of initialization filename.
   )
{
    BOOL ReturnValue = NULL;

#ifdef _WIN32_WCE
    CString buffer;
    buffer.Format(_T("%d"),iValue);
    ReturnValue = AfxGetApp()->WriteProfileString(lpAppName, lpKeyName, buffer);
#else
    TCHAR buffer[MAX_PATH];
    _stprintf_s(buffer,sizeof buffer,_T("%d"),iValue);
    ReturnValue = WritePrivateProfileString(lpAppName, lpKeyName, buffer, lpFileName );
#endif //_WIN32_WCE

    return ReturnValue;
}

static BOOL _GetPrivateProfileBool( 
    LPCTSTR lpAppName,  // Address of section name
    LPCTSTR lpKeyName,  // Address of key name
    BOOL    fDefault,   // Return value if key name is not found
    LPCTSTR lpFileName, // Address of initialization filename
    bool& DefaultUsed
    )
{
#ifdef _WIN32_WCE
    CString buffer;
    buffer = AfxGetApp()->GetProfileString(lpAppName, lpKeyName, _T(""));
#else
    TCHAR buffer[MAX_PATH];
    GetPrivateProfileString(lpAppName, lpKeyName, _T(""), buffer, sizeof(buffer), lpFileName );
#endif
    // Either we have a string to convert, or a default value:
    if (        !_tcsicmp(buffer,_T("0"))        ||
                !_tcsicmp(buffer,_T("NO"))       ||
                !_tcsicmp(buffer,_T("OFF"))      ||
                !_tcsicmp(buffer,_T("FALSE"))
        )
    {
        DefaultUsed = false;
        return FALSE;
    }
    else if (   !_tcsicmp(buffer,_T("1"))        ||
                !_tcsicmp(buffer,_T("ON"))       ||
                !_tcsicmp(buffer,_T("YES"))      ||
                !_tcsicmp(buffer,_T("TRUE"))
        )
    {
        DefaultUsed = false;
        return TRUE;
    }
    else
    {
        DefaultUsed = true;
        return fDefault;
    }
}

static BOOL _WritePrivateProfileBool( 
    LPCTSTR lpAppName,  // Address of section name
    LPCTSTR lpKeyName,  // Address of key name
    BOOL    bBool,      // Value to add
    TBoolType bType,    // The boolean type format written into the file 0/1, Yes/No, TRUE/FALSE, ON/OFF
    LPCTSTR lpFileName  // Address of initialization filename
    )
{
    BOOL ReturnValue = NULL;

#ifdef _WIN32_WCE
    CString buffer;

    switch(bType)
    {
    case BOOL_TYPE_0_1:
        if(bBool)
            buffer += _T("1");
        else
            buffer += _T("0");
        break;

    case BOOL_TYPE_YES_NO:
        if(bBool)
            buffer += _T("YES");
        else
            buffer += _T("NO");
        break;

    case BOOL_TYPE_TRUE_FALSE:
        if(bBool)
            buffer += _T("TRUE");
        else
            buffer += _T("FALSE");
        break;

    case BOOL_TYPE_ON_OFF:
    default:
        if(bBool)
            buffer += _T("ON");
        else
            buffer += _T("OFF");
        break;
    }

    ReturnValue = AfxGetApp()->WriteProfileString(lpAppName, lpKeyName, buffer);
#else

    TCHAR buffer[MAX_PATH];

    switch(bType)
    {
    case BOOL_TYPE_0_1:
        if(bBool)
            _stprintf_s(buffer,sizeof buffer,_T("1"));
        else
            _stprintf_s(buffer,sizeof buffer,_T("0"));
        break;

    case BOOL_TYPE_YES_NO:
        if(bBool)
            _stprintf_s(buffer,sizeof buffer,_T("YES"));
        else
            _stprintf_s(buffer,sizeof buffer,_T("NO"));
        break;

    case BOOL_TYPE_TRUE_FALSE:
        if(bBool)
            _stprintf_s(buffer,sizeof buffer,_T("TRUE"));
        else
            _stprintf_s(buffer,sizeof buffer,_T("FALSE"));
        break;

    case BOOL_TYPE_ON_OFF:
    default:
        if(bBool)
            _stprintf_s(buffer,sizeof buffer,_T("ON"));
        else
            _stprintf_s(buffer,sizeof buffer,_T("OFF"));
        break;

    }

    ReturnValue = WritePrivateProfileString(lpAppName, lpKeyName, buffer, lpFileName );
#endif

    return ReturnValue;
}

// class CWriteBackChanger
// {
// public:
//     CWriteBackChanger(CIniFile IniFile, EWriteBackMode Mode) : m_TheIniFile(IniFile), m_Mode(Mode)
//     {
//         switch (m_Mode)
//         {
//         case WRITEBACK_MODE_OFF:
//             m_SavedWriteBack = m_TheIniFile.GetWriteBackMode();
//             m_TheIniFile.SetWriteBackMode(false);
//             break;
//         case WRITEBACK_MODE_ON:
//             m_SavedWriteBack = m_TheIniFile.GetWriteBackMode();
//             m_TheIniFile.SetWriteBackMode(true);
//             break;
//         default:
//             m_Mode = WRITEBACK_MODE_DEFAULT;
//         }
//     }
//     ~CWriteBackChanger()
//     {
//         if (m_Mode != WRITEBACK_MODE_DEFAULT)
//             m_TheIniFile.SetWriteBackMode(m_SavedWriteBack);
//     }
// 
// private:
//     EWriteBackMode m_Mode;
//     CIniFile m_TheIniFile;
//     bool m_SavedWriteBack;
// };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile() : m_WriteBack(false)
{
}

void CIniFile::SetFileName(const char* IniFileName)
{
    m_FileName = IniFileName;
}

std::string  CIniFile::GetFileName() const
{
    return m_FileName;
}

// Write each read parameter back to ini file.
void CIniFile::SetWriteBackMode(bool On)
{
    m_WriteBack = On;
}

bool CIniFile::GetWriteBackMode()
{
    return m_WriteBack;
}

bool CIniFile::DeleteConfigSection(const char* Section)
{
    return WritePrivateProfileString(
        Section, NULL, NULL, m_FileName.c_str()) == TRUE;
}

double CIniFile::GetConfigDouble(const char* Section, const char* Key, double Default,
                                 EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    bool DefaultUsed;
    double Value = _GetPrivateProfileDouble(
        Section, Key, Default, m_FileName.c_str(), DefaultUsed);

    if (DefaultUsed && IsWriteBack(Mode))
        WriteConfigDouble(Section, Key, Value);

    return Value;
}

bool CIniFile::WriteConfigDouble(const char* Section, const char* Key, double Value)
{
    return _WritePrivateProfileDouble(
        Section, Key, Value, m_FileName.c_str()) == TRUE;
}


int CIniFile::GetConfigInt(const char* Section, const char* Key, int	Default,
                           EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    bool DefaultUsed;
    int Value = _GetPrivateProfileInt(
        Section, Key, Default, m_FileName.c_str(), DefaultUsed);

    if (DefaultUsed && IsWriteBack(Mode))
        WriteConfigInt(Section, Key, Value);
    
    return Value;
}

bool CIniFile::WriteConfigInt(const char* Section, const char* Key, int Value)
{
    return _WritePrivateProfileInt(
        Section, Key, Value, m_FileName.c_str()) == TRUE;
}


INT64 CIniFile::GetConfigInt64(const char* Section, const char* Key, INT64 Default,
                               EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    bool DefaultUsed;
    INT64 Value = _GetPrivateProfileInt64(
        Section, Key, Default, m_FileName.c_str(), DefaultUsed);

    if (DefaultUsed && IsWriteBack(Mode))
        WriteConfigInt64(Section, Key, Value);

    return Value;
}

bool CIniFile::WriteConfigInt64(const char* Section, const char* Key, INT64 Value)
{
    return _WritePrivateProfileInt64(
        Section, Key, Value, m_FileName.c_str()) == TRUE;
}


std::string CIniFile::GetConfigString(const char* Section, const char* Key, const char* Default,
                                      EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    const int MaxStringSize = 1024;
    char String[MaxStringSize];
    const char DefaultVal[] = "__IniFileDefault__";
    
    GetPrivateProfileString(Section, Key, DefaultVal, String, sizeof String, 
        m_FileName.c_str());

    if (!strcmp(String, DefaultVal))
    {
        if (IsWriteBack(Mode))
            WriteConfigString(Section, Key, Default);
        return Default;
    }

    return String;
}

bool CIniFile::WriteConfigString(const char* Section, const char* Key, const char* Value)
{
    return WritePrivateProfileString(
        Section, Key, Value, m_FileName.c_str()) == TRUE;
}


bool CIniFile::GetConfigBool(const char* Section, const char* Key, bool Default,
                             EWriteBackMode Mode /*= WRITEBACK_MODE_DEFAULT*/)
{
    bool DefaultUsed;
    bool Value = _GetPrivateProfileBool(
        Section, Key, Default, m_FileName.c_str(), DefaultUsed) != FALSE;

    if (DefaultUsed && IsWriteBack(Mode))
        WriteConfigBool(Section, Key, Value);
    
    return Value;
}

bool CIniFile::WriteConfigBool(const char* Section, const char* Key, bool Value)
{
    return _WritePrivateProfileBool(Section, Key, Value, BOOL_TYPE_TRUE_FALSE, 
        m_FileName.c_str()) == TRUE;
}


bool CIniFile::GetSectionNames(std::vector<std::string>& SectionNames)
{
    enum{ SECTION_NAMES_BUFFER_SIZE = 2048 };

    char SectionsBuffer[SECTION_NAMES_BUFFER_SIZE];
    DWORD CopiedBytes = GetPrivateProfileSectionNames(
                                SectionsBuffer,
                                SECTION_NAMES_BUFFER_SIZE,
                                m_FileName.c_str());
    int BufferPos = 0;
    while(CopiedBytes > (unsigned int)BufferPos)
    {
        std::string SectionName = SectionsBuffer + BufferPos;
        if(SectionName.empty())
            break;
        SectionNames.push_back(SectionName);
        BufferPos += SectionName.length() + 1; 
    }

    return true;
}


void CIniFile::GetListSection(const char* Section, const char* KeyPrefix, 
                    std::vector<ConfigListItem> & Result,
                    int StartId, int EndId, int MaxGap)
{
    char Key[200];
    int CurrentGap = 0;
    ConfigListItem ListItem;
    bool OrigConfigWriteBack = GetWriteBackMode();
    SetWriteBackMode(false);
    for(int i = StartId; (i <= EndId) && (CurrentGap < MaxGap); ++i)
    {
        sprintf_s(Key, "%s%d", KeyPrefix, i);
        ListItem.ItemId = i;
        ListItem.ItemValue = GetConfigString(Section, Key, "");
        if(!ListItem.ItemValue.empty())
        {
            Result.push_back(ListItem);
            CurrentGap = 0;
        }
        else
            CurrentGap++;
    }
    SetWriteBackMode(OrigConfigWriteBack);
}

void CIniFile::WriteListSection(const char* Section, const char* KeyPrefix, 
                                const std::vector<std::string> & List, int StartId)
{
    char Key[200];
    for(unsigned int i = 0; i < List.size() ; i++)
    {
        sprintf_s(Key, "%s%d", KeyPrefix, StartId + i);
        WriteConfigString(Section, Key, List[i].c_str());
    }
}

void CIniFile::GetAllEntriesOfSection(const char* SectionName, std::vector<KeyValueEntry> & Result)
{
    enum{ SECTION_KEYS_BUFFER_SIZE = 20000 };
    char EntriesBuffer[SECTION_KEYS_BUFFER_SIZE];

    DWORD CopiedBytes = GetPrivateProfileSection(
        SectionName,
        EntriesBuffer,
        SECTION_KEYS_BUFFER_SIZE,
        m_FileName.c_str());

    int BufferPos = 0;
    while(CopiedBytes > (unsigned int)BufferPos)
    {
        std::string EntryValue = EntriesBuffer + BufferPos;
        if(EntryValue.empty())
            break;

        //split key and value
        KeyValueEntry Entry;
        if(SplitString(EntryValue.c_str(), "=", Entry.Key, Entry.Value))
            Result.push_back(Entry);

        //move to the next section entry
        BufferPos += EntryValue.length() + 1; 
    }
}
