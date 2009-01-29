/*!	\file CStreamManager.cpp
	\author Michael Olsen
	\date 19/Jun/2007
*/

#include "SToolsInternalOS.h"


CStreamManager::CStreamManager()
	: mpTimer(NULL)//, mabStreamInUse(NULL)
{
	mbStutter = false;
	mppStreamsArray = new IStream*[1];
	mppStreamsArray[0] = NULL;
	miStreamsArrayLen = 0;

	mpTimer = ITimer::Create();

#ifdef _DEBUG
	// (lasse - 500 means no playback!) mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 500);
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 50);
#else	// _DEBUG
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 10);
#endif	// _DEBUG
} // constructor


CStreamManager::~CStreamManager()
{
	if (mpTimer) {
		mpTimer->Destroy();
	}

	/*
	std::list<IStream*>::iterator it = mStreams.begin();
	for (; it != mStreams.end(); it++) {
		IStream* pStream = *it;
		dynamic_cast<CStream*>(pStream)->Destroy();
	}

	if (mabStreamInUse) {
		delete[] mabStreamInUse;
	}
	*/

	// Delete those that were about to be added to active stream array
	{
		CAutoLock Lock(mMutex_ForStreams_ToBeAdded);
		while (mlistpStreams_ToBeAdded.size()) {
			IStream* p = mlistpStreams_ToBeAdded.back();
			mlistpStreams_ToBeAdded.pop_back();
			CStream* pC = dynamic_cast<CStream*>(p);
			if (pC) {
				pC->Destroy();
				pC = NULL;
			}
			p = NULL;
		}
	}

	// Delete active stream array
	{
		IStream** pp = mppStreamsArray;
		IStream* p = *pp++;
		while (p) {
			CStream* pC = dynamic_cast<CStream*>(p);
			if (pC) {
				pC->Destroy();
				pC = NULL;
			}
			// Next
			p = *pp++;
		}
	}
} // destructor


IStreamManager* IStreamManager::Create()
{
	return dynamic_cast<IStreamManager*>(new CStreamManager());
}

void CStreamManager::Destroy()
{
	delete dynamic_cast<CStreamManager*>(this);
}

void CStreamManager::CreateStreams(tint32 /*iPolyphony*/)
{
	// (lasse) No need to pre-allocate anymore - do nothing here!

	/*
	mabStreamInUse = new tbool[iPolyphony];

	tint32 iStream;
	for (iStream = 0; iStream < iPolyphony; iStream++) {
		IStream* pStream = CStream::Create();

		mStreams.push_back(pStream);

		mabStreamInUse[iStream] = false;
	}
	*/
} // CreateStreams


st::IStream* CStreamManager::GetStream()
{
	/*
	tint32 iStream;
	std::list<IStream*>::iterator it = mStreams.begin();
	for (iStream = 0; iStream < (tint32)mStreams.size(); iStream++, it++) {
		CAutoLock lock(mMutex_ForStreamIxInUse);
		
		if (!IsStreamIxInUse(iStream)) {
			SetStreamIxInUse(iStream, true);

			return *it;
		}
	}

	return NULL;
	*/

	// Allocate stream
	IStream* pStream = CStream::Create();
	// Queue stream for addition to handler
	CAutoLock Lock(mMutex_ForStreams_ToBeAdded);
	mlistpStreams_ToBeAdded.push_back(pStream);
	// return newly allocated stream
	return pStream;
} // GetStream


void CStreamManager::ReleaseStream(st::IStream* pStream)
{
	/*
	CAutoLock lock(mMutex_ForStreamIxInUse);

	tint32 iStream;
	std::list<IStream*>::iterator it = mStreams.begin();
	for (iStream = 0; iStream < (tint32)mStreams.size(); iStream++, it++) {
		if ((*it) == pStream) {
			// Mark this index as unused
			SetStreamIxInUse(iStream, false);

			return;
		}
	}

	ASSERT(NULL);
	*/

	// Queue this stream for deletion
	CAutoLock Lock(mMutex_ForStreams_ToBeDeleted);
	mlistpStreams_ToBeDeleted.push_back(pStream);
} // ReleaseStream


