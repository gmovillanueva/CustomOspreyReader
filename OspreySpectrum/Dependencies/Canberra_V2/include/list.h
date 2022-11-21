#pragma once
#include "ByteStream.h"
#include "DebugMacros.h"
#include <memory.h>
namespace Canberra {
	namespace Utility {
		namespace Core {

			///<summary>
			///This template is used to create a simple list.  STL is not used here because
			///the intention is to use this class for exporting from DLL's.
			///</summary>
			template <class T, class NumType>
			class ListBase {
				protected:
					T *_List;			//A pointer to the list
					NumType _Num;		//The number of list items

					///<summary>
					///This method will perform a deep copy
					///</summary>
					///<param name="v">The list to copy</param>
					///<returns>0==Success</returns>
					///<exception cref="ULONG">Error code</exception>
					void Copy(const ListBase<T, NumType> &v) {
						Clear();
						if (!v._Num) return;
						_List = new T[v._Num];
						if (!_List) {
							ULONG Stat = (ULONG) E_OUTOFMEMORY;
							DEBUG_ERROR_MESSAGE(L"List<T>", L"Copy", L"Allocation failure", Stat);
							throw Stat;
						}
						//memcpy(_List, v._List, sizeof(T)*v._Num);
						//Intentionally not using memcpy because template parameter
						//may be a class that encapsulates pointers and the equals
						//operator is overloaded.
						for(int i=0; i<v._Num; i++) {_List[i]=v._List[i];}
						_Num = v._Num;
					}

				public:
					ListBase<T, NumType>(const ListBase<T, NumType> &v) : _List(NULL), _Num(0) {Copy(v);}
					ListBase<T, NumType>() : _List(NULL), _Num(0) {}
					virtual ~ListBase<T, NumType>() {Clear();}

					///<summary>
					///Allow for deep copy for = operator
					///</summary>
					ListBase<T, NumType>& operator=( const ListBase<T, NumType> &Rhs) {Copy(Rhs); return *this;}

					///<summary>
					///This method will add a new item to the list
					///</summary>
					///<param name="v">The item to add</param>
					///<exception cref="ULONG">Error code</exception>
					///<returns>0==Success</returns>
					ULONG Add(const T&v) {
						T *temp = new T[_Num+1];
						if (!temp) {
							ULONG Stat = (ULONG) E_OUTOFMEMORY;
							DEBUG_ERROR_MESSAGE(L"List<T>", L"Add", L"Allocation failure", Stat);
							throw Stat;
						}
						if (_List) {
							//Intentionally not using memcpy because template parameter
							//may be a class that encapsulates pointers and the equals
							//operator is overloaded.
							//memcpy(temp, _List, _Num*sizeof(T));
							for(int i=0; i<_Num; i++) {temp[i] = _List[i];}
						}
						temp[_Num] = v;
						delete [] _List;
						_List = temp;
						_Num++;
						return 0;
					}

					///<summary>
					///This method will clear the list
					///</summary>
					///<param name="v">The item to add</param>
					void Clear() {delete [] _List;  _List=NULL; _Num=0;}

					///<summary>
					///This method will get a item from the list
					///</summary>
					///<param name="i">The item index</param>
					///<param name="v">The item value</param>
					///<exception cref="ULONG">Error code</exception>
					///<returns>0==Success</returns>
					T &GetItem(NumType i) {
						if ((i < 0) || (i>= _Num)) {
							ULONG Stat = (ULONG) E_INVALIDARG;
							DEBUG_ERROR_MESSAGE(L"List<T>", L"GetItem", L"Index < 0 or > number of items", Stat);
							throw Stat;
						}
						return _List[i];
					}

					///<summary>
					///This method will return the number of items in the list
					///</summary>
					///<returns>Number of</returns>
					ULONG GetNumberOfItems() {return _Num;}

			};

			///<summary>
			///This provides basic list support
			///</summary>
			template<class T>
			class List : public ListBase<T, LONG> {};

			///<summary>
			///This provides basic list support with the ability to
			///serialize the list contents
			///</summary>
			template<class T, class NumType>
			class SerializableListBase : public ListBase<T, NumType> {
			public:
					///<summary>
					///This method will the number of bytes the
					///data contained within uses
					///</summary>
					///<returns>value</returns>
					ULONG GetDataSize() { return this->_Num*sizeof(T) + sizeof(this->_Num);}

					///<summary>
					///This method will serialize this instance into the byte stream
					///</summary>
					///<param name="Str">The stream</param>
					///<param name="Offset">The byte position to start the write in the stream</param>
					///<exception cref="ULONG">The Win32 error code</exception>
					///<returns>The number of bytes serialized</returns>
					ULONG SerializeData(Canberra::Utility::Core::ByteStream &BS, ULONG Offset=0) {
						ULONG Size=GetDataSize();
						if (Offset < 0) {
							ULONG Stat = (ULONG) E_INVALIDARG;
							DEBUG_ERROR_MESSAGE(L"List<T>", L"Serialize", L"Invalid Offset size", Stat);
							throw Stat;
						}
						if ((Offset+Size) > BS.GetSize()) BS.Resize(Offset+Size);

						LONG Pos=Offset;
						LONG NumCpy=0;
						memcpy(const_cast<char *>(BS.GetBuffer()+Pos), &this->_Num, NumCpy=sizeof(this->_Num)); Pos+=NumCpy;
						if (this->_Num) {memcpy(const_cast<char *>(BS.GetBuffer()+Pos), this->_List, NumCpy=(sizeof(T)*this->_Num)); Pos+=NumCpy;}
						return Pos-Offset;
					}

					///<summary>
					///This method will deserialize this contents of the byte stream into this
					///instance
					///</summary>
					///<param name="Str">The stream</param>
					///<param name="Offset">The byte position to start the write in the stream</param>
					///<exception cref="ULONG">The Win32 error code</exception>
					///<returns>The number of bytes serialized</returns>
					ULONG DeserializeData(Canberra::Utility::Core::ByteStream &BS, ULONG Offset=0) {
						this->Clear();
						LONG Pos=Offset;
						LONG NumCpy=0;
						memcpy(&this->_Num, const_cast<char *>(BS.GetBuffer()+Pos), NumCpy=sizeof(this->_Num)); Pos+=NumCpy;
						if (this->_Num) {
							this->_List = new T[this->_Num];
							if (!this->_List) {
								ULONG Stat = (ULONG) E_OUTOFMEMORY;
								DEBUG_ERROR_MESSAGE(L"List<T>", L"DeserializeData", L"Allocation failure", Stat);
								throw Stat;
							}
							memcpy(this->_List, const_cast<char *>(BS.GetBuffer()+Pos), NumCpy=(sizeof(T)*this->_Num)); Pos+=NumCpy;
						}
						return Pos-Offset;
					}
			};

			///<summary>
			///This provides basic list support with the ability to
			///serialize the list contents
			///</summary>
			template<class T>
			class SerializableList : public SerializableListBase<T, LONG> {};
		}
	}
}
