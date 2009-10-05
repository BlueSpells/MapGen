#pragma once

#include "StdAfx.h"
#include <math.h>
#include <String.h>

#define MAX_STR_LEN 2048

static double logbase(double a, double base)
{
	return log(a) / log(base);
}

#define CalcEnumBitSize(ITEM)	(int)ceil(logbase(ITEM##_MaxEnum, 2))
#define DefineEnumBitSize(ITEM)	static int BitSize(ITEM) {return CalcEnumBitSize(ITEM);}

static int BitSize(bool) {return 1;}
static int BitSize(char String[]) {return (int)strlen(String)*8;}


#define DefineHoffmanEnumBitSize(ITEM)	static int BitSize(ITEM ITEMVALUE)			\
{																					\
	if (ITEMVALUE == ITEM##_MaxEnum - 1)											\
		return (int)ITEMVALUE-1;													\
	return ((int)ITEMVALUE);														\
}																					

#define DefineStructBitSize1(ITEM, SUBITEM1)										\
static int BitSize(ITEM ITEMVALUE) {return BitSize(ITEMVALUE.SUBITEM1);}

#define DefineStructBitSize2(ITEM, SUBITEM1, SUBITEM2)								\
static int BitSize(ITEM ITEMVALUE) {return BitSize(ITEMVALUE.SUBITEM1) + BitSize(ITEMVALUE.SUBITEM2);}

#define DefineStructBitSize3(ITEM, SUBITEM1, SUBITEM2, SUBITEM3)									\
	static int BitSize(ITEM ITEMVALUE) {return BitSize(ITEMVALUE.SUBITEM1)							\
	+ BitSize(ITEMVALUE.SUBITEM2) + BitSize(ITEMVALUE.SUBITEM3);}									\

#define DefineStructBitSize4(ITEM, SUBITEM1, SUBITEM2, SUBITEM3, SUBITEM4)							\
	static int BitSize(ITEM ITEMVALUE) {return BitSize(ITEMVALUE.SUBITEM1)							\
	+ BitSize(ITEMVALUE.SUBITEM2) + BitSize(ITEMVALUE.SUBITEM3) + BitSize(ITEMVALUE.SUBITEM4);}		\


#define DefineUnionStructBitSize2(ITEM, ITEMINDEXFIELD, ITEMUNIONFIELD, SUBITEM1, SUBITEM2)				\
static int BitSize(ITEM ITEMVALUE)																		\
{																										\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 0) return BitSize(ITEMVALUE.ITEMINDEXFIELD);														\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 1) return BitSize(ITEMVALUE.ITEMINDEXFIELD) + BitSize(ITEMVALUE.ITEMUNIONFIELD.SUBITEM1);		\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 2) return BitSize(ITEMVALUE.ITEMINDEXFIELD) + BitSize(ITEMVALUE.ITEMUNIONFIELD.SUBITEM2);		\
	ASSERT(false); return 0;\
}	

#define DefineUnionStructBitSize3(ITEM, ITEMINDEXFIELD, ITEMUNIONFIELD, SUBITEM1, SUBITEM2, SUBITEM3)	\
static int BitSize(ITEM ITEMVALUE)																		\
{																										\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 0) return BitSize(ITEMVALUE.ITEMINDEXFIELD);														\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 1) return BitSize(ITEMVALUE.ITEMINDEXFIELD) + BitSize(ITEMVALUE.ITEMUNIONFIELD.SUBITEM1);		\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 2) return BitSize(ITEMVALUE.ITEMINDEXFIELD) + BitSize(ITEMVALUE.ITEMUNIONFIELD.SUBITEM2);		\
	if (((int)ITEMVALUE.ITEMINDEXFIELD) == 3) return BitSize(ITEMVALUE.ITEMINDEXFIELD) + BitSize(ITEMVALUE.ITEMUNIONFIELD.SUBITEM3);		\
	ASSERT(false); return 0;\
}	

#define DefineStructWithBoolBitSize2(ITEM, SUBITEMBOOL, SUBITEMOTHER)								\
static int BitSize(ITEM ITEMVALUE) {return BitSize(ITEMVALUE.SUBITEMBOOL) + ((ITEMVALUE.SUBITEMBOOL) ? BitSize(ITEMVALUE.SUBITEMOTHER) : 0);}


