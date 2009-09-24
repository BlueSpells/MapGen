#pragma once
#include <vector>
#include "DeSerializer.h"

class CBufferDeSerializer :	public IDeSerializer
{
public:
	CBufferDeSerializer (const char* ContextStr, const BYTE* DataOrigin, int DataSize, int RelevantOffset = 0);
	~CBufferDeSerializer();

	// note: DataToGet must be already allocated to the size of sizeofDataToGet !!!
	virtual bool GetNextBufferField(BYTE *DataToGet, DWORD sizeofDataToGet);

    virtual bool GetNextCharField(char& DataToGet);
    virtual bool GetNextByteField(BYTE& DataToGet);
    virtual bool GetNextShortField(short& DataToGet);
    virtual bool GetNextUshortField(unsigned short& DataToGet);
    virtual bool GetNextIntField(int& DataToGet);
	virtual bool GetNextUintField(UINT& DataToGet);
    virtual bool GetNextInt64Field(INT64& DataToGet);
    virtual bool GetNextUint64Field(UINT64& DataToGet);
    virtual bool GetNextTimetField(time_t& DataToGet);
	virtual bool GetNextBoolField(bool& DataToGet);
    virtual bool GetNextFloatField(float& DataToGet);

	// note: GetNextStrField allocates the string itself (as the caller of this function
	//		 is unaware to the size of the string.
	virtual bool GetNextStrField(char*& DataToGet);
    virtual bool GetNextWstrField(wchar_t*& DataToGet);

	virtual int GetSize() { return m_DataSize; }

protected:
    const char* const m_ContextStr;
	const BYTE*       m_DataPtr;
    const int         m_DataSize;
	const BYTE* const m_DataOrigin;
};