void CStreamManager::OnTimer(tint32 iID)
{
	tbool bStutter = false;

/*	tint32 iStreamIx = 0;
	std::list<IStream*>::iterator it = mStreams.begin();
	for (; it != mStreams.end(); iStreamIx++, it++) {
//		if (IsStreamIxInUse(iStreamIx)) {
			IStream* pStream = *it;
			CStream* pCStream = dynamic_cast<CStream*>(pStream);
			if (pCStream) {
				pCStream->OnTimer();
				if (pCStream->GetStutter()) {
					bStutter = true;
				}
			}
//		}
	}*/

	IStream** pp = mppStreamsArray;
	IStream* pStream = *pp++;
	while (pStream) {
		CStream* pCStream = dynamic_cast<CStream*>(pStream);
		if (pCStream) {
			pCStream->OnTimer();
			if (pCStream->GetStutter()) {
				bStutter = true;
			}
		}
		// Next
		pStream = *pp++;
	}
	
	if (bStutter) {
		mbStutter = true;
	}

	// Handle adds and removes
	StreamsArrayHouseKeeping();
} // OnTimer


void CStreamManager::StreamsArrayHouseKeeping()
{
	CAutoLock Lock_Add(mMutex_ForStreams_ToBeAdded);
	CAutoLock Lock_Del(mMutex_ForStreams_ToBeDeleted);

	tint32 iToBeAdded = mlistpStreams_ToBeAdded.size();
	tint32 iToBeDeleted = mlistpStreams_ToBeDeleted.size();
	if ((iToBeAdded == 0) && (iToBeDeleted == 0)) {
		// Nothing to do - skip
		return;
	}

	tint32 iNewLen = miStreamsArrayLen;
	iNewLen += iToBeAdded;
	// Be pessimistic about it: Don't assume that we can actually remove anything
	//iNewLen -= 0 * iToBeDeleted;

	// Allocate new array (always one more so we won't crash on create zero length)
	// This way we can also zero-terminate for easier handling
	IStream** ppStreamArray_New = new IStream*[iNewLen + 1];
	IStream** ppStream_Src = mppStreamsArray;
	IStream** ppStream_Dst = ppStreamArray_New;

	// Put newly created streams first, so they get boosted
	while (mlistpStreams_ToBeAdded.size()) {
		IStream* pAdd = mlistpStreams_ToBeAdded.back();
		mlistpStreams_ToBeAdded.pop_back();
		*ppStream_Dst = pAdd;
		ppStream_Dst++;
	}

	// Copy those already in array - unless they've been marked for deletion
	if (mlistpStreams_ToBeDeleted.size()) {
		while (*ppStream_Src) {
			IStream* pOld = *ppStream_Src++;
			tbool bDeleteOld = false;
			std::list<IStream*>::iterator it = mlistpStreams_ToBeDeleted.begin();
			for ( ; it != mlistpStreams_ToBeDeleted.end(); it++) {
				if (*it == pOld) {
					// Mark this for delete
					bDeleteOld = true;
					break;
				}
			}
			if (bDeleteOld) {
				// Delete object
				CStream* pCOld = dynamic_cast<CStream*>(pOld);
				if (pCOld) {
					pCOld->Destroy();
					pCOld = NULL;
				}
				pOld = NULL;
				// We really removed something - decrease length in new array
				iNewLen--;
				// Remove from "to-be-deleted" list
				mlistpStreams_ToBeDeleted.erase(it);
				// No more?
				if (mlistpStreams_ToBeDeleted.size() == 0) {
					// No more to delete. Skip to faster copy loop below
					break;
				}
			}
			else {
				// This one's to be kept - copy it
				*ppStream_Dst++ = pOld;
			}
		}
	}

	// Copy the rest of those already in array (no more to be deleted)
	{
		IStream* p = *ppStream_Src++;
		while (p) {
			*ppStream_Dst++ = p;
			p = *ppStream_Src++;
		}
	}

	// Zero-terminate new array
	*ppStream_Dst = NULL;

#ifdef _DEBUG
	static tint32 iWeirdCount = 0;
	if (mlistpStreams_ToBeDeleted.size()) {
		iWeirdCount++;
		if (iWeirdCount >= 2) {
			// This is not good!
			// One time is ok, could happen if we delete immediately after addition
			// But twice in a row is a symptom of disease
			tint32 iSize_Debug = mlistpStreams_ToBeDeleted.size();
			ASSERT(0);
		}
	}
	else {
		iWeirdCount = 0;
	}
#endif // #ifdef _DEBUG

	// Swap arrays
	delete[] mppStreamsArray;
	mppStreamsArray = ppStreamArray_New;
	miStreamsArrayLen = iNewLen;
} // StreamsArrayHouseKeeping


tbool CStreamManager::GetStutter()
{
	if (!mbStutter)
		return false;

	mbStutter = false;
	return true;
} // GetStutter


