// ConfigVariables.h: interface for the CConfigXXXVariable classes.
//
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <string>

class CConfigVariable
{
public:
    CConfigVariable(
        int RefreshTime = 0     // 0 Means use default value
    );
    CConfigVariable(
        const char* Section, 
        const char* Key, 
        int RefreshTime = 0     // 0 Means use default value
    );
    void SetSectionAndKey(const char* Section, const char* Key);
protected:
    const char* GetSection() const { return m_Section.c_str(); }
    const char* GetKey() const { return m_Key.c_str(); }
    bool HasToRefresh();

private:
    std::string m_Section;
    std::string m_Key;
    int         m_LastRefresh;
    int         m_RefreshTime;
};

class CConfigInt : public CConfigVariable
{
public:
    CConfigInt(
        const char* Section, 
        const char* Key, 
        int Default = 0,
        int RefreshTime = 0     // 0 Means use default value
    );
    CConfigInt(
        int Default = 0,
        int RefreshTime = 0     // 0 Means use default value
    );
    
    int GetValue();
    operator int();
private:
    int         m_Default;
    int         m_Value;
};

class CConfigBool : public CConfigVariable
{
public:
    CConfigBool(
        const char* Section, 
        const char* Key, 
        bool Default = false,
        int RefreshTime = 0     // 0 Means use default value
    );
    CConfigBool(
        bool Default = false,
        int RefreshTime = 0     // 0 Means use default value
    );
    
    bool GetValue();
    operator bool();
private:
    bool m_Default;
    bool m_Value;
};

class CConfigString : public CConfigVariable
{
public:
    CConfigString(
        const char* Section, 
        const char* Key, 
        const char* Default = NULL,
        int RefreshTime = 0     // 0 Means use default value
        );
    CConfigString(
        const char* Default = NULL,
        int RefreshTime = 0     // 0 Means use default value
        );
    
    std::string GetValue();
    const char* GetStrValue();
    operator std::string();
    operator const char*();
private:
    std::string m_Default;
    std::string m_Value;
};

