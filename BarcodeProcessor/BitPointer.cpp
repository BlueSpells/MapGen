////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////


#include "stdafx.h"
#include "BitPointer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBitPointer::CBitPointer()
{
	m_puAddress = NULL;
	m_nBitOffset = 0;
	m_uBitMask = 1;
}

CBitPointer::CBitPointer(const CBitPointer& p_pbit)
{
	m_puAddress = p_pbit.m_puAddress;
	m_nBitOffset = p_pbit.m_nBitOffset;
	m_uBitMask = p_pbit.m_uBitMask;
}

CBitPointer::CBitPointer(const CBit& p_bit)
{
	Set(p_bit);
}

CBitPointer::CBitPointer(const CBit* p_pbit)
{
	Set(*p_pbit);
}

CBitPointer::CBitPointer(void* p_pvAddress, int p_nBitOffset)
{
	Set(p_pvAddress, p_nBitOffset);
}

CBitPointer::~CBitPointer()
{

}

#ifdef LOW_SPEED
void CBitPointer::Set(void* p_pvAddress, int p_nBitOffset)
{
	m_puAddress = (unsigned*)p_pvAddress + p_nBitOffset / CBit::s_nDirectlyAccessedBits;
	m_nBitOffset = p_nBitOffset % CBit::s_nDirectlyAccessedBits;
	
	if (m_nBitOffset < 0)
	{
		m_nBitOffset += CBit::s_nDirectlyAccessedBits;
		m_puAddress--;
	}
	m_uBitMask = (1 << m_nBitOffset);
}
#else
//faster version
void CBitPointer::Set(void* p_pvAddress, int p_nBitOffset)
{
	//setup standard ByteAddress+offset
	//offset range [0,7]
	Byte *pByte = ((Byte*)p_pvAddress) + p_nBitOffset / 8;
	m_nBitOffset = p_nBitOffset % 8;
	if (m_nBitOffset < 0)
	{
		m_nBitOffset += 8;
		pByte--;
	}
	//setup standard WordAddress+offset
	//offset range [0,31]
	//word address should be in word boundary
	//meaning word address least nibble = 0x0, 0x4, 0x8 or 0xC
	int nByteOffset = (Word)pByte % sizeof(Word);
	m_puAddress = (Word*)(pByte - nByteOffset);
	m_nBitOffset += nByteOffset * 8;

	//setup mask
	m_uBitMask = 1 << m_nBitOffset;
}
#endif

void CBitPointer::Set(const CBit& p_bit)
{
	Set(p_bit.GetAddress(), p_bit.GetBitOffset());
}

void* CBitPointer::GetAddress() const
{
	return m_puAddress;
}

void* CBitPointer::GetByteAddress() const
{
	return (char*)m_puAddress + m_nBitOffset / 8;
}

int CBitPointer::GetBitOffset() const
{
	return m_nBitOffset;
}

int CBitPointer::GetByteBitOffset() const
{
	return m_nBitOffset % 8;
}

CBitPointer& CBitPointer::New(size_t p_sizBitsSize)
{
	size_t sizSize;
	sizSize = p_sizBitsSize / CBit::s_nDirectlyAccessedBits;
	if (p_sizBitsSize % CBit::s_nDirectlyAccessedBits)
		sizSize++;
	m_puAddress = new unsigned[sizSize];
	m_nBitOffset = 0;
	m_uBitMask = 1;
	return *this;
}

void CBitPointer::Delete()
{
	if (m_puAddress)
	{
		delete [] m_puAddress;
		m_puAddress = NULL;
		m_nBitOffset = 0;
		m_uBitMask = 1;
	}
}


CBitPointer& CBitPointer::operator=(const CBitPointer& p_pbit)
{
	m_puAddress = p_pbit.m_puAddress;
	m_nBitOffset = p_pbit.m_nBitOffset;
	m_uBitMask = p_pbit.m_uBitMask;
	return *this;
}

CBitPointer& CBitPointer::operator=(void* p_pvAddress)
{
	m_puAddress = (unsigned*)p_pvAddress;
	m_nBitOffset = 0;
	m_uBitMask = 1;
	return *this;
}

CBitPointer& CBitPointer::operator=(CBit* p_pbit)
{
	Set(*p_pbit);
	return *this;
}

CBitPointer::operator void*() const
{
	return m_puAddress;
}

