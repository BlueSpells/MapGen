#pragma once

#include <vector>
#include <map>
#include "Common/Utils.h"
#include "Common/collectionhelper.h"
#include "ScriptSyntaxDefinitions.h"

static bool InterperetArgument(IN std::string Argument, OUT std::string &Parameter, OUT std::string &Value)
{
	CTokenParser Parser(Argument.c_str());
	int i = 0;
	while (Parser.MoreTokens())
	{
		switch (i)
		{
		case 0:
			Parameter = Parser.GetNextToken(ParemterNameFromValueSeperator);
			break;
		case 1:
			Value = Parser.GetNextToken(ParemterNameFromValueSeperator);
			break;
		};
		i++;
	}
	if (i != 2)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Illegal Argument %s! %d parameters within! Is space missing?", Argument.c_str(), i);
		return false;
	}
	return true;
}


static bool InterperetArguments(IN int ContextLine, IN std::vector<std::string> CommandArguments, OUT std::map<std::string, std::string> &ParsedArguments)
{
	for (unsigned int iArgument = 0; iArgument < CommandArguments.size(); iArgument++)
	{
		std::string Parameter;
		std::string Value;

		if (!InterperetArgument(CommandArguments[iArgument], Parameter, Value))
			return false;

		if (!InsertValueToMap(ParsedArguments, Parameter, Value))
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to add Parameter %s to Parameters Map. Perhaps exists twice?", ContextLine, Parameter);
			return false;
		}
	}

	return true;
}

// The following is in order to avoid error on second line of InterperetArgumentValueAsIntXBit
static bool BitSize(std::vector<std::string> Dummy) {return 0xDeadBabe;} 
static bool BitSize(std::string Dummy) {return 0xDeadBabe;} 

template <class T> 
bool InterperetArgumentValueAsIntXBit(int ContextLine, std::string ParameterName, std::string ArgumentValue, T &Value)
{
	int IntegerValue = atoi(ArgumentValue.c_str());
	int MaxVal = (int)pow((double)2, BitSize(Value)) - 1;
	if (IntegerValue > MaxVal)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Parameter %s contains value %s (=%d) which is higher than maximum allowed (%d)!!", 
			ContextLine, ParameterName.c_str(), ArgumentValue.c_str(), IntegerValue, MaxVal);
		return false;
	}

	Value = (T)IntegerValue;

	return true;
}

static std::string LowerCase(std::string &originstr)
{
	std::string str = originstr;
	for(int i=0; i<str.size();++i)
		str[i] = tolower(str[i]);
	return str;
}

static std::string StrVectorToStr(std::vector<std::string> StrVector)
{
	if (StrVector.size() == 0)
		return "Empty std::vector<std::string>";

	std::string Str = StrVector[0];
	for (unsigned int i = 1; i < StrVector.size(); i ++)
	{
		Str += StrVectorDelimiter;
		Str += StrVector[i];
	}

	return Str;
}

static std::vector<std::string> StrToStrVector(std::string Str)
{
	std::vector<std::string> StrVector;

	CTokenParser StrParser(Str.c_str());

	StrVector.push_back(StrParser.GetNextToken(StrVectorDelimiter));
	while (StrParser.MoreTokens())
	{
		// Patch. GetNextToken returns empty strings..
		std::string enumValue = StrParser.GetNextToken(StrVectorDelimiter);
		if (enumValue.size() > 0) 
			StrVector.push_back(enumValue);
	}

	return StrVector;
}


static bool InterperetArgumentValueAsBool(int ContextLine, std::string ParameterName, std::string ArgumentValue, bool &Value)
{
	if (LowerCase(ArgumentValue) == "true")
	{
		Value = true;
		return true;
	}
	if (LowerCase(ArgumentValue) == "false")
	{
		Value = false;
		return true;
	}
	
	LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Parameter %s expected to be boolean, yet it contains value other than true\false: %s!", 
		ContextLine, ParameterName.c_str(), ArgumentValue.c_str());
	return false;
}

