// Config.h: interface for the Configuration routines
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "IniFile.h"
#include <string>
#include <vector>

void SetConfigFileName(const char* ConfigFileName);
std::string GetConfigFileName();


// Write every parameter read from config back
// This applies only to the Double/Int/String/Bool routines
void SetConfigWritebackMode(bool On);

bool GetConfigWritebackMode();

bool DeleteConfigSection(const char* Section);

double GetConfigDouble(const char* Section, const char* Key, double Default,
                       EWriteBackMode Mode = WRITEBACK_MODE_DEFAULT);
bool WriteConfigDouble(const char* Section, const char* Key, double Value);

int GetConfigInt(const char* Section, const char* Key, int Default,
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

void GetConfigStringVector(const char* Section, const char* Key, std::vector<std::string>& StringVector);
bool WriteConfigStringVector(const char* Section, const char* Key, const std::vector<std::string>& StringVector);

void GetConfigIntVector(const char* Section, const char* Key, std::vector<int>& IntVector, const char* Default = "");
bool WriteConfigIntVector(const char* Section, const char* Key, const std::vector<int>& IntVector);

struct ValueAndName
{
	int Value;
	const char* Name;
};

const char* Value2Name(const ValueAndName Names[], int Count, int Value, const char* Default);
int Name2Value(const ValueAndName Names[], int Count, const char* Name, int Default, const char* LogCaption = NULL);

int GetConfigEnum(
	  const char* Section, 
	  const char* Key, 
	  const ValueAndName Enums[], 
	  int EnumCount, 
	  int Default);

std::string GetArrayKeyName(const char* ArrayName, int Index);

//the GetListSection functions retrieves list of items
//from a specific config section. It assumes that the key of each item
//is combined from prefix and enumerator. (E.g. 'Channel_1' has prefix of 'Channel_' and 
//item enumerator '1'). It will return a list of config items which include the
//item id and item value.

void GetListSection(const char* Section, const char* KeyPrefix, 
                    std::vector<ConfigListItem>& Result, 
                    int StartId = 0, int EndId = 1000000, int MaxGap = 5);
void WriteListSection(const char* Section, const char* KeyPrefix,
                      const std::vector<std::string>& List, int StartId = 0);

void GetAllEntriesOfSection(const char* SectionName, std::vector<KeyValueEntry>& Result);

//////////////////////////////////////////////////////////////////////////

CPoint GetConfigPoint(const char* Section, const char* Key, const CPoint& Default);
void WriteConfigPoint(const char* Section, const char* Key, const CPoint& Value);

//////////////////////////////////////////////////////////////////////////

CRect GetConfigRect(const char* Section, const char* Key, const CRect& Default);
bool WriteConfigRect(const char* Section, const char* Key, const CRect& Value);

//////////////////////////////////////////////////////////////////////////

void GetWindowPlacement(CWnd* Window,  const char* Section,  bool Show);
void GetWindowOrigin(CWnd* Window,  const char* Section); void WriteWindowPlacement(CWnd* Window, const char* Section);

//////////////////////////////////////////////////////////////////////////

// Routines used to save multiple values in a single string
std::string GetLineString(const char*& Line, const char* Default);
int GetLineInt(const char*& Line, int Default);
void WriteLineString(std::string& Line, const char* Value);
void WriteLineInt(std::string& Line, int Value);

