#include "StdAfx.h"
#include "LogEvent.h"
#include "BufferDeSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBufferDeSerializer::CBufferDeSerializer(const char* ContextStr, const BYTE* DataOrigin, int DataSize, int RelevantOffset) :
    m_ContextStr(ContextStr), m_DataPtr(DataOrigin + RelevantOffset), m_DataSize(DataSize - RelevantOffset), m_DataOrigin(DataOrigin + RelevantOffset)
{
    Assert(RelevantOffset >= 0);
    Assert(DataSize >= RelevantOffset);
}

CBufferDeSerializer::~CBufferDeSerializer()
{
}

/*virtual*/ bool CBufferDeSerializer::GetNextBufferField(BYTE *DataToGet, DWORD sizeofDataToGet)
{
	Assert(DataToGet != NULL); // DataToGet must be already allocated (to the size of sizeofDataToGet) 

	Assert(m_DataOrigin <= m_DataPtr);
	Assert(m_DataOrigin + m_DataSize >= m_DataPtr + sizeofDataToGet);

	if ((m_DataOrigin  + m_DataSize >= m_DataPtr + sizeofDataToGet) && (m_DataOrigin <= m_DataPtr))
	{
		memcpy(DataToGet, m_DataPtr, sizeofDataToGet);
		m_DataPtr += sizeofDataToGet;
		return true;
	}
	else	
	{
		if (m_ContextStr)
		{
			LogEvent(LE_ERROR, "%s::GetNextBufferField, pass the end of the buffer",
				m_ContextStr); 
		}
		else
		{
			LogEvent(LE_ERROR, "GetNextBufferField, pass the end of the buffer");
		}
		return false; 
	}
}

/*virtual*/ bool CBufferDeSerializer::GetNextCharField(char& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextByteField(BYTE& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextShortField(short& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextUshortField(unsigned short& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextIntField(int& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextUintField(UINT& DataToGet)
{
	return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextInt64Field(INT64& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextUint64Field(UINT64& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextTimetField(time_t& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextBoolField(bool& DataToGet)
{
	return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextFloatField(float& DataToGet)
{
    return GetNextBufferField((BYTE*)&DataToGet, sizeof DataToGet);
}

/*virtual*/ bool CBufferDeSerializer::GetNextStrField(char*& DataToGet)
{
    size_t sizeofString = strlen((char *)m_DataPtr) + 1;
    DataToGet = new char[sizeofString];
    return GetNextBufferField((BYTE*)DataToGet, sizeofString);
}

/*virtual*/ bool CBufferDeSerializer::GetNextWstrField(wchar_t*& DataToGet)
{
    size_t sizeofString = (wcslen((wchar_t *)m_DataPtr) + 1) * sizeof(wchar_t);
    DataToGet = new wchar_t[sizeofString];
    return GetNextBufferField((BYTE*)DataToGet, sizeofString);
}
