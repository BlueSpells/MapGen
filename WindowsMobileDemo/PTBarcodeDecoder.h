#pragma once
#include "ibarcodedecoder.h"
#include "PtAPICE.h"
#include "BarcodeProcessor/BitBufferCodec.h"

#define MAX_BARCODE_DATA_LEN 8000
#define MAX_BARCODE_COUNT    8

struct INTRABARCODEINFO
{
	DWORD      dwX1, dwY1;    //Four corners' coordinates in pixels of the barcode. 
	DWORD      dwX2, dwY2;
	DWORD      dwX3, dwY3;
	DWORD      dwX4, dwY4;
	BYTE       Data[MAX_BARCODE_DATA_LEN];         //Pointer to the buffer that contains the barcode's data.
	DWORD      dwDataLen;     //The barcode data's length in bytes.
};

struct INTRATOTALBARCODEINFO
{
	INTRABARCODEINFO  InfoList[MAX_BARCODE_COUNT];    //Pointer to the start address of the list of barcodes' info.  
	DWORD             dwTotalCount; //The number of barcode that have been decoded.
};


class CPTBarcodeDecoder : public IBarcodeDecoder
{
public:
	CPTBarcodeDecoder(void);
	~CPTBarcodeDecoder(void);

	virtual bool Init();
	virtual bool DecodeImage(PTCHAR ImageFileName, std::vector<SBarcodeData> &DecodedBuffers);

private:
	// Attributes
	bool					  m_IsInitialized;
	INTRATOTALBARCODEINFO     m_BarInfo;
	PTIMAGE                   m_image;
	PTDECODEPARA              m_para;
	float                     m_scale;
	CBitBufferCodec			  m_Codec;

	// Operations
	void InitBarCodeInfo( );
	bool OpenFile(PTCHAR ImageFileName);
	bool DecodeQRCode();
	bool ParseDecodedBarcode( PTTOTALBARCODEINFO* pBar, bool IsFirstDecoding );
	bool HaveWeRemovedAllDollars();
	bool ExtractBarcodeData( PTTOTALBARCODEINFO* pBar, bool IsFirstDecoding );
};
