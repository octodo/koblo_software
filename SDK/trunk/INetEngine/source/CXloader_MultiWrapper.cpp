
#include "ineInternalOS.h"

//! Lock for modifying pointer to CURL multi instance
static volatile tint32 giCURLMulti_LockLevel = 0;
//! Static pointer to the one and only CURL multi instance wrapper. The instance holds a number of active download/upload threads (every IUploader and IDownloader has 0 or 1 active thread)
static CXloader_MultiWrapper* gpCXloader_MultiWrapper = NULL;


CXloader_MultiWrapper::CXloader_MultiWrapper()
{
	mpCURLMulti = curl_multi_init();

	miTimerID = 1904;
	mpTimer = ITimer::Create();
	mpTimer->Init(miTimerID, this, 200);
} // constructor


CXloader_MultiWrapper::~CXloader_MultiWrapper()
{
	if (mpTimer) {
		mpTimer->Destroy();
		mpTimer = NULL;
	}
	if (mpCURLMulti) {
		curl_multi_cleanup(mpCURLMulti);
		mpCURLMulti = NULL;
	}
} // destructor


void CXloader_MultiWrapper::GetLockForMultiInstance()
{
	// Attempt lock
	while (++giCURLMulti_LockLevel != 1) {
		// We're not alone - release again
		giCURLMulti_LockLevel--;
		// Sleep 1 - 3 ms (random)
		tint32 iSleepMS = (tint32)(rand() * (3.0f / RAND_MAX));
		iSleepMS++;
		ITime::SleepMS(iSleepMS);
	}
} // GetLockForMultiInstance


void CXloader_MultiWrapper::ReleaseLockForMultiInstance()
{
	giCURLMulti_LockLevel--;
} // ReleaseLockForMultiInstance


tbool CXloader_MultiWrapper::Add(CXloader* pXloader, std::string* psError)
{
	GetLockForMultiInstance();

	*psError = "";

	// Maybe create global instance
	if (gpCXloader_MultiWrapper == NULL) {
		gpCXloader_MultiWrapper = new CXloader_MultiWrapper();
	}

	if (!Find(pXloader)) {
		// Not previously added - add now
		gpCXloader_MultiWrapper->mlistXloaders.push_back(pXloader);
		// Start transaction
		CURLMcode rc = curl_multi_add_handle(
			gpCXloader_MultiWrapper->mpCURLMulti,
			pXloader->mpCURLEasyHandle
			);
		if (rc != 0) {
			tchar pszErr[512];
			sprintf(pszErr, "curl_multi_add_handle(..) returned %d", rc);
			*psError = pszErr;
		}
	}

	ReleaseLockForMultiInstance();

	return (psError->size() == 0);
} // Add


tbool CXloader_MultiWrapper::Remove(CXloader* pXloader, std::string* psError)
{
	GetLockForMultiInstance();

	*psError = "";

	if (gpCXloader_MultiWrapper) {
		std::list<CXloader*>& rlist = gpCXloader_MultiWrapper->mlistXloaders;

		std::list<CXloader*>::iterator it;
		if (Find(pXloader, &it)) {
			// Was added - now remove it
			rlist.erase(it);
			// Stop any ongoing transactions
			CURLMcode rc = curl_multi_remove_handle(
				gpCXloader_MultiWrapper->mpCURLMulti,
				pXloader->mpCURLEasyHandle
				);
			if (rc != 0) {
				tchar pszErr[512];
				sprintf(pszErr, "curl_multi_remove_handle(..) returned %d", rc);
				*psError = pszErr;
			}
		}

		if (rlist.size() == 0) {
			// All IUploader and IDownloader instances were removed - kill multi handler
			delete gpCXloader_MultiWrapper;
			gpCXloader_MultiWrapper = NULL;
		}
	}

	ReleaseLockForMultiInstance();

	return (psError->size() == 0);
} // Remove


tbool CXloader_MultiWrapper::Find(CXloader* pXloader, std::list<CXloader*>::iterator* pit)
{
	std::list<CXloader*>& rlist = gpCXloader_MultiWrapper->mlistXloaders;
	*pit = rlist.begin();
	tbool bFound = false;
	for ( ; *pit != rlist.end(); (*pit)++) {
		if (**pit == pXloader) {
			// Found it
			return true;
		}
	}
	// Nope - not there
	return false;
} // Find


tbool CXloader_MultiWrapper::Find(CXloader* pXloader)
{
	std::list<CXloader*>::iterator it_Ignored;
	return Find(pXloader, &it_Ignored);
} // Find


void CXloader_MultiWrapper::OnTimer(tint32 iID)
{
	if (iID == miTimerID) {
		GetLockForMultiInstance();

		CURLMcode rc = (CURLMcode)0;

		tint32 iRunningHandles_Dummy = 0;
		do {
			rc = curl_multi_perform(mpCURLMulti, &iRunningHandles_Dummy);
		} while (rc == CURLM_CALL_MULTI_PERFORM);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_multi_perform(..) returned %d", rc);
			// Nah.... don't let's do that... Is caught directly in CXloader
			//SetAllError(pszErr);

			// Release lock
			ReleaseLockForMultiInstance();
			return; // false;
		}

		// Find out if they're done
		CURLMsg* pMsgStruct = NULL;
		tint32 iEnumerator = 0;
		do {
			pMsgStruct = curl_multi_info_read(mpCURLMulti, &iEnumerator);
			if (pMsgStruct) {
				switch (pMsgStruct->msg) {
					case CURLMSG_DONE:
						{
							// Web transfer is done
							CXloader* pXloader = NULL;
							CURL* pEasyHandle = pMsgStruct->easy_handle;
							CURLcode rc = curl_easy_getinfo(
								pEasyHandle,
								CURLINFO_PRIVATE,
								&pXloader);
							if (pXloader != NULL) {
								// Set return code
								pXloader->SetMultiSaysDone(pMsgStruct->data.result);
							}
						}
						break;
				}
			}
		} while (pMsgStruct);

		ReleaseLockForMultiInstance();
	}
} // OnTimer


void CXloader_MultiWrapper::SetAllError(const tchar* pszErr)
{
	std::list<CXloader*>::iterator it = mlistXloaders.begin();
	for ( ; it != mlistXloaders.end(); it++) {
		CXloader* p = *it;
		p->AppendError(pszErr);
	}
} // SetAllError
