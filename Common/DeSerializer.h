#pragma once

#include "Serializer.h"

class IDeSerializer
{
public:
    virtual ~IDeSerializer() {}

    // note: DataToGet must be already allocated to the size of sizeofDataToGet !!!
	virtual bool GetNextBufferField(BYTE* DataToGet, DWORD sizeofDataToGet) = 0;

    virtual bool GetNextCharField(char& DataToGet) = 0;
    virtual bool GetNextByteField(BYTE& DataToGet) = 0;
    virtual bool GetNextShortField(short& DataToGet) = 0;
    virtual bool GetNextUshortField(unsigned short& DataToGet) = 0;
    virtual bool GetNextIntField(int& DataToGet) = 0;
	virtual bool GetNextUintField(UINT& DataToGet) = 0;
    virtual bool GetNextInt64Field(INT64& DataToGet) = 0;
	virtual bool GetNextUint64Field(UINT64& DataToGet) = 0;
    virtual bool GetNextTimetField(time_t& DataToGet) = 0;
	virtual bool GetNextBoolField(bool& DataToGet) = 0;
    virtual bool GetNextFloatField(float& DataToGet) = 0;
    // note: GetNextStrField allocates the string itself (as the caller of this function
    //		 is unaware to the size of the string.
    virtual bool GetNextStrField(char*& DataToGet) = 0;
    virtual bool GetNextWstrField(wchar_t*& DataToGet) = 0;

    // note: GetNextDataField allocates the string itself (as the caller of this function
    //		 is unaware to the size of the string.
    bool GetNextDataField(BYTE*& DataToGet, UINT& sizeofDataToGet)
    {
        if (!GetNextUintField(sizeofDataToGet))
            return false;
        DataToGet = new BYTE[sizeofDataToGet];
        return GetNextBufferField(DataToGet, sizeofDataToGet);
    }

    // note: GetNextTinyDataField allocates the string itself (as the caller of this function
    //		 is unaware to the size of the string.
    bool GetNextTinyDataField(BYTE*& DataToGet, UINT& sizeofDataToGet)
    {
        BYTE ByteSizeofDataToGet;
        if (!GetNextByteField(ByteSizeofDataToGet))
            return false;
        sizeofDataToGet = ByteSizeofDataToGet;
        DataToGet = new BYTE[sizeofDataToGet];
        return GetNextBufferField(DataToGet, sizeofDataToGet);
    }

    bool GetNextIntAsShortField(int& DataToGet)
    {
        SHORT ShortData;
        if (!GetNextShortField(ShortData))
            return false;
        DataToGet = ShortData;
        return true;
    }

    bool GetNextIntAsUshortField(int& DataToGet)
    {
        USHORT UshortData;
        if (!GetNextUshortField(UshortData))
            return false;
        DataToGet = UshortData;
        return true;
    }

    bool GetNextIntAsByteField(int& DataToGet)
    {
        BYTE ByteData;
        if (!GetNextByteField(ByteData))
            return false;
        DataToGet = ByteData;
        return true;
    }

    bool GetNextIntAsCharField(int& DataToGet)
    {
        char CharData;
        if (!GetNextCharField(CharData))
            return false;
        DataToGet = CharData;
        return true;
    }

    bool GetNextUintAsUshortField(UINT& DataToGet)
    {
        USHORT UshortData;
        if (!GetNextUshortField(UshortData))
            return false;
        DataToGet = UshortData;
        return true;
    }

    bool GetNextUintAsByteField(UINT& DataToGet)
    {
        BYTE ByteData;
        if (!GetNextByteField(ByteData))
            return false;
        DataToGet = ByteData;
        return true;
    }

    bool GetNextStrField(std::string& DataToGet)
    {
        char* Str;
        bool Ret = GetNextStrField(Str);
        if (!Ret)
            return false;
        DataToGet = Str;
        delete[] Str;
        return true;
    }

    bool GetNextWstrField(std::wstring& DataToGet)
    {
        wchar_t* Wstr;
        bool Ret = GetNextWstrField(Wstr);
        if (!Ret)
            return false;
        DataToGet = Wstr;
        delete[] Wstr;
        return true;
    }

    bool GetNextIntVectorField(std::vector<int>& IntVector)
    {
        UINT VectorSize;
        bool Ret = GetNextUintField(VectorSize);
        if (!Ret)
            return false;
        IntVector.resize(VectorSize);
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && GetNextIntField(IntVector[i]);
        return Ret;
    }

    bool GetNextTinyIntVectorField(std::vector<int>& IntVector)
    {
        BYTE ByteVectorSize;
        bool Ret = GetNextByteField(ByteVectorSize);
        if (!Ret)
            return false;
        UINT VectorSize = ByteVectorSize;
        IntVector.resize(VectorSize);
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && GetNextIntField(IntVector[i]);
        return Ret;
    }

    template <typename DataType>
    bool GetNextVectorField(std::vector<DataType>& DataVector)
    {
        UINT VectorSize;
        bool Ret = GetNextUintField(VectorSize);
        if (!Ret)
            return false;
        DataVector.resize(VectorSize);
        for (UINT i = 0; i < VectorSize; ++i)
            Ret = Ret && GetNextFlatStructField(DataVector[i]);
        return Ret;
    }

    template <typename DataType>
    bool GetNextEnumField(DataType& DataToGet)
    {
        Assert(sizeof DataToGet == sizeof(int));
        return GetNextIntField(*(int*)&DataToGet);
    }

    template <typename DataType>
    bool GetNextEnumAsByteField(DataType& DataToGet)
    {
        Assert(sizeof DataToGet == sizeof(int));
        BYTE ByteDataToGet;
        if (!GetNextByteField(ByteDataToGet))
            return false;
        DataToGet = (DataType)(ByteDataToGet == ByteEnumMinusOne ? -1 : ByteDataToGet);
        return true;
    }

    template <typename DataType>
    bool GetNextFlatStructField(DataType& DataToGet)
    {
        return GetNextBufferField((BYTE*)&DataToGet, sizeof DataType);
    }

 	virtual int GetSize() = 0;
};
