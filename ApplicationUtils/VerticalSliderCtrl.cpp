#include "StdAfx.h"
#include "VerticalSliderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CVerticalSliderCtrl::CVerticalSliderCtrl() :
m_FlipMinMax(false)
{
}

CVerticalSliderCtrl::~CVerticalSliderCtrl()
{
}

void CVerticalSliderCtrl::SetFlipMinMax(bool Flip)
{
	m_FlipMinMax = Flip;
}

int CVerticalSliderCtrl::GetPos() const
{
	int Pos = CSliderCtrl::GetPos();
	if(m_FlipMinMax)
		return GetRangeMax() - GetRangeMin() - Pos;
	else
		return Pos;
}

void CVerticalSliderCtrl::SetPos(int Pos)
{
	if(m_FlipMinMax)
		CSliderCtrl::SetPos(GetRangeMax() - GetRangeMin() - Pos);
	else
		CSliderCtrl::SetPos(Pos);
}