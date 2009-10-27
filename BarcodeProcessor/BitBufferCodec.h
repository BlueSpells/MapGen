#pragma once

class CBitBufferCodec
{
public:
	CBitBufferCodec(void);
	~CBitBufferCodec(void);

	void Encode(const BYTE *Buffer, const int BufferSize);
	void Decode(const BYTE *Buffer, const int BufferSize);
	void DetachBuffer(BYTE *&Buffer, int &BufferSize);

	BYTE *GetData() {return m_ResultBuffer;}
	int   GetDataSize() {return m_ResultBufferSize;}

private:
	void CleanBuffer();

	BYTE *m_ResultBuffer;
	int   m_ResultBufferSize;
};
