#include "CounterData.h"
#include "ApplicationSerializer.h"
#include "SerializableType.h"
#include <vector>
#include <memory.h>

typedef std::vector< Canberra::DataTypes::Spectroscopy::CounterData::Sample > Samples;

///<summary>
///An instance of this class is used to encapsulate all
///information about all counter values
///</summary>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Spectroscopy::CounterData::Sample::Sample(void) {
	_Counter = NULL;
	_NumCounters=0;
	_StartTime=0;
	_ElapsedTime=0;
}

///<summary>
///An instance of this class is used to encapsulate all
///information about all counter values
///</summary>
///<param name="Start">The start time</param>
///<param name="Elapsed">The elapsed time (uS)</param>
///<param name="Cnts">The counter attributes</param>
///<param name="Num">The number of counter attributes</param>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Spectroscopy::CounterData::Sample::Sample(const LONGLONG &Start, const LONGLONG &Elapsed,const  CounterAttributes * const Cnts, LONG Num) {
	if ((NULL == Cnts) || (Num < 0)) throw (ULONG) E_INVALIDARG;
	_Counter=NULL;
	_NumCounters = Num;
	_StartTime=Start;
	_ElapsedTime=Elapsed;
	if (Cnts) {
		_Counter = new CounterAttributes[Num];
		if (NULL == _Counter) throw (ULONG) E_OUTOFMEMORY;
		memcpy(_Counter, const_cast<CounterAttributes *>(Cnts), Num*sizeof(CounterAttributes));
	}
}

Canberra::DataTypes::Spectroscopy::CounterData::Sample::~Sample(void) {
	if (NULL != _Counter) delete [] _Counter;
	_Counter = NULL;
	_NumCounters = 0;
	_StartTime=0;
	_ElapsedTime=0;
}

///<summary>
///This method will get a the counts from the specified counter
///</summary>
///<param name="i">The counter to get</param>
///<returns>The value</returns>
Canberra::DataTypes::Spectroscopy::CounterAttributes Canberra::DataTypes::Spectroscopy::CounterData::Sample::GetCounter(ULONG i) {
	CounterAttributes Null;
	memset(&Null, 0, sizeof(CounterAttributes));
	if (!_Counter) return Null;

	if (!_Counter || (i >= _NumCounters)) {
		DEBUG_ERROR_MESSAGE(L"CounterData::Sample", L"GetCount", "Exceeding array dim", E_FAIL);
		return Null;
	}
	return _Counter[i];
}

///<summary>
///This method will get the number of counters
///</summary>
///<returns>The value</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::GetNumberOfCounters() {
	return _NumCounters;
}

///<summary>
///This method will get the start time
///</summary>
///<returns>The value</returns>
LONGLONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::GetStartTime() {
	return _StartTime;
}

///<summary>
///This method will get the elapsed time (uS)
///</summary>
///<returns>The value</returns>
LONGLONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::GetElapsedTime() {
	return _ElapsedTime;
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::Serialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;

	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_StartTime, NumCpy=sizeof(_StartTime)); Pos+=NumCpy;
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_ElapsedTime, NumCpy=sizeof(_ElapsedTime)); Pos+=NumCpy;
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &_NumCounters, NumCpy=sizeof(_NumCounters)); Pos+=NumCpy;
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), _Counter, NumCpy=(sizeof(CounterAttributes)*_NumCounters)); Pos+=NumCpy;
	return Pos-Offset;
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<param name="NumAnalog">The number of analog signals</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::Deserialize(Canberra::Utility::Core::ByteStream &Str, const LONG Offset, const LONG SampSize)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;

	if (_Counter) {delete [] _Counter; _Counter=NULL;}

	memcpy(&_StartTime, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_StartTime)); Pos+=NumCpy;
	memcpy(&_ElapsedTime, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_ElapsedTime)); Pos+=NumCpy;
	memcpy(&_NumCounters, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(_NumCounters)); Pos+=NumCpy;
	if (_Counter) {delete [] _Counter; _Counter=NULL;}
	if (_NumCounters) {
		_Counter = new CounterAttributes[_NumCounters];
		if (NULL == _Counter) {
			throw (ULONG) E_OUTOFMEMORY;
		}
		memcpy(_Counter, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=(sizeof(CounterAttributes)*_NumCounters)); Pos+=NumCpy;
	}
	if (SampSize > (Pos-Offset)) {
		Pos += SampSize - (Pos-Offset);
	}
	return Pos-Offset;
}


