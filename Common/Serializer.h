#pragma once

#include <vector>

class ISerializer
{
public:
    virtual ~ISerializer() {}

    virtual bool AppendBuffer(const BYTE* const Data, UINT DataSize) = 0;

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
    bool AppendFlatStruct(const DataType& DataToAppend)
    {
        return AppendBuffer((const BYTE*)&DataToAppend, sizeof DataToAppend);
    }

	virtual int GetSize() const = 0;
};

