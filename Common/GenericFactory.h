// GenericFactory.h: interface for the GenericFactory related classes.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <map>

#include "CollectionHelper.h"

#define REGISTER_CLASS(AncestorType, ManufacturedType) \
	GenericFactoryRegistrator<AncestorType, C##ManufacturedType>::RegisterClass(#ManufacturedType)

#define CREATE_CLASS(AncestorType, ManufacturedName) \
	GenericFactory<AncestorType>::GetFactory().Create(ManufacturedName)

#define REGISTER_CLASS_PARAM(AncestorType, ParamType, ManufacturedType) \
	GenericFactoryRegistratorParam<AncestorType, C##ManufacturedType, ParamType>::RegisterClass(#ManufacturedType)

#define CREATE_CLASS_PARAM(AncestorType, ParamType, ManufacturedName, ParamValue) \
	GenericFactoryParam<AncestorType, ParamType>::GetFactory().Create(ManufacturedName, ParamValue)

#define REGISTER_CLASS_2PARAMS(AncestorType, Param1Type, Param2Type, ManufacturedType) \
	GenericFactoryRegistrator2Params<AncestorType, C##ManufacturedType, Param1Type, Param2Type>::RegisterClass(#ManufacturedType)

#define CREATE_CLASS_2PARAMS(AncestorType, Param1Type, Param2Type, ManufacturedName, Param1Value, Param2Value) \
	GenericFactory2Params<AncestorType, Param1Type, Param2Type>::GetFactory().Create(ManufacturedName, Param1Value, Param2Value)

#define REGISTER_CLASS_3PARAMS(AncestorType, Param1Type, Param2Type, Param3Type, ManufacturedType) \
	GenericFactoryRegistrator3Params<AncestorType, C##ManufacturedType, Param1Type, Param2Type, Param3Type>::RegisterClass(#ManufacturedType)

#define CREATE_CLASS_3PARAMS(AncestorType, Param1Type, Param2Type, Param3Type, ManufacturedName, Param1Value, Param2Value, Param3Value) \
	GenericFactory3Params<AncestorType, Param1Type, Param2Type, Param3Type>::GetFactory().Create(ManufacturedName, Param1Value, Param2Value, Param3Value)

// Use:
// #define REGISTER_MYCLASS(ManufacturedType) REGISTER_CLASS_2PARAMS(MyClass, int, const int&, ManufacturedType)
//
// #define CREATE_MYCLASS(ManufacturedName, Param1Value, Param2Value) \
//		CRREATE_CLASS_2PARAMS(MyClass, int, const int&, ManufacturedName, Param1Value, Param2Value)

template <class AncestorType, class ManufacturedType> class GenericFactoryRegistrator
{
public:
	static AncestorType* CreateInstance()
	{
		return new ManufacturedType;
	}

	static bool RegisterClass(const char* Type)
	{
		return GenericFactory<AncestorType>::GetFactory().RegisterCreator(Type, CreateInstance);
	}
};

template <class AncestorType> class GenericFactory
{
public:
	static GenericFactory<AncestorType>& GetFactory()
	{
		static GenericFactory<AncestorType> TheFactory;
		return TheFactory;
	}

	typedef AncestorType* (*CreateFunctionPtr)();

	bool RegisterCreator(const char* Type, CreateFunctionPtr Creator)
    {
        return InsertValueToMap(m_Creators, std::string(Type), Creator);
    }

	AncestorType* Create(const std::string& Type)
	{
		CreatorMapIter Iter = m_Creators.find(Type);
		if (Iter == m_Creators.end())
			return NULL;

		CreateFunctionPtr Creator = Iter->second;
		return Creator();
	}

private:
    typedef std::map<std::string/*Type*/, CreateFunctionPtr> CreatorMap;
    typedef typename CreatorMap::iterator CreatorMapIter;

    CreatorMap m_Creators;
};

template <class AncestorType, class ManufacturedType, class Parameter1Class> class GenericFactoryRegistratorParam
{
public:
	static AncestorType* CreateInstance(Parameter1Class Parameter1)
	{
		return new ManufacturedType(Parameter1);
	}

	static bool RegisterClass(const char* Type)
	{
		return GenericFactoryParam<AncestorType, Parameter1Class>::GetFactory().RegisterCreator(Type, CreateInstance);
	}
};

