// MapHelper.h: interface for the CMapHelper class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>

#pragma once

#include <map> //map
#include <set> 
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// A template routine to save a few recurring lines


template <class V> 
bool IsValueInVector(const std::vector<V>& Vector, const V& Key)
{
    typedef std::vector<V> VectorType;

    VectorType::const_iterator Iter;
    Iter = std::find(Vector.begin( ), Vector.end( ), Key);

    if(Iter == Vector.end())
        return false;

    return true;
}

template <class V> 
bool RemoveValueFromVector(std::vector<V>& Vector, const V& Key)
{
    typedef std::vector<V> VectorType;

    VectorType::iterator Iter;
    Iter = std::find(Vector.begin( ), Vector.end( ), Key);

    if(Iter == Vector.end())
        return false;

    Vector.erase(Iter);
    return true;
}

template <class V> 
bool IsValueInSet(const std::set<V>& Set, const V& Key)
{
    typedef std::set<V> SetType;

    SetType::const_iterator Iter;
    Iter = std::find(Set.begin( ), Set.end( ), Key);

    if(Iter == Set.end())
        return false;

    return true;
}

template <class V> 
bool InsertValueToSet(std::set<V>& Set, const V& Key)
{
    typedef std::set<V> SetType;

    std::pair<SetType::iterator, bool> Result = Set.insert(Key);

    if (!Result.second)
    {
        return false;
    }

    return true;
}

template <class K, class D> 
bool InsertValueToMap(std::map<K, D>& Map, const K& Key, const D& Data)
{
    typedef std::map<K, D> MapType;
    MapType::value_type Value(Key, Data);
    std::pair<MapType::iterator, bool> Result = Map.insert(Value);
    return Result.second;
}

template <class K, class D> 
bool InsertValueToMultiMap(std::multimap<K, D>& Map, const K& Key, const D& Data)
{
    typedef std::multimap<K, D> MapType;
    MapType::value_type Value(Key, Data);
    MapType::iterator Iter = Map.insert(Value);
    return Iter != Map.end();
}

template<class K, class D>
bool RemoveValueFromMap(std::map<K, D>& Map, const K& Key)
{
    typedef std::map<K, D> MapType;
    MapType::iterator Iter = Map.find(Key);
    if (Iter == Map.end())
        return false;
    Map.erase(Iter);
    return true;
}

template<class K, class D>
bool IsValueInMap(const std::map<K, D>& Map, const K& Key)
{
    typedef std::map<K, D> MapType;
    MapType::const_iterator Iter = Map.find(Key);
    return (Iter != Map.end());
}

template<class K, class D>
bool GetValueFromMap(const std::map<K, D>& Map, const K& Key, D& Data)
// Returns a copy of the value. Editing the value will not affect the map.
{
    typedef std::map<K, D> MapType;
    MapType::const_iterator Iter = Map.find(Key);
    if (Iter == Map.end())
        return false;
    Data = Iter->second;
    return true;
}

template<class K, class D>
bool GetValueFromMapByIndex(const std::map<K, D>& Map, const unsigned int& Index, D& Data)
// Returns a copy of the value. Editing the value will not affect the map.
{
	if (Map.size() <= Index)
		return false;

	std::map<K, D>::const_iterator Iter = Map.begin();
	for (unsigned int i = 0; i < Index; i++)
		Iter++;

	Data = Iter->second;
	return true;
}

template<class K, class D>
bool GetValueInMap(std::map<K, D>& Map, const K& Key, D* &Data, bool CreateIfNonExistant = false) 
// Returns a pointer to the value. Editing the value will affect the map.
{
	typedef std::map<K, D> MapType;
	MapType::iterator Iter = Map.find(Key);
	if (Iter == Map.end())
	{
		if (!CreateIfNonExistant)
			return false;

		D DefaultData;
		if (!InsertValueToMap(Map, Key, DefaultData))
			return false;

		return GetValueInMap(Map, Key, Data);
	}
	Data = &Iter->second;
	return true;
}


template<class K, class D>
bool GetKeyFromMap(const std::map<K, D>& Map, const D& Data, K& Key)
{
    typedef std::map<K, D> MapType;
    MapType::const_iterator EndIter = Map.end(Key);
    MapType::const_iterator Iter = Map.begin(Key);

    for(; Iter!=EIter ; ++Iter)
    {
        if(Data == Iter->second)
        {
            Key = Iter->first;
            return true;
        }
    }
    
    return false;
}

template<class K, class D>
bool ReplaceValueInMap(std::map<K, D>& Map, const K& Key, const D& Data)
{
    typedef std::map<K, D> MapType;
    MapType::iterator Iter = Map.find(Key);
    if (Iter == Map.end())
        return false;
    Iter->second = Data;
    return true;
}


template<class K, class D>
bool InsertOrReplaceValueInMap(std::map<K, D>& Map, const K& Key, const D& Data)
{
	typedef std::map<K, D> MapType;
	MapType::iterator Iter = Map.find(Key);
	if (Iter == Map.end())
		return InsertValueToMap(Map, Key, Data);
	Iter->second = Data;
	return true;
}

template<class K, class D>
bool GetFirstValueFromMapAndRemove(std::map<K, D>& Map, D& Data)
{
    typedef std::map<K, D> MapType;
    if (Map.empty())
        return false;
    MapType::iterator Iter = Map.begin();
    Data = Iter->second;
    Map.erase(Iter);
    return true;
}

//////////////////////////////////////////////////////////////////////
// CMapHelper

template< class K, class D, class Pred = std::less<K> > class CMapHelper
{
private:
    typedef std::map< K, D, Pred> MapType;
public:
    bool Insert( const K KeyValue, const D DataValue)
    {
        MapType::value_type MapValue( KeyValue, DataValue);
        std::pair<MapType::iterator, bool> Result = m_Map.insert(MapValue);
        return Result.second;
    }
    bool Clear()
    {
        m_Map.clear();
        return m_Map.begin() == m_Map.end();
    }
    unsigned int Size()
    {
        return (unsigned int)m_Map.size();
    }
    bool Empty()
    {
        return (bool)m_Map.empty();
    }
    class iter
    {
    public:
        iter(CMapHelper& MapHelper)
        :   m_Map(MapHelper.m_Map)
        ,   m_Iterator(m_Map.begin())
        ,   m_Init(true)
        {}
        bool Advance()
        {
            if (m_Init)
                m_Init = false;
            else if(Valid())
                m_Iterator++;
            return Valid();
        }
        bool Find(const K& KeyValue)
        {
            m_Iterator = m_Map.find(KeyValue); //??partial string
            return Valid();
        }
        K Key() //carefull: Valid not checked
        {
            //Assert(Valid());
            return m_Iterator->first;
        }
        D Data() //carefull: Valid not checked
        {
            //Assert(Valid());
            return m_Iterator->second;
        }
        void Set(const D& Data) //can be replaced by D& Data() //carefull: Valid not checked
        {
            //Assert(Valid());
            //m_Iterator->second = Data;
            D& MapData = m_Iterator->second;
            MapData = Data;
        }
    private:
        bool Valid()
        {
            return  m_Iterator != m_Map.end();
        }
        MapType& m_Map;
        typename MapType::iterator m_Iterator;
        bool m_Init;
    };
private:
    friend class iter;
    MapType m_Map;
};

//////////////////////////////////////////////////////////////////////
