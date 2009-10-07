#include "StdAfx.h"
#include "PavementCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/PavementItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

CPavementCommand::CPavementCommand(void) : IScriptCommand(PavementCommand)
{
}

CPavementCommand::~CPavementCommand(void)
{
}

/*virtual*/ ECommandResult CPavementCommand::OnCommandImplementation(IN int ContextLine, std::string CommandWord, std::vector<std::string> CommandArguments, void *&Element, EElementType &ElementType)
{
	Assert(CommandWord == PavementCommand);

	std::map<std::string, std::string> ParsedArguments;
	if (!InterperetArguments(ContextLine, CommandArguments, ParsedArguments))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to parse arguments.", ContextLine);
		return CommandFailed;
	}

	EShapeType ShapeValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, Shape, ParsedArguments, ShapeValue))
		return CommandFailed;
	
	ETextureType TextureValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, Texture, ParsedArguments, TextureValue))
		return CommandFailed;
	
	Int4Bit FillValue = ConvertIntToInt4Bit(0);
	if (TextureValue == SolidFill)
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, FillType, ParsedArguments, FillValue))
			return CommandFailed;

	bool IsAdjacentToParkingValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, IsAdjacentToParking, ParsedArguments, IsAdjacentToParkingValue))
		return CommandFailed;

	// Dealing with the following union field differently than normal.
	// Instead of first getting the union and then parsing it, the code asks directly for the 
	// exact union value by using different names of the different union possibilities:
	USizeOrSide SizeOrSide_Value;
	if (IsAdjacentToParkingValue)
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SizeOrSide_Side, ParsedArguments, SizeOrSide_Value.Side))
			return CommandFailed;
	}
	else
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SizeOrSide_Size, ParsedArguments, SizeOrSide_Value.Size))
			return CommandFailed;

	ECurvatureType SpecialVertexCurvatureValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SpecialVertexCurvature, ParsedArguments, SpecialVertexCurvatureValue))
		return CommandFailed;
	
	Int6Bit ShortenVertexCoordinateValue = ConvertIntToInt6Bit(0);
	if (IsAdjacentToParkingValue)
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, ShortenVertexCoordinate, ParsedArguments, ShortenVertexCoordinateValue))
			return CommandFailed;


	std::vector<SVertexParameters> ListOfVerticesValue;
	std::vector<std::string> ListOfVerticesVector;
	if(!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, VertexList, ParsedArguments, ListOfVerticesVector))
		return CommandFailed;

	int ListOfVerticesSize = (int)ShapeValue + 2 - (int)IsAdjacentToParkingValue;
	
	for (unsigned int i =0; i < ListOfVerticesVector.size(); i++)
	{
		SVertexParameters FullVertexValue;
		std::vector<std::string> FullVertexVector;
		if(!ExtractAndInterperetListItem(ContextLine, VertexList, VertexList_VertexParameters, ListOfVerticesVector, FullVertexVector, i))
			return CommandFailed;

		if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_CurvatureType, FullVertexVector, FullVertexValue.CurvatureType))
			return CommandFailed;

		std::string FullVertexCoordinatesStr;
		if (IsAdjacentToParkingValue || SizeOrSide_Value.Size)
		{ //6 bit
			if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_X, FullVertexVector, FullVertexCoordinatesStr))
				return CommandFailed;
			if (!ExtractAndInterperetUnionField(ContextLine, VertexList_VertexParameters_X, VertexList_VertexParameters_X_Size6bits, FullVertexCoordinatesStr, FullVertexValue.X.Size6bits))
				return CommandFailed;

			if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_Y, FullVertexVector, FullVertexCoordinatesStr))
				return CommandFailed;
			if (!ExtractAndInterperetUnionField(ContextLine, VertexList_VertexParameters_Y, VertexList_VertexParameters_Y_Size6bits, FullVertexCoordinatesStr, FullVertexValue.Y.Size6bits))
				return CommandFailed;
		}
		else
		{
			if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_X, FullVertexVector, FullVertexCoordinatesStr))
				return CommandFailed;
			if (!ExtractAndInterperetUnionField(ContextLine, VertexList_VertexParameters_X, VertexList_VertexParameters_X_Size8bits, FullVertexCoordinatesStr, FullVertexValue.X.Size8bits))
				return CommandFailed;

			if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_Y, FullVertexVector, FullVertexCoordinatesStr))
				return CommandFailed;
			if (!ExtractAndInterperetUnionField(ContextLine, VertexList_VertexParameters_Y, VertexList_VertexParameters_Y_Size8bits, FullVertexCoordinatesStr, FullVertexValue.Y.Size8bits))
				return CommandFailed;
		}
		ListOfVerticesValue.push_back(FullVertexValue);
	}



	CPavementItem *PavementItem = new CPavementItem;
	PavementItem->Encode(ShapeValue, TextureValue, FillValue, IsAdjacentToParkingValue, SizeOrSide_Value, ListOfVerticesValue, SpecialVertexCurvatureValue, ShortenVertexCoordinateValue );

	if (IsAdjacentToParkingValue || SizeOrSide_Value.Size)
		LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: Shape = %s, Texture = %s, FillType = %d, IsAdjacentToParking = %s, SizeOrSide = %s, SpecialVertexCurvature = %s, ShortenVertexCoordinate = %d, Vertex(6bit)=(%s, %d, %d)", 
			PavementCommand, EnumToString(ShapeValue).c_str(), EnumToString(TextureValue).c_str(), FillValue, BooleanStr(SizeOrSide_Value.Size), EnumToString(SpecialVertexCurvatureValue).c_str(), ShortenVertexCoordinate, 
			EnumToString(ListOfVerticesValue[0].CurvatureType).c_str(), ListOfVerticesValue[0].X.Size6bits, ListOfVerticesValue[0].Y.Size6bits);
	else
		LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: Shape = %s, Texture = %s, FillType = %d, IsAdjacentToParking = %s, SizeOrSide = %s, SpecialVertexCurvature = %s, ShortenVertexCoordinate = %d, Vertex(8bit)=(%s, %d, %d)", 
		PavementCommand, EnumToString(ShapeValue).c_str(), EnumToString(TextureValue).c_str(), FillValue, BooleanStr(SizeOrSide_Value.Size), EnumToString(SpecialVertexCurvatureValue).c_str(), ShortenVertexCoordinate, 
		EnumToString(ListOfVerticesValue[0].CurvatureType).c_str(), ListOfVerticesValue[0].X.Size8bits, ListOfVerticesValue[0].Y.Size8bits);


	Element = (void *)PavementItem;
	ElementType = AddItem;
	return CommandSucceeded;
}

