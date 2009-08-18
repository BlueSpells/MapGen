////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////


#ifndef __BITPOINTER_H__
#define __BITPOINTER_H__

#include "Bit.h"

#include <stddef.h>

class CBitPointer  
{
public:
	CBitPointer();
	CBitPointer(const CBitPointer& p_pbit);
	CBitPointer(const CBit& p_bit);
	CBitPointer(const CBit* p_pbit);
	CBitPointer(void* p_pvAddress, int p_nBitOffset = 0);
	virtual ~CBitPointer();

	void Set(void* p_pvAddress, int p_nBitOffset = 0);
	void Set(const CBit& p_bit);

	void* GetAddress() const;
	void* GetByteAddress() const;

	int   GetBitOffset() const;
	int   GetByteBitOffset() const;

	CBitPointer& New(size_t p_sizBitsSize);
	void Delete();

	CBitPointer& operator=(const CBitPointer& p_pbit);
	CBitPointer& operator=(void* p_pvAddress);
	CBitPointer& operator=(CBit* p_pbit);

	operator void*() const;

	CBit operator*();

	CBit operator[](int p_nBitIndex);

	CBitPointer& operator++();
	CBitPointer& operator--();
	CBitPointer operator++(int);
	CBitPointer operator--(int);

	CBitPointer operator+(int p_nBitOffset) const;
	CBitPointer operator-(int p_nBitOffset) const;

	ptrdiff_t operator-(const CBitPointer& p_pbit) const;
	ptrdiff_t operator-(const void* p_pvAddress) const;
	ptrdiff_t operator-(const CBit* p_pbit) const;

	CBitPointer& operator+=(int p_nBitOffset);
	CBitPointer& operator-=(int p_nBitOffset);

	bool operator==(const CBitPointer& p_pbit) const;
	bool operator!=(const CBitPointer& p_pbit) const;
	bool operator==(const void* p_pvAddress) const;
	bool operator!=(const void* p_pvAddress) const;
	bool operator==(const CBit* p_pbit) const;
	bool operator!=(const CBit* p_pbit) const;
/*
   bool operator>(const void* p_pvAddress) const;
   bool operator<(const void* p_pvAddress) const;
   bool operator>=(const void* p_pvAddress) const;
   bool operator<=(const void* p_pvAddress) const;
*/
   bool operator>(const CBitPointer& p_pbit) const;
   bool operator<(const CBitPointer& p_pbit) const;
   bool operator>=(const CBitPointer& p_pbit) const;
   bool operator<=(const CBitPointer& p_pbit) const;

	typedef CBit::Byte Byte;
	typedef CBit::Word Word;

protected:
	Word* m_puAddress;
	int	m_nBitOffset;
	Word	m_uBitMask;
};

#endif // #ifndef __BITPOINTER_H__
