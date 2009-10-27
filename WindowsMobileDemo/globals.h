// ***
//  
//  glogal.h: Global definitions 
//
//  By Ciro
//
// ***

#pragma once

/*#include "wmimgvwr.h"*/

#define IV_WINTASK_HEIGHT  26

typedef struct _IVIMG_DATA_
{
   int dc_x;
   int dc_y;
   int dc_cx;
   int dc_cy;

   int  dc_crop_cx;
   int  dc_crop_cy;

   int  bmp_x;
   int  bmp_y;

   int  bmp_last_x;
   int  bmp_last_y;

   int  bmp_cx;
   int  bmp_cy;

   BOOL bmp_scroll_x;
   BOOL bmp_scroll_y;

   int  stylus_xB;
   int stylus_yB;

   int stylus_xE;
   int stylus_yE;

} IVIMGDATA;

/*extern CWmimgvwrApp vg_ceImgVwr;*/
extern CString      vg_cmd_line;
extern int          vg_wintask_h;