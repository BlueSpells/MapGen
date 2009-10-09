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

	unsigned int ListOfVerticesSize;
	if ((ShapeValue == Rect) && (SpecialVertexCurvatureValue == Rectangular))
	{
		ListOfVerticesSize = 1;
	}
	else
	{	
		ListOfVerticesSize = (int)ShapeValue + 2 - (int)IsAdjacentToParkingValue;
	}
	if (!(ListOfVerticesSize ==	ListOfVerticesVector.size()))
		return CommandFailed;

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
		{ //8 bit
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

	CString TextureAndFillStr;
	if (!IsAdjacentToParkingValue)
	{
		TextureAndFillStr.Format(" Texture = %s,", EnumToString(TextureValue).c_str());
		if (TextureValue == SolidFill)
			TextureAndFillStr.AppendFormat(" FillType = %d,", FillValue);
	}

	CString ShortenVertexCoordinateStr;
	if (IsAdjacentToParkingValue)
		ShortenVertexCoordinateStr.Format(" ShortenVertexCoordinate = %d,", ShortenVertexCoordinateValue);


	CString SizeOrSideStr;
	if (IsAdjacentToParkingValue)
		SizeOrSideStr.Format("Side=%s", BooleanStr(SizeOrSide_Value.Side));
	else
		SizeOrSideStr.Format("Size=%s", BooleanStr(SizeOrSide_Value.Size));

	CString ListOfVerticesStr;
	if (IsAdjacentToParkingValue || SizeOrSide_Value.Size)
		ListOfVerticesStr = "Vertex(6bit)=";
	else
		ListOfVerticesStr = "Vertex(8bit)=";
	for (unsigned int i =0; i < ListOfVerticesValue.size(); i++)
	{
		if (IsAdjacentToParkingValue || SizeOrSide_Value.Size)
			ListOfVerticesStr.AppendFormat("(%s, %d, %d)", EnumToString(ListOfVerticesValue[i].CurvatureType).c_str(), ListOfVerticesValue[i].X.Size6bits, ListOfVerticesValue[i].Y.Size6bits);
		else
			ListOfVerticesStr.AppendFormat("(%s, %d, %d)", EnumToString(ListOfVerticesValue[i].CurvatureType).c_str(), ListOfVerticesValue[i].X.Size8bits, ListOfVerticesValue[i].Y.Size8bits);
	}


	CPavementItem *PavementItem = new CPavementItem;
	PavementItem->Encode(ShapeValue, TextureValue, FillValue, IsAdjacentToParkingValue, SizeOrSide_Value, ListOfVerticesValue, SpecialVertexCurvatureValue, ShortenVertexCoordinateValue );

	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: IsAdjacentToParking = %s, %s, Shape = %s,%s SpecialVertexCurvature = %s,%s %s)", 
		PavementCommand, BooleanStr(IsAdjacentToParkingValue), SizeOrSideStr, EnumToString(ShapeValue).c_str(), TextureAndFillStr, EnumToString(SpecialVertexCurvatureValue).c_str(), ShortenVertexCoordinateStr, ListOfVerticesStr);

	Element = (void *)PavementItem;
	ElementType = AddItem;
	return CommandSucceeded;
}
