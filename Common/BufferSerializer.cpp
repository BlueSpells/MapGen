#include "StdAfx.h"
#include "LogEvent.h"
#include "BufferSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBufferSerializer::CBufferSerializer(int RelevantOffset /*= 0*/, int AllocExtraPercentage /*= 100*/, int InitialSize /*= DATA_BUFFER_INITIAL_BUFFER_SIZE*/) :
    m_BufferSize(InitialSize+RelevantOffset), m_Buffer((BYTE*)malloc(m_BufferSize)), m_DataEnd(RelevantOffset), m_AllocPercentage(AllocExtraPercentage)
{
    Assert(RelevantOffset >= 0);
    Assert(InitialSize >= RelevantOffset);
    Assert(m_Buffer != NULL);
}

CBufferSerializer::~CBufferSerializer()
{
	free(m_Buffer);
}

BYTE* CBufferSerializer::GetBuffer()
{
    return m_Buffer;
}

/*virtual*/ bool CBufferSerializer::AppendBuffer(const BYTE* const Data, UINT DataSize)
{
	// Should we increase buffer size?
	if(m_DataEnd + DataSize > m_BufferSize)
	{
		// calculate needed size
		UINT NewBufferSize = (m_DataEnd + DataSize);
		if(m_AllocPercentage > 0)
		{
			NewBufferSize += NewBufferSize * m_AllocPercentage / 100;
		}

		// reallocate buffer
		BYTE* TempPointer = (BYTE*)realloc(m_Buffer, NewBufferSize);
		if(NULL != TempPointer)
		{
			m_Buffer		= TempPointer;
			m_BufferSize	= NewBufferSize;
		}
		else
			return false;
	}

	// copy data to end of buffer
	memcpy(m_Buffer + m_DataEnd, Data, DataSize);
	m_DataEnd += DataSize;

	return true;
}

/*virtual*/ bool CBufferSerializer::AppendChar(char CharValue)
{
    return AppendBuffer((BYTE*)&CharValue, sizeof CharValue);
}

/*virtual*/ bool CBufferSerializer::AppendByte(BYTE ByteValue)
{
    return AppendBuffer((BYTE*)&ByteValue, sizeof ByteValue);
}

/*virtual*/ bool CBufferSerializer::AppendShort(short ShortValue)
{
    return AppendBuffer((BYTE*)&ShortValue, sizeof ShortValue);
}

/*virtual*/ bool CBufferSerializer::AppendUshort(unsigned short UshortValue)
{
    return AppendBuffer((BYTE*)&UshortValue, sizeof UshortValue);
}

/*virtual*/ bool CBufferSerializer::AppendInt(int IntValue)
{
    return AppendBuffer((BYTE*)&IntValue, sizeof IntValue);
}

/*virtual*/ bool CBufferSerializer::AppendUint(UINT UintValue)
{
	return AppendBuffer((BYTE*)&UintValue, sizeof UintValue);
}

/*virtual*/ bool CBufferSerializer::AppendInt64(INT64 IntValue)
{
    return AppendBuffer((BYTE*)&IntValue, sizeof IntValue);
}

/*virtual*/ bool CBufferSerializer::AppendUint64(UINT64 UintValue)
{
    return AppendBuffer((BYTE*)&UintValue, sizeof UintValue);
}

/*virtual*/ bool CBufferSerializer::AppendTimet(time_t TimetValue)
{
    return AppendBuffer((BYTE*)&TimetValue, sizeof TimetValue);
}

/*virtual*/ bool CBufferSerializer::AppendBool(bool Value)
{
    Assert(sizeof Value == 1);
	return AppendBuffer((BYTE*)&Value, sizeof Value);
}

/*virtual*/ bool CBufferSerializer::AppendFloat(float FloatValue)
{
    return AppendBuffer((BYTE*)&FloatValue, sizeof FloatValue);
}

/*virtual*/ bool CBufferSerializer::AppendStr(const char* Str)
{
    return AppendBuffer((BYTE*)Str, strlen(Str) + 1);
}

/*virtual*/ bool CBufferSerializer::AppendWstr(const wchar_t* Wstr)
{
    return AppendBuffer((BYTE*)Wstr, (wcslen(Wstr) + 1) * sizeof(wchar_t));
}

/*virtual*/ int CBufferSerializer::GetSize() const
{
	return m_DataEnd;
}

/*virtual*/ const BYTE* CBufferSerializer::GetData() const
{
	return m_Buffer;
}

/*virtual*/ void CBufferSerializer::Clear(bool FreeMemory)
{
	// If memory is not needed to be cleaned m_DataEnd = 0 is enough.
	m_DataEnd = 0;
	if( FreeMemory )
	{
		free(m_Buffer);
		m_BufferSize = INITIAL_BUFFER_SIZE;
		m_Buffer = (BYTE*)malloc(m_BufferSize);
	}
}

/*virtual*/ void CBufferSerializer::RemoveFront(UINT SizeToRemove)
{
	if(SizeToRemove >= m_DataEnd)
	{
		Clear();
		return;
	}
	int NewDataSize = m_DataEnd - SizeToRemove;
	Assert(NewDataSize > 0);

	memmove(m_Buffer, m_Buffer + SizeToRemove, NewDataSize);
	m_DataEnd = NewDataSize;
}