/*

CComplexItem *ComplexItem = new CComplexItem;
std::vector<IItem *> ListOfEncodedItemsValue;
std::vector<std::string> ListOfItemsVector;
if(!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, ListOfEncodedItems, ParsedArguments, ListOfItemsVector))
{
ComplexItem->Encode(UID_Value, IsVerticalMirrorValue, IsHorizontalMirrorValue, IsVerticalReplicationValue,
IsHorizontalReplicationValue, ((IsVerticalReplicationValue) ? &VerticalReplicationValue : NULL),
((IsHorizontalReplicationValue) ? &HorizontalReplicationValue : NULL));

LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: UID = %d, IsVerticalMirror = %s, IsHorizontalMirror = %s, IsVerticalReplication = %s, IsHorizontalReplication = %s, VerticalReplication = [%d %d], HorizontalReplication = [%d %d])", 
ComplexItemCommand, UID_Value, BooleanStr(IsVerticalMirrorValue), BooleanStr(IsHorizontalMirrorValue), BooleanStr(IsVerticalReplicationValue), BooleanStr(IsHorizontalReplicationValue), 
VerticalReplicationValue.TimesToReplicate, VerticalReplicationValue.GapBetweenReplicas, HorizontalReplicationValue.TimesToReplicate, HorizontalReplicationValue.GapBetweenReplicas );

}
else
{		
bool IsReplicationPartOfDefinitionValue;
if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, IsReplicationPartOfDefinition, ParsedArguments, IsReplicationPartOfDefinitionValue))
return CommandFailed;
Int5Bit NumberOfObjectsInComplexValue;
if (!ExtractAndInterperetArgumentValue(ContextLine, ComplexItemCommand, NumberOfObjectsInComplex, ParsedArguments, NumberOfObjectsInComplexValue))
return CommandFailed;

for (int i = 0; i < NumberOfObjectsInComplexValue; i++)
{
IItem *ItemWithinComplex = NULL;
if(!ExtractAndInterperetListItem(ContextLine, ListOfEncodedItems, ListOfEncodedItems_Item, ListOfItemsVector, ItemWithinComplex, i))
return CommandFailed;
ListOfEncodedItemsValue.push_back(ItemWithinComplex);
}

*/