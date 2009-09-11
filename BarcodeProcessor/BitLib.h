////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////

#ifndef __BITLIB_H__
#define __BITLIB_H__

#include "BitPointer.h"

typedef CBitPointer BitPtr;

//copy bit strings from source to destenation with bit count length
CBitPointer bitcpy( CBitPointer p_pbitDestination, 	
						   CBitPointer p_pbitSource, 
							size_t p_cLength);

//compare two bit strings
// result = -1 => less than
// result =  0 => equal
// result = +1 => greater than
int bitcmp(	CBitPointer p_pbitString1, 
				CBitPointer p_pbitString2, 
				size_t p_cLength);

//invert bit string
CBitPointer bitinv(CBitPointer p_bitString, size_t p_cLength);

//reverse bit string order
CBitPointer bitrev(CBitPointer p_pbitString, size_t p_cLength);

//count of bits having value 0 or 1
int bitcnt(CBitPointer p_pbitString, int p_iValue, size_t p_cLength);

//count of bits having value 1
int bitcnt(CBitPointer p_pbitString, size_t p_cLength);

void bitzero(CBitPointer p_pbitString, size_t p_cLength);

#endif