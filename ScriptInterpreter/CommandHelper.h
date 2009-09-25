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
bool BitSize(std::vector<std::string> Dummy) {return 0xDeadBabe;} 
bool BitSize(std::string Dummy) {return 0xDeadBabe;} 

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

	Value = *((T *)&IntegerValue); // cannot use (T)IntegerValue, as you get error C2440 for std::string

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


bool InterperetArgumentValueAsBool(int ContextLine, std::string ParameterName, std::string ArgumentValue, bool &Value)
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

bool InterperetArgumentValueAsUnion(int ContextLine, std::string ParameterName, std::string ArgumentValue, std::string &Value)
{
	Value = ArgumentValue; // what else to do?
	// do nothing at this point. interpretation will be done externally
	return true;
}

template <class T> 
bool InterperetArgumentValueAsEnum(int ContextLine, std::string ParameterName, std::string ArgumentValue, T &Value, std::vector<std::string> SortedEnumValues)
{
	for (unsigned int i = 0; i < SortedEnumValues.size(); i++)
	{
		if (LowerCase(ArgumentValue) == LowerCase(SortedEnumValues[i]))
		{
			Value = *(T *)&i; // cannot use (T)IntegerValue, as you get error C2440 for std::string
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


bool ExtractAndInterperetArgumentValueAsBool(int ContextLine, std::string Command, std::string ParameterName, std::map<std::string, std::string> ParsedArguments, bool &Value)
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

std::string CleanCharacter(std::string &Argument, std::string UndesiredChars)
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

bool InterperetArgumentValueAsStruct(int ContextLine, std::string ParameterName, std::string ArgumentValue, std::vector<std::string> &Value, std::vector<std::string> SortedStructFields)
{
	if (ArgumentValue[0] != StructBegin[0])
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Syntax Error. Parameter %s expected to be a struct, and '[' should appear as first character of struct! ArgumentValue=%s", 
			ContextLine, ParameterName.c_str(), ArgumentValue.c_str());
		return false;
	}

	if (ArgumentValue[ArgumentValue.size()-1] != StructEnd[0])
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Syntax Error. Parameter %s expected to be a struct, and ']' should appear as last character of struct! ArgumentValue=%s", 
			ContextLine, ParameterName.c_str(), ArgumentValue.c_str());
		return false;
	}

	std::string Brackets;
	Brackets = StructBegin;
	Brackets += StructEnd;

	CTokenParser StructParser(ArgumentValue.c_str());
	
	StructParser.GetNextToken(StructBegin); // pass the first '['. important for internal struct recognition
	while (StructParser.MoreTokens())
	{
		CTokenParser InternalStructHelper(StructParser);
		std::string InternalStructCheck = InternalStructHelper.GetNextToken(StructBegin);
		if (InternalStructCheck.size() == 0)
		{
			Value.push_back(StructParser.GetNextToken(StructEnd) + StructEnd);
		}
		else if (InternalStructCheck != StructEnd)
			Value.push_back(CleanCharacter(StructParser.GetNextToken(StructFieldsDelimeter), Brackets));
		else
			StructParser.GetNextToken(StructFieldsDelimeter); // just to pass to next char (which should be last)
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

const char* Int = "Int";
const char* Bit = "Bit";
const char* Bool = "bool";
const char* Enum = "enum";
const char* Struct = "struct";
const char* Union = "union";
const char* List = "list";

bool IsParameterIntXBit(std::string ParameterType)
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
			StructName.c_str(), Field.c_str(), ContextLine);
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


template <class T> 
bool ExtractAndInterperetUnionField(int ContextLine, std::string UnionArgument, std::string Field, std::string UnionValue, T &Value)
{
	std::vector<std::string> UnionDefinitionVector = StrToStrVector(UnionArgument);
	std::vector<std::string> FieldDefinitionVector = StrToStrVector(Field);
	if (UnionDefinitionVector.size() < 2)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Struct definition of %s (mentioned in line %d).", UnionArgument.c_str(), ContextLine);
		return false;
	}
	if (FieldDefinitionVector.size() < 1)
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": Error in Field definition of field %s of struct %s (mentioned in line %d).", Field.c_str(), UnionArgument.c_str(), ContextLine);
		return false;
	}

	std::string UnionName = UnionDefinitionVector[0];
	std::string UnionType = UnionDefinitionVector[1];
	std::string FieldName = FieldDefinitionVector[0];

	Assert(UnionType == Union);
	if (UnionType != Union)
	{
		LogEvent(LE_ERROR, __FUNCTION__ "Argument considered as union (%s) is in fact %s! (mentioned in line %d)",
			UnionName.c_str(), UnionType.c_str(), ContextLine);
		return false;
	}

	bool FieldFound = false;
	for (unsigned int i = 2; i < UnionDefinitionVector.size(); i++)
	{
		if (UnionDefinitionVector[i] == FieldName)
		{
			FieldFound = true;
			break;
		}
	}

	if (!FieldFound)
	{
		LogEvent(LE_ERROR, __FUNCTION__ "union %s does not contain a field name %s! (mentioned in line %d)",
			UnionName.c_str(), Field.c_str(), ContextLine);
		return false;
	}

	std::string FieldValue = UnionValue;

	if (!InterperetArgumentValue(ContextLine, Field, FieldValue, Value))
	{
		LogEvent(LE_ERROR, __FUNCTION__ ": [Line #%d]: Failed to extract field %s from union %s", ContextLine, FieldName.c_str(), UnionName.c_str());
		return false;
	}	

	return true;
}