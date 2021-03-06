#include "StdAfx.h"
#include "PTBarcodeDecoder.h"

CPTBarcodeDecoder::CPTBarcodeDecoder(void) : m_IsInitialized(false)
{
}

CPTBarcodeDecoder::~CPTBarcodeDecoder(void)
{
	if (m_IsInitialized)
		PtFreeImage(&m_image);
}

/*virtual*/ bool CPTBarcodeDecoder::Init()
{
	//PtDMDecodeRegister    ("18310708290105222081");//demo
	/*int rv = */PtQRDecodeRegister    ("18210708290105166561");//demo

//  ToDo: Seems like PtQRDecodeRegister fails when in demo version!!
// 	if (rv != PT_QRDECODE_SUCCESS)
// 		return false;

	InitBarCodeInfo();
	PtInitImage(&m_image);
	m_scale=1.0;
	memset( &m_para, 0 ,sizeof(m_para) );

	m_IsInitialized = true;
	return true;
}

// we need MAX_TIMES_TO_LOOP because if there really is a $ in the text, 
// the loop would otherwise run infinitely.
#define MAX_TIMES_TO_LOOP	3

/*virtual*/ bool CPTBarcodeDecoder::DecodeImage(PTCHAR ImageFileName, std::vector<SBarcodeData> &DecodedBuffers)
{
	if (!OpenFile(ImageFileName))
		return false;

	if (!DecodeQRCode())
 		return false;


	for(int count=0; count<int(m_BarInfo.dwTotalCount); count++)
	{
		SBarcodeData BarcodeData;
		m_Codec.Decode(m_BarInfo.InfoList[count].Data, m_BarInfo.InfoList[count].dwDataLen);
		m_Codec.DetachBuffer(BarcodeData.Data, BarcodeData.DataLength);
		DecodedBuffers.push_back(BarcodeData);
	}

	return true;
}

bool CPTBarcodeDecoder::ParseDecodedBarcode( PTTOTALBARCODEINFO* pBar, bool IsFirstDecoding )
{
	int  count;
	char ch[200];
	
	if (!ExtractBarcodeData(pBar, IsFirstDecoding))
		return false;

	sprintf( ch, "Find %d barcodes\n", m_BarInfo.dwTotalCount );
	CString str;
	str = ch;
	for( count=0; count<int(m_BarInfo.dwTotalCount); count++)
	{
		int i;
// 		sprintf( ch, " barcode %d:\n", count+1 );
// 		str += ch;
// 		for( i=0; i<int(m_BarInfo.InfoList[count].dwDataLen); i++)
// 			str += m_BarInfo.InfoList[count].Data[i];
		
		str+=" Hex:\n";
		for( i=0; i<int(m_BarInfo.InfoList[count].dwDataLen); i++)
		{
			sprintf( ch, "%x%x ", m_BarInfo.InfoList[count].Data[i]>>4&0x0F, m_BarInfo.InfoList[count].Data[i]&0x0F);   
			str += ch;
		} 
		
		str+="\n\n";
	}
	::AfxMessageBox(str);

	return true;
}

bool CPTBarcodeDecoder::DecodeQRCode()
{
	if(m_image.pBits==NULL)
	{
		AfxMessageBox(L"Please Open a file first");
		return false;
	} 

	InitBarCodeInfo( );

	CWaitCursor wait;
	PTTOTALBARCODEINFO QRCodeInfo;
	PtQRDecodeInit(&QRCodeInfo);

	// Todo: Check performance of PtQRDecodeFromFile VS PtQRDecode !!

	//if(  PtQRDecodeFromFile ( GetPathName(), &m_para, &QRCodeInfo ) != PT_QRDECODE_SUCCESS )
	bool ok = true;
	
	int i = 0;
	do 
	{
		if(  PtQRDecode ( &m_image, &m_para, &QRCodeInfo ) != PT_QRDECODE_SUCCESS )
		{
			::AfxMessageBox(L"An error occurred while recognition ");
			ok = false;
		}
		else
			ok = ExtractBarcodeData(&QRCodeInfo, i==0);//ParseDecodedBarcode(&QRCodeInfo, IsFirstDecoding);

	} while(!HaveWeRemovedAllDollars() && ++i <= MAX_TIMES_TO_LOOP && ok);


	PtQRDecodeFree( &QRCodeInfo );//release the mem allocated while decoding
	return ok;
}

