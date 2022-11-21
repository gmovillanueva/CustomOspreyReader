#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "list.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

			///<summary>
			///This class encapsulates all SCA definitions.  An SCA definition contains the values for
			///the LLD and ULD of the SCA.
			///</summary>
			class DATATYPES_DLLIMPORT SCAdefinitions : public Canberra::Serialization::SerializableObject {
			public:

#pragma pack(push, 1)
				//This defines a single SCA
				typedef struct DefinitionTag {
					float LLD;			//ULD (%)
					float ULD;			//LLD (%)
				} Definition;
#pragma pack(pop)

				//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT Canberra::Utility::Core::SerializableList<Canberra::DSA3K::DataTypes::SCAdefinitions::Definition> ;

			protected:
				Canberra::Utility::Core::SerializableList<Definition> _Data;	//The data
				void Copy(const SCAdefinitions &v);
				void HandleEndianness();

			public:
				SCAdefinitions(const SCAdefinitions &v);
				SCAdefinitions();
				virtual ~SCAdefinitions();

				//Deep copy during assignment
				SCAdefinitions& operator=( const SCAdefinitions &Rhs);

				//Serialization stuff
				ULONG GetDataSize();
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Adds a def
				ULONG Add(const Definition &v);

				//Clears all defs
				void Clear();

				//Accessor to a definition
				Definition &GetDefinition(ULONG i);

				//Accessor to # of definitions
				ULONG GetNumberOfDefinitions();
			};

			///<summary>
			///This class encapsulates all SCA data associated within a time period of a SCA
			///acquisition.
			///</summary>
			class DATATYPES_DLLIMPORT SCAbuffer : public Canberra::Serialization::SerializableObject {
			public:

				///<summary>
				///This class encapsulates all SCA data associated with a specific instance in time
				///Basically, there is one entry per SCA preset.  When all SCA's reach the SCA preset
				///time an entry is created in the buffer
				///</summary>
				class DATATYPES_DLLIMPORT BufferEntry {
				public:
#pragma pack(push, 1)
					//This defines the header for each buffer datum
					typedef struct EntryHeaderTag {
						LONGLONG StartTime;			//Start time when this sample was acquired
						LONGLONG ElapsedReal;		//The elapsed real time uS
						LONGLONG ElapsedLive;		//The elapsed live time uS
						ULONG Flags;				//The flags
						ULONG Spare;
					} EntryHeader;

					//The SCA data
					typedef struct SCAdataTag {
						DWORD Counter;				//The counter value
						DWORD Flags;				//The counter flags (overflow, etc..)
					} SCAdata;
#pragma pack(pop)
					//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT Canberra::Utility::Core::SerializableList<Canberra::DSA3K::DataTypes::SCAbuffer::BufferEntry::SCAdata> ;

				protected:
					EntryHeader _Header;										//The header
					Canberra::Utility::Core::SerializableList<SCAdata> _Data;	//The data
					void Copy(const BufferEntry &v);							//Deep copy
					void HandleEndianness();

				public:
					BufferEntry(const BufferEntry &v);
					BufferEntry();
					virtual ~BufferEntry();

					BufferEntry& operator=( const BufferEntry &Rhs);

					//Gets the header for this entry
					EntryHeader & GetHeader();

					//Gets the data for the specified SCA#
					SCAdata & GetData(LONG i);

					//Gets the number of data elements (i.e., # of SCA's)
					ULONG GetNumberOfData();

					//Clears all data
					void Clear();

					//Serialization stuff
					ULONG GetDataSize();
					ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
					ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				};

#pragma pack(push, 1)
				//This defines the header for the entire buffer
				typedef struct BufferHeaderTag {
					LONGLONG StartTime;			//Start time when acq was started
					ULONG Flags;				//Flags
					ULONG BytesPerSample;		//Bytes per sample
				} BufferHeader;
#pragma pack(pop)

				//EXPIMP_TEMPLATE template class DATATYPES_DLLIMPORT Canberra::Utility::Core::List<Canberra::DSA3K::DataTypes::SCAbuffer::BufferEntry> ;

			protected:
				BufferHeader _Header;											//The buffer header
				Canberra::Utility::Core::List<BufferEntry> _Data;				//The entries
				void Copy(const SCAbuffer &v);									//Deep copy
				void HandleEndianness();

			public:
				SCAbuffer(const SCAbuffer &v);
				SCAbuffer();
				virtual ~SCAbuffer();

				SCAbuffer& operator=( const SCAbuffer &Rhs);

				//Serialization stuff
				ULONG GetDataSize();
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Clears the buffer
				void Clear();

				//Gets a buffer entry (i.e., one sample in time)
				BufferEntry & GetEntry(ULONG i);

				//Gets the associated header
				BufferHeader & GetHeader();

				//Accessor to # of buffered elements
				ULONG GetNumberOfEntries();

			};

		}
	}
}