///<summary>
///This method will returns the number of bytes this
///instance manages for serialization
///</summary>
///<returns>The number of bytes to serialize</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::Sample::GetSize()
{
	LONG Size = sizeof(_StartTime);
	Size +=  sizeof(_ElapsedTime);
	Size +=  sizeof(_NumCounters);
	Size += _NumCounters*sizeof(CounterAttributes);
	return Size;
}

///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::CounterData::Sample::Copy(const Canberra::DataTypes::Spectroscopy::CounterData::Sample &v) {
	_StartTime = v._StartTime;
	_ElapsedTime = v._ElapsedTime;
	_NumCounters = v._NumCounters;
	delete [] _Counter; _Counter=NULL;
	if (_NumCounters) {
		_Counter = new CounterAttributes[_NumCounters];
		memcpy(_Counter, v._Counter, sizeof(CounterAttributes)*_NumCounters);
	}
}
Canberra::DataTypes::Spectroscopy::CounterData::Sample& Canberra::DataTypes::Spectroscopy::CounterData::Sample::operator=( const Sample &Rhs) {Copy(Rhs); return *this;}


///<summary>
///This method will initialize an instance of this class
///</summary>
///<param name="v">The src</param>
Canberra::DataTypes::Spectroscopy::CounterData::Sample::Sample(const Sample &v) {_Counter=NULL; Copy(v);}


///<summary>
///An instance of this class is used to encapsulate all counter
///information
///</summary>
///<exception cref="ULONG">The Win32 error code</exception>
Canberra::DataTypes::Spectroscopy::CounterData::CounterData(void) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::CounterData) {
	_Samples = new Samples;
	if (NULL == _Samples) throw (ULONG) E_OUTOFMEMORY;
}

Canberra::DataTypes::Spectroscopy::CounterData::~CounterData(void) {
	if (NULL != _Samples) {
		Samples *Data = reinterpret_cast<Samples *>(_Samples);
		delete Data;
		_Samples = NULL;
	}
}

//Accessor to the number of samples
ULONG Canberra::DataTypes::Spectroscopy::CounterData::GetNumberOfSamples() {
	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	if (!Data) return 0;
	return Data->size();
}

//Accessor to the sample
Canberra::DataTypes::Spectroscopy::CounterData::Sample Canberra::DataTypes::Spectroscopy::CounterData::GetSample(ULONG i) {
	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	Sample Samp;
	if (!Data || (i >= (ULONG) Data->size())) return Samp;
	return (*Data)[i];
}

///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::SerializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;
	USHORT Tmp=(USHORT) GetNumberOfSamples();

	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &Tmp, NumCpy=sizeof(Tmp)); Pos+=NumCpy;
	Tmp = (USHORT) GetSample(0).GetSize();
	memcpy(const_cast<char *>(Str.GetBuffer()+Pos), &Tmp, NumCpy=sizeof(Tmp)); Pos+=NumCpy;

	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	for(Samples::iterator It=(*Data).begin(); It != (*Data).end(); It++) {
		Pos += It->Serialize(Str, Pos);
	}

	return Pos-Offset;
}


///<summary>
///This method will serialize this instance into the byte stream
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the write in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes serialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::SerializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	return SerializeData(Str, Canberra::Serialization::MetaData::MetaData(), Offset);
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::DeserializeDataEx(Canberra::Utility::Core::ByteStream &Str, const LONG Offset)
{
	return DeserializeData(Str, Canberra::Serialization::MetaData::MetaData(), Offset);
}

