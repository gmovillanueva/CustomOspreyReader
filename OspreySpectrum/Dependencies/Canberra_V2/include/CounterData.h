#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"

#pragma once
namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {
#pragma pack(push, 1)
			//Attributes of a counter
			typedef struct CounterAttributesTag {
				ULONG UncorrValue;		//The uncorrected counter value
				ULONG Value;			//The counter value
				ULONG Flags;			//The counter flags
			} CounterAttributes;
#pragma pack(pop)
			//
			//An instance of this class represents data from
			//the MCA counters
			//
			class DATATYPES_DLLIMPORT CounterData : public Canberra::Serialization::SerializableObject
			{

			public:
				class DATATYPES_DLLIMPORT Sample {
					LONGLONG _StartTime;			//Filetime (UTC)
					LONGLONG _ElapsedTime;			//uS
					ULONG _NumCounters;				//Number of counter values
					CounterAttributes *_Counter;	//The counter attributes
					void Copy(const Sample &v);		//Performs deep copy of instance

				public:
					Sample(const LONGLONG &Start, const LONGLONG &Elapsed, const CounterAttributes * const Cnts, LONG Num);
					Sample();
					virtual ~Sample();
					Sample(const Sample&);
					Sample& operator=(const Sample&);

					//Returns the counter value
					CounterAttributes GetCounter(ULONG i);

					//The elapsed time (uS)
					LONGLONG GetElapsedTime();

					//The start time (Filetime UTC)
					LONGLONG GetStartTime();

					//Access to the number of counts
					ULONG GetNumberOfCounters();

					//Serializes this instance to a stream
					ULONG Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);

					//Deserializes this instance from a stream
					ULONG Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0, const LONG SampSize=0);

					//Size of this info in bytes
					ULONG GetSize();
				};

			protected:
				void *_Samples;						//The samples

				//Deep copy
				void Copy(const CounterData &v);

				//Serializes this instance to a stream
				ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Deserializes this instance from a stream
				ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0);

				//Size of this info in bytes
				ULONG GetDataSize();

			public:
				CounterData& operator=(const CounterData&);
				CounterData(const CounterData&);
				CounterData();
				virtual ~CounterData();

				//Accessor to the number of samples
				ULONG GetNumberOfSamples();

				//Accessor to the sample data
				Sample GetSample(ULONG i);

				//Adds a counter sample
				void Add(const Sample &);

				//Clears the counter sample collection
				void Clear();

				ULONG DeserializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
				ULONG SerializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0);
			};
		}
	}
}
