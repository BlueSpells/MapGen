#pragma once

#include <vector>
#include "Utils.h"
#include "LogEvent.h"

#define ByteEnumMinusOne (UCHAR_MAX-1)
#define IsByteEnumValue(x) (x >= -1  &&  x <= UCHAR_MAX  &&  x != ByteEnumMinusOne)

class ISerializer
{
public:
    virtual ~ISerializer() {}

    virtual bool AppendBuffer(const BYTE* const Data, UINT DataSize) = 0;

    virtual bool AppendChar(char CharValue) = 0;
    virtual bool AppendByte(BYTE ByteValue) = 0;
    virtual bool AppendShort(short ShortValue) = 0;
    virtual bool AppendUshort(unsigned short UshortValue) = 0;
    virtual bool AppendInt(int IntValue) = 0;
	virtual bool AppendUint(UINT UintValue) = 0;
    virtual bool AppendInt64(INT64 IntValue) = 0;
    virtual bool AppendUint64(UINT64 UintValue) = 0;
    virtual bool AppendTimet(time_t TimetValue) = 0;
	virtual bool AppendBool(bool Value) = 0;
    virtual bool AppendFloat(float FloatValue) = 0;
    virtual bool AppendStr(const char* Str) = 0;
    virtual bool AppendWstr(const wchar_t* Str) = 0;

    bool AppendData(const BYTE* const Data, UINT DataSize)
    {
        if (!AppendUint(DataSize))
            return false;
        return AppendBuffer(Data, DataSize);
    }

    bool AppendTinyData(const BYTE* const Data, UINT DataSize)
    {
        Assert(IsByteValue(DataSize));
        if (!AppendByte((BYTE)DataSize))
            return false;
        return AppendBuffer(Data, DataSize);
    }

    bool AppendIntAsShort(int IntValue)
    {
        Assert(IsShortValue(IntValue));
        return AppendShort((SHORT)IntValue);
    }

    bool AppendIntAsUshort(int IntValue)
    {
        Assert(IsUshortValue(IntValue));
        return AppendUshort((USHORT)IntValue);
    }

    bool AppendIntAsByte(int IntValue)
    {
        Assert(IsByteValue(IntValue));
        return AppendByte((char)IntValue);
    }

    bool AppendIntAsChar(int IntValue)
    {
        Assert(IsCharValue(IntValue));
        return AppendChar((char)IntValue);
    }

    bool AppendUintAsUshort(int UshortValue)
    {
        Assert(IsUshortValue(UshortValue));
        return AppendUshort((USHORT)UshortValue);
    }

    bool AppendUintAsByte(UINT UintValue)
    {
        Assert(IsByteValue(UintValue));
        return AppendByte((BYTE)UintValue);
    }

    bool AppendStr(const std::string& Str)
    {
        return AppendStr(Str.c_str());
    }

    bool AppendWstr(const std::wstring& Wstr)
    {
        return AppendWstr(Wstr.c_str());
    }

    bool AppendIntVector(std::vector<int> IntVector)
    {
        UINT VectorSize = (UINT)IntVector.size();
        bool Ret = AppendUint(VectorSize);
        if (!Ret)
            return false;
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && AppendInt(IntVector[i]);
        return Ret;
    }

    bool AppendTinyIntVector(std::vector<int> IntVector)
    {
        UINT VectorSize = (UINT)IntVector.size();
        Assert(IsByteValue(VectorSize));
        bool Ret = AppendByte((BYTE)VectorSize);
        if (!Ret)
            return false;
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && AppendInt(IntVector[i]);
        return Ret;
    }

    template <typename DataType>
    bool AppendVector(std::vector<DataType> DataVector)
    {
        UINT VectorSize = DataVector.size();
        bool Ret = AppendUint(VectorSize);
        if (!Ret)
            return false;
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && AppendFlatStruct(DataVector[i]);
        return Ret;
    }

//#define AppendIntVector(IntVector) AppendVector<int>(IntVector)
//     template <typename DataType>
//         bool AppendEnum (const DataType& DataToAppend)
//     {
//         Assert(sizeof DataToAppend == sizeof(int));
//         return AppendBuffer((const BYTE*)&DataToAppend, sizeof DataToAppend);
//     }

    template <typename DataType>
        bool AppendEnum (const DataType& DataToAppend)
    {
        Assert(sizeof DataToAppend == sizeof(int));
        return AppendInt(DataToAppend);
    }

    template <typename DataType>
        bool AppendEnumAsByte (const DataType& DataToAppend)
    {
        Assert(sizeof DataToAppend == sizeof(int));
        Assert(IsByteEnumValue(DataToAppend));
        return AppendByte((BYTE)(DataToAppend == -1 ? ByteEnumMinusOne : DataToAppend));
    }

    template <typename DataType>
    bool AppendFlatStruct(const DataType& DataToAppend)
    {
        return AppendBuffer((const BYTE*)&DataToAppend, sizeof DataToAppend);
    }

	virtual int GetSize() const = 0;
};

