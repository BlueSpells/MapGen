#pragma once

static const int MaxNumberOfArgumentsPerLine	= 64;
static const int MaxNumberOfLinesPerCommand		= 64;
static const char *CommandDelimiter			= ":";
static const char *ArgumentsDelimiter		= "|";
static const char *ListDelimeter			= " ";
static const char *ListBegin				= "{";
static const char *ListEnd					= "}";
static const char *StructFieldsDelimeter	= ",";
static const char *StructBegin				= "[";
static const char *StructEnd				= "]";
static const char *CommandEnd				= ";";
static const char *ParemterNameFromValueSeperator = "=";
static const char *EndOfScript				= "End";


#define ArgumentNameOf(x) #x
static const char *StrVectorDelimiter = ", ";		// note: this is not part of the script language but is the separator between 
// enum values in the enum lists defined below:


// Explanation about Syntax:
// Each parameter is defined as a list, separated by comma+space (", ")
// First item in list is the Parameter name as it appears in the script
// Second item in list is the Parameter type (enum, struct, union, Int8Bit, Int4Bit, char, bool, etc..
// If Item is either struct, enum or union, there will be more parameters, indicating:
// In enum: the possible values, ORDERED as the enum itself (according value)
// In struct: the name of the fields, ORDERED as expected to be filled by the user
// In union: the name of the fields

// Parking Syntax:
// ---------------
static const char *ParkingCommand = "Parking";

static const std::string ParkingAngle			= "Angle, enum, Perpendicular, Parallel, Right45Degrees, Left45Degrees";
static const std::string ParkingType			= "Type, enum, Regular, Handicap, Bikes, BigParking";
static const std::string IsMirrorDoubled		= "IsMirrorDoubled, bool";
static const std::string Orientation			= "Orientation, enum, Degrees0, Degrees90, Degrees180, Degrees270";
static const std::string IsPeriodicBetweenPoles = "IsPeriodicBetweenPoles, bool";

static const std::string PeriodicBetweenPoles						= "PeriodicBetweenPoles, struct, NumberOfParkingPlaces, PolesType";
static const std::string PeriodicBetweenPoles_NumberOfParkingPlaces = "NumberOfParkingPlaces, Int4Bit";
static const std::string PeriodicBetweenPoles_PolesType				= "PolesType, enum, MarksOnRoad, Circular, Squared, Rectangular, EPolesType_MaxEnum";

static const std::string Multiplicity						= "Multiplicity, struct, IsMultiplied, Multiples";
static const std::string Multiplicity_IsMultiplied			= "IsMultiplied, bool";
static const std::string Multiplicity_Multiples				= "Multiples, Int8Bit";

static const std::string StructureShape								= "StructureShape, struct, AngleType, Shape";
static const std::string StructureShape_AngleType					= "AngleType, enum, None, AngleUpTo90, AngleUpTo360, Circular";
static const std::string StructureShape_Shape						= "Shape, union, SmallAngle, BigAngle, RadiusCoordinates";
static const std::string StructureShape_Shape_SmallAngle			= "SmallAngle, Int6Bit";
static const std::string StructureShape_Shape_BigAngle				= "BigAngle, Int8Bit";
static const std::string StructureShape_Shape_RadiusCoordinates		= "RadiusCoordinates, struct, X, Y";
static const std::string StructureShape_Shape_RadiusCoordinates_X	= "X, Int8Bit";
static const std::string StructureShape_Shape_RadiusCoordinates_Y	= "Y, Int8Bit";



// BasicItem Syntax:
// ---------------
static const char *BasicItemCommand = "BasicItem";

static const std::string BasicItemType			= "Type, enum, Elevator, Staircase, WC, CarGate, PayingMachine";


// Position Syntax:
// ---------------
static const char *PositionCommand = "Position";

static const std::string PositionItemType		= "Type, enum, CR, HorizontalJump, VerticalJump, DiagonalJump";
static const std::string X						= "X, Int8Bit";
static const std::string Y						= "Y, Int8Bit";
