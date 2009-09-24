// FileLoader.cpp: implementation of the CFileLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileLoader.h"
#include "LogEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFileLoader::CFileLoader(): m_Buffer(NULL), m_Length(0), m_CurrentPosition(0)
{
}

CFileLoader::~CFileLoader()
{
	Close();
}

//return length of loaded data
int CFileLoader::Load(const char* FileName)
{
	Close();
    if(strlen(FileName) == 0)
    {
        LogEvent(LE_ERROR, "CFileLoader::Load(), Empty file name");
        return 0;
    }

	//open file	and read the contained messages
	//FILE* fp = fopen(FileName, "rb");
    FILE * fp = NULL;
    Verify (fopen_s(&fp, FileName, "rb") == 0);
	if(fp == NULL)
	{
		LogEvent(LE_ERROR, "CFileLoader::Load(), failed to open file '%s' ",
			FileName);
		return 0;
	}
	
	//get the file size	and load its data
	fseek(fp, 0, SEEK_END);
	int NumBytes = ftell(fp);
	m_Buffer = new BYTE[NumBytes];
	if(m_Buffer == NULL)
	{
		LogEvent(LE_ERROR, "CFileLoader::Load(), failed to allocate buffer for file length '%d' ",
			NumBytes);
		m_Length = 0;
		fclose(fp);
		return 0;
	}

	fseek(fp, 0, SEEK_SET);
	m_Length = fread(m_Buffer, 1, NumBytes, fp);
	if(m_Length != NumBytes)
	{
		LogEvent(LE_ERROR, "FileLoader::Load(), read %d while file size is %d",
			m_Length, NumBytes);
	}
	fclose(fp);
	return m_Length;
}


BYTE* CFileLoader::GetAt(int Pos)
{
	if(m_Buffer && (Pos <= m_Length))
		return m_Buffer + Pos;
	else
		return NULL;
}

void CFileLoader::Close()
{
	if(m_Buffer)
	{
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	m_Length = 0;
    m_CurrentPosition = 0;
}

int CFileLoader::GetLength() const
{
	return m_Length;
}

int CFileLoader::SetFilePosition(int Position)
{
    m_CurrentPosition = Position;
    if(m_CurrentPosition < 0)
        m_CurrentPosition = 0;
    else
    {
        if(m_CurrentPosition >= m_Length)
            m_CurrentPosition = m_Length -1;
    }
    return m_CurrentPosition;
}

int CFileLoader::ReadData(BYTE* BufferToCopyOn, int BufferSize, int& CopiedSize, bool Cyclic)
{
    CopiedSize = 0;

    CopiedSize = CopyData(BufferToCopyOn, BufferSize);
    if(Cyclic && (CopiedSize < BufferSize))
    {
        //only one cycle
        Assert(m_CurrentPosition == m_Length);
        m_CurrentPosition = 0;
        CopiedSize += CopyData(BufferToCopyOn + CopiedSize, BufferSize - CopiedSize);
    }
    return m_CurrentPosition;
}

int CFileLoader::CopyData(BYTE* BufferToCopyOn, int BufferSize)
{
    int AvailableBytes = m_Length - m_CurrentPosition;
    int CopySize = min(BufferSize, AvailableBytes);
    if(CopySize > 0)
    {
        memcpy(BufferToCopyOn, m_Buffer + m_CurrentPosition, CopySize);
        m_CurrentPosition += CopySize;
    }
    return CopySize;
}