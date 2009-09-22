#pragma once
#include "afxcmn.h"

class CVerticalSliderCtrl :
	public CSliderCtrl
{
public:
	CVerticalSliderCtrl();
	~CVerticalSliderCtrl();

	void SetFlipMinMax(bool Flip);
	int  GetPos() const;
	void SetPos(int nPos);

private:
	bool m_FlipMinMax;
};
