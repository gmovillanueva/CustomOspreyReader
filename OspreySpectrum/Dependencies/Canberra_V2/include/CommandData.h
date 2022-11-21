#pragma once
#include "DataTypesImpExp.h"
#include "ByteStream.h"
#include "SerializableObjectCodes.h"
#include "DebugMacros.h"
#include "CommandCodes.h"

#pragma pack(push, 1)
struct CommandInfoS {
	ULONG Code;						//The code
	USHORT Input;					//The ADC input number being operated on
	USHORT NumArgs;					//The number of arguments
};
#pragma pack(pop)

namespace Canberra {
	namespace DataTypes {
		namespace Application {


			//
			//An instance of this class is used to define a single
			//command used to control the device
			//
			class DATATYPES_DLLIMPORT CommandData : public Canberra::Serialization::SerializableObject
			{
				CommandData(const CommandData&);
				CommandData& operator=(const CommandData&);

			protected:
				struct CommandInfoS _Data;			//The command information
				void * _Args;						//The arguments associated with the command

				//Serializes this instance to a stream
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Deserializes this instance from a stream
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Size of this info in bytes
				ULONG GetDataSize();

			public:
				CommandData();
				CommandData(LONG Code, USHORT Input);
				virtual ~CommandData(void);

				//Accessor to the code
				LONG GetCommandCode();
				void PutCommandCode(LONG v);

				//Accessor to the input
				USHORT GetInput();
				void PutInput(USHORT v);

				//Clears the argument list
				void ClearArguments();

				//Adds an argument to the argument list
				void AddArgument(Canberra::Serialization::SerializableObject *);

				//List of arguments
				Canberra::Serialization::SerializableObject *GetArgument(const USHORT Index);

				//Accessor to the input
				USHORT GetNumberOfArguments();


			};
		}
	}
}
