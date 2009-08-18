////////////////////////////////////
// Written By: Eng. Ahmad Hawwash //
// email: ahmad_hawwash@yahoo.com //
// Last Update: 16-8-2008         //
////////////////////////////////////

#ifndef __BIT_H__
#define __BIT_H__

class CBit  
{
public:
	CBit();
	CBit(void* p_pvAddress, int p_nBitOffset = 0);
	CBit(const CBit& p_bit);
	virtual ~CBit();

	void Set(void* p_pvAddress, int p_nBitOffset = 0);

	void* GetAddress() const;
	void* GetByteAddress() const;

	int   GetBitOffset() const;
	int   GetByteBitOffset() const;

	operator int() const;
    CBit& operator=(const CBit& p_pbitSource);
	CBit& operator=(int p_nValue);
	CBit& operator+=(int p_nValue);
	CBit& operator-=(int p_nValue);
	CBit& operator|=(int p_nValue);
	CBit& operator&=(int p_nValue);
	CBit& operator^=(int p_nValue);
	int operator~();

	static const int s_nDirectlyAccessedBits;

	void Print() const;

	typedef unsigned char Byte;
	typedef unsigned int  Word;
protected:
	Word* m_puAddress;
	int	m_nBitOffset;
	Word  m_uBitMask;
};

#endif // #ifndef __BIT_H__
