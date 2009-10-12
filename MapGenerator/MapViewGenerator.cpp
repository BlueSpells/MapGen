#include "StdAfx.h"
#include "MapViewGenerator.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapViewGenerator::CMapViewGenerator() : m_MapImage(NULL)
{
}

CMapViewGenerator::~CMapViewGenerator(void)
{
	if (m_MapImage != NULL)
		delete m_MapImage;
}

void CMapViewGenerator::Init(SViewDimensions ViewDimensions)
{
	m_ViewDimensions = ViewDimensions;
}

void CMapViewGenerator::Init(RECT ViewRect)
{
	m_ViewDimensions.Height = ViewRect.bottom - ViewRect.top;
	m_ViewDimensions.Width  = ViewRect.right - ViewRect.left;
}

bool CMapViewGenerator::LoadImage(const char *FileName)
{
	if (m_MapImage)
		delete m_MapImage;
	m_MapImage = new FCObjImageExtended;

	return m_MapImage->Load(FileName);
}

bool CMapViewGenerator::LoadImage(const FCObjImageExtended &MapImage)
{
	if (!MapImage.IsValidImage())
		return false;

	if (m_MapImage)
		delete m_MapImage;
	m_MapImage = new FCObjImageExtended(MapImage);
	
	return true;
}


FCObjImageExtended *CMapViewGenerator::GenerateView()
{
	FCObjImageExtended *View = new FCObjImageExtended(*m_MapImage);
	View->StretchImage(m_ViewDimensions.Width, m_ViewDimensions.Height);
	
	m_CurrentView.top		= 0;
	m_CurrentView.bottom	= m_MapImage->Height();
	m_CurrentView.left		= 0;
	m_CurrentView.right		= m_MapImage->Width();

	return View;
}

FCObjImageExtended *CMapViewGenerator::ZoomIn(int ZoomDegree/* = 2*/)
{
	FCObjImageExtended *View = new FCObjImageExtended;
	int NewHeight = (m_CurrentView.bottom - m_CurrentView.top) / ZoomDegree;
	int NewWidth  = (m_CurrentView.right - m_CurrentView.left) / ZoomDegree;
	
	m_CurrentView.top		+= NewHeight/2;
	m_CurrentView.bottom	-= NewHeight/2;
	m_CurrentView.left		+= NewWidth/2;
	m_CurrentView.right		-= NewWidth/2;
	
	m_MapImage->GetSubBlock(View, m_CurrentView);
	View->StretchImage(m_ViewDimensions.Width, m_ViewDimensions.Height);

	return View;
}

FCObjImageExtended *CMapViewGenerator::ZoomOut(int ZoomDegree/* = 2*/)
{
	FCObjImageExtended *View = new FCObjImageExtended;
	int NewHeight = (m_CurrentView.bottom - m_CurrentView.top) * ZoomDegree;
	int NewWidth  = (m_CurrentView.right - m_CurrentView.left) * ZoomDegree;

	m_CurrentView.top		-= NewHeight/2;
	m_CurrentView.bottom	+= NewHeight/2;
	m_CurrentView.left		-= NewWidth/2;
	m_CurrentView.right		+= NewWidth/2;

	CorrectView();

	m_MapImage->GetSubBlock(View, m_CurrentView);
	View->StretchImage(m_ViewDimensions.Width, m_ViewDimensions.Height);

	return View;
}


FCObjImageExtended *CMapViewGenerator::ScrollDown(int VerticalCoordinate)
{
	FCObjImageExtended *View = new FCObjImageExtended;

	int CurrentVerticalLength = m_CurrentView.bottom - m_CurrentView.top;
	m_CurrentView.top = VerticalCoordinate - CurrentVerticalLength/2;
	m_CurrentView.bottom = VerticalCoordinate + CurrentVerticalLength/2;

	CorrectView();

	m_MapImage->GetSubBlock(View, m_CurrentView);
	View->StretchImage(m_ViewDimensions.Width, m_ViewDimensions.Height);

	return View;
}

FCObjImageExtended *CMapViewGenerator::ScrollRight(int HorizontalCoordinate)
{
	FCObjImageExtended *View = new FCObjImageExtended;

	int CurrentHorizontalLength = m_CurrentView.right - m_CurrentView.left;
	m_CurrentView.left = HorizontalCoordinate - CurrentHorizontalLength/2 ;
	m_CurrentView.right = HorizontalCoordinate + CurrentHorizontalLength/2;

	CorrectView();

	m_MapImage->GetSubBlock(View, m_CurrentView);
	View->StretchImage(m_ViewDimensions.Width, m_ViewDimensions.Height);

	return View;
}

void CMapViewGenerator::CorrectView()
{
	if (m_CurrentView.top < 0)
	{
		int Exceeding = 0 - m_CurrentView.top;
		if (m_CurrentView.bottom + Exceeding < m_MapImage->Height())
		{
			m_CurrentView.top	 += Exceeding;		
			m_CurrentView.bottom += Exceeding;
		}
		else
		{
			m_CurrentView.top	 = 0;		
			m_CurrentView.bottom = m_MapImage->Height();
		}
	}

	if (m_CurrentView.bottom > m_MapImage->Height())
	{
		int Exceeding = m_CurrentView.bottom - m_MapImage->Height();
		if (m_CurrentView.top - Exceeding > 0)
		{
			m_CurrentView.top	 -= Exceeding;		
			m_CurrentView.bottom -= Exceeding;
		}
		else
		{
			m_CurrentView.top	 = 0;		
			m_CurrentView.bottom = m_MapImage->Height();
		}
	}

	if (m_CurrentView.left < 0)
	{
		int Exceeding = 0 - m_CurrentView.left;
		if (m_CurrentView.right + Exceeding < m_MapImage->Width())
		{
			m_CurrentView.left	+= Exceeding;		
			m_CurrentView.right += Exceeding;
		}
		else
		{
			m_CurrentView.left	= 0;		
			m_CurrentView.right = m_MapImage->Width();
		}
	}

	if (m_CurrentView.right > m_MapImage->Width())
	{
		int Exceeding = m_CurrentView.right - m_MapImage->Width();
		if (m_CurrentView.left - Exceeding > 0)
		{
			m_CurrentView.left	-= Exceeding;		
			m_CurrentView.right -= Exceeding;
		}
		else
		{
			m_CurrentView.left	 = 0;		
			m_CurrentView.right = m_MapImage->Width();
		}
	}
}


int CMapViewGenerator::GetMapHeight()
{
	return m_MapImage->Height();
}

int CMapViewGenerator::GetMapWidth()
{
	return m_MapImage->Width();
}

bool CMapViewGenerator::IsInZoom()
{
	if (m_CurrentView.top > 0)
		return true;
	if (m_CurrentView.bottom < m_MapImage->Height())
		return true;
	if (m_CurrentView.left > 0)
		return true;
	if (m_CurrentView.right > m_MapImage->Width())
		return true;

	return false;
}

RECT CMapViewGenerator::GetCurrentView()
{
	return m_CurrentView;
}