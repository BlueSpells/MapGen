#pragma once
#include "IItemDecoder.h"

class CParkingDecoder :
	public IItemDecoder
{
public:
	CParkingDecoder(int Version);
	~CParkingDecoder(void);

	virtual bool	IsOfThisType(CBitPointer *BitPointer) = 0;
	virtual void	Decode(CBitPointer *BitPointer, void *Context, int &UsedBits) = 0;
};