static bool InterperetArgumentValueAsUnion(int ContextLine, std::string ParameterName, std::string ArgumentValue, std::string &Value)
{
	Value = ArgumentValue; // what else to do?
	// do nothing at this point. interpretation will be done externally
	return false;
}

template <class T> 
bool InterperetArgumentValueAsEnum(int ContextLine, std::string ParameterName, std::string ArgumentValue, T &Value, std::vector<std::string> SortedEnumValues)
{
	for (unsigned int i = 0; i < SortedEnumValues.size(); i++)
	{
		if (LowerCase(ArgumentValue) == LowerCase(SortedEnumValues[i]))
		{
			Value = (T)i;
			return true;
		}
	}

	LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Parameter %s expected to be an enum with the following optional values: %s, yet it contains the value: %s!!", 
		ContextLine, ParameterName.c_str(), StrVectorToStr(SortedEnumValues).c_str(), ArgumentValue.c_str());
	return false;
}

template <class T> 
bool InterperetArgumentValueAsEnum(int ContextLine, std::string ParameterName, std::string ArgumentValue, T &Value, std::string SortedEnumValues)
{
	return InterperetArgumentValueAsEnum(ContextLine, ParameterName, ArgumentValue, Value, StrToStrVector(SortedEnumValues));
}


static bool ExtractAndInterperetArgumentValueAsBool(int ContextLine, std::string Command, std::string ParameterName, std::map<std::string, std::string> ParsedArguments, bool &Value)
{
	std::string ArgumentValue;
	if (!GetValueFromMap(ParsedArguments, ParameterName, ArgumentValue))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: %s command must contain parameter %s", ContextLine, Command, ParameterName);
		return false;
	}
	if (!InterperetArgumentValueAsBool(ContextLine, ParameterName, ArgumentValue, Value))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract value %s from parameter %s", ContextLine, ArgumentValue, ParameterName);
		return false;
	}	

	return true;
}


template <class T> 
bool ExtractAndInterperetArgumentValueAsIntXBit(int ContextLine, std::string Command, std::string ParameterName, std::map<std::string, std::string> ParsedArguments, T &Value)
{
	std::string ArgumentValue;
	if (!GetValueFromMap(ParsedArguments, ParameterName, ArgumentValue))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: %s command must contain parameter %s", ContextLine, Command, ParameterName);
		return false;
	}
	if (!InterperetArgumentValueAsIntXBit(ContextLine, ParameterName, ArgumentValue, Value))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract value %s from parameter %s", ContextLine, ArgumentValue, ParameterName);
		return false;
	}	

	return true;
}


template <class T> 
bool ExtractAndInterperetArgumentValueAsEnum(int ContextLine, std::string Command, std::string ParameterName, std::map<std::string, std::string> ParsedArguments, T &Value, std::string SortedEnumValues)
{
	std::string ArgumentValue;
	if (!GetValueFromMap(ParsedArguments, ParameterName, ArgumentValue))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: %s command must contain parameter %s", ContextLine, Command, ParameterName);
		return false;
	}
	if (!InterperetArgumentValueAsEnum(ContextLine, ParameterName, ArgumentValue, Value, SortedEnumValues))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract value %s from parameter %s", ContextLine, ArgumentValue, ParameterName);
		return false;
	}	

	return true;
}

static std::string CleanCharacter(std::string &Argument, std::string UndesiredChars)
{
	std::string CleanedArgument;

	for (unsigned int i = 0; i < Argument.size(); i++)
	{
		bool HasCharFound = false;
		for (unsigned int iChar = 0; iChar < UndesiredChars.size(); iChar++)
			if (Argument[i] == UndesiredChars[iChar])
			{
				HasCharFound = true;
				break;
			}
		if (!HasCharFound)
			CleanedArgument += Argument[i];
	}

	Argument = CleanedArgument;
	return Argument;
}

