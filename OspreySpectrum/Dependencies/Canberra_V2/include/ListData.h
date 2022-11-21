#pragma once
#include "DataTypesImpExp.h"
#include "SerializableObjectCodes.h"
#include "DebugMacros.h"

namespace Canberra {
	namespace DataTypes {
		namespace Spectroscopy {

			//List flags
			typedef enum ListFlagsTag {
				listOverflowBufferA=0x1,			//First buffer overflowed
				listOverflowBufferB=0x2,			//Second buffer overflowed
				listMissingData=0x4,				//Failed to send previous data buffers.  Hence, data is missing
				listTimeOverflowMask=0x8000,		//Time event overflowed
				listVersionMask=0xFFFF0000			//Version information
			} ListFlags;

			//
			//This is a base class from which all list data derives.  It contains
			//data that is common amongst all list data
			//
			class DATATYPES_DLLIMPORT ListDataBase : public Canberra::Serialization::SerializableObject
			{
			protected:
				ULONGULONG _StartTime;		//The start time (FILETIME)
				ULONGULONG _RealTime;			//Real time uSec
				ULONGULONG _LiveTime;			//Live time uSec
				ULONG _Timebase;					//The time base clock interval (nS)
				ULONG _Flags;						//The flags
				ULONG _NumEvents;					//The number of events

