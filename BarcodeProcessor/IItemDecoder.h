#pragma once
#include "BitPointer.h"

class IItemDecoder
{
public:
	IItemDecoder(int Version) : m_Version(Version) {}
	~IItemDecoder(void) {}

	virtual bool	IsOfThisType(CBitPointer *BitPointer) = 0;
	virtual void	Decode(CBitPointer *BitPointer, void *Context, int &UsedBits) = 0;

	int m_Version;
};