static bool InterperetArgumentValueAsStruct(int ContextLine, std::string ParameterName, std::string ArgumentValue, std::vector<std::string> &Value, std::vector<std::string> SortedStructFields)
{
	if (ArgumentValue.find(StructBegin[0]) != 0 /*|| ArgumentValue.find_end(StructBegin[0]) != 0*/)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Syntax Error. Parameter %s expected to be a struct, and '[' should appear as first (and only) character of struct! ArgumentValue=%s", 
			ContextLine, ParameterName.c_str(), ArgumentValue.c_str());
		return false;
	}

	if (ArgumentValue.find(StructEnd[0]) != ArgumentValue.size()-1 /*|| ArgumentValue.find_end(StructBegin[0]) != ArgumentValue.size()-1*/)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Syntax Error. Parameter %s expected to be a struct, and ']' should appear as last (and only) character of struct! ArgumentValue=%s", 
			ContextLine, ParameterName.c_str(), ArgumentValue.c_str());
		return false;
	}

	std::string Brackets;
	Brackets = StructBegin;
	Brackets += StructEnd;

	CTokenParser StructParser(ArgumentValue.c_str());
	
	while (StructParser.MoreTokens())
	{
		Value.push_back(CleanCharacter(StructParser.GetNextToken(StructFieldsDelimeter), Brackets));
	}

	if (SortedStructFields.size() < Value.size())
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Parameter %s expected to have %d fields (%s), yet it contains %d fields (values: %s)!!", 
			ContextLine, ParameterName.c_str(), SortedStructFields.size(), StrVectorToStr(SortedStructFields).c_str(), Value.size(), ArgumentValue.c_str());
		return false;
	}
	if (SortedStructFields.size() > Value.size())
	{
		LogEvent(LE_NOTICE, __FUNCTION__ ": [Line #%d]: Parameter %s expected to have %d fields (%s), yet it contains only %d fields (values: %s). This might be intentional", 
		ContextLine, ParameterName.c_str(), SortedStructFields.size(), StrVectorToStr(SortedStructFields).c_str(), Value.size(), ArgumentValue.c_str());
	}

	return true;
}

static const char* Int = "Int";
static const char* Bit = "Bit";
static const char* Bool = "bool";
static const char* Enum = "enum";
static const char* Struct = "struct";
static const char* Union = "union";
static const char* List = "list";

static bool IsParameterIntXBit(std::string ParameterType)
{
	return (ParameterType.find(Int) == 0 && ParameterType.find(Bit) == ParameterType.size()-3);
}

template <class T> 
bool InterperetArgumentValue(int ContextLine, std::vector<std::string> ParameterDefinitionVector, std::string ArgumentValue, T &Value)
{
	Assert(ParameterDefinitionVector.size() >= 2);

	std::string ParameterName = ParameterDefinitionVector[0];
	std::string ParameterType = ParameterDefinitionVector[1];

	// BASIC TYPES
	if (IsParameterIntXBit(ParameterType))
		return InterperetArgumentValueAsIntXBit(ContextLine, ParameterName, ArgumentValue, Value);
	
	if (ParameterType == Bool)
		return InterperetArgumentValueAsBool(ContextLine, ParameterName, ArgumentValue, (bool &)Value);

	// COMPLEX TYPES (with further definitions)
	std::vector<std::string> FurtherDefinitions;
	for (unsigned int i = 2; i < ParameterDefinitionVector.size(); i++)
		FurtherDefinitions.push_back(ParameterDefinitionVector[i]);

	if (FurtherDefinitions.size() == 0)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Parameter Type is %d, and expected to have at least one %s defined (mentioned in line %d).", 
			ParameterType.c_str(), ((ParameterType == Enum) ? "enum value" : "field"), ContextLine);
		return false;
	}

	if (ParameterType == Enum)
		return InterperetArgumentValueAsEnum(ContextLine, ParameterName, ArgumentValue, Value, FurtherDefinitions);

	if (ParameterType == Struct)
		return InterperetArgumentValueAsStruct(ContextLine, ParameterName, ArgumentValue, (std::vector<std::string> &)Value, FurtherDefinitions);

	if (ParameterType == Union)
		return InterperetArgumentValueAsUnion(ContextLine, ParameterName, ArgumentValue, (std::string &)Value);

	LogEvent(LE_ERROR, __FUNCTION__ ": Unrecognized Parameter Type definition of %s (Unrecognized Type: %s) (mentioned in line %d).", ParameterName.c_str(), ParameterType.c_str(), ContextLine);
	Assert(false);
	return false;
}

