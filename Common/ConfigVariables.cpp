// ConfigVariables.cpp: implementation of the the CConfigXXXVariable classes.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigVariables.h"
#include "Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
static const char* GeneralConfigSection = "General";

static int GetConfigRefreshTime()
{
    const int DefaultRefreshTime = 60;
    static int RefreshTime = 0;
    if (RefreshTime == 0)
        RefreshTime = GetConfigInt(
            GeneralConfigSection, "RefreshTime", DefaultRefreshTime);
    
    return RefreshTime;
}

//////////////////////////////////////////////////////////////////////

CConfigVariable::CConfigVariable(
    const char* Section, 
    const char* Key, 
    int RefreshTime/*= 0*/
) : m_Section(Section), m_Key(Key),
    m_LastRefresh(0), 
    m_RefreshTime(RefreshTime > 0 ? RefreshTime : GetConfigRefreshTime())
{
}

CConfigVariable::CConfigVariable(
    int RefreshTime/*= 0*/
) : m_LastRefresh(0), 
    m_RefreshTime(RefreshTime > 0 ? RefreshTime : GetConfigRefreshTime())
{
}

void CConfigVariable::SetSectionAndKey(const char* Section, const char* Key)
{
    m_Section = Section;
    m_Key = Key;
}

bool CConfigVariable::HasToRefresh()
{
    if (m_Section.empty() || m_Key.empty() || m_RefreshTime == 0)
        return false;
    
    int CurrentTime = time(NULL);
    if (CurrentTime - m_LastRefresh > m_RefreshTime)
    {
        m_LastRefresh = CurrentTime;
        return true;
    }
    else
        return false;
}

//////////////////////////////////////////////////////////////////////

CConfigInt::CConfigInt(
    const char* Section, 
    const char* Key, 
    int Default/* = 0*/,        
    int RefreshTime/* = 0*/
) : CConfigVariable(Section, Key, RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
    GetValue();
}

CConfigInt::CConfigInt(
    int Default/* = 0*/,        
    int RefreshTime/* = 0*/
) : CConfigVariable(RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
    GetValue();
}

int CConfigInt::GetValue()
{
    if (HasToRefresh())
        m_Value = GetConfigInt(GetSection(), GetKey(), m_Default);
    
    return m_Value;
}

CConfigInt::operator int()
{
    return GetValue();
}

//////////////////////////////////////////////////////////////////////

CConfigBool::CConfigBool(
    const char* Section, 
    const char* Key, 
    bool Default /* = false*/,
    int RefreshTime /*= 0*/
) : CConfigVariable(Section, Key, RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
    GetValue();
}

CConfigBool::CConfigBool(
    bool Default /* = false*/,
    int RefreshTime /*= 0*/
) : CConfigVariable(RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
}

bool CConfigBool::GetValue()
{
    if (HasToRefresh())
        m_Value = GetConfigBool(GetSection(), GetKey(), m_Default);
    
    return m_Value;
}

CConfigBool::operator bool()
{
    return GetValue();
}

//////////////////////////////////////////////////////////////////////

CConfigString::CConfigString(
    const char* Section, 
    const char* Key, 
    const char* Default/* = NULL*/,        
    int RefreshTime/* = 0*/
) : CConfigVariable(Section, Key, RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
    GetValue();
}

CConfigString::CConfigString(
    const char* Default/* = NULL*/,        
    int RefreshTime/* = 0*/
) : CConfigVariable(RefreshTime),
    m_Default(Default), 
    m_Value(Default)
{
    GetValue();
}

std::string CConfigString::GetValue()
{
    if (HasToRefresh())
        m_Value = GetConfigString(GetSection(), GetKey(), m_Default.c_str());

    return m_Value;
}

const char* CConfigString::GetStrValue()
{
    if (HasToRefresh())
        m_Value = GetConfigString(GetSection(), GetKey(), m_Default.c_str());

    return m_Value.c_str();
}

CConfigString::operator std::string()
{
    return GetValue();
}

CConfigString::operator const char*()
{
    return GetStrValue();
}
