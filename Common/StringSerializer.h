#pragma once
#include <string>
#include "Serializer.h"

class CStringSerializer : public ISerializer
{
public:
    CStringSerializer() : m_First(true) {}
    virtual ~CStringSerializer() {}
    const std::string& GetString() const { return m_Str; }

    virtual bool AppendBuffer(const BYTE* const Data, UINT DataSize);

    virtual bool AppendChar(char CharValue);
    virtual bool AppendByte(BYTE ByteValue);
    virtual bool AppendShort(short ShortValue);
    virtual bool AppendUshort(unsigned short UshortValue);
    virtual bool AppendInt(int IntValue);
    virtual bool AppendUint(UINT UintValue);
    virtual bool AppendInt64(INT64 IntValue);
    virtual bool AppendUint64(UINT64 UintValue);
    virtual bool AppendTimet(time_t TimetValue);
    virtual bool AppendBool(bool Value);
    virtual bool AppendFloat(float FloatValue);
    virtual bool AppendStr(const char* Str);
    virtual bool AppendWstr(const wchar_t* Wstr);

    virtual int GetSize() const;

private:
    void AppendDelimiter();
    std::string m_Str;
    bool m_First;
};
