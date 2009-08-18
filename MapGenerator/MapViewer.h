#pragma once

#include "MapViewGenerator.h"

class CMapViewer
{
public:
	CMapViewer(CWnd *OwnerWindow);
	~CMapViewer(void);

	bool ShowMap(RECT ViewRect, const FCObjImageExtended &MapImage);
	void ZoomIn(int ZoomDegree = 2);
	void ZoomOut(int ZoomDegree = 2);
	void ScrollDown(int VerticalCoordinate);
	void ScrollRight(int HorizontalCoordinate);

	int GetMapHeight()		{return m_MapViewGenerator.GetMapHeight();}
	int GetMapWidth()		{return m_MapViewGenerator.GetMapWidth();}
	bool IsInZoom()			{return m_MapViewGenerator.IsInZoom();}
	RECT GetCurrentView()	{return m_MapViewGenerator.GetCurrentView();}

private:
	CMapViewGenerator m_MapViewGenerator;
	CStatic m_Viewer;
	CWnd *m_OwnerWindow;
	RECT m_ViewRect;
};