template <class T> 
bool InterperetArgumentValue(int ContextLine, std::string Parameter, std::string ArgumentValue, T &Value)
{
	std::vector<std::string> ParameterDefinitionVector = StrToStrVector(Parameter);
	if (ParameterDefinitionVector.size() < 2)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Parameter definition of %s (mentioned in line %d).", Parameter.c_str(), ContextLine);
		return false;
	}

	return InterperetArgumentValue(ContextLine, ParameterDefinitionVector, ArgumentValue, Value);
}


template <class T> 
bool ExtractAndInterperetArgumentValue(int ContextLine, std::string Command, std::string Parameter, std::map<std::string, std::string> ParsedArguments, T &Value)
{
	std::vector<std::string> ParameterDefinitionVector = StrToStrVector(Parameter);
	if (ParameterDefinitionVector.size() < 1)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Parameter definition of %s (mentioned in line %d).", Parameter.c_str(), ContextLine);
		return false;
	}

	std::string ParameterName = ParameterDefinitionVector[0];

	std::string ArgumentValue;
	if (!GetValueFromMap(ParsedArguments, ParameterName, ArgumentValue))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: %s command must contain parameter %s", ContextLine, Command.c_str(), ParameterName.c_str());
		return false;
	}
	if (!InterperetArgumentValue(ContextLine, ParameterDefinitionVector, ArgumentValue, Value))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract value %s from parameter %s", ContextLine, ArgumentValue.c_str(), ParameterName.c_str());
		return false;
	}	

	return true;
}


template <class T> 
bool ExtractAndInterperetStructField(int ContextLine, std::string StructArgument, std::string Field, std::vector<std::string> StructVector, T &Value)
{
	std::vector<std::string> StructDefinitionVector = StrToStrVector(StructArgument);
	std::vector<std::string> FieldDefinitionVector = StrToStrVector(Field);
	if (StructDefinitionVector.size() < 2)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Struct definition of %s (mentioned in line %d).", StructArgument.c_str(), ContextLine);
		return false;
	}
	if (FieldDefinitionVector.size() < 1)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Field definition of field %s of struct %s (mentioned in line %d).", Field.c_str(), StructArgument.c_str(), ContextLine);
		return false;
	}

	std::string StructName = StructDefinitionVector[0];
	std::string StructType = StructDefinitionVector[1];
	std::string FieldName = FieldDefinitionVector[0];

	Assert(StructType == Struct);
	if (StructType != Struct)
	{
		LogEvent(LE_ERROR, __FUNCTION__ "Argument considered as struct (%s) is in fact %s! (mentioned in line %d)",
			StructName.c_str(), StructType.c_str(), ContextLine);
		return false;
	}

	int FieldPosition = -1;
	for (unsigned int i = 2; i < StructDefinitionVector.size(); i++)
	{
		if (StructDefinitionVector[i] == FieldName)
		{
			FieldPosition = i - 2;
			break;
		}
	}

	if (FieldPosition == -1)
	{
		LogEvent(LE_ERROR, __FUNCTION__ "struct %s does not contain a field name %s! (mentioned in line %d)",
			StructName.c_str(), StructType.c_str(), ContextLine);
		return false;
	}

	std::string FieldValue = StructVector[FieldPosition];
	
	if (!InterperetArgumentValue(ContextLine, Field, FieldValue, Value))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract field %s from struct %s", ContextLine, FieldName.c_str(), StructName.c_str());
		return false;
	}	

	return true;
}