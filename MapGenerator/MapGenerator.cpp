#include "StdAfx.h"
#include "MapGenerator.h"

CMapGenerator::CMapGenerator(void) : m_NumberOfObjects(0), m_MapImage(NULL)
{
}

CMapGenerator::~CMapGenerator(void)
{
	if (m_MapImage != NULL)
		delete m_MapImage;
}

void CMapGenerator::LoadBitmap(CString FileName)
{
	m_MapImage = new FCObjImageExtended;
	m_MapImage->Load(FileName);
	m_NumberOfObjects = 1;
}

void CMapGenerator::AddObject(EMapObjectType Object, SLocation Location, int Quantity, EDuplicationDirection Duplicate)
{
	FCObjImageExtended *ObjectImage = PrepareObjectImage(Object, Quantity, Duplicate);

	if (m_NumberOfObjects == 0)
	{
		AddFirstObject(ObjectImage, Location, Quantity);
		return; // DO NOT delete ObjectImage !!
	}

	int RightExpand = max(Location.x + ObjectImage->Width(), m_MapImage->Width()) - m_MapImage->Width();
	int DownExpand  = max(Location.y + ObjectImage->Height(), m_MapImage->Height()) - m_MapImage->Height();

	if (RightExpand > 0 || DownExpand > 0)
		m_MapImage->ExpandFrame(0, 0, RightExpand, DownExpand);

	m_MapImage->CoverBlock(*ObjectImage, Location.x, Location.y);

	m_NumberOfObjects += Quantity;
	delete ObjectImage;

	m_MapImage->Save("AutoSave.bmp");
}


FCObjImageExtended *CMapGenerator::PrepareObjectImage(EMapObjectType Object, int Quantity, EDuplicationDirection Duplicate)
{
	FCObjImageExtended *ObjectImage = new FCObjImageExtended;
	const char *BitmapFileName = GetBitmapFileNameForObject(Object);
	ObjectImage->Load(BitmapFileName);
	int TheHeight = ObjectImage->Height();

	if (Quantity > 1)
	{
		FCObjImageExtended DuplicateImage;
		DuplicateImage.Load(BitmapFileName);

		if (Duplicate == DUPLICATE_DOWN)
		{
			ObjectImage->ExpandFrame(0, 0, 0, DuplicateImage.Height() * (Quantity - 1), false);
		}
		else // Duplicate == DUPLICATE_RIGHT
		{
			ObjectImage->ExpandFrame(0, 0, DuplicateImage.Width() * (Quantity - 1), 0, false);
		}

		for (int i = 1; i < Quantity; i++)
		{
			if (Duplicate == DUPLICATE_DOWN)
			{
				ObjectImage->CoverBlock(DuplicateImage, 0, DuplicateImage.Height() * i);
			}
			else // Duplicate == DUPLICATE_RIGHT
			{
				ObjectImage->CoverBlock(DuplicateImage, DuplicateImage.Width() * i, 0);
			}
		}
	}

	return ObjectImage;
}


void CMapGenerator::AddFirstObject(FCObjImageExtended *ObjectImage, SLocation Location, int Quantity)
{
	m_MapImage = ObjectImage;
	m_MapImage->ExpandFrame(Location.x, Location.y, 0, 0);

	m_NumberOfObjects += Quantity;
	m_MapImage->Save("AutoSave.bmp");
}


const char *CMapGenerator::GetBitmapFileNameForObject(EMapObjectType Object)
{
	switch (Object)
	{
	case NORMAL_PARKING:
		return "..\\MapGenerator\\ParkingImages\\Regular parking 0deg.bmp";
		break;
	case PARALLEL_PARKING:
		return "..\\MapGenerator\\ParkingImages\\Parallel parking 0deg.bmp";
		break;
	case ELEVATOR:
		return "..\\MapGenerator\\ParkingImages\\Elevator.bmp";
		break;
	default:
		ASSERT(false);
		break;
	};

	return "";
}

void CMapGenerator::SaveToBitmap(CString FileName)
{
	m_MapImage->Save(FileName);
}

void FCObjImageExtended::ExpandFrame (int iLeft, int iTop, int iRight, int iBottom, bool FillBackGround /*= true*/, RGBQUAD BackGroundColor)
{
	if ((ColorBits() < 8) || (iLeft < 0) || (iTop < 0) || (iRight < 0) || (iBottom < 0))
	{
		assert(false) ; return ;
	}
	if ((iLeft == 0) && (iTop == 0) && (iRight == 0) && (iBottom == 0))
		return ;

	// backup image then create expanded image
	const FCObjImageExtended     imgOld(*this) ;
	if (!Create (imgOld.Width()+iLeft+iRight, imgOld.Height()+iTop+iBottom, imgOld.ColorBits()))
	{
		assert(false) ; return ;
	}

	if (FillBackGround)
	{
		RECT rc = {0,0,Width(),Height()};
		__FillImageRect (*this, rc, &BackGroundColor);
	}

	// adjust image's position
	SetGraphObjPos (imgOld.GetGraphObjPos().x - iLeft, imgOld.GetGraphObjPos().y - iTop) ;

	// duplicate source image
	CoverBlock (imgOld, iLeft, iTop) ;
}

void FCObjImageExtended::StretchImage(int NewWidth, int NewHeight)
{
	//Stretch(NewWidth, NewHeight);
	Stretch_Smooth(NewWidth, NewHeight);
}

void CMapGenerator::SquareImage()
{
	int SideLengthExtension = m_MapImage->Width() - m_MapImage->Height();
	if (SideLengthExtension > 0)
	{
		m_MapImage->ExpandFrame(0, 0, 0, SideLengthExtension);
	}
	if (SideLengthExtension < 0)
	{
		m_MapImage->ExpandFrame(0, 0, -SideLengthExtension, 0);
	}
	
}

FCObjImageExtended *CMapGenerator::GetMapObject() const
{
	return m_MapImage;
}