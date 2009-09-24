#pragma once 
#include "Common/DeSerializer.h"

template<unsigned _MaxLength, typename _T> struct SSerializebleTinyConstArray
{
	SSerializebleTinyConstArray(const _T _Array[], int _ArrayLength)
	{
		Set(_Array, _ArrayLength);
	}

	SSerializebleTinyConstArray() : ArrayLength(0) {ZeroMemory(Array, _MaxLength*sizeof(_T));}

	void Set(const _T _Array[], int _ArrayLength)
	{
		ZeroMemory(Array, _MaxLength*sizeof(_T));

		ArrayLength = _ArrayLength;
		Assert(ArrayLength <= _MaxLength);
		if (ArrayLength > _MaxLength)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": ArrayLength (%d) > _MaxLength (%d)", _ArrayLength, _MaxLength);
			ArrayLength = _MaxLength;
		}
		memcpy(Array, _Array, ArrayLength*sizeof(_T));
	}

	SSerializebleTinyConstArray<_MaxLength, _T>& operator = (const SSerializebleTinyConstArray<_MaxLength, _T>& OtherArray)
	{
		Set(OtherArray.Array, OtherArray.ArrayLength);

		return *this;
	}

	bool operator == (const SSerializebleTinyConstArray<_MaxLength, _T>& Other) const
	{
		Assert(ArrayLength <= _MaxLength);
		Assert(Other.ArrayLength <= _MaxLength);

		if (ArrayLength != Other.ArrayLength)
			return false;

		return CompareBuffers((BYTE *)&Array, (BYTE *)&Other.Array, ArrayLength*sizeof(_T));
	}

	bool Serialize(ISerializer* Serializer) const
	{
		Assert(IsByteValue(ArrayLength));
		Assert(ArrayLength <= _MaxLength);
		bool Result = Serializer->AppendByte((BYTE)ArrayLength);
		Result = Result && Serializer->AppendBuffer((BYTE*)Array, ((UINT)ArrayLength)*sizeof(_T));
		return Result;
	}

	bool DeSerialize(IDeSerializer* DeSerializer)
	{
		BYTE Length;
		bool Result = DeSerializer->GetNextByteField(Length);
		Assert(Length <= _MaxLength);
		if (Length > _MaxLength)
		{
			LogEvent(LE_ERROR, __FUNCTION__ ": ArrayLength (%u) > _MaxLength (%d)", Length, _MaxLength);
			_T* Buf = new _T[Length];
			DeSerializer->GetNextBufferField((BYTE *)Buf, ((UINT)Length)*sizeof(_T));
			ArrayLength = _MaxLength;
			memcpy(Array, Buf, ArrayLength*sizeof(_T));
			delete[] Buf;
			Result = false;
		}
		else
		{
			ZeroMemory(Array, _MaxLength*sizeof(_T));
			ArrayLength = Length;
			Result = Result && DeSerializer->GetNextBufferField((BYTE *)Array, ((UINT)ArrayLength)*sizeof(_T));
		}
		return Result;
	}

	_T Array[_MaxLength];
	int  ArrayLength;
};
