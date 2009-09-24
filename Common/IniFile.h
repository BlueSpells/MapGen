// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// This will be a struct used in GetListSection() 
struct ConfigListItem
{
    int ItemId;
    std::string ItemValue;
};

struct KeyValueEntry
{
    std::string Key;
    std::string Value;
};

//////////////////////////////////////////////////////////////////////////

enum EWriteBackMode
{
    WRITEBACK_MODE_OFF,
    WRITEBACK_MODE_ON,
    WRITEBACK_MODE_DEFAULT,
};

#ifdef NDEBUG
#define NDEBUG_WRITEBACK_MODE_OFF WRITEBACK_MODE_OFF
#else
#define NDEBUG_WRITEBACK_MODE_OFF WRITEBACK_MODE_DEFAULT
#endif

class CIniFile  
{
public:
	CIniFile();

    void SetFileName(const char* IniFileName);
    std::string GetFileName() const;

    // Write each read parameter back to ini file.
    void SetWriteBackMode(bool On);
    bool GetWriteBackMode();
    
    bool DeleteConfigSection(const char* Section);
    
    double GetConfigDouble(const char* Section, const char* Key, double Default,
        EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
    bool WriteConfigDouble(const char* Section, const char* Key, double Value);
    int GetConfigInt(const char* Section, const char* Key, int	Default,
        EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
    bool WriteConfigInt(const char* Section, const char* Key, int Value);
    INT64 GetConfigInt64(const char* Section, const char* Key, INT64 Default,
        EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
    bool WriteConfigInt64(const char* Section, const char* Key, INT64 Value);
    std::string GetConfigString(const char* Section, const char* Key, const char* Default,
        EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
    bool WriteConfigString(const char* Section, const char* Key, const char* Value);
    bool GetConfigBool(const char* Section, const char* Key, bool Default,
        EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
    bool WriteConfigBool(const char* Section, const char* Key, bool Value);

    bool GetSectionNames(std::vector<std::string>& SectionNames);

    //the GetListSection functions retrieves list of items
    //from a specific config section. It assumes that the key of each item
    //is combined from prefix and enumerator. (E.g. 'Channel_1' has prefix of 'Channel_' and 
    //item enumerator '1'). It will return a list of config items which include the
    //item id and item value.
    void GetListSection(const char* Section, const char* KeyPrefix, 
                        std::vector<ConfigListItem> & Result,
                        int StartId = 0, int EndId = 1000000, int MaxGap = 5);
    void WriteListSection(const char* Section, const char* KeyPrefix,
                          const std::vector<std::string> & List, int StartId = 0);

    //return all the keys and values of a specific section
    void GetAllEntriesOfSection(const char* SectionName,
        std::vector<KeyValueEntry> & Result);
private:
    bool IsWriteBack(EWriteBackMode Mode)
    {
        return (Mode == WRITEBACK_MODE_ON)  ||  ((Mode != WRITEBACK_MODE_OFF)  &&  m_WriteBack);
    }

    std::string m_FileName;
    bool        m_WriteBack;
};
