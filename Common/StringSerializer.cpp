#include "StdAfx.h"
#include "StringSerializer.h"
#include "Utils.h"

void CStringSerializer::AppendDelimiter()
{
    if (m_First)
        m_First = false;
    else
        m_Str += ", ";
}
/*virtual*/ bool CStringSerializer::AppendBuffer(const BYTE* const Data, UINT DataSize)
{
    Data;
    AppendDelimiter();
    char Buf[64];
    sprintf_s(Buf, "Buf %u", DataSize);
    m_Str += Buf;
    return true;
}
/*virtual*/ bool CStringSerializer::AppendChar(char CharValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurItoa(CharValue, Buf, 10);
    return true;
}

/*virtual*/ bool CStringSerializer::AppendByte(BYTE ByteValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurItoa(ByteValue, Buf, 10);
    return true;
}

/*virtual*/ bool CStringSerializer::AppendShort(short ShortValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurItoa(ShortValue, Buf, 10);
    return true;
}

/*virtual*/ bool CStringSerializer::AppendUshort(unsigned short UshortValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurItoa(UshortValue, Buf, 10);
    return true;
}

/*virtual*/ bool CStringSerializer::AppendInt(int IntValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurItoa(IntValue, Buf, 10);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendUint(UINT UintValue)
{
    AppendDelimiter();
    char Buf[32];
    m_Str += OurUitoa(UintValue, Buf, 10);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendInt64(INT64 IntValue)
{
    AppendDelimiter();
    char Buf[64];
    m_Str += OurItoa64(IntValue, Buf, 10);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendUint64(UINT64 UintValue)
{
    AppendDelimiter();
    char Buf[64];
    m_Str += OurUitoa64(UintValue, Buf, 10);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendTimet(time_t TimetValue)
{
    AppendDelimiter();
    char Buf[64];
    m_Str += OurUitoa64(TimetValue, Buf, 10);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendBool(bool Value)
{
    AppendDelimiter();
    m_Str += BooleanStr(Value);
    return true;
}
/*virtual*/ bool CStringSerializer::AppendFloat(float FloatValue)
{
    AppendDelimiter();
    char Buf[128];
    sprintf_s(Buf, "%f", FloatValue);
    m_Str += Buf;
    return true;
}
/*virtual*/ bool CStringSerializer::AppendStr(const char* Str)
{
    AppendDelimiter();
    m_Str += Str;
    return true;
}
/*virtual*/ bool CStringSerializer::AppendWstr(const wchar_t* Wstr)
{
    AppendDelimiter();
    m_Str += Wchar2Str(Wstr);
    return true;
}
/*virtual*/ int CStringSerializer::GetSize() const
{
    return m_Str.size();
}
