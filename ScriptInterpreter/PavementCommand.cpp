#include "StdAfx.h"
#include "PavementCommand.h"
#include "CommandHelper.h"
#include "BarcodeProcessor/PavementItem.h"
#include "ScriptSyntaxDefinitions.h"
#include "BarcodeProcessor/EnumsUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

	bool IsAdjacentToParkingValue;
	if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, IsAdjacentToParking, ParsedArguments, IsAdjacentToParkingValue))
		return CommandFailed;

	SCurvatureParameters SpecialVertexCurvatureValue;
	std::vector<std::string> SpecialVertexCurvatureVector;
	SpecialVertexCurvatureValue.Type = Linear;
	CString SpecialVertexCurvatureStr;
	if (!((ShapeValue == Wall) && (IsAdjacentToParkingValue)))
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SpecialVertexCurvature, ParsedArguments, SpecialVertexCurvatureVector))
			return CommandFailed;

		if (ShapeValue == Rect)
			SpecialVertexCurvatureValue.Type = Rectangular;
		else
		{
			if (!ExtractAndInterperetStructField(ContextLine, SpecialVertexCurvature, SpecialVertexCurvature_Type, SpecialVertexCurvatureVector, SpecialVertexCurvatureValue.Type))
				return CommandFailed;
			SpecialVertexCurvatureStr.Format(", SpecialVertexCurvature = [%s", EnumToString(SpecialVertexCurvatureValue.Type).c_str());

			if ((SpecialVertexCurvatureValue.Type == Convex) || (SpecialVertexCurvatureValue.Type == Concave))
			{
				if (!ExtractAndInterperetStructField(ContextLine, SpecialVertexCurvature, SpecialVertexCurvature_IsCircular, SpecialVertexCurvatureVector, SpecialVertexCurvatureValue.IsCircular))
					return CommandFailed;
				SpecialVertexCurvatureStr.AppendFormat(", %s", BooleanStr(SpecialVertexCurvatureValue.IsCircular));
			}
			SpecialVertexCurvatureStr.AppendFormat("]");
		}
	}


	Int6Bit ShortenVertexCoordinateValue = ConvertIntToInt6Bit(0);
	CString ShortenVertexCoordinateStr;
	if ((IsAdjacentToParkingValue) && (ShapeValue != Wall))
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, ShortenVertexCoordinate, ParsedArguments, ShortenVertexCoordinateValue))
			return CommandFailed;
		ShortenVertexCoordinateStr.Format(", ShortenVertexCoordinate = %d", ShortenVertexCoordinateValue);
	}

	// Dealing with the following union field differently than normal.
	// Instead of first getting the union and then parsing it, the code asks directly for the 
	// exact union value by using different names of the different union possibilities:


	USizeOrSide SizeOrSide_Value;
	SizeOrSide_Value.ReducedSize = false;
	CString SizeOrSideStr;
	if (IsAdjacentToParkingValue)
	{
		if (!(ShapeValue == Wall))
		{
			if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SizeOrSide_Side, ParsedArguments, SizeOrSide_Value.Side))
				return CommandFailed;
			SizeOrSideStr.Format(", Side=%s", BooleanStr(SizeOrSide_Value.Side));
		}
	}
	else
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, SizeOrSide_ReducedSize, ParsedArguments, SizeOrSide_Value.ReducedSize))
			return CommandFailed;
		SizeOrSideStr.Format(", ReducedSize=%s", BooleanStr(SizeOrSide_Value.ReducedSize));
	}

	ETextureType TextureValue = Texture_Pavement;
	Int4Bit FillValue = ConvertIntToInt4Bit(0);
	CString TextureAndFillStr;
	if (!IsAdjacentToParkingValue && (ShapeValue != Wall))
	{
		if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, Texture, ParsedArguments, TextureValue))
			return CommandFailed;
		TextureAndFillStr.Format(", Texture = %s", EnumToString(TextureValue).c_str());
		if (TextureValue == SolidFill)
		{
			if (!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, FillType, ParsedArguments, FillValue))
				return CommandFailed;
			TextureAndFillStr.AppendFormat(", FillType = %d", FillValue);
		}
	}


	std::vector<SVertexParameters> ListOfVerticesValue;
	std::vector<std::string> ListOfVerticesVector;
	bool ListOfVerticesSizeCheck = false;  //Checking if number of given vertexes is correct
	if ((ShapeValue == Quadrangle) || (ShapeValue == Pentagon) || (!IsAdjacentToParkingValue))
	{
		if(!ExtractAndInterperetArgumentValue(ContextLine, PavementCommand, VertexList, ParsedArguments, ListOfVerticesVector))
			return CommandFailed;
	}

	switch (ShapeValue)
	{
		case Wall:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 0)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Right_Triangle:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 0)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Triangle:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 0)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 2)))
				ListOfVerticesSizeCheck = true;
			break;
		case Rect:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 0)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 1)))
				ListOfVerticesSizeCheck = true;
			break;
		case Quadrangle:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 2)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 3)))
				ListOfVerticesSizeCheck = true;
			break;
		case Pentagon:
			if ((IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 3)) || (!IsAdjacentToParkingValue && (ListOfVerticesVector.size() == 4)))
				ListOfVerticesSizeCheck = true;
			break;
	}

	CString ListOfVerticesStr;
	if (ListOfVerticesSizeCheck)
	{
		if ((ShapeValue == Quadrangle) || (ShapeValue == Pentagon) || (!IsAdjacentToParkingValue))
			if (IsAdjacentToParkingValue || SizeOrSide_Value.ReducedSize)
				ListOfVerticesStr = ", Vertex(6bit)=";
			else
				ListOfVerticesStr = ", Vertex(8bit)=";
		
		CString CurvatureParametersStr;
		for (unsigned int i =0; i < ListOfVerticesVector.size(); i++)
		{
			SVertexParameters FullVertexValue;
			std::vector<std::string> FullVertexVector;
			if(!ExtractAndInterperetListItem(ContextLine, VertexList, VertexList_VertexParameters, ListOfVerticesVector, FullVertexVector, i))
				return CommandFailed;

			std::string FullVertexCoordinatesStr;
			if (IsAdjacentToParkingValue || SizeOrSide_Value.ReducedSize)
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

			if (ShapeValue == Rect)
				FullVertexValue.CurvatureType = Rectangular;
			else
			{
				if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_CurvatureType, FullVertexVector, FullVertexValue.CurvatureType))
					return CommandFailed;
				CurvatureParametersStr.Format(", %s", EnumToString(FullVertexValue.CurvatureType).c_str());

				if ((FullVertexValue.CurvatureType == Convex) || (FullVertexValue.CurvatureType == Concave))
				{
					if (!ExtractAndInterperetStructField(ContextLine, VertexList_VertexParameters, VertexList_VertexParameters_IsCircular, FullVertexVector, FullVertexValue.IsCircular))
						return CommandFailed;
					CurvatureParametersStr.AppendFormat(", %s", BooleanStr(FullVertexValue.IsCircular));
				}
			}

			ListOfVerticesValue.push_back(FullVertexValue);

			if (IsAdjacentToParkingValue || SizeOrSide_Value.ReducedSize)
				ListOfVerticesStr.AppendFormat("(%d, %d%s)", ListOfVerticesValue[i].X.Size6bits, ListOfVerticesValue[i].Y.Size6bits, CurvatureParametersStr);
			else
				ListOfVerticesStr.AppendFormat("(%d, %d%s)", ListOfVerticesValue[i].X.Size8bits, ListOfVerticesValue[i].Y.Size8bits, CurvatureParametersStr);
		}
	}
	else
		ListOfVerticesStr = "Wrong number of vertexes!!";


	CPavementItem *PavementItem = new CPavementItem;
	PavementItem->Encode(ShapeValue, TextureValue, FillValue, IsAdjacentToParkingValue, SizeOrSide_Value, ListOfVerticesValue, SpecialVertexCurvatureValue, ShortenVertexCoordinateValue );

	LogEvent(LE_INFO, __FUNCTION__ ": %s Command Parsed Successfully: Shape = %s, IsAdjacentToParking = %s%s%s%s%s%s", 
		PavementCommand, EnumToString(ShapeValue).c_str(), BooleanStr(IsAdjacentToParkingValue), SpecialVertexCurvatureStr, ShortenVertexCoordinateStr, SizeOrSideStr, ListOfVerticesStr, TextureAndFillStr);

	Element = (void *)PavementItem;
	ElementType = AddItemCommand;
	return CommandSucceeded;
}
