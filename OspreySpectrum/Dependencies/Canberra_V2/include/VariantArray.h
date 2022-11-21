#pragma once
#include "UtilityString.h"
#include "Variant.h"
#include "UtilityImpExp.h"

using namespace std;


//
//	Currently, the UTILITY_IMPORT_USAGE macro MUST be defined by all users of
//	the Utility DLL (and thus, the Variant class).  This sets up the declarations
//	to import rather than export classes and methods.
//
//	This macro causes the UTILITY_DLLIMPORT and EXPIMP_TEMPLATE macros to be
//	defined as either importing or exporting.  This is particularly important
//	here because we must export our usage of the STL vector<> class.
//

namespace Canberra {
	namespace Utility {
		namespace Core {

			class UTILITY_DLLIMPORT VariantArray
			{
			public:
				VariantArray( Variant::VariantType type, size_t initialSize=10 );
				VariantArray( const VariantArray& rhs );
				VariantArray( Variant::VariantType type, const Variant& var );

				VariantArray& operator =( const VariantArray& rhs );
				void Copy( const VariantArray& rhs );
				virtual ~VariantArray(void);

			private:
				VariantArray( const Variant& var );		// exlicitly prevent this

			public:
				HRESULT Add(const Variant& var);
				HRESULT Get(size_t index, Variant& result) const;
				HRESULT Put(size_t index, const Variant& var);
				HRESULT Insert(size_t index, const Variant& var);
				HRESULT Remove(size_t index, Variant& result);
				HRESULT Clear();
				void InsureCapacity(size_t newCapacity);
				const String ToString(LCID=1033) const;

				inline Variant::VariantType GetArrayType() const { return m_ArrayType; }

				HRESULT PutArrayType(Variant::VariantType v);

				inline size_t GetCapacity() const { return m_capacity; }

				inline size_t GetSize() const { return m_size; }

				inline size_t GetReallocSize() const { return m_reallocSize; }
				inline void SetReallocSize( size_t newIncrement )  { m_reallocSize = newIncrement; }

				bool Equals(const VariantArray& otherArray) const;

			protected:
				HRESULT Realloc(const size_t newCapacity);
				HRESULT SetAllVariantsToNull();

			private:
				Variant::VariantType		m_ArrayType;
				size_t			m_size;
				Variant*		m_pArray;
				size_t			m_capacity;
				size_t			m_reallocSize;
			};
		}
	}
}
