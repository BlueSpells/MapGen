// ***
//  
//  CwmimgvwrDlg: Draws image on Window 
//
//  By Ciro
//
// ***
#include "stdafx.h"
/*#include "wmimgvwr.h"*/
#include "wmimgvwrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWmimgvwrDlg dialog

CWmimgvwrDlg::CWmimgvwrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWmimgvwrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWmimgvwrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   this->m_bmp_loaded = FALSE;
}

void CWmimgvwrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWmimgvwrDlg)
	DDX_Control(pDX, IDC_BTN_APP_EXIT, m_btn_exit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWmimgvwrDlg, CDialog)
	//{{AFX_MSG_MAP(CWmimgvwrDlg)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_APP_EXIT, OnBtnAppExit)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWmimgvwrDlg message handlers

BOOL CWmimgvwrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

   // *** SELECT IMAGE - BEGIN
   if ( !vg_cmd_line.GetLength() ) 
   {
	   CFileDialog dlgOpen(TRUE, TEXT("*.bmp"), NULL, OFN_EXPLORER | OFN_HIDEREADONLY, NULL, this);
	   CString allFilter;
	   CString 	strTitle = _T("Open Image\0");

	   allFilter = _T("Bitmap (*.bmp)");
      allFilter += L'\0';
	   allFilter += _T("*.bmp");
      allFilter += L'\0';

	   allFilter += _T("JPEG (*.jpg)");
      allFilter += L'\0';
	   allFilter += _T("*.jpg");
      allFilter += L'\0';

	   dlgOpen.m_ofn.nMaxCustFilter++;
	   dlgOpen.m_ofn.lpstrFilter = allFilter;

	   dlgOpen.m_ofn.lpstrTitle = strTitle;
	   
	   if (dlgOpen.DoModal() == IDCANCEL)
		   this->OnOK();

	   vg_cmd_line = dlgOpen.GetPathName();
   }
   // *** SELECT IMAGE - END

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen

   // *** FULL SCREEN - BEGIN
	SHINITDLGINFO shidi;
   (void) ::ZeroMemory(&shidi, sizeof(shidi));
   shidi.dwMask  = SHIDIM_FLAGS;
   shidi.dwFlags = SHIDIF_FULLSCREENNOMENUBAR;
   shidi.hDlg    = this->m_hWnd;

   ::SHInitDialog(&shidi);

   this->uf_full_screen(); 
   // *** FULL SCREEN - END

	return TRUE;  
}

void CWmimgvwrDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

   RECT aRect;

   aRect.top = 27;
   aRect.bottom = 47;
   aRect.left = 1;
   aRect.right = 21;

   this->m_btn_exit.MoveWindow(&aRect);  

   if ( this->m_bmp_loaded ) 
   {
      this->m_current_bmp.Detach(); 
      this->m_bmp_loaded = FALSE;
   }
}

void CWmimgvwrDlg::OnPaint() 
{
	CPaintDC dc(this); 

   if ( vg_cmd_line.GetLength() == 0 )
      return;
	
   if ( !this->m_bmp_loaded )
   {
      if ( !this->uf_load_bitmap(dc) )
         return;
   }

   dc.BitBlt( this->m_img_pos.dc_x, 
              this->m_img_pos.dc_y, 
              this->m_img_pos.dc_crop_cx, 
              this->m_img_pos.dc_crop_cy, 
              &this->m_dcMem, 
              this->m_img_pos.bmp_x, 
              this->m_img_pos.bmp_y, 
              SRCCOPY );
}

BOOL CWmimgvwrDlg::uf_calc_bmp_pos(void)
{
   if ( this->m_img_pos.dc_cx < this->m_img_pos.bmp_cx )
   {
      this->m_img_pos.dc_crop_cx = this->m_img_pos.dc_cx;
      this->m_img_pos.bmp_x = (this->m_img_pos.bmp_cx / 2) - (this->m_img_pos.dc_cx / 2); 
      this->m_img_pos.dc_x = 0;
      this->m_img_pos.bmp_scroll_x = TRUE;
   }
   else
   {
      this->m_img_pos.dc_crop_cx = this->m_img_pos.bmp_cx;
      this->m_img_pos.dc_x = (this->m_img_pos.dc_cx / 2) - (this->m_img_pos.bmp_cx / 2); 
      this->m_img_pos.bmp_x = 0;
      this->m_img_pos.bmp_scroll_x = FALSE;
   }

   if ( this->m_img_pos.dc_cy < this->m_img_pos.bmp_cy )
   {
      this->m_img_pos.dc_crop_cy = this->m_img_pos.dc_cy;
      this->m_img_pos.bmp_y = (this->m_img_pos.bmp_cy / 2) - (this->m_img_pos.dc_cy / 2); 
      this->m_img_pos.dc_y = 0;
      this->m_img_pos.bmp_scroll_y = TRUE;
   }
   else
   {
      this->m_img_pos.dc_crop_cy = this->m_img_pos.bmp_cy;
      this->m_img_pos.dc_y = (this->m_img_pos.dc_cy / 2) - (this->m_img_pos.bmp_cy / 2) + vg_wintask_h/2;
      this->m_img_pos.bmp_y = 0;
      this->m_img_pos.bmp_scroll_y = FALSE;
   }

   return TRUE;
}

