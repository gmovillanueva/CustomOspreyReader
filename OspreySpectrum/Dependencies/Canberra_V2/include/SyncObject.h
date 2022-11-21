#pragma once
#include "UtilityImpExp.h"
#include "UtilityString.h"
namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//This is an abstract class for encapsulating a synchronization
			//object.  In a nutshell some form of crit. section, mutex, event,
			//or semaphore.
			//
			class UTILITY_DLLIMPORT SyncObject
			{
			protected:
				bool _IsNamed;					//State to indicate whether this objects has been created with a name
				bool _IsGlobal;					//State to indicate whether can be used across process bounds
				bool _Initialized;				//State to indicate that the sync object has been allocated
				String _Name;					//Name associated with this object


			public:
				SyncObject(const String &Name) :  _IsNamed(false), _IsGlobal(false), _Initialized(false), _Name(Name) {}
				virtual ~SyncObject(void) {}

				//This method creates the underlying object
				virtual ULONG Create()=0;

				//This method releases the underlying object
				virtual ULONG Close()=0;

				//This method will wait to sync the underlying object on the calling thread
				virtual bool WaitOne(LONG TimeO=0)=0;

				//This method will gain ownership of the underlying object on the calling thread
				virtual ULONG Lock()=0;

				//This method will release ownership of the underlying object on the calling thread
				virtual ULONG Unlock()=0;

				//This method will test ownership of the underlying object on the calling thread
				virtual bool TryLock()=0;

				//This property will return information indicatin whether the object is open
				bool GetIsOpen() {return _Initialized;}

				//Name of this object
				String GetName() {return _Name;}

				//Returns state indicating the object is global (i.e., can be used across process bounds)
				bool GetIsGlobal() {return _IsGlobal;}

				//Returns state indicating the object has been named during creation
				bool GetIsNamed() {return _IsNamed;}

			};
		}
	}
}