				//Serializes this instance to a stream
				virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /*MD*/, const LONG Offset=0) {
					if (Str.GetSize() < GetDataSize()) {Str.Resize(GetDataSize());}
					LONG Pos=Offset;
					LONG NumCpy=0;
					SWAPNBYTES(&_StartTime, sizeof(_StartTime));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_StartTime, NumCpy=sizeof(_StartTime)); Pos+=NumCpy;
					SWAPNBYTES(&_RealTime, sizeof(_RealTime));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_RealTime, NumCpy=sizeof(_RealTime)); Pos+=NumCpy;
					SWAPNBYTES(&_LiveTime, sizeof(_LiveTime));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_LiveTime, NumCpy=sizeof(_LiveTime)); Pos+=NumCpy;
					SWAPNBYTES(&_Timebase, sizeof(_Timebase));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Timebase, NumCpy=sizeof(_Timebase)); Pos+=NumCpy;
					SWAPNBYTES(&_Flags, sizeof(_Flags));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_Flags, NumCpy=sizeof(_Flags)); Pos+=NumCpy;
					SWAPNBYTES(&_NumEvents, sizeof(_NumEvents));
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_NumEvents, NumCpy=sizeof(_NumEvents)); Pos+=NumCpy;

					return Pos-Offset;
				}

				//Deserializes this instance from a stream
				virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData & /*MD*/, const LONG Offset=0) {
					LONG Pos=Offset;
					LONG NumCpy=0;
					memcpy(&_StartTime, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_StartTime)); Pos+=NumCpy;
					SWAPNBYTES(&_StartTime, sizeof(_StartTime));
					memcpy(&_RealTime, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_RealTime)); Pos+=NumCpy;
					SWAPNBYTES(&_RealTime, sizeof(_RealTime));
					memcpy(&_LiveTime, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_LiveTime)); Pos+=NumCpy;
					SWAPNBYTES(&_LiveTime, sizeof(_LiveTime));
					memcpy(&_Timebase, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Timebase)); Pos+=NumCpy;
					SWAPNBYTES(&_Timebase, sizeof(_Timebase));
					memcpy(&_Flags, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_Flags)); Pos+=NumCpy;
					SWAPNBYTES(&_Flags, sizeof(_Flags));
					_NumEvents = 0;
					LONG Version=((_Flags&listVersionMask)>>16);
					if (Version > 0) {
						ULONG Stat=DSA3K_UNSUPPORTED_LISTFORMAT;
						DEBUG_ERROR_MESSAGE(L"ListDataBase", L"DeserializeData", L"Detected a version of the list data format that is not supported", Stat);
						throw Stat;
					}
					memcpy(&_NumEvents, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_NumEvents)); Pos+=NumCpy;
					SWAPNBYTES(&_NumEvents, sizeof(_NumEvents));
					return Pos-Offset;
				}

				//Size of this info in bytes
				virtual ULONG GetDataSize() {
					return sizeof(_StartTime) + sizeof(_RealTime) + sizeof(_Flags) + sizeof(_LiveTime) + sizeof(_NumEvents) + sizeof(_Timebase);
				}

			public:
				ListDataBase(LONG ListTypeT) : Canberra::Serialization::SerializableObject(ListTypeT) {Clear();}
				ListDataBase(LONG ListTypeT, LONGLONG StartT, ULONGULONG RealT, ULONGULONG LiveT, ULONG TimeB, ULONG Flgs, LONG NumEvents) : Canberra::Serialization::SerializableObject(ListTypeT) {
					Clear();
					_StartTime=StartT;
					_RealTime=RealT;
					_LiveTime = LiveT;
					_NumEvents = NumEvents;
					_Timebase=TimeB;
					_Flags=Flgs;
				}
				virtual ~ListDataBase() {Clear();}

				//Accessor to the number of events
				ULONG GetNumberOfEvents() {return _NumEvents;}

				//Accessor to the start time (FILETIME)
				LONGLONG GetStartTime() {return _StartTime;}

				//Accessor to the live time (uS)
				ULONGULONG GetLiveTime() {return _LiveTime;}

				//Accessor to the real time (uS)
				ULONGULONG GetRealTime() {return _RealTime;}

				//Accessor to the time base (nS)
				ULONG GetTimebase() {return _Timebase;}

				//Accessor to the flags
				ULONG GetFlags() {return _Flags;}

				//Clears the counter sample collection
				virtual void Clear() {
					_StartTime=0;
					_RealTime=0;
					_LiveTime=0;
					_NumEvents=0;
					_Timebase=0;
				}

				ULONG DeserializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0) {
					return DeserializeData(Str, Canberra::Serialization::MetaData(), Offset);
				}
				ULONG SerializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0) {
					return SerializeData(Str, Canberra::Serialization::MetaData(), Offset);
				}
			};

			//
			//This is a template that contains the generic portion
			//of list data
			//
			template<class EventT, const LONG ListTypeT>
			class ListDataImpl : public ListDataBase
			{
			protected:
				EventT *_Events;					//The events

				//Deep copy
				virtual void Copy(const ListDataImpl<EventT, ListTypeT> &Src) {
					Clear();
					_StartTime=Src._StartTime;
					_RealTime=Src._RealTime;
					_LiveTime=Src._LiveTime;
					_NumEvents=Src._NumEvents;
					_Timebase=Src._Timebase;
					_Flags=Src._Flags;
					if (Src._Events) {
						_Events = new EventT[_NumEvents];
						memcpy(_Events, Src._Events, sizeof(EventT)*_NumEvents);
					}
				}

				//Handles the endianness of the platform
				//This method is invoked before and after serialization
				virtual void HandleEndianness() {}

				//Serializes this instance to a stream
				virtual ULONG SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0) {
					if (Str.GetSize() < GetDataSize()) {Str.Resize(GetDataSize());}
					LONG Pos=Offset;
					LONG NumCpy=0;
					NumCpy = ListDataBase::SerializeData(Str, MD, Offset); Pos += NumCpy;
					HandleEndianness();
					memcpy(const_cast<char *>(Str.GetBuffer()+Pos), _Events, NumCpy=sizeof(EventT)*_NumEvents); Pos+=NumCpy;
					HandleEndianness();
					return Pos-Offset;
				}

				//Deserializes this instance from a stream
				virtual ULONG DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &MD, const LONG Offset=0) {

					LONG Pos=Offset;
					LONG NumCpy=0;
					NumCpy = ListDataBase::DeserializeData(Str, MD, Offset); Pos += NumCpy;
					if (_Events) {delete [] _Events; _Events=NULL;}
					if (_NumEvents) {
						_Events = new EventT[_NumEvents];
						if (!_Events) {
							ULONG Stat=(ULONG) E_OUTOFMEMORY;
							DEBUG_ERROR_MESSAGE(L"ListDataImpl", L"DeserializeData", L"Failed to allocate memory.  Size may be incorrenct", Stat);
							throw Stat;
						}
						memcpy(_Events, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(EventT)*_NumEvents); Pos+=NumCpy;
					}
					HandleEndianness();
					return Pos-Offset;
				}

				//Size of this info in bytes
				virtual ULONG GetDataSize() {
					return ListDataBase::GetDataSize() + sizeof(EventT)*_NumEvents;
				}

			public:
				ListDataImpl<EventT, ListTypeT>& operator=(const ListDataImpl<EventT, ListTypeT>& Rhs) {Copy(Rhs); return *this;}
				ListDataImpl<EventT, ListTypeT>(const ListDataImpl<EventT, ListTypeT>& Rhs) : ListDataBase(ListTypeT), _Events(NULL)  {Copy(Rhs);}
				ListDataImpl<EventT, ListTypeT>() : ListDataBase(ListTypeT), _Events(NULL)  {Clear();}
				ListDataImpl<EventT, ListTypeT>(LONGLONG StartT, ULONGULONG RealT, ULONGULONG LiveT, ULONG TimeB, ULONG Flgs, LONG NumEvents, EventT *Ev) : ListDataBase(ListTypeT, StartT, RealT, LiveT, TimeB, Flgs, NumEvents), _Events(NULL) {
					if (NumEvents && Ev) {
						_Events=new EventT[NumEvents];
						if (!_Events) {
							DEBUG_ERROR_MESSAGE(L"ListDataImpl", L"ctor", L"Error allocating memory", E_OUTOFMEMORY);
							throw (ULONG) E_OUTOFMEMORY;
						}
						memcpy(_Events, Ev, sizeof(EventT)*NumEvents);
					}
					else {
						_NumEvents=0;
					}
				}
				virtual ~ListDataImpl<EventT, ListTypeT>() {Clear();}

				//Accessor to the events
				virtual EventT GetEvent(LONG i) {
					if ((i > -1) && (i < (LONG) _NumEvents) && _Events) {
						return _Events[i];
					}
					else {
						EventT Data;
						memset(&Data, 0, sizeof(Data));
						return Data;
					}
				}

				//Accessor to the event pointer
				EventT * GetEventPtr() {return _Events;}

				//Clears the counter sample collection
				virtual void Clear() {
					ListDataBase::Clear();
					if (_Events) {delete [] _Events; _Events=NULL;}
				}

				virtual ULONG DeserializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0) {
					return DeserializeData(Str, Canberra::Serialization::MetaData(), Offset);
				}
				virtual ULONG SerializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset=0) {
					return SerializeData(Str, Canberra::Serialization::MetaData(), Offset);
				}
			};

