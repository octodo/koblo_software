/*!	\file CStreamManager.cpp
	\author Michael Olsen
	\date 19/Jun/2007
*/

#include "SToolsInternalOS.h"

CStreamManager::CStreamManager()
	: mpTimer(NULL), mabStreamInUse(NULL)
{
	mpTimer = ITimer::Create();

#ifdef _DEBUG
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 1000);
#else	// _DEBUG
	mpTimer->Init(0, dynamic_cast<ITimerCallback*>(this), 10);
#endif	// _DEBUG

	mbStutter = false;
}

CStreamManager::~CStreamManager()
{
	if (mpTimer) {
		mpTimer->Destroy();
	}

	std::list<IStream*>::iterator it = mStreams.begin();
	for (; it != mStreams.end(); it++) {
		IStream* pStream = *it;
		dynamic_cast<CStream*>(pStream)->Destroy();
	}

	if (mabStreamInUse) {
		delete[] mabStreamInUse;
	}
}

IStreamManager* IStreamManager::Create()
{
	return dynamic_cast<IStreamManager*>(new CStreamManager());
}

void CStreamManager::Destroy()
{
	delete dynamic_cast<CStreamManager*>(this);
}

void CStreamManager::CreateStreams(tint32 iPolyphony)
{
	mabStreamInUse = new tbool[iPolyphony];

	tint32 iStream;
	for (iStream = 0; iStream < iPolyphony; iStream++) {
		IStream* pStream = CStream::Create();

		mStreams.push_back(pStream);

		mabStreamInUse[iStream] = false;
	}
}

st::IStream* CStreamManager::GetStream()
{
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
}

tbool CStreamManager::IsStreamIxInUse(tint32 iIx)
{
	CAutoLock lock(mMutex_ForStreamIxInUse);

	return mabStreamInUse[iIx];
} // IsStreamIxInUse


void CStreamManager::SetStreamIxInUse(tint32 iIx, tbool bInUse)
{
	CAutoLock lock(mMutex_ForStreamIxInUse);

	mabStreamInUse[iIx] = bInUse;
} // SetStreamIxInUse


void CStreamManager::ReleaseStream(st::IStream* pStream)
{
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
}


void CStreamManager::OnTimer(tint32 iID)
{
	tbool bStutter = false;
	
	tint32 iStreamIx = 0;
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
	}
	
	if (bStutter) {
		mbStutter = true;
	}
} // OnTimer


tbool CStreamManager::GetStutter()
{
	if (!mbStutter)
		return false;

	mbStutter = false;
	return true;
} // GetStutter


