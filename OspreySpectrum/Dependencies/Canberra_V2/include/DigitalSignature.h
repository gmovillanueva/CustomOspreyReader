#pragma once
#include "DataTypesImpExp.h"
#include "ByteStream.h"
#include "Serializable.h"
#if (defined(WIN32) && defined(SECURE))
#include <wincrypt.h>
#endif

namespace Canberra {
	namespace DataTypes {
		namespace Serialization {

			//
			//This class encapsulates a digital signature
			//
			class DATATYPES_DLLIMPORT DigitalSignature : public Canberra::Serialization::Serializable
			{
				DigitalSignature(const DigitalSignature&);
				DigitalSignature& operator=(const DigitalSignature&);

			protected:
				Canberra::Utility::Core::ByteStream _Key;				//The public key
				Canberra::Utility::Core::ByteStream _Signature;			//The digital signature
				Canberra::Utility::Core::ByteStream _RandomData;		//The random signature data
				bool _ReverseBytes;										//State indicating whether byte order should be reversed (req'd for .NET and Java)
				bool _IncludeRandomData;								//State indicating whether random data is added to the signature to improve security
				bool _EnableSigning;									//This enables signing
#if (defined(WIN32) && defined(SECURE))
				HCRYPTPROV _Provider;									//The security provider
				HCRYPTKEY _UserKey;										//The user key
				ULONG _HashAlgorithm;									//The hash algorithm to use
#endif
				void Allocate();										//Allocates security info
				void Release();											//Releases security info

			public:
				DigitalSignature(bool);
				DigitalSignature(void);
				virtual ~DigitalSignature(void);

				//Creates a signature and public key
				ULONG Sign(const Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

				//Verifies the cached signature
				ULONG Verify(const Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

				//Serializes the classes information into this stream
				ULONG Serialize(Canberra::Utility::Core::ByteStream &, const LONG Offset=0);

				//Deserializes the information in the stream into the class instance
				ULONG Deserialize(Canberra::Utility::Core::ByteStream &,const  LONG Offset=0);

				///<summary>
				///Accessor to enable signing
				///</summary>
				///<exceptions cref="ULONG">Win32 error code </exceptions>
				bool GetEnableSigning();
				void PutEnableSigning(bool v);

				//Accessor to the reverse bytes state.  This is req'd for .NET and Java clients
				bool GetReverseByte();
				void PutReverseByte(bool v);

				//Accessor to state indicating whether random data is added to the stream to improve security
				bool GetIncludeRandomData();
				void PutIncludeRandomData(bool v);

				//Returns the size of the signature block
				ULONG GetSize();

#if (defined(WIN32) && defined(SECURE))
				//The hash alg identifier
				ULONG GetHashAlorithmId();
				void PutHashAlorithmId(ULONG v);
#endif

			};
		}
	}
}
