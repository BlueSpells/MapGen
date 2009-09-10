#pragma once
#include "iitem.h"

#include <vector>


struct SReplication
{
	enum EGapBetweenReplicas
	{
		EGapBetweenReplicas_MaxEnum = 64	
	}GapBetweenReplicas;

	enum ETimesToReplicate
	{
		ETimesToReplicate_MaxEnum = 64	
	}TimesToReplicate;

	SReplication() {ZeroMemory(this, sizeof(*this));}
	SReplication(int _GapBetweenReplicas, int _TimesToReplicate)
		: GapBetweenReplicas((EGapBetweenReplicas)_GapBetweenReplicas), TimesToReplicate((ETimesToReplicate)_TimesToReplicate) {}
};
DefineEnumBitSize(SReplication::EGapBetweenReplicas);
DefineEnumBitSize(SReplication::ETimesToReplicate);
DefineStructBitSize2(SReplication, GapBetweenReplicas, TimesToReplicate);


class CComplexItem :
	public IItem
{
public:
	CComplexItem(void);
	~CComplexItem(void);

	// For First Usage \ ComplexItem Definition
	void Encode(int ComplexItemUID, std::vector<IItem *> ListOfEncodedItems,
				bool IsVerticalMirror, bool ISHorizontalMirror, 
				bool IsVeritcalReplication, bool IsHotizontalReplication, bool IsReplicationPartOfDefinition,
				SReplication *VeritcalReplication = NULL, SReplication *HotizontalReplication = NULL);

	// For Later Uses (after this ComplexItemUID has been defined)
	void Encode(int ComplexItemUID, bool IsVerticalMirror, bool ISHorizontalMirror, 
		bool IsVeritcalReplication, bool IsHotizontalReplication,
		SReplication *VeritcalReplication = NULL, SReplication *HotizontalReplication = NULL);


	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	
	// IMPORTANT NOTE:
	// Decoding of Complex item is done in two phases.
	// The first: the ComplexItem only returns its UID.
	// Then it is recalled in second phase with UsedBits>0 as it returned in first phase, and 
	// with Context set to 0 if it's first definition and set to 1 if it is not.
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context);
	virtual void	AddDecodedItemToList(IItem *ItemWithinComplex);

private:
	virtual void	DecodePhase1(IN const CBitPointer &Data, IN OUT int &UsedBits);
	virtual void	DecodePhase2(IN const CBitPointer &Data, IN OUT int &UsedBits, IN int *Context);

	Int5Bit m_UID;
	bool m_IsFirstDefinitionOfComplexItem;
	Int5Bit m_NumberOfObjectsInComplex;

	std::vector<IItem *> m_ListOfEncodedItems;
	bool m_IsVerticalMirror;
	bool m_IsHorizontalMirror;
	bool m_IsVeritcalReplication;
	bool m_IsHotizontalReplication;
	bool m_IsReplicationPartOfDefinition;
	SReplication m_VeritcalReplication;
	SReplication m_HotizontalReplication;
};