///<summary>
///This method will deserialize info from the byte stream into this instance
///</summary>
///<param name="Str">The stream</param>
///<param name="MD">The metadata</param>
///<param name="Offset">The byte position to start the read in the stream</param>
///<exception cref="ULONG">The Win32 error code</exception>
///<returns>The number of bytes deserialized</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::DeserializeData(Canberra::Utility::Core::ByteStream &Str, const Canberra::Serialization::MetaData &, const LONG Offset)
{
	Canberra::DataTypes::Serialization::ApplicationSerializer Serializer;
	LONG Pos=Offset;
	LONG NumCpy=0;
	USHORT NumSamps=0;
	USHORT SampSize=0;

	memcpy(&NumSamps, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(NumSamps)); Pos+=NumCpy;
	memcpy(&SampSize, const_cast<char *>(Str.GetBuffer()+Pos), NumCpy=sizeof(SampSize)); Pos+=NumCpy;
	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	if (Data) Data->clear();
	else {
		Data = new Samples;
		if (!Data) throw (ULONG) E_OUTOFMEMORY;
		_Samples = Data;
	}
	for(USHORT i=0; i<NumSamps; i++) {
		Sample Samp;
		Pos += Samp.Deserialize(Str, Pos, SampSize);
		Data->push_back(Samp);
	}

	return Pos-Offset;
}


///<summary>
///This method will returns the number of bytes this
///instance manages for serialization
///</summary>
///<returns>The number of bytes to serialize</returns>
ULONG Canberra::DataTypes::Spectroscopy::CounterData::GetDataSize()
{
	LONG Size = 2*sizeof(USHORT);
	Size += GetNumberOfSamples()*GetSample(0).GetSize();
	return Size;
}

///<summary>
///This method will add a sample to the counter data
///collection
///</summary>
///<exception cref="ULONG">The Win32 error code</exception>
void Canberra::DataTypes::Spectroscopy::CounterData::Add(const Sample &v)
{
	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	if (!Data) {
		Data = new Samples;
		if (!Data) throw (ULONG) E_OUTOFMEMORY;
		_Samples = Data;
	}
	Data->push_back(v);
	return;
}

///<summary>
///This method will clear the counter data collection
///</summary>
void Canberra::DataTypes::Spectroscopy::CounterData::Clear()
{
	Samples *Data = reinterpret_cast<Samples *>(_Samples);
	if (Data) Data->clear();
	return;
}


///<summary>
///This method will perform a deep copy
///</summary>
///<param name="v">The src</param>
void Canberra::DataTypes::Spectroscopy::CounterData::Copy(const Canberra::DataTypes::Spectroscopy::CounterData &v) {
	if (NULL != _Samples) {
		Samples *Data = reinterpret_cast<Samples *>(_Samples);
		delete Data;
		_Samples = NULL;
	}
	if (NULL == v._Samples) return;

	Samples *Dest = new Samples;
	_Samples = Dest;
	if (!Dest) throw (ULONG) E_OUTOFMEMORY;

	Samples *Src = reinterpret_cast<Samples *>(v._Samples);
	for(size_t i=0; i<Src->size(); i++) {
		Dest->push_back((*Src)[i]);
	}
}
Canberra::DataTypes::Spectroscopy::CounterData& Canberra::DataTypes::Spectroscopy::CounterData::operator=( const CounterData &Rhs) {Copy(Rhs); return *this;}


///<summary>
///This method will initialize an instance of this class
///</summary>
///<param name="v">The src</param>
Canberra::DataTypes::Spectroscopy::CounterData::CounterData(const CounterData &v) :
	Canberra::Serialization::SerializableObject(Canberra::DataTypes::Serialization::SerializableObjects::CounterData) {	_Samples=NULL; Copy(v);}
