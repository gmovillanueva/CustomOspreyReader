#include "LockManager.h"
#include "DebugMacros.h"

///<summary>
///This method will initialize and lock the
///the sync object
///</summary>
///<param name="L">The lock to manage</param>
///<param name="LockIt">The state of the lock</param>
///<exception name="ULONG">The error info</exception>
Canberra::Utility::Core::LockManager::LockManager(SyncObject *L, bool LockIt) : _Lock(L), _Locked(false) {
	if (NULL == L) {
		DEBUG_MESSAGE(L"LockManager", L"LockManager", L"NULL == SyncObject *, You must have a valid pointer");
		throw (ULONG) E_INVALIDARG;
	}
	if (LockIt) Lock();
}
Canberra::Utility::Core::LockManager::~LockManager(void) {Unlock();}

///<summary>
///This method will lock the sync object
///If it is already locked nothing occurs
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::LockManager::Lock() {
	if (_Locked) return 0;

	_Locked=true;
	return _Lock->Lock();
}

///<summary>
///This method will unlock the sync object
///If it is already unlocked nothing occurs
///</summary>
///<returns>0==Success</returns>
ULONG Canberra::Utility::Core::LockManager::Unlock() {
	if (!_Locked) return 0;
	_Locked=false;
	return _Lock->Unlock();
}
