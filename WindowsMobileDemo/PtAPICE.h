//  Copyright(C) 2002-2007, PartiTek Inc. All rights reserved.
//
//  Title:  PtAPI.h for WinCE/WinMobile
//======================================================================================
//
//  Description: Include file for the  PartiTek Inc. API and definitions.
//               The modules included are:
//
//  PtImageRW          Image file reading/writing.
//  PtQRDecdoe         QR Code symbol reading.
//  PtDMDecode         Data Matrix symbol reading.  
  
//======================================================================================
//
//  Version:    2.0
//
//  Date:       07 November 2007
// ======================================================================================

#ifndef  _PT_API_H
#define  _PT_API_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#pragma pack(8)  // 8 bytes alignment 

#if !defined( PTAPI )
#define PTAPI  __stdcall
#endif


// * **********************************************************
// *  Image file reading/writing APIs and definitions.
// * **********************************************************
#if !defined( PT_IMAGERW_API )
#define PT_IMAGERW_API  __declspec( dllimport )
#endif


/*----------------------------------------------------------------------------------------+
|                                    DEFINES SECTION                                      |
+----------------------------------------------------------------------------------------*/
/* Status of an operation */
#define  PT_IMAGERW_FAIL                   0x00000000 //An error occurred in an operation.
#define  PT_IMAGERW_SUCCESS                0x00000001 //An operation is successful.
#define  PT_IMAGERW_ALLOC_ERROR            0x00000100 //Error while allocating memory.
#define  PT_IMAGERW_FORMAT_UNSUPPORTED     0x00000101 //The format of image is unsupported. 

/*----------------------------------------------------------------------------------------+
|                                  STRUCTURES SECTION                                     |
|                        Please refer to the SDK manul for more detail.                   |
+-----------------------------------------------------------------------------------------*/
//The PTIMAGE structure contains the image information.
typedef struct tagPTIMAGE 
{
    DWORD            dwWidth;       //The width of the image in pixels.
    DWORD            dwHeight;      //The height of the image in pixels.
    unsigned char*   pBits;         //Pointer to the image data.
    unsigned char*   pPalette;      //Pointer to the palette data (RGBQUAD)for 1,4,8 bits image.
    WORD             wBitsPerPixel; //Number of bits per pixel.
}PTIMAGE, *pPTIMAGE;

/*---------------------------------------------------------------------------------------+
|                                  FUNCTIONS SECTION                                     |
|                        Please refer to the SDK manul for more detail.                  |
+---------------------------------------------------------------------------------------*/
/*Initialize the PTIMAGE structure.*/
PT_IMAGERW_API  void  PTAPI  PtInitImage
(
    PTIMAGE* pImage           //Pointer to a PTIMAGE structure.
);


/*Load an image from a file*/
PT_IMAGERW_API  int   PTAPI  PtLoadImage
( 
    const TCHAR* FileName,     //Name of the file.
    PTIMAGE*     pImage,       //Pointer to a PTIMAGE structure.
    DWORD        dwFrameIndex  //The index of the frame wanted to load(start from 0).Some image files contain multi-frames, such as tiff file.
);


/*Save an image to a file*/
PT_IMAGERW_API  int   PTAPI  PtSaveImage
( 
    const TCHAR* FileName,     //Name of the file
    PTIMAGE*     pImage        //Pointer to a PTIMAGE structure.
);


/*Display an image with the hDC.*/
PT_IMAGERW_API  void  PTAPI  PtShowImage
(
    PTIMAGE*  pImage,         //Pointer to a PTIMAGE structure.
    HDC       hDc,            //Contains a handle of a window's device context to display the image. 
    int       StartX,         //The x-coordinate in pixels, of the upper-left corner of the destination rectangle.
    int       StartY,         //The y-coordinate in pixels, of the upper-left corner of the destination rectangle.
    float     scale           //Used to stretch or compress the image.
);

/*Allocate the memory for data and palette*/
PT_IMAGERW_API  int   PTAPI  PtCreateImage
(
    PTIMAGE* pImage,          //Pointer to a PTIMAGE structure.
    DWORD    ImageSize,       //The data dimension in byte.
    DWORD    PaletteSize      //The palette dimension in byte.
);


/*Free the memory allocated by the engine in PtLoadImage or PtCreateImage function.*/
PT_IMAGERW_API  void  PTAPI  PtFreeImage
(
    PTIMAGE* pImage           //Pointer to a PTIMAGE structure.
);


/*Get the frames of an image file. Some image files contain multi-frames, such as tiff file.*/
PT_IMAGERW_API  int   PTAPI  PtGetImageFrames
( 
    const TCHAR* FileName     //Name of the file.
);


// * **********************************************************
// *  QR Code symbol reading APIs and definitions
// * **********************************************************
#if !defined( PT_QRDECODE_API )
#define PT_QRDECODE_API __declspec( dllimport )
#endif

/*----------------------------------------------------------------------------------------+
|                                  STRUCTURES SECTION                                     |
|                        Please refer to the SDK manul for more detail.                   |
+-----------------------------------------------------------------------------------------*/
/*The PTDECODEPARA structure is used to decide parameter when decoding barcodes from an image.*/
typedef struct tagPTDECODEPARA
{
    DWORD       dwStartX;     //The start X-coordinate in pixels of the search window in the image to decode the symbol.
    DWORD       dwStartY;     //The start Y-coordinate in pixels of the search window in the image to decode the symbol.
    DWORD       dwEndX;       //The end X-coordinate in pixels of the search window in the image to decode the symbol.
    DWORD       dwEndY;       //The end Y-coordinate in pixels of the search window in the image to decode the symbol.
    DWORD       dwMaxCount;   //The maximal number of symbols to be searched. If it's set to 0 then search the all symbols.
} PTDECODEPARA, *pPTDECODEPARA 
;


