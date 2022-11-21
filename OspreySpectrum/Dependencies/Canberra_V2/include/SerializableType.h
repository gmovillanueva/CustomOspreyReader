#pragma once
#include "SerializationImpExp.h"
#include "UtilityString.h"
#include "DebugMacros.h"
#include "MetaData.h"
#include "SerializableObject.h"
#include "endianUtils.h"
#include <memory.h>

namespace Canberra {
	namespace Serialization {

		//
		//This template may be used to serializing most data types
		//such as any of the following:
		//	-) Native types: int, uint, short, ushort, double, float, long, ulong, char, uchar
		//	-) Structure types:  Structures must not contain references (pointers)
		//
		//The serialized data will contain a metadata section that defines the information
		//necessary to deserialize the type.  See MetaData.h for all the information in the
		//metadata section.
		//
		//The template may be used to serialize other types provided you subclass this template.
		//IMPORTANT:	If you serialize custom types you must defined its type code uniquely see
		//				SerializedObject.h for an enumeration you can add to
		//
		//See some example serializable types at the bottom of this file
		//
		template <class Value_t>
		class SERIALIZATION_IMPORT_USAGE SerializableType {
			protected:
				Value_t _Value;					//The value to serialize

				ULONG GetValueSize() {return sizeof(Value_t);}
				ULONG SerializeValue(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					LONG Pos=Offset;
					LONG NumCpy=0;
					size_t DataSize=const_cast<MetaData &>(MD).GetDataSize();
					Canberra::Utility::Core::ByteStream Buf(DataSize);

					memcpy(const_cast<char *>(Buf.GetBuffer()), &_Value, DataSize);
					SWAPNBYTES(Buf.GetBuffer(), DataSize);

					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), Buf.GetBuffer(), NumCpy=DataSize); Pos+=NumCpy;
					return Pos-Offset;
				}
				ULONG DeserializeValue(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					LONG Pos=Offset;
					LONG NumCpy=0;
					size_t DataSize=const_cast<MetaData &>(MD).GetDataSize();

					memcpy(&_Value, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=DataSize); Pos+=NumCpy;
					SWAPNBYTES(&_Value, DataSize);
					return Pos-Offset;
				}

			public:
				SerializableType(Value_t v) {PutValue(v); }
				SerializableType() {memset(&_Value, 0, sizeof(_Value));}
				virtual ~SerializableType() {}

