#pragma once

// ToDo: in the future perhaps solve some the warnings in ImageStone (if it becomes a real integral part of the code)
#pragma warning( push, 3 )
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4267)
#pragma warning(disable:4311)
#pragma warning(disable:4189)
#include "ImageStone/ImageStone.h"
#pragma warning( pop )

enum EMapObjectType
{
	NORMAL_PARKING,
	PARALLEL_PARKING,
	ELEVATOR
};

struct SLocation
{
	int x;
	int y;
};

enum EDuplicationDirection
{
	DUPLICATE_DOWN,
	DUPLICATE_RIGHT
};


class FCObjImageExtended : public FCObjImage
{
public:
	void ExpandFrame (int iLeft, int iTop, int iRight, int iBottom, bool FillBackGround = true, RGBQUAD BackGroundColor = PCL_RGBA(79,79,79));
	void StretchImage(int NewWidth, int NewHeight);
};


class CMapGenerator
{
public:
	CMapGenerator(void);
	~CMapGenerator(void);

	void LoadBitmap(CString FileName);
	void AddObject(EMapObjectType Object, SLocation Location, int Quantity, EDuplicationDirection Duplicate);
	void SaveToBitmap(CString FileName);
	FCObjImageExtended *GetMapObject() const;
	void SquareImage();

private:
	FCObjImageExtended *PrepareObjectImage(EMapObjectType Object, int Quantity, EDuplicationDirection Duplicate);
	void AddFirstObject(FCObjImageExtended *ObjectImage, SLocation Location, int Quantity);
	const char *GetBitmapFileNameForObject(EMapObjectType Object);

	int m_NumberOfObjects;
	FCObjImageExtended *m_MapImage;
};
