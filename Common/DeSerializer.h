#pragma once

class IDeSerializer
{
public:
    virtual ~IDeSerializer() {}

    // note: DataToGet must be already allocated to the size of sizeofDataToGet !!!
	virtual bool GetNextBufferField(BYTE* DataToGet, DWORD sizeofDataToGet) = 0;

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
        int VectorSize;
        bool Ret = GetNextIntField(VectorSize);
        if (!Ret)
            return false;
        IntVector.resize(VectorSize);
        for (int i = 0; i < VectorSize; ++i)
            Ret = Ret && GetNextIntField(IntVector[i]);
        return Ret;
    }

    template <typename DataType>
    bool GetNextVectorField(std::vector<DataType>& DataVector)
    {
        int VectorSize;
        bool Ret = GetNextIntField(VectorSize);
        if (!Ret)
            return false;
        DataVector.resize(VectorSize);
        for (int i = 0; i < VectorSize; ++i)
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
    bool GetNextFlatStructField(DataType& DataToGet)
    {
        return GetNextBufferField((BYTE*)&DataToGet, sizeof DataType);
    }

 	virtual int GetSize() = 0;
};
