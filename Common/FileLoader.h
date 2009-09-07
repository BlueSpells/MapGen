// FileLoader.h: interface for the CFileLoader class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CFileLoader
{
public:
	CFileLoader();
	~CFileLoader();

	//return length of loaded data
	int Load(const char* FileName);
	void Close();
    int GetLength() const;

    //read data from current position and increment the file position. Returns the new position.
    int ReadData(BYTE* BufferToCopyOn, int BufferSize, int& CopiedSize, bool Cyclic = false);

    //returns the new position
    int SetFilePosition(int Position); 

    BYTE* GetAt(int Pos);

private:
    // Hide copy ctor and assignment operator
    CFileLoader(const CFileLoader &);
    CFileLoader & operator=(const CFileLoader &);

private:
    int CopyData(BYTE* BufferToCopyOn, int BufferSize); //return Num Bytes copied

private:
	BYTE* m_Buffer;
	int m_Length;
    int m_CurrentPosition;
};

