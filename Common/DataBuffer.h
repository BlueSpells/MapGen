#pragma once

//////////////////////////////////////////////////////////////////////////
// CDataBuffer 
// Helper class that allows appending of data and automatically 
// allocates memory for the appended data.
// 
//////////////////////////////////////////////////////////////////////////

class CDataBuffer
{
public:
    enum
    {
        DATA_BUFFER_INITIAL_BUFFER_SIZE = 128
    };

    // AllocExtraPercentage = 0 meaning it will allocate the minimum size necessary for the current append
	CDataBuffer(int AllocExtraPercentage = 100, int InitialSize = DATA_BUFFER_INITIAL_BUFFER_SIZE, bool ShouldAlocateBuffer = true, int DataEnd = 0);
	~CDataBuffer();
    CDataBuffer(const CDataBuffer& Buffer);
    CDataBuffer& operator=(const CDataBuffer& OtherBuffer);

	bool Append(const BYTE* const Data, int DataSize);
    bool AppendInt(int IntValue);
    bool AppendBool(bool Value);
    bool AppendStr(const char * Str);

    template <typename DataType> 
    bool AppendType(const DataType& TypeToAppend)
    {
        int Len = sizeof TypeToAppend;
        return Append((const BYTE*)&TypeToAppend, Len);
    }

    bool UpdateData(int Position, const BYTE* const DataToUpdate, int DataSize);
	int GetDataSize() const;
    //Gives pointer to internal buffer.
	const BYTE* GetData() const;

    //////////////////////////////////////////////////////////////////////////
    // Frees the buffer memory and resets the buffer 
	void Clear( bool FreeMemory = true );


    void RemoveFront(int SizeToRemove);

protected:
	BYTE* m_Buffer;
	int m_BufferSize;
	int m_DataEnd;
    int m_AllocPercentage;
};
