#include "VariantArray.h"
#include "VariantSerializer.h"
#include "SerializableType.h"
using namespace Canberra::Utility::Core;

///<summary>
///This is a helper class used to serialize variant information
///</summary>
Canberra::Serialization::VariantSerializer::VariantSerializer(void){}
Canberra::Serialization::VariantSerializer::~VariantSerializer(void){}

///<summary>
///This method will return the number of bytes needed to serialize
///this variant
///</summary>
///<param name="Value">The variant to check the size on</param>
///<returns>The number of bytes</returns>
ULONG Canberra::Serialization::VariantSerializer::GetSize(Variant &Value)
{
	ULONG Size=0;
	switch(Value.GetType()) {
		case Variant::Bool: {
			Canberra::Serialization::Bool v;
			Size=v.GetSize();
			break;
		}
		case Variant::Null: {
			Canberra::Serialization::Null v;
			Size=v.GetSize();
			break;
		}
		case Variant::Ubyte: {
			Canberra::Serialization::Ubyte v;
			Size=v.GetSize();
			break;
		}
		case Variant::Byte: {
			Canberra::Serialization::Byte v;
			Size=v.GetSize();
			break;
		}
		case Variant::Ushort: {
			Canberra::Serialization::Ushort v;
			Size=v.GetSize();
			break;
		}
		case Variant::Short: {
			Canberra::Serialization::Short v;
			Size=v.GetSize();
			break;
		}
		case Variant::Ulong: {
			Canberra::Serialization::Ulong v;
			Size=v.GetSize();
			break;
		}
		case Variant::Long: {
			Canberra::Serialization::Long v;
			Size=v.GetSize();
			break;
		}
		case Variant::Float: {
			Canberra::Serialization::Float v;
			Size=v.GetSize();
			break;
		}
		case Variant::Double: {
			Canberra::Serialization::Double v;
			Size=v.GetSize();
			break;
		}
		case Variant::Wstring: {
			Canberra::Serialization::Wstring v;
			v.PutValue(Value);
			Size=v.GetSize();
			break;
		}
		case Variant::Uint64: {
			Canberra::Serialization::Uint64 v;
			Size=v.GetSize();
			break;
		}
		case Variant::Int64: {
			Canberra::Serialization::Int64 v;
			Size=v.GetSize();
			break;
		}
		case Variant::DateTime: {
			Canberra::Serialization::DateTime v;
			Size=v.GetSize();
			break;
		}
		default:
			if (Value.GetIsArray()) {
				MetaData Hdr(0, 0);
				VariantArray &Arr=const_cast<VariantArray &>(Value.getArrayValue());
				Variant Item;
				Arr.Get(0, Item);
				Size = Arr.GetSize()*GetSize(Item)+sizeof(LONG)+Hdr.GetSize();
			}
			else {
				DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"GetSize", L"Cannot get size unknown type", E_FAIL);
			}
			break;
	}

	return Size;
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<param name="Value">The variant to serialize</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::Serialization::VariantSerializer::Serialize(ByteStream &Str, Variant &Value, const LONG Offset)
{
	if (!Value.GetIsArray()) {
		switch(Value.GetType()) {
			case Variant::Null: {
				Canberra::Serialization::Null v;
				return v.Serialize(Str, Offset);
			}
			case Variant::Bool: {
				Canberra::Serialization::Bool v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Ubyte: {
				Canberra::Serialization::Ubyte v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Byte: {
				Canberra::Serialization::Byte v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Ushort: {
				Canberra::Serialization::Ushort v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Short: {
				Canberra::Serialization::Short v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Ulong: {
				Canberra::Serialization::Ulong v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Long: {
				Canberra::Serialization::Long v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Float: {
				Canberra::Serialization::Float v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Double: {
				Canberra::Serialization::Double v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Wstring: {
				Canberra::Serialization::Wstring v(const_cast<wchar_t*>((const wchar_t*)Value));
				return v.Serialize(Str, Offset);
			}
			case Variant::Uint64: {
				Canberra::Serialization::Uint64 v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::Int64: {
				Canberra::Serialization::Int64 v(Value);
				return v.Serialize(Str, Offset);
			}
			case Variant::DateTime: {
				Canberra::Serialization::DateTime v(Value);
				return v.Serialize(Str, Offset);
			}
			default:
				DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Deserialize", L"Cannot get size unknown type", E_FAIL);
				break;
		}
	}
	else {

		//Create a meta data section
		ULONG Size=GetSize(Value);
		if ((Offset+Size) > Str.GetSize()) Str.Resize(Offset+Size);
		MetaData Spare(0,0);
		MetaData MD(Canberra::Serialization::SerializableObject::Array, Size-Spare.GetSize());
		ULONG Pos=Offset;
		ULONG NumCpy = MD.Serialize(Str, Pos); Pos+=NumCpy;

		//After meta data write the number of array elements
		size_t Num=Value.getArrayValue().GetSize();
		memcpy(const_cast<char *>(Str.GetBuffer())+Pos, &Num, sizeof(Num)); Pos+=sizeof(Num);

		//Write each array element
		HRESULT Stat=S_OK;
		VariantArray &Arr=const_cast<VariantArray &>(Value.getArrayValue());
		Variant Item;
		for(size_t i=0; i<Num; i++) {
			Stat=Arr.Get(i, Item);
			if (FAILED(Stat)) {
				DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Serialize", L"VariantArray::getArrayValue() failed", Stat);
				throw Stat;
			}
			NumCpy = Serialize(Str, Item, Pos); Pos+=NumCpy;
		}
		return Pos-Offset;
	}
	return 0;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<param name="Variant">The variant to get the deserialized data</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::Serialization::VariantSerializer::Deserialize(ByteStream &Str, Variant &Value, const LONG Offset)
{
	Value.Clear();
	MetaData Hdr;
	ULONG Size=0;
	LONG Pos = Offset;
	LONG NumCpy = Hdr.Deserialize(Str, Offset); Pos+=NumCpy;

	if (Str.GetSize() < (Offset+Hdr.GetDataSize() + Hdr.GetSize())) {
		ULONG Stat = (ULONG) E_INVALIDARG;
		DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Deserialize", L"Stream to deserialize is < Header size", Stat);
		throw Stat;
	}

	switch(Hdr.GetDataType()) {
		case SerializableObject::Array: {
			//Extract the number of elements
			size_t Num=0;
			memcpy(&Num, const_cast<char *>(Str.GetBuffer())+Pos, sizeof(Num)); Pos+=sizeof(Num);

			//For each element deserialize and place into the array
			HRESULT Stat=S_OK;
			VariantArray Ret(Variant::Unknown);
			Variant Item;
			for(size_t i=0; i<Num; i++) {
				NumCpy = Deserialize(Str, Item, Pos);  Pos+=NumCpy;
				if (0 == i) {
					Stat=Ret.PutArrayType(Item.GetType());
					if (FAILED(Stat)) {
						DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Deserialize", L"VariantArray::PutArrayType() failed", Stat);
						throw Stat;
					}
				}
				Stat = Ret.Add(Item);
				if (FAILED(Stat)) {
					DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Deserialize", L"VariantArray::Add() failed", Stat);
					throw Stat;
				}
			}
			Size=Pos-Offset;
			Value.Assign(Ret);
			break;
		}
		case SerializableObject::Null: {
			Canberra::Serialization::Null v;
			Size = v.Deserialize(Str, Offset);
			Value.MakeNull();
			break;
		}
		case SerializableObject::Bool: {
			Canberra::Serialization::Bool v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Ubyte: {
			Canberra::Serialization::Ubyte v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Byte: {
			Canberra::Serialization::Byte v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Ushort: {
			Canberra::Serialization::Ushort v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Short: {
			Canberra::Serialization::Short v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Ulong: {
			Canberra::Serialization::Ulong v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Long: {
			Canberra::Serialization::Long v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Float: {
			Canberra::Serialization::Float v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Double: {
			Canberra::Serialization::Double v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::String: {
			Canberra::Serialization::Wstring v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Uint64: {
			Canberra::Serialization::Uint64 v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::Int64: {
			Canberra::Serialization::Int64 v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		case SerializableObject::DateTime: {
			Canberra::Serialization::DateTime v;
			Size = v.Deserialize(Str, Offset);
			Value.Assign(v.GetValue());
			break;
		}
		default:
			DEBUG_ERROR_MESSAGE(L"VariantSerializer", L"Deserialize", L"Cannot get size unknown type", E_FAIL);
			break;
	}
	return Size;
}
