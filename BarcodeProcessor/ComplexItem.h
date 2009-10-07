#pragma once
#include "iitem.h"

#include <vector>


struct SReplication
{
	Int6Bit GapBetweenReplicas;
	Int6Bit TimesToReplicate;

	SReplication() {ZeroMemory(this, sizeof(*this));}
	SReplication(int _GapBetweenReplicas, int _TimesToReplicate)
		: GapBetweenReplicas(ConvertIntToInt6Bit(_GapBetweenReplicas)), TimesToReplicate(ConvertIntToInt6Bit(_TimesToReplicate)) {}
};
DefineStructBitSize2(SReplication, GapBetweenReplicas, TimesToReplicate);


class CComplexItem :
	public IItem
{
public:
	CComplexItem(void);
	~CComplexItem(void);

	// For First Usage \ ComplexItem Definition
	void Encode(Int5Bit ComplexItemUID, std::vector<IItem *> ListOfEncodedItems,
				bool IsVerticalMirror, bool IsHorizontalMirror, 
				bool IsVerticalReplication, bool IsHorizontalReplication, bool IsReplicationPartOfDefinition,
				SReplication *VerticalReplication = NULL, SReplication *HorizontalReplication = NULL);

	// The following Encode does not add the items contained by the complex item, but creates only the complex header.
	// The items themselves will be encoded and added separately later.
	// The ComplexCommand uses this overload version of Encode.
	void Encode(Int5Bit ComplexItemUID, Int5Bit NumberOfEncodedItems,
		bool IsVerticalMirror, bool IsHorizontalMirror, 
		bool IsVerticalReplication, bool IsHorizontalReplication, bool IsReplicationPartOfDefinition,
		SReplication *VerticalReplication = NULL, SReplication *HorizontalReplication = NULL);


	// For Later Uses (after this ComplexItemUID has been defined)
	void Encode(Int5Bit ComplexItemUID, bool IsVerticalMirror, bool ISHorizontalMirror, 
		bool IsVerticalReplication, bool IsHorizontalReplication,
		SReplication *VerticalReplication = NULL, SReplication *HorizontalReplication = NULL);


	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	
	// IMPORTANT NOTE:
	// Decoding of Complex item is done in two phases.
	// The first: the ComplexItem only returns its UID.
	// Then it is recalled in second phase with UsedBits>0 as it returned in first phase, and 
	// with Context set to 0 if it's first definition and set to 1 if it is not.
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context);
	virtual void	AddDecodedItemToList(IItem *ItemWithinComplex);

	virtual	std::string	GetItemBitBufferParsedString(std::string ParsedString, CBitPointer BitPtr);

private:
	virtual void	DecodePhase1(IN const CBitPointer &Data, IN OUT int &UsedBits);
	virtual void	DecodePhase2(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context);

	Int5Bit m_UID;
	bool m_IsFirstDefinitionOfComplexItem;
	Int5Bit m_NumberOfObjectsInComplex;

	std::vector<IItem *> m_ListOfEncodedItems;
	bool m_IsVerticalMirror;
	bool m_IsHorizontalMirror;
	bool m_IsVerticalReplication;
	bool m_IsHorizontalReplication;
	bool m_IsReplicationPartOfDefinition;
	SReplication m_VerticalReplication;
	SReplication m_HorizontalReplication;
};
