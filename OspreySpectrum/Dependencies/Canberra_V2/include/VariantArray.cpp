#include "VariantArray.h"
#include "DebugMacros.h"

namespace Canberra { namespace Utility { namespace Core {

//
//	Constructor
//
VariantArray::VariantArray(Variant::VariantType type, size_t initialSize) :
	m_ArrayType(type), m_size(0), m_pArray(NULL),
	m_capacity(0), m_reallocSize(initialSize/2)
{
	if (initialSize < 1) initialSize = 10;
	if (m_reallocSize < 1) m_reallocSize = 5;
	InsureCapacity(initialSize);
}


VariantArray::VariantArray(const VariantArray& rhs) :
	m_ArrayType(Variant::Unknown), m_size(0), m_pArray(NULL), m_capacity(0)
{
	Copy(rhs);
}


VariantArray::VariantArray(Variant::VariantType type, const Variant& var) :
	m_ArrayType(type), m_size(0), m_pArray(NULL),
	m_capacity(0), m_reallocSize(5)
{
	InsureCapacity(5);
	if (type == var.GetType()) Add(var);
}


VariantArray& VariantArray::operator =( const VariantArray& rhs )
{
	Copy(rhs);
	return *this;
}


void VariantArray::Copy( const VariantArray& rhs )
{
	m_ArrayType = rhs.m_ArrayType;
	m_reallocSize = rhs.m_reallocSize;

	size_t minSize = rhs.m_size + m_reallocSize;
	if (minSize > rhs.m_capacity) minSize = rhs.m_capacity;

	if (m_capacity < minSize)
	{
		InsureCapacity(minSize);
	}

	SetAllVariantsToNull();
	m_size = rhs.m_size;
	for (size_t i = 0; i < m_size; ++i)
	{
		m_pArray[i] = rhs.m_pArray[i];
	}
}


VariantArray::~VariantArray(void)
{
	delete [] m_pArray;
	m_pArray = NULL;
}


//
//	Two arrays are equals if, and only if, their sizes
//	are identical and the array members at each index
//	are also equal
//
bool VariantArray::Equals(const VariantArray& otherArray) const
{
	bool result = (GetSize() == otherArray.GetSize());

	for (size_t i = 0; result && i < GetSize(); ++i)
	{
		Variant v1, v2;
		Get(i, v1);
		otherArray.Get(i, v2);
		result &= (v1.Compare(v2) == 0);
	}

	return result;
}


HRESULT VariantArray::Get(size_t index, Variant& result) const
{
	result.Clear();

	if (index >= m_size || index < 0)
	{
		return E_FAIL;
	}

	result = m_pArray[index];
	return S_OK;
}

HRESULT VariantArray::PutArrayType(Canberra::Utility::Core::Variant::VariantType t) {
	if ((m_ArrayType != t) && (m_size > 0)) {
		DEBUG_ERROR_MESSAGE(L"VariantArray", L"PutArrayType", L"Can not set array type with a populated array.  Invoke Clear() first", (ULONG) E_FAIL);
		return E_FAIL;
	}
	m_ArrayType = t;
	return S_OK;
}

HRESULT VariantArray::Add(const Canberra::Utility::Core::Variant &var)
{
	//
	//	The Variant being added MUST be of the same type as this
	//	collection is defined to hold.
	//
	if (m_ArrayType != var.GetType())
	{
		return E_FAIL;
	}

	//
	//	m_size can be treated as the index of the next available
	//	spot in the array, so it is where we append new entries.
	//
	InsureCapacity(m_size+1);
	m_pArray[m_size++] = var;

	return S_OK;
}


HRESULT VariantArray::Put(size_t index, const Variant& var)
{
	//
	//	The Variant being added MUST be of the same type as this
	//	collection is defined to hold.
	//
	if (m_ArrayType != var.GetType())
	{
		return E_FAIL;
	}

	InsureCapacity(index+1);
	m_pArray[index] = var;

	if (m_size <= index)
	{
		m_size = index + 1;
	}

	return S_OK;
}


HRESULT VariantArray::Insert(size_t index, const Canberra::Utility::Core::Variant &var)
{
	//
	//	The Variant being added MUST be of the same type as this
	//	collection is defined to hold.
	//
	if (m_ArrayType != var.GetType())
	{
		return E_FAIL;
	}

	//
	//	If the insertion point is beyond the current size of the
	//	array, than it become the same operation as a Put
	//
	if (index >= m_size)
	{
		return Put(index, var);
	}

	//
	//	Otherwise, the insert is guaranteed to grow the
	//	the array size by one, so we have to make sure we
	//	have sufficient capacity.
	//
	InsureCapacity(m_size+1);

	//
	//	Move everything from index up to make room for new entry
	//
	for (size_t i = m_size++; i > index; --i)
	{
		m_pArray[i] = m_pArray[i-1];
	}

	m_pArray[index] = var;

	return S_OK;
}


HRESULT VariantArray::Remove(size_t index, Canberra::Utility::Core::Variant &result)
{
	HRESULT hr = Get(index, result);

	if (SUCCEEDED(hr))
	{
		//
		//	Move all entries above index down to fill in the hole
		//	we made by removing the entry at index
		//
		--m_size;
		for (size_t i = index; i < m_size; ++i)
		{
			m_pArray[i] = m_pArray[i+1];
		}
	}

	return hr;
}


//
//	This function sets all Variants in the array to type null and sets
//	the size of the collection to zero.  The Capacity of the collection
//	remains unchanged (i.e., the array itself is not reallocated or
//	deallocated).
//
HRESULT VariantArray::Clear()
{
	m_size = 0;
	return SetAllVariantsToNull();
}


HRESULT VariantArray::SetAllVariantsToNull()
{
	for (size_t i = 0; i < m_capacity; ++i)
	{
		m_pArray[i].MakeNull();
	}

	return S_OK;
}



//
//	Note:	The minimum increase in capacity will be
//			ReallocSize, even if this results in more
//			than the requested capacity.
//
void VariantArray::InsureCapacity(size_t newCapacity)
{
	if (newCapacity < 1) newCapacity = 10;
	if (newCapacity > m_capacity)
	{
		if (newCapacity < (m_capacity + m_reallocSize))
		{
			newCapacity = (m_capacity + m_reallocSize);
		}
		Realloc(newCapacity);
	}
}


//
//	Note:	This function can both grow and shrink the array.
//			If the array is shrunk, the Variants at
//			index > newCapacity-1 will be thrown away.
//
HRESULT VariantArray::Realloc(const size_t newCapacity)
{
	Variant* oldArray = m_pArray;
	size_t oldCapacity = m_capacity;

	m_capacity = newCapacity;
	m_pArray = new Variant[m_capacity];
	SetAllVariantsToNull();

	for (size_t i = 0; i < m_capacity && i < oldCapacity; ++i)
	{
		m_pArray[i] = oldArray[i];
	}

	delete [] oldArray;
	return S_OK;
}


const String VariantArray::ToString(LCID locale) const
{
	String result(L"[");
	String COMMA(", ");
	Variant var;
	for (size_t i = 0; i < GetSize(); i++)
	{
		if (i)
		{
			result += COMMA;
		}

		if (SUCCEEDED(Get(i, var)))
		{
			result += var.ToString(locale);
		}
	}
	result += String("]");
	return result;
}

}}}		// end of namespace
