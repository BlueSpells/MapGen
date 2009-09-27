////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////

#include "stdafx.h"
#include "BitLib.h"


void bitzero(CBitPointer p_pbitString, size_t p_cLength)
{
	CBitPointer pbitDest = p_pbitString;
	while(p_cLength--)
	{
		*pbitDest++ = 0;
	}
}

CBitPointer bitcpy(	CBitPointer p_pbitDestination, 	
						   CBitPointer p_pbitSource, 
							size_t p_cLength)
{
	//assert(p_cLength >= 0)
	CBitPointer pbitDest = p_pbitDestination;
	while(p_cLength--)
	{
		*pbitDest++ = *p_pbitSource++;
	}
	return p_pbitDestination;
}

int bitcmp(	CBitPointer p_pbitString1, 
				CBitPointer p_pbitString2, 
				size_t p_cLength)
{
	//assert(p_cLength >= 0);
	int ret = 0;
	while( p_cLength-- && !ret)
	{
		ret = (*p_pbitString1++ - *p_pbitString2++);
	}
	return ret;
}

CBitPointer bitinv(CBitPointer p_bitString, size_t p_cLength)
{
	//assert(p_cLength >= 0)
	CBitPointer pbitInvert = p_bitString;
	
	while(p_cLength--)
	{
		*pbitInvert++ = !(*pbitInvert);
	}
	return p_bitString;
}

CBitPointer bitrev(CBitPointer p_pbitString, size_t p_cLength)
{
	//assert(p_cLength >= 0);
	CBitPointer pbitForward = p_pbitString;
	CBitPointer pbitReverse = p_pbitString + (int)p_cLength -1;
	size_t cHalfLength = p_cLength / 2;
	int value;
	while(cHalfLength--)
	{
		value = *pbitForward;
		*pbitForward++ = *pbitReverse;
		*pbitReverse-- = value;
	}
	return p_pbitString;
}

int bitcnt(CBitPointer p_pbitString, int p_iValue, size_t p_cLength)
{
	//assert(p_cLength >= 0);
	int cCount = 0;

	p_iValue = (p_iValue? 1 : 0);
	while(p_cLength--)
	{
		cCount += ( (*p_pbitString++ == p_iValue) ? 1 : 0 );
	}
	return cCount;
}

int bitcnt(CBitPointer p_pbitString, size_t p_cLength)
{
	return bitcnt(p_pbitString, 1, p_cLength);
}

std::string BinaryString(CBitPointer p_pbitString, size_t p_cLength)
{
	std::string retval;

	for (size_t i = 0; i < p_cLength; i++)
		retval += (p_pbitString[i] == 0 ? '0' : '1');

	return retval;
}