BOOL CWmimgvwrDlg::uf_load_bitmap(CDC &_dc)
{
   HBITMAP hBmp = NULL;
   RECT cliRect;
   BITMAP hBmpInfo;

   hBmp = ::SHLoadImageFile(vg_cmd_line);
   if ( !hBmp )
   {
      ::MessageBox( this->m_hWnd, L"Can not load image.", L"Image Viewer", MB_ICONERROR );
      this->OnOK(); 
      this->m_bmp_loaded = FALSE;
      return FALSE;
   }

   this->m_current_bmp.Attach(hBmp);  
   this->m_current_bmp.GetBitmap(&hBmpInfo); 

   if ( this->m_dcMem.GetSafeHdc() )
      this->m_dcMem.DeleteDC();

   this->m_dcMem.CreateCompatibleDC(&_dc); 

   this->m_dcMem.SelectObject(&this->m_current_bmp);

   this->GetClientRect(&cliRect);

   memset( &this->m_img_pos, 0x00, sizeof(this->m_img_pos) );

   this->m_img_pos.bmp_cx = hBmpInfo.bmWidth;
   this->m_img_pos.bmp_cy = hBmpInfo.bmHeight;  
   
   this->m_img_pos.dc_cx = cliRect.right;
   this->m_img_pos.dc_cy = cliRect.bottom;  

   this->uf_calc_bmp_pos();

   this->uf_full_screen(FALSE); 

   this->m_bmp_loaded = TRUE;

   return TRUE;
}

BOOL CWmimgvwrDlg::PreTranslateMessage(MSG* pMsg) 
{
   if ( !pMsg )
      return FALSE;

   if ( pMsg->hwnd != this->m_hWnd )
      return FALSE;

   switch ( pMsg->message )
   {
      case WM_LBUTTONUP:
      {
         this->m_btn_exit.ShowWindow(SW_SHOW); 
      }
      break;

      case WM_LBUTTONDOWN:
      {
         this->m_img_pos.stylus_xB = LOWORD(pMsg->lParam);
         this->m_img_pos.stylus_yB = HIWORD(pMsg->lParam);

         this->m_img_pos.stylus_xE = LOWORD(pMsg->lParam);
         this->m_img_pos.stylus_yE = HIWORD(pMsg->lParam);
      }
      break;

      case WM_MOUSEMOVE:
      {
         POINT pt; 
         int wx = 0;
         int wy = 0;
         pt.x = LOWORD(pMsg->lParam); 
         pt.y = HIWORD(pMsg->lParam);

         if ( this->m_img_pos.bmp_scroll_x )
         {
            wx = (pt.x - this->m_img_pos.stylus_xB) * -1;
            
            this->m_img_pos.bmp_x += wx;
            if ( wx < 0 && this->m_img_pos.bmp_x < 0 )
               this->m_img_pos.bmp_x = 0;

            if ( wx > 0 && this->m_img_pos.bmp_cx - this->m_img_pos.bmp_x < this->m_img_pos.dc_crop_cx )
               this->m_img_pos.bmp_x -= wx;

            this->m_img_pos.stylus_xB = this->m_img_pos.stylus_xE;
            this->m_img_pos.stylus_xE = pt.x;
         }

         if ( this->m_img_pos.bmp_scroll_y )
         {
            wy = (pt.y - this->m_img_pos.stylus_yB) * -1;
            
            this->m_img_pos.bmp_y += wy;
            if ( wy < 0 && this->m_img_pos.bmp_y < -vg_wintask_h )
               this->m_img_pos.bmp_y = -vg_wintask_h;

            if ( wy > 0 && this->m_img_pos.bmp_cy - this->m_img_pos.bmp_y < this->m_img_pos.dc_crop_cy )
               this->m_img_pos.bmp_y -= wy;

            this->m_img_pos.stylus_yB = this->m_img_pos.stylus_yE;
            this->m_img_pos.stylus_yE = pt.y;
         }

         if ( this->m_img_pos.bmp_scroll_x || this->m_img_pos.bmp_scroll_y )
         {
            if ( this->m_img_pos.bmp_last_x != this->m_img_pos.bmp_x || 
                 this->m_img_pos.bmp_last_y != this->m_img_pos.bmp_y )
            this->Invalidate(FALSE);
         }

         this->m_img_pos.bmp_last_y = this->m_img_pos.bmp_y;
         this->m_img_pos.bmp_last_x = this->m_img_pos.bmp_x;

         this->m_btn_exit.ShowWindow(SW_HIDE); 
      }  
       break;

      default:
          break;
   }

	return FALSE;
}

void CWmimgvwrDlg::OnBtnAppExit() 
{
	this->OnOK();
}

void CWmimgvwrDlg::OnOK()
{
   //ExitProcess(0); // R.C.
	CDialog::OnOK(); // R.C.
}

HBRUSH CWmimgvwrDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   static HBRUSH hBrush = ::CreateSolidBrush(RGB(225,225,225));

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   pDC->SetBkMode(OPAQUE);
   pDC->SetBkColor(RGB(225,225,225));
   
   hbr = hBrush;

	return hbr;
}
