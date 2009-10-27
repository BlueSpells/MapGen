#include "StdAfx.h"
#include "BitBufferCodec.h"

// We are working with a barcode encoder program which run over the first two bytes
#define NUMBER_OF_BYTES_FOR_PADDING	2

CBitBufferCodec::CBitBufferCodec(void) : m_ResultBuffer(NULL), m_ResultBufferSize(0)
{
}

CBitBufferCodec::~CBitBufferCodec(void)
{
	CleanBuffer();
}


void CBitBufferCodec::Encode(const BYTE *Buffer, const int BufferSize)
{
	CleanBuffer();
	
	m_ResultBufferSize = BufferSize + NUMBER_OF_BYTES_FOR_PADDING;
	m_ResultBuffer = new BYTE[m_ResultBufferSize];
	
	for (int i = 0; i < NUMBER_OF_BYTES_FOR_PADDING; i++)
		m_ResultBuffer[i] = 0x00;

	memcpy(m_ResultBuffer+NUMBER_OF_BYTES_FOR_PADDING, Buffer, BufferSize);
}

void CBitBufferCodec::Decode(const BYTE *Buffer, const int BufferSize)
{
	CleanBuffer();

	m_ResultBufferSize = BufferSize - NUMBER_OF_BYTES_FOR_PADDING;
	m_ResultBuffer = new BYTE[m_ResultBufferSize];

	memcpy(m_ResultBuffer, Buffer+NUMBER_OF_BYTES_FOR_PADDING, BufferSize);
}

void CBitBufferCodec::CleanBuffer()
{
	ASSERT((m_ResultBuffer!=NULL) == (m_ResultBufferSize > 0));

	if (m_ResultBuffer)
		delete m_ResultBuffer;
	m_ResultBufferSize = 0;
}

void CBitBufferCodec::DetachBuffer(BYTE *&Buffer, int &BufferSize)
{
	Buffer = m_ResultBuffer;
	BufferSize = m_ResultBufferSize;

	// Detach CANNOT use CleanBuffer as we don't want to delete the very buffer we are detaching !!
	m_ResultBuffer = NULL;
	m_ResultBufferSize = 0;
}