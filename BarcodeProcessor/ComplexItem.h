#pragma once
#include "iitem.h"

#include <vector>

enum EComplexItemUID
{
	EComplexItemUID_MaxEnum = 16
};
DefineEnumBitSize(EComplexItemUID);

enum ENumberOfItemsInComplexStructure
{
	ENumberOfItemsInComplexStructure_MaxEnum = 16
};
DefineEnumBitSize(ENumberOfItemsInComplexStructure);

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

	void Encode(std::vector<IItem *> ListOfEncodedItems, int ComplexItemUID, int NumberOfItems, 
				bool IsVerticalMirror, bool ISHorizontalMirror, bool IsVeritcalReplication, bool IsHotizontalReplication,
				SReplication *VeritcalReplication = NULL, SReplication *HotizontalReplication = NULL);

	void Encode(int ComplexItemUID);


	virtual void	InsertItemType();
	virtual bool	IsOfThisType(CBitPointer *Data);
	virtual void	Decode(IN const CBitPointer &Data, IN OUT int &UsedBits) {;}
};