CBit CBitPointer::operator*()
{
	return CBit(m_puAddress, m_nBitOffset);
}

CBit CBitPointer::operator[](int p_nBitIndex)
{
	return CBit(m_puAddress, m_nBitOffset + p_nBitIndex);
}

CBitPointer& CBitPointer::operator++()
{
	m_nBitOffset++;
	m_uBitMask <<= 1;
	if (m_nBitOffset > (CBit::s_nDirectlyAccessedBits-1))
	{
		m_nBitOffset = 0;
		m_uBitMask = 1;
		m_puAddress++;
	}
	return *this;
}

CBitPointer& CBitPointer::operator--()
{
	m_nBitOffset--;
	m_uBitMask >>= 1;
	if (m_nBitOffset < 0)
	{
		m_nBitOffset = CBit::s_nDirectlyAccessedBits-1;
		m_uBitMask  = (1 << m_nBitOffset);
		m_puAddress--;
	}
	return *this;
}

CBitPointer CBitPointer::operator++(int)
{
	CBitPointer pbit(*this);
	++*this;
	return pbit;
}

CBitPointer CBitPointer::operator--(int)
{
	CBitPointer pbit(*this);
	--*this;
	return pbit;
}

CBitPointer CBitPointer::operator+(int p_nBitOffset) const
{
	return CBitPointer(m_puAddress, m_nBitOffset + p_nBitOffset);
}

CBitPointer CBitPointer::operator-(int p_nBitOffset) const
{
	return CBitPointer(m_puAddress, m_nBitOffset - p_nBitOffset);
}


ptrdiff_t CBitPointer::operator-(const CBitPointer& p_pbit) const
{
	return (m_puAddress - p_pbit.m_puAddress) * CBit::s_nDirectlyAccessedBits + 
		   m_nBitOffset - p_pbit.m_nBitOffset;
}

ptrdiff_t CBitPointer::operator-(const void* p_pvAddress) const
{
	return (m_puAddress - (const unsigned*)p_pvAddress) * CBit::s_nDirectlyAccessedBits + 
		   m_nBitOffset;
}

ptrdiff_t CBitPointer::operator-(const CBit* p_pbit) const
{
	return *this - CBitPointer(p_pbit);
}

CBitPointer& CBitPointer::operator+=(int p_nBitOffset)
{
	Set(m_puAddress, m_nBitOffset + p_nBitOffset);
	return *this;
}

CBitPointer& CBitPointer::operator-=(int p_nBitOffset)
{
	Set(m_puAddress, m_nBitOffset - p_nBitOffset);
	return *this;
}

bool CBitPointer::operator==(const CBitPointer& p_pbit) const
{
	return (m_puAddress == p_pbit.m_puAddress) && 
		    (m_nBitOffset == p_pbit.m_nBitOffset);
}

bool CBitPointer::operator==(const void* p_pvAddress) const
{
	return (m_puAddress == p_pvAddress) && 
		    (m_nBitOffset == 0);
}

bool CBitPointer::operator==(const CBit* p_pbit) const
{
	return (m_puAddress == p_pbit->GetAddress()) && 
		    (m_nBitOffset == p_pbit->GetBitOffset());
}

bool CBitPointer::operator!=(const CBitPointer& p_pbit) const
{
	return (m_puAddress != p_pbit.m_puAddress) || 
		    (m_nBitOffset != p_pbit.m_nBitOffset);
}

bool CBitPointer::operator!=(const void* p_pvAddress) const
{
	return (m_puAddress != p_pvAddress) || 
		    (m_nBitOffset != 0);
}

bool CBitPointer::operator!=(const CBit* p_pbit) const
{
	return (m_puAddress != p_pbit->GetAddress()) || 
		    (m_nBitOffset != p_pbit->GetBitOffset());
}

bool CBitPointer::operator>(const CBitPointer& p_pbit) const
{
   return (*this - p_pbit) > 0;
}

bool CBitPointer::operator<(const CBitPointer& p_pbit) const
{
   return (*this - p_pbit) < 0;
}

bool CBitPointer::operator>=(const CBitPointer& p_pbit) const
{
   return (*this - p_pbit) >= 0;
}

bool CBitPointer::operator<=(const CBitPointer& p_pbit) const
{
   return (*this - p_pbit) <= 0;
}

