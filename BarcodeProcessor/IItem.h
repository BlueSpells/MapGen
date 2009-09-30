#pragma once

#include "IAbstractBarcodeElement.h"

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

class IItem : public IAbstractBarcodeElement
{
public:
	IItem(EItemType Type) {m_Type = Type; IncreaseBitBufferSize(BitSize(Type));}
	~IItem(void) {}

	virtual void	InsertItemType() = 0;
	virtual bool	IsOfThisType(CBitPointer *Data) = 0;
	EItemType		GetType() {return m_Type;}

#pragma warning(push)
#pragma warning (disable:4239)
#pragma warning (disable:4172)
	CBitPointer&	GetItemContentBuffer() {return GetBitBuffer()+BitSize(m_Type);}
#pragma warning(pop)

	virtual CBitPointer&	AllocateBitBuffer()							
	{IAbstractBarcodeElement::AllocateBitBuffer(); InsertItemType(); return GetItemContentBuffer();}

	virtual	std::string	GetBitBufferParsedString()
	{std::string Str; CBitPointer BitPtr = GetBitBuffer(); AddItemToBitString(m_Type, BitPtr, Str); return GetItemBitBufferParsedString(Str, BitPtr);}

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr) = 0;

private:
	EItemType m_Type;
};