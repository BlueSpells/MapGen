#include "StdAfx.h"
#include "BasicItem.h"

// Another change

CBasicItem::CBasicItem(void) : IItem(BasicComponent)
{
}

CBasicItem::~CBasicItem(void)
{
}


/*virtual*/ void CBasicItem::InsertItemType()
{

}

/*virtual*/ bool CBasicItem::IsOfThisType(CBitPointer *Data)
{
	// TODO - add true implementation!!

	return false; // temporary!!
}

/*virtual*/ void CBasicItem::Decode(IN const CBitPointer &Data, IN OUT int &UsedBits)
{

}