/*The PTBARCODEINFO structure contains a barcode information after decoding*/
typedef struct tagPTBARCODEINFO
{
    DWORD      dwX1, dwY1;    //Four corners' coordinates in pixels of the barcode. 
    DWORD      dwX2, dwY2;
    DWORD      dwX3, dwY3;
    DWORD      dwX4, dwY4;
    BYTE*      pData;         //Pointer to the buffer that contains the barcode's data.
    DWORD      dwDataLen;     //The barcode data's length in bytes.
}PTBARCODEINFO, *pPTBARCODEINFO
;


/*The PTTOTALBARCODEINFO structure contains all barcodes' information after decoding*/
typedef struct tagPTTOTALBARCODEINFO
{
    PTBARCODEINFO*  pInfoList;    //Pointer to the start address of the list of barcodes' info.  
    DWORD           dwTotalCount; //The number of barcode that have been decoded.
}PTTOTALBARCODEINFO, *pPTOTALBARCODEINFO
;


/*--------------------------------------------------------------------------------------+
|                                    DEFINES SECTION                                    |
+--------------------------------------------------------------------------------------*/
/* Status of an operation */
#define PT_QRDECODE_FAIL                0x00000000//An error occurred in an operation.
#define PT_QRDECODE_SUCCESS             0x00000001//An operation is successful.
#define PT_QRDECODE_ALLOC_ERROR         0x00000300//Error while allocating the memory.
#define PT_QRDECODE_IMAGE_INVALID       0x00000301//The image to be decode is invalid. 
#define PT_QRDECODE_PARAMETERS_INVALID  0x00000302//The parameters input to a function are invalid.
 
/*--------------------------------------------------------------------------------------+
|                                  FUNCTIONS SECTION                                    |
|                        Please refer to the SDK manul for more detail.                 |
+--------------------------------------------------------------------------------------*/
/*Verify the licnese key for the engine.*/
PT_QRDECODE_API  int   PTAPI PtQRDecodeRegister
( 
    char* pKeyStr                  //Pointer to the string of license key.
);


/*Initialize the PTTOTALBARCODEINFO structure.*/
PT_QRDECODE_API  void  PTAPI  PtQRDecodeInit
(
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Decode QR Code barcode from an image.*/
PT_QRDECODE_API  int  PTAPI  PtQRDecode
(
    PTIMAGE*            pImage,    //Pointer to a PTIMAGE structure.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Decode QR Code barcode from an image file. The function supports multiframes image, such as tiff*/
PT_QRDECODE_API  int  PTAPI  PtQRDecodeFromFile
( 
    const TCHAR*         FileName,  //Name of the image file.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Decode  QR Code barcode from a device dependent bitmap.*/
PT_QRDECODE_API  int  PTAPI  PtQRDecodeFromBitmap
( 
    HBITMAP             hBitmap,   //Handle to the bitmap.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Free the memory allocated in the function PtQRDecode.*/
PT_QRDECODE_API  void  PTAPI  PtQRDecodeFree
( 
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


// * **********************************************************
// *  Data Matrix symbol reading APIs and definitions
// * **********************************************************
#if !defined( PT_DMDECODE_API )
#define PT_DMDECODE_API __declspec( dllimport )
#endif

/*--------------------------------------------------------------------------------------+
|                                    DEFINES SECTION                                    |
+--------------------------------------------------------------------------------------*/
/* Status of an operation */
#define PT_DMDECODE_FAIL                0x00000000//An error occurred in an operation.
#define PT_DMDECODE_SUCCESS             0x00000001//An operation is successful.
#define PT_DMDECODE_ALLOC_ERROR         0x00000300//Error while allocating the memory.
#define PT_DMDECODE_IMAGE_INVALID       0x00000301//The image to be decode is invalid. 
#define PT_DMDECODE_PARAMETERS_INVALID  0x00000302//The parameters input to a function are invalid.
 
/*--------------------------------------------------------------------------------------+
|                                  FUNCTIONS SECTION                                    |
|                        Please refer to the SDK manul for more detail.                 |
+--------------------------------------------------------------------------------------*/
/*Verify the licnese key for the engine.*/
PT_DMDECODE_API  int   PTAPI  PtDMDecodeRegister
( 
    char* pKeyStr                  //Pointer to the string of license key.
);


/*Initialize the PTTOTALBARCODEINFO structure.*/
PT_DMDECODE_API  void  PTAPI  PtDMDecodeInit
(
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Decode DM Code barcode from an image.*/
PT_DMDECODE_API  int  PTAPI  PtDMDecode
(
    PTIMAGE*            pImage,    //Pointer to a PTIMAGE structure.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


/*Decode Data Matrix symbol from an image file. The function supports multiframes image, such as tiff*/
PT_DMDECODE_API  int  PTAPI  PtDMDecodeFromFile
( 
    const TCHAR*         FileName,  //Name of the image file.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);

/*Decode Data Matrix symbol from a device dependent bitmap.*/
PT_DMDECODE_API  int  PTAPI  PtDMDecodeFromBitmap
( 
    HBITMAP             hBitmap,   //Handle to the bitmap.
    PTDECODEPARA*       pPara,     //Pointer to a PTDECODEPARA structure.
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);

/*Free the memory allocated in the function PtDMDecode.*/
PT_DMDECODE_API  void  PTAPI  PtDMDecodeFree
( 
    PTTOTALBARCODEINFO* pInfo      //Pointer to a PTTOTALBARCODEINFO structure.
);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
