#pragma once
#include "Serializer.h"

class CBufferSerializer : public ISerializer
{
public:
	enum { INITIAL_BUFFER_SIZE = 128 };

	// AllocExtraPercentage = 0 meaning it will allocate the minimum size necessary for the current append
	CBufferSerializer(int RelevantOffset = 0, int AllocExtraPercentage = 100, int InitialSize = INITIAL_BUFFER_SIZE);
	~CBufferSerializer();
    BYTE* GetBuffer();

    virtual bool AppendBuffer (const BYTE* const Data, UINT DataSize);

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
	//Gives pointer to internal buffer.
	const BYTE* GetData() const;

	//////////////////////////////////////////////////////////////////////////
	// Frees the buffer memory and resets the buffer 
	void Clear(bool FreeMemory = true);
	void RemoveFront(UINT SizeToRemove);

protected:
    UINT      m_BufferSize;
	BYTE*     m_Buffer;
	UINT      m_DataEnd;
	const int m_AllocPercentage;
};
