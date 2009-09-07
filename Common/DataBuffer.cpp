#include "StdAfx.h"
#include "DataBuffer.h"
#include "LogEvent.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDataBuffer::CDataBuffer(int AllocExtraPercentage /* = 100 */,
                         int InitialSize /* = DATA_BUFFER_INITIAL_BUFFER_SIZE */,
						 bool ShouldAlocateBuffer /*= true*/,
						 int DataEnd /* = 0*/)
: m_BufferSize(InitialSize)
, m_AllocPercentage(AllocExtraPercentage)
, m_DataEnd(DataEnd)
{
	if (ShouldAlocateBuffer)
		m_Buffer = (BYTE*)malloc(m_BufferSize);
}

CDataBuffer::CDataBuffer(const CDataBuffer& Buffer)
: m_BufferSize(Buffer.GetDataSize()), m_AllocPercentage(Buffer.m_AllocPercentage),
    m_DataEnd(0)
{
    m_Buffer = (BYTE*)malloc(m_BufferSize);
    Append(Buffer.GetData(), Buffer.GetDataSize());
}

CDataBuffer::~CDataBuffer()
{
	if (m_Buffer)
		free(m_Buffer);
}

CDataBuffer& CDataBuffer::operator=(const CDataBuffer& OtherBuffer)
{
    if(this == &OtherBuffer)
        return *this;

    free(m_Buffer);
    m_BufferSize = OtherBuffer.m_BufferSize;
    m_AllocPercentage = OtherBuffer.m_AllocPercentage;
    m_DataEnd = 0;
    m_Buffer = (BYTE*)malloc(m_BufferSize);
    Append(OtherBuffer.GetData(), OtherBuffer.GetDataSize());
    return *this;
}


bool CDataBuffer::Append(const BYTE* const Data, int DataSize)
{
	if(m_DataEnd + DataSize > m_BufferSize)
	{
		//int NewBufferSize = m_BufferSize;
		//while(m_DataEnd + DataSize > m_BufferSize)
		//	NewBufferSize *= 2;
		int NewBufferSize = (m_DataEnd + DataSize);
        if(m_AllocPercentage > 0)
        {
            NewBufferSize += NewBufferSize * m_AllocPercentage / 100;
        }
		BYTE* TempPointer = (BYTE*)realloc(m_Buffer, NewBufferSize);
		if(NULL != TempPointer)
		{
			m_Buffer		= TempPointer;
			m_BufferSize	= NewBufferSize;
		}
		else
			return false;
	}
	memcpy(m_Buffer + m_DataEnd, Data, DataSize);
	m_DataEnd += DataSize;

	return true;
}

bool CDataBuffer::AppendInt(int IntValue)
{
    return Append((BYTE*)((int*)&IntValue), sizeof(int));
}

bool CDataBuffer::AppendBool(bool Value)
{
    return Append((BYTE*)((bool*)&Value), sizeof(bool));
}

bool CDataBuffer::AppendStr(const char * Str)
{
    return Append((BYTE*)Str, strlen(Str) + 1);
}


// Override data that is already in the buffer
bool CDataBuffer::UpdateData(int Position, const BYTE* const DataToUpdate, int DataSize)
{
    if (Position + DataSize > m_DataEnd)
        return false;

    memcpy(m_Buffer + Position, DataToUpdate, DataSize);
    return true;
}

int CDataBuffer::GetDataSize() const
{
	return m_DataEnd;
}

const BYTE* CDataBuffer::GetData() const
{
	return m_Buffer;
}


void CDataBuffer::Clear( bool FreeMemory )
{
	// If memory is not needed to be cleaned m_DataEnd = 0 is enough.
    m_DataEnd = 0;
    if( FreeMemory )
    {
	    free(m_Buffer);
        m_BufferSize = DATA_BUFFER_INITIAL_BUFFER_SIZE;
        m_Buffer = (BYTE*)malloc(m_BufferSize);
    }
}

void CDataBuffer::RemoveFront(int SizeToRemove)
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




//////////////////////////////////////////////////////////////////////////
/// DataBuffer tests

#ifdef _DEBUG
#undef new
#endif //_DEBUG

#ifdef _TEST
#include "LogEvent.h"

struct TestStruct
{
    int Int;
    char Str[50];
};

void TestDataBuffer()
{
	CDataBuffer DataBuffer;
	char* String = "SimpleString";
	int StringLength = 12;
	int NumberOfRepetitions = 1000;
	for(int i = 0; i < NumberOfRepetitions; ++i)
		DataBuffer.Append((BYTE*)String, StringLength);
	Assert(DataBuffer.GetDataSize() == StringLength * NumberOfRepetitions);
	const BYTE* Data = DataBuffer.GetData();
	for(int j = 0; j < NumberOfRepetitions; ++j)
	{
		bool Equal = (0 == memcmp(String, Data, StringLength));
        Assert(Equal);
		Data += StringLength;
	}

    CDataBuffer AnotherBuffer(DataBuffer);
    char* UpdateString = "UpdateString";
    int UpdateStringLen = (int)strlen(UpdateString);
    int UpdatePosition = 10;
    Assert(AnotherBuffer.UpdateData(UpdatePosition, (BYTE*)UpdateString,
                                    UpdateStringLen) == true);
    Data = AnotherBuffer.GetData();
    Assert(memcmp(Data + UpdatePosition, UpdateString, UpdateStringLen) == 0);

    TestStruct TestType;
    AnotherBuffer.AppendType(TestType);

    CDataBuffer AssignedBuffer = AnotherBuffer;
    Assert(AssignedBuffer.GetDataSize() == AnotherBuffer.GetDataSize());
    Assert(memcmp(AssignedBuffer.GetData(), AnotherBuffer.GetData(), AnotherBuffer.GetDataSize()) == 0);
}

#endif