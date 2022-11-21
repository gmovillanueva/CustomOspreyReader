#include "SyncObject.h"
#include "winerror.h"

#pragma once

namespace Canberra {
	namespace Utility {
		namespace Core {

			//
			//An instance of this class is used to manage
			//any type of synchronization object.
			//
			class UTILITY_DLLIMPORT LockManager
			{
			protected:
				SyncObject *_Lock;			//The lock instance
				bool _Locked;				//The lock state

			public:

				///<summary>
				///This method will initialize and lock the
				///the sync object
				///</summary>
				///<param name="L">The lock to manage</param>
				///<param name="LockIt">The state of the lock</param>
				///<exception name="ULONG">The error info</exception>
				LockManager(SyncObject *L, bool LockIt=true);
				virtual ~LockManager(void);

				///<summary>
				///This method will lock the sync object
				///If it is already locked nothing occurs
				///</summary>
				///<returns>0==Success</returns>
				ULONG Lock();

				///<summary>
				///This method will unlock the sync object
				///If it is already unlocked nothing occurs
				///</summary>
				///<returns>0==Success</returns>
				ULONG Unlock() ;
			};
		}
	}
}