				//Accessor to the value to be serialized
				Value_t GetValue() {return _Value;}
				void PutValue(Value_t v) {memcpy(&_Value, &v, sizeof(v));}

			};

			//A serializable float
			class SERIALIZATION_IMPORT_USAGE Float : public SerializableType<float>, public SerializableObject {
			public:
				Float() : SerializableType<float>(0.0F), SerializableObject(SerializableObject::Float) {}
				Float(float v) : SerializableType<float>(v), SerializableObject(SerializableObject::Float) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable bool
			class SERIALIZATION_IMPORT_USAGE Bool : public SerializableType<bool>, public SerializableObject  {
			public:
				Bool() : SerializableType<bool>(false), SerializableObject(SerializableObject::Bool) {}
				Bool(bool v) : SerializableType<bool>(v), SerializableObject(SerializableObject::Bool) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable double
			class SERIALIZATION_IMPORT_USAGE Double : public SerializableType<double>, public SerializableObject {
			public:
				Double() : SerializableType<double>(0.0F), SerializableObject(SerializableObject::Double) {}
				Double(double v) : SerializableType<double>(v), SerializableObject(SerializableObject::Double) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable byte
			class SERIALIZATION_IMPORT_USAGE Byte : public SerializableType<char>, public SerializableObject {
			public:
				Byte() : SerializableType<char>(0), SerializableObject(SerializableObject::Byte) {}
				Byte(char v) : SerializableType<char>(v), SerializableObject(SerializableObject::Byte) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable Int64
			class SERIALIZATION_IMPORT_USAGE Int64 : public SerializableType<LONGLONG>, public SerializableObject {
			public:
				Int64() : SerializableType<LONGLONG>(0), SerializableObject(SerializableObject::Int64) {}
				Int64(LONGLONG v) : SerializableType<LONGLONG>(v) , SerializableObject(SerializableObject::Int64){}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable UInt64
			class SERIALIZATION_IMPORT_USAGE Uint64 : public SerializableType<ULONGULONG>, public SerializableObject {
			public:
				Uint64() : SerializableType<ULONGULONG>(0), SerializableObject(SerializableObject::Uint64) {}
				Uint64(ULONGULONG v) : SerializableType<ULONGULONG>(v), SerializableObject(SerializableObject::Uint64) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable short
			class SERIALIZATION_IMPORT_USAGE Short : public SerializableType<short>, public SerializableObject  {
			public:
				Short() : SerializableType<short>(0), SerializableObject(SerializableObject::Short) {}
				Short(short v) : SerializableType<short>(v), SerializableObject(SerializableObject::Short) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable long
			class SERIALIZATION_IMPORT_USAGE Long : public SerializableType<int>, public SerializableObject   {
			public:
				Long() : SerializableType<int>(0), SerializableObject(SerializableObject::Long) {}
				Long(int v) : SerializableType<int>(v), SerializableObject(SerializableObject::Long) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable ubyte
			class SERIALIZATION_IMPORT_USAGE Ubyte : public SerializableType<unsigned char>, public SerializableObject   {
			public:
				Ubyte() : SerializableType<unsigned char>(0), SerializableObject(SerializableObject::Ubyte) {}
				Ubyte(unsigned char v) : SerializableType<unsigned char>(v), SerializableObject(SerializableObject::Ubyte) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable ushort
			class SERIALIZATION_IMPORT_USAGE Ushort : public SerializableType<unsigned short>, public SerializableObject {
			public:
				Ushort() : SerializableType<unsigned short>(0), SerializableObject(SerializableObject::Ushort) {}
				Ushort(unsigned short v) : SerializableType<unsigned short>(v), SerializableObject(SerializableObject::Ushort) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable ulong
			class SERIALIZATION_IMPORT_USAGE Ulong : public SerializableType<unsigned int>, public SerializableObject {
			public:
				Ulong() : SerializableType<unsigned int>(0), SerializableObject(SerializableObject::Ulong) {}
				Ulong(unsigned int v) : SerializableType<unsigned int>(v), SerializableObject(SerializableObject::Ulong) {}
			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};

			//A serializable Null
			class SERIALIZATION_IMPORT_USAGE Null : public SerializableObject   {
			public:
				Null() : SerializableObject(SerializableObject::Null) {}
				virtual ~Null() {}

			protected:
				///<summary>
				///This method will return the size of the value member
				///of this class which is 0 because it has no data
				///</summary>
				///<returns>The number of bytes</returns>
				virtual ULONG GetDataSize() {return 0;}

				///<summary>
				///This method will do nothing because a Null type has no data
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the write in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes serialized</returns>
				virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &, const MetaData &, const LONG =0)
				{
					return 0;
				}

				///<summary>
				///This method will do nothing because a Null type has no data
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the read in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes deserialized</returns>
				virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &, const MetaData &, const LONG =0) {return 0;}
			};



			//A serializable DateTime
			class SERIALIZATION_IMPORT_USAGE DateTime : public SerializableType<FILETIME>, public SerializableObject   {
			public:

				DateTime() : SerializableType<FILETIME>(), SerializableObject(SerializableObject::DateTime) {}
				explicit DateTime(FILETIME v) : SerializableType<FILETIME>(v), SerializableObject(SerializableObject::DateTime) {}
				virtual ~DateTime() {}

			protected:
				ULONG GetDataSize() {return GetValueSize();}
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return SerializeValue(Str, MD, Offset);
				}
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0) {
					return DeserializeValue(Str, MD, Offset);
				}
			};


			//A serializable UNICODE string
			class SERIALIZATION_IMPORT_USAGE Wstring : public SerializableObject {
			protected:
				Canberra::Utility::Core::String _Value;

			public:
				Wstring() : SerializableObject(SerializableObject::String) {}
				explicit Wstring(wchar_t *v) : SerializableObject(SerializableObject::String) {PutValue(v);}
				virtual ~Wstring() {_Value.Clear();}

				//Accessor to the value
				wchar_t * GetValue() {return (wchar_t *) _Value;}
				void PutValue(const wchar_t * v) {_Value = v;}

			protected:

				//Returns the size of this type (bytes).  Note: Win32 wchar_t is 2 bytes and Unix and Mac-OSX have 4 bytes for wchar_t
				ULONG GetDataSize() {return _Value.GetLength()*2;}

				///<summary>
				///This method will serialize string into the byte stream
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the write in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes serialized</returns>
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
#ifndef HANDLE_UTF8
					LONG ActSize= _Value.GetLength();
					LONG BufSize = ActSize*2+1;				//WIN32 wchar_t is 2 bytes, Unix and Mac is 4 bytes
					std::auto_ptr<char> Buf(new char[BufSize]);
					memset(Buf.get(), 0, sizeof(char)*BufSize);
					char *p = (char *) _Value;
					for(LONG i=0, j=0; j< ActSize; i+=2, j++) {
						Buf.get()[i] = p[j];
					}

					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), Buf.get(), NumCpy=const_cast<MetaData &>(MD).GetDataSize()); Pos+=NumCpy;
#else
					throw E_NOTIMPL
#endif
					return Pos-Offset;
				}

				///<summary>
				///This method will deserialize info from the byte stream into this instance
				///string
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the read in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes deserialized</returns>
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)
				{
#ifndef HANDLE_UTF8
					LONG BufSize=const_cast<MetaData &>(MD).GetDataSize()+2;
					std::auto_ptr<char> Buf(new char[BufSize]);
					if (NULL == Buf.get()) {
						ULONG Stat = (ULONG) E_OUTOFMEMORY;
						DEBUG_ERROR_MESSAGE(L"Wstring", L"Deserialize", L"Failed during memory allocation", Stat);
						throw Stat;
					}
					LONG Pos=Offset;
					LONG NumCpy=0;
					memset(Buf.get(), 0, sizeof(char)*BufSize);
					char *p = Buf.get();
					for(size_t i=0, j=0; j<const_cast<MetaData &>(MD).GetDataSize(); i++, j+=2) {
						p[i] = (const_cast<char *>(Str.GetBuffer()+Pos))[j];
					}
					NumCpy=const_cast<MetaData &>(MD).GetDataSize(); Pos+=NumCpy;
#else
					throw E_NOTIMPL;
#endif
					_Value.CopyFrom(Buf.get());
					return Pos-Offset;
				}
			};

			//A serializable Binary Large Object
			class SERIALIZATION_IMPORT_USAGE Blob : public SerializableObject {

			public:
				Blob() : SerializableObject(SerializableObject::Blob) {}
				explicit Blob(Canberra::Utility::Core::ByteStream &v) : SerializableObject(SerializableObject::Blob) {PutValue(v);}
				virtual ~Blob() {_Value.Clear();}
				void PutValue(Canberra::Utility::Core::ByteStream v) {_Value.Copy(v);}
				void CopyTo(Canberra::Utility::Core::ByteStream &v) {v.Copy(_Value);}

			protected:
				Canberra::Utility::Core::ByteStream _Value;

				//Returns the size of this type (bytes)
				ULONG GetDataSize() {return _Value.GetSize();}

				///<summary>
				///This method will serialize string into the byte stream
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the write in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes serialized</returns>
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)
				{
					LONG Pos=Offset;
					LONG NumCpy=0;
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), const_cast<char *>(_Value.GetBuffer()), NumCpy=const_cast<MetaData &>(MD).GetDataSize()); Pos+=NumCpy;
					return Pos-Offset;
				}

				///<summary>
				///This method will deserialize info from the byte stream into this instance
				///string
				///</summary>
				///<param name="Str">The stream</param>
				///<param name="MD">The metadata</param>
				///<param name="Offset">The byte position to start the read in the stream</param>
				///<exception cref="ULONG">The Win32 error code</exception>
				///<returns>The number of bytes deserialized</returns>
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const MetaData &MD, const LONG Offset=0)
				{
					if (!const_cast<MetaData &>(MD).GetDataSize()) {
						_Value.Clear();
						return 0;
					}
					_Value.Resize(const_cast<MetaData &>(MD).GetDataSize());
					LONG Pos=Offset;
					LONG NumCpy=0;
					memcpy(const_cast<char *>(_Value.GetBuffer()), const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=const_cast<MetaData &>(MD).GetDataSize()); Pos+=NumCpy;
					return Pos-Offset;
				}
			};

	}
}