bool CPTBarcodeDecoder::OpenFile(PTCHAR ImageFileName)
{
	memset( &m_para, 0, sizeof(PTDECODEPARA) );
	InitBarCodeInfo();
	PtFreeImage(&m_image);
	int frames=PtGetImageFrames( ImageFileName);
	if( frames <=0 )
	{
		::AfxMessageBox (L"open file error");
		return false;
	}
	int ret=PtLoadImage( ImageFileName, &m_image, frames-1);
	if( ret != PT_IMAGERW_SUCCESS )
	{
		::AfxMessageBox (L"open file error");
		return false;
	}
	m_scale=1.0;

	return true;
}

void CPTBarcodeDecoder::InitBarCodeInfo( )
{
	m_BarInfo.dwTotalCount = 0;
	for( int i=0; i<MAX_BARCODE_COUNT; i++)
	{
		m_BarInfo.InfoList[i].dwDataLen = 0;
		memset( m_BarInfo.InfoList[i].Data , 0, MAX_BARCODE_DATA_LEN);
		m_BarInfo.InfoList[i].dwX1 = 0;
		m_BarInfo.InfoList[i].dwX2 = 0;
		m_BarInfo.InfoList[i].dwX3 = 0;
		m_BarInfo.InfoList[i].dwX4 = 0;
		m_BarInfo.InfoList[i].dwY1 = 0;
		m_BarInfo.InfoList[i].dwY2 = 0;
		m_BarInfo.InfoList[i].dwY3 = 0;
		m_BarInfo.InfoList[i].dwY4 = 0;
	}
}

bool CPTBarcodeDecoder::HaveWeRemovedAllDollars()
{
	ASSERT(m_BarInfo.dwTotalCount > 0);
	if( m_BarInfo.dwTotalCount<=0 )
	{
		::AfxMessageBox(L"Error: No barcodes");
		return false;
	}

	for(int count=0; count<int(m_BarInfo.dwTotalCount); count++)
	{
		for(int i=0; i<int(m_BarInfo.InfoList[count].dwDataLen); i++)
		{
			if (m_BarInfo.InfoList[count].Data[i] == '$')
				return false;
		} 
	}
	return true;
}

bool CPTBarcodeDecoder::ExtractBarcodeData( PTTOTALBARCODEINFO* pBar, bool IsFirstDecoding )
{
	if( pBar->dwTotalCount<=0 )
	{
		::AfxMessageBox(L"No barcodes were found");
		return false;
	}

	m_BarInfo.dwTotalCount = pBar->dwTotalCount;
	for(int i=0; i<int(pBar->dwTotalCount); i++ )
	{
		if (IsFirstDecoding)
		{
			m_BarInfo.InfoList[i].dwDataLen = pBar->pInfoList[i].dwDataLen;
			memcpy( m_BarInfo.InfoList[i].Data , pBar->pInfoList[i].pData, pBar->pInfoList[i].dwDataLen );
			m_BarInfo.InfoList[i].dwX1 =pBar->pInfoList[i].dwX1;
			m_BarInfo.InfoList[i].dwX2 =pBar->pInfoList[i].dwX2;
			m_BarInfo.InfoList[i].dwX3 =pBar->pInfoList[i].dwX3;
			m_BarInfo.InfoList[i].dwX4 =pBar->pInfoList[i].dwX4;
			m_BarInfo.InfoList[i].dwY1 =pBar->pInfoList[i].dwY1;
			m_BarInfo.InfoList[i].dwY2 =pBar->pInfoList[i].dwY2;
			m_BarInfo.InfoList[i].dwY3 =pBar->pInfoList[i].dwY3;
			m_BarInfo.InfoList[i].dwY4 =pBar->pInfoList[i].dwY4;

		}
		else
		{
			for(int j=0; j<int(pBar->pInfoList[i].dwDataLen); j++)
			{
				if (pBar->pInfoList[i].pData[j] != '$' && m_BarInfo.InfoList[i].Data[j] == '$')
					m_BarInfo.InfoList[i].Data[j] = pBar->pInfoList[i].pData[j];
			} 
		}
	}

	return true;
}