template <class AncestorType, class Parameter1Class> class GenericFactoryParam
{
public:
	static GenericFactoryParam<AncestorType, Parameter1Class>& GetFactory()
	{
		static GenericFactoryParam<AncestorType, Parameter1Class> TheFactory;
		return TheFactory;
	}

	typedef AncestorType* (*CreateFunctionPtr)(Parameter1Class Parameter1);

	bool RegisterCreator(const char* Type, CreateFunctionPtr Creator)
	{
		return InsertValueToMap(m_Creators, std::string(Type), Creator);
	}

	AncestorType* Create(const std::string& Type, Parameter1Class Parameter1)
	{
		CreatorMapIter Iter = m_Creators.find(Type);
		if (Iter == m_Creators.end())
			return NULL;

		CreateFunctionPtr Creator = Iter->second;
		return Creator(Parameter1);
	}

private:
	typedef std::map<std::string/*Type*/, CreateFunctionPtr> CreatorMap;
	typedef typename CreatorMap::iterator CreatorMapIter;

	CreatorMap m_Creators;
};

template <class AncestorType, class ManufacturedType, class Parameter1Class, class Parameter2Class> class GenericFactoryRegistrator2Params
{
public:
	static AncestorType* CreateInstance(Parameter1Class Parameter1, Parameter2Class Parameter2)
	{
		return new ManufacturedType(Parameter1, Parameter2);
	}

	static bool RegisterClass(const char* Type)
	{
		return GenericFactory2Params<AncestorType, Parameter1Class, Parameter2Class>::GetFactory().RegisterCreator(Type, CreateInstance);
	}
};

template <class AncestorType, class Parameter1Class, class Parameter2Class> class GenericFactory2Params
{
public:
	static GenericFactory2Params<AncestorType, Parameter1Class, Parameter2Class>& GetFactory()
	{
		static GenericFactory2Params<AncestorType, Parameter1Class, Parameter2Class> TheFactory;
		return TheFactory;
	}

	typedef AncestorType* (*CreateFunctionPtr)(Parameter1Class Parameter1, Parameter2Class Parameter2);

	bool RegisterCreator(const char* Type, CreateFunctionPtr Creator)
	{
		return InsertValueToMap(m_Creators, std::string(Type), Creator);
	}

	AncestorType* Create(const std::string& Type, Parameter1Class Parameter1, Parameter2Class Parameter2)
	{
		CreatorMapIter Iter = m_Creators.find(Type);
		if (Iter == m_Creators.end())
			return NULL;

		CreateFunctionPtr Creator = Iter->second;
		return Creator(Parameter1, Parameter2);
	}

private:
	typedef std::map<std::string/*Type*/, CreateFunctionPtr> CreatorMap;
	typedef typename CreatorMap::iterator CreatorMapIter;

	CreatorMap m_Creators;
};

template <class AncestorType, class ManufacturedType, class Parameter1Class, class Parameter2Class, class Parameter3Class> 
class GenericFactoryRegistrator3Params
{
public:
	static AncestorType* CreateInstance(Parameter1Class Parameter1, Parameter2Class Parameter2, Parameter3Class Parameter3)
	{
		return new ManufacturedType(Parameter1, Parameter2, Parameter3);
	}

	static bool RegisterClass(const char* Type)
	{
		return GenericFactory3Params<AncestorType, Parameter1Class, Parameter2Class, Parameter3Class>::GetFactory().RegisterCreator(Type, CreateInstance);
	}
};

template <class AncestorType, class Parameter1Class, class Parameter2Class, class Parameter3Class> class GenericFactory3Params
{
public:
	static GenericFactory3Params<AncestorType, Parameter1Class, Parameter2Class, Parameter3Class>& GetFactory()
	{
		static GenericFactory3Params<AncestorType, Parameter1Class, Parameter2Class, Parameter3Class> TheFactory;
		return TheFactory;
	}

	typedef AncestorType* (*CreateFunctionPtr)(Parameter1Class Parameter1, Parameter2Class Parameter2, Parameter3Class Parameter3);

	bool RegisterCreator(const char* Type, CreateFunctionPtr Creator)
	{
		return InsertValueToMap(m_Creators, std::string(Type), Creator);
	}

	AncestorType* Create(const std::string& Type, Parameter1Class Parameter1, Parameter2Class Parameter2, Parameter3Class Parameter3)
	{
		CreatorMapIter Iter = m_Creators.find(Type);
		if (Iter == m_Creators.end())
			return NULL;

		CreateFunctionPtr Creator = Iter->second;
		return Creator(Parameter1, Parameter2, Parameter3);
	}

private:
	typedef std::map<std::string/*Type*/, CreateFunctionPtr> CreatorMap;
	typedef typename CreatorMap::iterator CreatorMapIter;

	CreatorMap m_Creators;
};
