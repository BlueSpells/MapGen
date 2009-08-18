////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////

#include "stdafx.h"
#include "Bit.h"

const int CBit::s_nDirectlyAccessedBits =  sizeof(CBit::Word) * 8;

CBit::CBit()
{
	Set(NULL, 0);
}

CBit::CBit(void* p_pvAddress, int p_nBitOffset)
{
	Set(p_pvAddress, p_nBitOffset);
}

CBit::CBit(const CBit& p_bit)
{
	m_puAddress = p_bit.m_puAddress;
	m_nBitOffset = p_bit.m_nBitOffset;
	m_uBitMask = p_bit.m_uBitMask;
}

CBit::~CBit()
{

}

#ifdef LOW_SPEED
void CBit::Set(void* p_pvAddress, int p_nBitOffset)
{
	m_puAddress = (unsigned*)p_pvAddress + p_nBitOffset / s_nDirectlyAccessedBits;
	m_nBitOffset = p_nBitOffset % s_nDirectlyAccessedBits;
	
	if (m_nBitOffset < 0)
	{
		m_nBitOffset += s_nDirectlyAccessedBits;
		m_puAddress--;
	}
	m_uBitMask = 1 << m_nBitOffset;
}
#else
//faster version
void CBit::Set(void* p_pvAddress, int p_nBitOffset)
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

void* CBit::GetAddress() const
{
	return m_puAddress;
}

void* CBit::GetByteAddress() const
{
	return (Byte*)m_puAddress + (m_nBitOffset / 8);
}

int CBit::GetBitOffset() const
{
	return m_nBitOffset;
}

int CBit::GetByteBitOffset() const
{
	return m_nBitOffset % 8;
}

CBit::operator int() const
{
	return ((*m_puAddress & m_uBitMask)? 1 : 0);
}

CBit& CBit::operator=(const CBit& p_pbitSource)
{
   *this = (int)p_pbitSource;
   return *this;
}

CBit& CBit::operator=(const int p_nValue)
{
	if (p_nValue)
		*m_puAddress |= m_uBitMask;
	else
		*m_puAddress &= ~m_uBitMask;
	return *this;
}

CBit& CBit::operator+=(int p_nValue)
{
	p_nValue = (p_nValue ? 1 : 0);
	*this = *this ^ p_nValue;
	return *this;
}

CBit& CBit::operator-=(int p_nValue)
{
	p_nValue = (p_nValue ? 1 : 0);
	*this = *this ^ p_nValue;
	return *this;
}

CBit& CBit::operator|=(int p_nValue)
{
	p_nValue = (p_nValue ? 1 : 0);
	*this = *this | p_nValue;
	return *this;
}

CBit& CBit::operator&=(int p_nValue)
{
	p_nValue = (p_nValue ? 1 : 0);
	*this = *this & p_nValue;
	return *this;
}

CBit& CBit::operator^=(int p_nValue)
{
	p_nValue = (p_nValue ? 1 : 0);
	*this = *this ^ p_nValue;
	return *this;
}

int CBit::operator~()
{
	return ((*m_puAddress & m_uBitMask)? 0 : 1);
}

void CBit::Print() const
{
	printf("CBit Dump:\n");
	printf("Address= %08X\n", m_puAddress);
	printf("Bit Offset= %02d\n", m_nBitOffset);
	printf("Bit Mask= %08X\n", m_uBitMask);
	int v = *this;
	printf("Bit Value= %d\n\n", v);
}