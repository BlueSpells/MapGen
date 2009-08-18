#pragma once

#include "MapGenerator.h"

struct SViewDimensions
{
	int Width;
	int Height;
};

class CMapViewGenerator
{
public:
	CMapViewGenerator(void);
	~CMapViewGenerator(void);

	// Note: the caller must delete the returned FcoImageObject;
	void Init(SViewDimensions ViewDimensions);
	void Init(RECT ViewRect);

	bool LoadImage(const char *FileName);
	bool LoadImage(const FCObjImageExtended &MapImage);

	FCObjImageExtended *GenerateView();
	FCObjImageExtended *ZoomIn(int ZoomDegree = 2);
	FCObjImageExtended *ZoomOut(int ZoomDegree = 2);
	FCObjImageExtended *ScrollDown(int VerticalCoordinate);
	FCObjImageExtended *ScrollRight(int HorizontalCoordinate);
	
	int GetMapHeight();
	int GetMapWidth();
	bool IsInZoom();
	RECT GetCurrentView();

private:
	void CorrectView();

	FCObjImageExtended *m_MapImage;
	SViewDimensions m_ViewDimensions;
	RECT			m_CurrentView;
};
