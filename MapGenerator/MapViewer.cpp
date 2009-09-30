#include "StdAfx.h"
#include "MapViewer.h"

CMapViewer::CMapViewer(CWnd *OwnerWindow) : m_OwnerWindow(OwnerWindow)
{
}

CMapViewer::~CMapViewer(void)
{
}


bool CMapViewer::ShowMap(RECT ViewRect, const FCObjImageExtended &MapImage)
{
	m_MapViewGenerator.Init(ViewRect);
	m_ViewRect = ViewRect;

	if (!m_MapViewGenerator.LoadImage(MapImage))
		return false;

	FCObjImageExtended *GeneratedView  = m_MapViewGenerator.GenerateView();

	m_Viewer.DestroyWindow();

	m_Viewer.Create(("my static"), /*WS_CHILD|*/WS_VISIBLE|SS_BITMAP, ViewRect, m_OwnerWindow);
	m_Viewer.SetBitmap(FCWin32::CreateDDBHandle (*GeneratedView));

	delete GeneratedView;
	return true;
}


void CMapViewer::ZoomIn(int ZoomDegree/* = 2*/)
{
	FCObjImageExtended *GeneratedView  = m_MapViewGenerator.ZoomIn(ZoomDegree);

	m_Viewer.DestroyWindow();

	m_Viewer.Create(("my static"), /*WS_CHILD|*/WS_VISIBLE|SS_BITMAP, m_ViewRect, m_OwnerWindow);
	m_Viewer.SetBitmap(FCWin32::CreateDDBHandle (*GeneratedView));

	delete GeneratedView;
}


void CMapViewer::ZoomOut(int ZoomDegree/* = 2*/)
{
	FCObjImageExtended *GeneratedView  = m_MapViewGenerator.ZoomOut(ZoomDegree);

	m_Viewer.DestroyWindow();

	m_Viewer.Create(("my static"), /*WS_CHILD|*/WS_VISIBLE|SS_BITMAP, m_ViewRect, m_OwnerWindow);
	m_Viewer.SetBitmap(FCWin32::CreateDDBHandle (*GeneratedView));

	delete GeneratedView;
}

void CMapViewer::ScrollDown(int VerticalCoordinate)
{
	FCObjImageExtended *GeneratedView  = m_MapViewGenerator.ScrollDown(VerticalCoordinate);

	m_Viewer.DestroyWindow();

	m_Viewer.Create(("my static"), /*WS_CHILD|*/WS_VISIBLE|SS_BITMAP, m_ViewRect, m_OwnerWindow);
	m_Viewer.SetBitmap(FCWin32::CreateDDBHandle (*GeneratedView));

	delete GeneratedView;
}

void CMapViewer::ScrollRight(int HorizontalCoordinate)
{
	FCObjImageExtended *GeneratedView  = m_MapViewGenerator.ScrollRight(HorizontalCoordinate);

	m_Viewer.DestroyWindow();

	m_Viewer.Create(("my static"), /*WS_CHILD|*/WS_VISIBLE|SS_BITMAP, m_ViewRect, m_OwnerWindow);
	m_Viewer.SetBitmap(FCWin32::CreateDDBHandle (*GeneratedView));

	delete GeneratedView;
}