#pragma pack(push, 1)
			//Data specific for List mode
			struct ListDataS {
				DWORD Event;	//The event
			};
#pragma pack(pop)
			//template class DATATYPES_DLLIMPORT ListDataImpl<struct ListDataS, Canberra::DataTypes::Serialization::SerializableObjects::ListData> ;

			//
			//An instance of this class defines all of the
			//measurement data associated with List mode
			//
			class DATATYPES_DLLIMPORT ListData : public ListDataImpl<struct ListDataS, Canberra::DataTypes::Serialization::SerializableObjects::ListData>{
			public:
				ListData() {}
				ListData(LONGLONG StartT, ULONGULONG RealT, ULONGULONG LiveT, ULONG TimeB, ULONG Flgs, ULONG NumData, struct ListDataS *Data) : ListDataImpl<struct ListDataS, Canberra::DataTypes::Serialization::SerializableObjects::ListData>(StartT, RealT, LiveT, TimeB, Flgs, NumData, Data) {}
				virtual ~ListData(){}
			protected:
				void HandleEndianness()
				{
					for(ULONG i=0; i<_NumEvents; i++) {
						SWAPNBYTES(&(_Events[i].Event), sizeof(_Events[i].Event));
					}
				}
			};

#pragma pack(push, 1)
			//Data specific for TList mode
			struct TlistDataS {
				USHORT Event;	//The event
				USHORT Time;	//The time (tick count)
			};
#pragma pack(pop)
			//template class DATATYPES_DLLIMPORT ListDataImpl<struct TlistDataS, Canberra::DataTypes::Serialization::SerializableObjects::TlistData> ;

			//
			//An instance of this class defines all of the
			//measurement data associated with Timestamped List mode
			//
			class DATATYPES_DLLIMPORT TlistData : public ListDataImpl<struct TlistDataS, Canberra::DataTypes::Serialization::SerializableObjects::TlistData>{
			protected:
					void HandleEndianness()
					{
						for(ULONG i=0; i<_NumEvents; i++) {
							SWAPNBYTES(&(_Events[i].Event), sizeof(_Events[i].Event));
							SWAPNBYTES(&(_Events[i].Time), sizeof(_Events[i].Time));
						}
					}
			public:
					TlistData() {}
					TlistData(LONGLONG StartT, ULONGULONG RealT, ULONGULONG LastRo, ULONG TimeB, ULONG Flgs, ULONG NumData, struct TlistDataS *Data) : ListDataImpl<struct TlistDataS, Canberra::DataTypes::Serialization::SerializableObjects::TlistData>(StartT, RealT, LastRo, TimeB, Flgs, NumData, Data) {}
					virtual ~TlistData(){}
			};
		}
	}
}