#define BitCopyAndContinue(DestinationBitPtr, SourceData)										\
{																								\
	ASSERT((std::string(typeid(SourceData).name())).find("Signed") == std::string::npos			\
			|| (std::string(typeid(SourceData).name())).find("Bit") == std::string::npos);		\
	bitcpy(DestinationBitPtr, CBitPointer(&(SourceData), 0), BitSize(SourceData));				\
	DestinationBitPtr += BitSize(SourceData);													\
}

#define BitCopySignedXBitAndContinue(DestinationBitPtr, SourceData)								\
{																								\
	ASSERT((std::string(typeid(SourceData).name())).find("Signed") != std::string::npos			\
			&& (std::string(typeid(SourceData).name())).find("Bit") != std::string::npos);		\
	BitCopyAndContinue(DestinationBitPtr, SourceData.sign);										\
	BitCopyAndContinue(DestinationBitPtr, SourceData.value);									\
}

#define BitCopyItemAndContinue(DestinationBitPtr, SourceItem)									\
{																								\
	bitcpy(DestinationBitPtr, (SourceItem).GetBitBuffer(), (SourceItem).GetBitBufferSize());	\
	DestinationBitPtr += (int)((SourceItem).GetBitBufferSize());								\
}

#define BitPasteAndContinue(SourceBitPtr, DestinationData)										\
{																								\
	ZeroMemory(&DestinationData, sizeof DestinationData);										\
	ASSERT((std::string(typeid(DestinationData).name())).find("Signed") == std::string::npos	\
			|| (std::string(typeid(DestinationData).name())).find("Bit") == std::string::npos);	\
	bitcpy(CBitPointer(&(DestinationData), 0), SourceBitPtr, BitSize(DestinationData));			\
	SourceBitPtr += BitSize(DestinationData);													\
}

#define BitPasteSignedXBitAndContinue(SourceBitPtr, DestinationData)							\
{																								\
	ASSERT((std::string(typeid(DestinationData).name())).find("Signed") != std::string::npos	\
			&& (std::string(typeid(DestinationData).name())).find("Bit") != std::string::npos);	\
	BitPasteAndContinue(SourceBitPtr, DestinationData.sign);									\
	BitPasteAndContinue(SourceBitPtr, DestinationData.value);									\
}

#define BitDecodeAndContinue(DecodingObject, BitPtr)			\
{																\
	int UsedBytes = 0;											\
	DecodingObject->Decode(BitPtr, UsedBytes, NULL);			\
	BitPtr += UsedBytes;										\
}

#define DefineNumericValue(NUMBEROFBITS)												\
enum Int##NUMBEROFBITS##Bit { Int##NUMBEROFBITS##Bit_NumberOfBits = NUMBEROFBITS };		\
static int BitSize(Int##NUMBEROFBITS##Bit ) {return NUMBEROFBITS;}						\
static Int##NUMBEROFBITS##Bit ConvertIntToInt##NUMBEROFBITS##Bit(int x)					\
{ASSERT(x < pow((double)2,NUMBEROFBITS)); return (Int##NUMBEROFBITS##Bit)x;}			\
struct SignedInt##NUMBEROFBITS##Bit {bool sign; Int##NUMBEROFBITS##Bit value;};			\
DefineStructBitSize2(SignedInt##NUMBEROFBITS##Bit, sign, value);						\
static SignedInt##NUMBEROFBITS##Bit ConvertIntToSignedInt##NUMBEROFBITS##Bit(int x)		\
{SignedInt##NUMBEROFBITS##Bit num; num.sign = (x >= 0);									\
num.value = ConvertIntToInt##NUMBEROFBITS##Bit(abs(x)); return num;}					


DefineNumericValue(32);
DefineNumericValue(16);
DefineNumericValue(9);
DefineNumericValue(8);
DefineNumericValue(6);
DefineNumericValue(5);
DefineNumericValue(4);


#define AddItemToBitString(Item, BitPtr, BitString)	\
{													\
BitString += BinaryString(BitPtr, BitSize(Item));	\
BitString += " | ";		  BitPtr += BitSize(Item);	\
}
