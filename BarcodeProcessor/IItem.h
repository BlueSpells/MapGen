#pragma once

#include "BitPointer.h"
#include "ItemHelper.h"
#include "BitLib.h"

// NOTE: since the item type is Hoffman based the order is extremely important!!
enum EItemType
{
	SubItem,
	Parking,
	Position,
	Pavement,
	BasicComponent,
	ComplexStructure,
	ExtendedItem,
	EItemType_MaxEnum
};
DefineHoffmanEnumBitSize(EItemType);

class IItem
{
public:
	IItem(EItemType Type) {m_Type = Type; m_NumberOfBits=BitSize(Type);}
	~IItem(void) {}

	virtual void	InsertItemType() = 0;
	virtual bool	IsOfThisType(CBitPointer *Data) = 0;
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context /*when necessary*/) = 0;

	EItemType		GetType()									{return m_Type;}
	CBitPointer&	GetBitBuffer()								{return m_BitBuffer;}
	void			IncreaseBitBufferSize(size_t NumberOfBits)	{m_NumberOfBits += NumberOfBits;}
	size_t			GetBitBufferSize()							{return m_NumberOfBits;}
#pragma warning(push)
#pragma warning (disable:4239)
#pragma warning (disable:4172)
	CBitPointer&	AllocateBitBuffer()							
		{m_BitBuffer = new CBitPointer[m_NumberOfBits]; bitzero(m_BitBuffer, m_NumberOfBits); InsertItemType(); return m_BitBuffer+BitSize(m_Type);}
#pragma warning(pop)

private:
	EItemType m_Type;
	CBitPointer m_BitBuffer;
	size_t m_NumberOfBits;
};