
#include "ineInternalOS.h"

CXloader::CXloader(tbool bIsUploader)
{
	mbIsUploader = bIsUploader;
	mpfileToUpload = mpfileForReply = NULL;
	mpszParamsAssembled = NULL;
	miParamsAssembledLen = 0;
	mbIsInitialized = mbIsTransfering = mbIsFailed = mbIsMultiDone = mbIsDone = false;
	meMIMEType = MIME_TYPE_NONE;
	meSpecificVerb = VERB_DEFAULT;
	mbAllowRedirects = true;

	miLockLevel_ForReplyBuffer = 0;
	
	// Make sure we only close what's been opened
	mpCURLEasyHandle = NULL;
	mpFormPost_First = mpFormPost_Last = NULL;
	mpSList_ExtraHeaders = NULL;
	
	// No longer - will use libCURL instead
	//Constructor_OSSpecific();
} // constructor


CXloader::~CXloader()
{
	Abort();
	WipeParams();
	CloseConnection();

	// Now it's OK to delete handle (could be kept around for speed optimization)
	if (mpCURLEasyHandle) {
		curl_easy_cleanup(mpCURLEasyHandle);
		mpCURLEasyHandle = NULL;
	}
	
	/*
	// Unhook and maybe release CURL multi instance
	{
		GetLockForMultiInstance();
		//
		// Unhook CURL multi instance
		giCURLMultiHooks--;
		// Maybe release CURL multi instance
		if (giCURLMultiHooks == 0) {
			// No more use for instance - release it
			curl_multi_cleanup(gpCURLMulti);
			gpCURLMulti = NULL;
		}
		//
		ReleaseLockForMultiInstance();
	}
	*/

	// No longer - will use libCURL instead
	//Destructor_OSSpecific();
} // destructor


IDownloader* IDownloader::Create()
{
	return dynamic_cast<IDownloader*>(new CXloader(false));
} // Create


IUploader* IUploader::Create()
{
	return dynamic_cast<IUploader*>(new CXloader(true));
} // Create


void CXloader::Destroy()
{
	delete dynamic_cast<CXloader*>(this);
} // Destroy


tbool CXloader::Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort /*= 80*/, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	// Call IUploader code, just without file pointer and param name
	return Init(pszHost, pszPage, NULL, NULL, iPort, pszUser, pszPassword, iTimeOutSecs);
} // Init (for IDownloader)


tbool CXloader::Init(const tchar* pszHost, const tchar* pszPage, IFile* pfileToUpload, tchar* pszParamName /*= "Upload"*/, tint32 iPort /*= 80*/, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	Abort();
	WipeParams();
	ZapReplyBuffer();
	mbIsInitialized = false;
	
	// Verify sanity of host
	{
		if ((pszHost == NULL) || (*pszHost == '\0')) {
			SetError("No param name");
			return false;
		}
		for (tchar* pc = (tchar*)pszHost; *pc; pc++) {
			tchar c = *pc;
			if (
				((c < 'A') || (c > 'Z'))
				&&
				((c < 'a') || (c > 'z'))
				&&
				((c < '0') || (c > '9'))
				&&
				(c != '.')
				) {
				SetError("Not valid url-host");
				return false;
			}
		}
	}
	
	// Verify sanity of page
	{
		if (pszPage != NULL) {
			for (tchar* pc = (tchar*)pszPage; *pc; pc++) {
				tchar c = *pc;
				if (
					((c < 'A') || (c > 'Z'))
					&&
					((c < 'a') || (c > 'z'))
					&&
					((c < '0') || (c > '9'))
					&&
					(c != '.')
					&&
					(c != '/')
					) {
					SetError("Not valid url-document");
					return false;
				}
			}
		}
	}
	
	// Verify sanity of port
	if (iPort <= 0) {
		SetError("Not a valid port");
		return false;
	}
	
	// Verify sanity of time-out
	if (iTimeOutSecs < 0) {
		SetError("Negative time-out");
		return false;
	}
	
	msHost = pszHost;
	msPage = (pszPage == NULL) ? "" : pszPage;
	mpfileToUpload = pfileToUpload;
	msUploadFileParamName =
		((pszParamName == NULL) || (*pszParamName = '\0'))
		? "Upload" : pszParamName;
	miPort = iPort;
	msUser = (pszUser == NULL) ? "" : pszUser;
	msPassword = (pszPassword == NULL) ? "" : pszPassword;
	muiTimeOutSecs = (tuint32)iTimeOutSecs;

	// Extract path and name from upload IFile object
	msUploadFileNameAndExtOnly = "";
	if (mpfileToUpload) {
		tchar pszFilePathName[1024];
		if ((dynamic_cast<IFileMemory*>(mpfileToUpload)) != NULL) {
			// This is a memory file - no name
			sprintf(pszFilePathName, "(no name)");
		}
		else {
			mpfileToUpload->GetPathName(pszFilePathName);
			tchar* pszColon = strrchr(pszFilePathName, ':');
			if (pszColon == NULL) {
				SetError("Internal error: Unexpected format of file path");
				return false;
			}
			msUploadFileNameAndExtOnly = pszColon + 1;
		}
	}

	mbIsInitialized = true;
	meMIMEType = MIME_TYPE_NONE;
	meSpecificVerb = VERB_DEFAULT;
	mbAllowRedirects = true;
	mpfileForReply = NULL;
	
	muiUploadProgress = muiReplyProgress = muiReplySize;
	return true;
} // Init (for IUploader)


tbool CXloader::SetNameOfUploadFile(tchar* pszOverrideName)
{
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		SetError("We can't rename upload file as we've already begun transfer");
		return false;
	}
	
	// Success
	msUploadFileNameAndExtOnly = (pszOverrideName == NULL) ? "" : pszOverrideName;
	return true;
} // SetNameOfUploadFile


tbool CXloader::SetReplyMIMEType(E_MIME_Type eMIME)
{
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		SetError("We can't set media type as we've already begun transfer");
		return false;
	}
	
	// Success
	meMIMEType = eMIME;
	return true;
} // SetReplyMIMEType


const tchar* CXloader::GetMIMEString()
{
	switch (meMIMEType) {
		case MIME_TYPE_TEXT:	return "text/*";
		case MIME_TYPE_HTML:	return "text/html";
		case MIME_TYPE_XML:		return "text/xml";
		case MIME_TYPE_BINARY:	return "application/octet-stream";
		case MIME_TYPE_OGG:		return "audio/ogg";
		case MIME_TYPE_MP3:		return "audio/mpeg";
	}
	return "*/*";
} // GetMIMEString


tbool CXloader::SetSpecificVerb(EVerbType eVerb)
{
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		SetError("We can't change http-verb as we've already begun transfer");
		return false;
	}
	
	tbool bIsUploader = (mpfileToUpload != NULL);
	
	switch (eVerb) {
		case VERB_DEFAULT:
			meSpecificVerb = eVerb;
			return true;
			
		case VERB_GET:
		{
			// Only IDownloader may use GET
			if (!bIsUploader)
				return true;
		}
			break;
			
		case VERB_POST:
			meSpecificVerb = eVerb;
			return true;
			
		case VERB_PUT:
		{
			// Only IUploader may use PUT
			if (bIsUploader)
				return true;
		}
			break;
	}
	
	// Fall-through to error
	return false;
} // SetSpecificVerb


EVerbType CXloader::GetActuallyUsedVerb()
{
	// Did calling app set a specific VERB to use?
	switch (meSpecificVerb) {
		case VERB_GET:
		case VERB_POST:
		case VERB_PUT:
			return meSpecificVerb;
	}

	if (mbIsUploader) {
		if (mlist_sParamNames.size() > 0) {
			// IUploaders are POST as default when there are more parameters than the file itself
			return VERB_POST;
		}
		else {
			// IUploaders are PUT as default when there's only the file
			return VERB_PUT;
		}
	}
	else {
		if (mlist_sParamNames.size() > 0) {
			// IDownloaders are POST as default if they have parameters
			return VERB_POST;
		}
		else {
			// IDownloaders without parameters are GET as default
			return VERB_GET;
		}
	}
} // GetActuallyUsedVerb


const tchar* CXloader::GetVerbString(EVerbType eVerb)
{
	switch (eVerb) {
		case VERB_GET:	return "GET";
		case VERB_POST:	return "POST";
		case VERB_PUT:	return "PUT";
	}
	
	// What?!
	return "";
} // GetVerbString


tbool CXloader::SetAllowRedirects(tbool bAllow)
{
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		SetError("We can't change redirect behaviour as we've already begun transfer");
		return false;
	}

	mbAllowRedirects = bAllow;
	return true;
} // SetAllowRedirects


tbool CXloader::DisableAutoRedirects()
{ return SetAllowRedirects(false); }


tbool CXloader::EnableAutoRedirects()
{ return SetAllowRedirects(true); }


tbool CXloader::AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen)
{
	CAutoLock Lock(mMutex_ForParams);
	
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		SetError("We can't add more parameters as we've already begun transfer");
		return false;
	}
	
	// Verify sanity of param name
	{
		if ((pszParamName == NULL) || (*pszParamName == '\0')) {
			SetError("No param name");
			return false;
		}
		for (tchar* pc = (tchar*)pszParamName; *pc; pc++) {
			tchar c = *pc;
			if (
				((c < 'A') || (c > 'Z'))
				&&
				((c < 'a') || (c > 'z'))
				&&
				((c < '0') || (c > '9'))
				&&
				((c != '_') && (c != '[') && (c != ']'))
				) {
				SetError("Not valid paramname");
				return false;
			}
		}
	}
	
	// Convert param data to URL-encoded safe string
	tchar* pszUrlEncoded = NULL;
	tint32 iUrlEncodedLen = 0;
	{
		if (iParamDataLen < 0) {
			iParamDataLen = (pcParamData == NULL) ? 0 : strlen(pcParamData);
		}
		iUrlEncodedLen = (iParamDataLen == 0) ? 0 : IINetUtil::CalcURLEncodedLen(pcParamData, iParamDataLen);
		if (iUrlEncodedLen > 0) {
			pszUrlEncoded = new tchar[iUrlEncodedLen + 1];
			if (pszUrlEncoded == NULL) {
				SetError("Out of memory");
				return false;
			}
			tint32 iActuallyEncoded = IINetUtil::URLEncode(pcParamData, iParamDataLen, pszUrlEncoded);
			if (iActuallyEncoded != iUrlEncodedLen) {
				SetError("Insane output length");
				delete[] pszUrlEncoded;
				return false;
			}
		}
	}
	
	// Add param name and encoded value to lists
	try {
		mlist_sParamNames.push_back(pszParamName);
		mlist_iParamDataLen.push_back(iUrlEncodedLen);
		mlist_pszParamDataUrlEncoded.push_back(pszUrlEncoded);
	}
	catch (...) {
		// Unspecified error - probably out of memory
		delete[] pszUrlEncoded;
		return false;
	}
	
	// Success
	return true;
} // AddParam


tbool CXloader::AssembleParams(EVerbType eVerb)
{
	CAutoLock Lock(mMutex_ForParams);
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	if ((mpszParamsAssembled) || (mpFormPost_First)) {
		SetError("Already assembled parameters");
		return false;
	}
	
	// Verify sanity of lists
	tint32 iNames = mlist_sParamNames.size();
	tint32 iLens = mlist_iParamDataLen.size();
	tint32 iDatas = mlist_pszParamDataUrlEncoded.size();
	if ((iNames != iLens) || (iLens != iDatas)) {
		SetError("List lengths aren't all the same");
		return false;
	}

	if (mbIsUploader)
		return AssembleParams_ForMultiPartForm(eVerb);
	else
		return AssembleParams_ForUrlEncoded(eVerb);
} // AssembleParams
	

tbool CXloader::AssembleParams_ForUrlEncoded(EVerbType eVerb)
{
	// Calculate space needed
	{
		miParamsAssembledLen = 0;
		
		// First accumulate lengths
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		for ( ; itName != mlist_sParamNames.end(); itName++, itDataLen++) {
			std::string& rsName = *itName;
			tint32 iLen = *itDataLen;
			
			// First param name
			miParamsAssembledLen += rsName.length();
			
			// Then maybe param data
			if (iLen > 0) {
				// An equation-sign and then data
				miParamsAssembledLen += 1 + iLen;
			}

			// Then a parameter delimiter
			miParamsAssembledLen++;			
		}
	}

	// Remove last parameter delimiter
	if (miParamsAssembledLen > 0) miParamsAssembledLen--;
	
	// Attempt to allocate space for params + trailing zero
	mpszParamsAssembled = new tchar[miParamsAssembledLen + 1];
	if (mpszParamsAssembled == NULL) {
		SetError("Out of memory");
		return false;
	}
	
	// And action! Assemble the parameter string
	{
		tchar* pszDst = mpszParamsAssembled;
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		std::list<tchar*>::iterator itParamData = mlist_pszParamDataUrlEncoded.begin();
		while (itName != mlist_sParamNames.end()) {
			// Get elements
			std::string& rsName = *itName;
			tint32 iNameLen = rsName.length();
			tchar* pcData = *itParamData;
			tint32 iDataLen = *itDataLen;
			
			// Copy param name
			memcpy(pszDst, rsName.c_str(), iNameLen);
			pszDst += iNameLen;
			
			// Maybe copy param data
			if (iDataLen > 0) {
				*pszDst++ = '=';
				memcpy(pszDst, pcData, iDataLen);
				pszDst += iDataLen;
			}
			
			// We delimit parameters
			*pszDst++ = '&';
			
			// Advance to next
			itName++;
			itDataLen++;
			itParamData++;
		}

		// Remove last delimiter
		if (miParamsAssembledLen > 0) {
			pszDst--;
		}

		// Append zero-termination
		*pszDst = '\0';
	}

	return true;
} // AssembleParams_ForUrlEncoded


tbool CXloader::AssembleParams_ForMultiPartForm(EVerbType eVerb)
{
	// First add any non-file parameters
	{
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		std::list<tchar*>::iterator itParamData = mlist_pszParamDataUrlEncoded.begin();
		while (itName != mlist_sParamNames.end()) {
			// Get elements
			std::string& rsName = *itName;
			//tint32 iNameLen = rsName.length();
			tchar* pcData = *itParamData;
			//tint32 iDataLen = *itDataLen;

			CURLFORMcode rc = (CURLFORMcode)0;

			// Add param name
			rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
				CURLFORM_PTRNAME, rsName.c_str());
			if (rc != 0) {
				tchar pszErr[128];
				sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_PTRNAME", rc);
				SetError(pszErr);
				return false;
			}

			/*
			// Set param contents length
			rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
				CURLFORM_BUFFERLENGTH, iDataLen);
			if (rc != 0) {
				tchar pszErr[128];
				sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_BUFFERLENGTH", rc);
				SetError(pszErr);
				return false;
			}
			*/

			// Add param contents
			rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
				CURLFORM_BUFFERPTR, pcData);
			if (rc != 0) {
				tchar pszErr[128];
				sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_BUFFERPTR", rc);
				SetError(pszErr);
				return false;
			}
			
			// Set contents type for param
			rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
				CURLFORM_CONTENTTYPE, "application/x-www-form-urlencoded");
			if (rc != 0) {
				tchar pszErr[128];
				sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_CONTENTTYPE", rc);
				SetError(pszErr);
				return false;
			}
			
			// Advance to next
			itName++;
			itDataLen++;
			itParamData++;
		}
	}

	// Finally add file
	{
		CURLFORMcode rc = (CURLFORMcode)0;

		// Add upload file param name
		rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
			CURLFORM_PTRNAME, msUploadFileParamName.c_str());
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_PTRNAME := upload file param name", rc);
			SetError(pszErr);
			return false;
		}

		// Upload as stream (avoid loading all of file in memory at once)
		rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last, CURLFORM_STREAM, this);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_STREAM", rc);
			SetError(pszErr);
			return false;
		}

		// Inform of total size of file to upload
		tint32 iTotalSize = (tint32)(mpfileToUpload->GetSizeWhenOpened());
		rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last, CURLFORM_CONTENTSLENGTH, iTotalSize);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_CONTENTSLENGTH", rc);
			SetError(pszErr);
			return false;
		}

		// Set file name for upload
		rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
			CURLFORM_FILENAME, msUploadFileNameAndExtOnly.c_str());
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_formadd(..) returned %d for CURLFORM_FILENAME", rc);
			SetError(pszErr);
			return false;
		}
	}

	return true;
} // AssembleParams_ForMultiPartForm


void CXloader::WipeParams()
{
	CAutoLock Lock(mMutex_ForParams);
	
	mlist_sParamNames.clear();
	
	std::list<tchar*>::iterator it = mlist_pszParamDataUrlEncoded.begin();
	while (it != mlist_pszParamDataUrlEncoded.end()) {
		tchar* psz = *it;
		if (psz) delete[] psz;
		mlist_pszParamDataUrlEncoded.erase(it);
		it = mlist_pszParamDataUrlEncoded.begin();
	}
	
	mlist_iParamDataLen.clear();
	
	if (mpszParamsAssembled) {
		delete[] mpszParamsAssembled;
		mpszParamsAssembled = NULL;
	}

	if (mpFormPost_First) {
		// Free entire chain of multi-part form params by freeing the first
		curl_formfree(mpFormPost_First);
		mpFormPost_First = mpFormPost_Last = NULL;
	}
} // WipeParams


size_t Static_ReadFunction_ForUpload(void *ptr, size_t size, size_t nmemb, void *stream)
{
	CXloader* pUploader = (CXloader*)stream;
	return pUploader->ReadFunction_ForUpload(ptr, size, nmemb);
} // Static_ReadFunction_ForUpload


size_t CXloader::ReadFunction_ForUpload(void *ptr, size_t size, size_t nmemb)
{
	if (mbIsFailed) {
		// We can't continue
		return CURL_READFUNC_ABORT;
	}

	tuint64 uiMaxNumberOfBytes = (tuint64)size * (tuint64)nmemb;

	tuint64 uiFileIx = mpfileToUpload->GetCurrentFilePosition();
	tuint64 uiFileSize = mpfileToUpload->GetSizeWhenOpened();
	tuint64 uiBytesAvailable = uiFileSize - uiFileIx;

	tuint64 uiBytesWanted = (uiBytesAvailable < uiMaxNumberOfBytes) ? uiBytesAvailable : uiMaxNumberOfBytes;
	tuint64 uiActuallyRead = mpfileToUpload->Read((tchar*)ptr, uiBytesWanted);
	if (uiActuallyRead != uiBytesWanted) {
		SetError("Unable to read file for upload");
		return CURL_READFUNC_ABORT;
	}

	return (size_t)uiActuallyRead;
} // ReadFunction_ForUpload


int Static_SeekFunction_ForUpload(void *instream, curl_off_t offset, int origin)
{
	CXloader* pUploader = (CXloader*)instream;
	return pUploader->SeekFunction_ForUpload(offset, origin);
} // Static_SeekFunction_ForUpload


int CXloader::SeekFunction_ForUpload(curl_off_t offset, int origin)
{
	if (mbIsFailed) {
		// We can't continue
		return CURL_READFUNC_ABORT;
	}
	tuint64 uiWantedPos = origin;
	uiWantedPos += offset;
	tuint64 uiActualPos = mpfileToUpload->Seek(uiWantedPos);
	if (uiActualPos != uiWantedPos) {
		SetError("SeekFunction_ForUpload failed");
		// We can't continue
		return CURL_READFUNC_ABORT;
	}
	// Success
	return 0;
} // SeekFunction_ForUpload


size_t Static_WriteFunction_ForReply(void *ptr, size_t size, size_t nmemb, void *stream)
{
	CXloader* pUploader = (CXloader*)stream;
	return pUploader->WriteFunction_ForReply(ptr, size, nmemb);
} // Static_WriteFunction_ForReply


size_t CXloader::WriteFunction_ForReply(void *ptr, size_t size, size_t nmemb)
{
	if (mbIsFailed) {
		// We can't continue
		// Signal that by returning a value that differs from write request
		return 0;
		// Special case: If file is empty we'll get called once with a size of 0
		// and so returning 0 won't signal error - but since we'll never get
		// called again after that anyway it won't matter.
	}

	tuint64 uiBytesToWrite = (tuint64)size * (tuint64)nmemb;
	if (uiBytesToWrite == 0) {
		// That's OK - reply is empty
		return 0;
	}
	muiUploadProgress += uiBytesToWrite;

	if (mpfileForReply) {
		// Write directly into file buffer
		return (size_t)(mpfileForReply->Write((tchar*)ptr, uiBytesToWrite));
	}
	else {
		//CAutoLock Lock(mMutex_ForReplyBuffer);
		// Get lock
		while (++miLockLevel_ForReplyBuffer != 1) {
			miLockLevel_ForReplyBuffer--;
			ITime::SleepMS(1);
		}

		CXloader_ReplyChainLink* pLink = new CXloader_ReplyChainLink(ptr, (tuint32)uiBytesToWrite);
		mlist_pReplyChain.push_back(pLink);
		
		// Release lock
		miLockLevel_ForReplyBuffer--;

		return (size_t)uiBytesToWrite;
	}
} // WriteFunction_ForReply


void CXloader::ZapReplyBuffer()
{
	while (++miLockLevel_ForReplyBuffer != 1) {
		miLockLevel_ForReplyBuffer--;
		ITime::SleepMS(1);
	}
	
	std::list<CXloader_ReplyChainLink*>::iterator it = mlist_pReplyChain.begin();
	for ( ; it != mlist_pReplyChain.end(); it++) {
		CXloader_ReplyChainLink* pLink = *it;
		delete pLink;
	}
	mlist_pReplyChain.clear();
	
	// Release lock
	miLockLevel_ForReplyBuffer--;
} // ZapReplyBuffer


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, const void* pData, const tchar* pszExtraInfo /*= ""*/)
{
	CURLcode rc = curl_easy_setopt(mpCURLEasyHandle, iOption, pData);
	if (rc != 0) {
		tchar pszErr[512];
		sprintf(pszErr, "curl_easy_setopt(..) returned error %d for %s%s\n%s", rc, pszOption, pszExtraInfo, mpszErrorBuffer);
		SetError(pszErr);
		return false;
	}
	return true;
} // SetOpt(void*)


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, tint32 iData, const tchar* pszExtraInfo /*= ""*/)
{
	return SetOpt(iOption, pszOption, (const void*)iData, pszExtraInfo);
} // SetOpt(tint64)


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, tuint32 uiData, const tchar* pszExtraInfo /*= ""*/)
{
	return SetOpt(iOption, pszOption, (const void*)uiData, pszExtraInfo);
} // SetOpt(tuint64)


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, const tchar* pszData, const tchar* pszExtraInfo /*= ""*/)
{
	return SetOpt(iOption, pszOption, (const void*)pszData, pszExtraInfo);
} // SetOpt(const tchar*)


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, const std::string& rsData, const tchar* pszExtraInfo /*= ""*/)
{
	return SetOpt(iOption, pszOption, (const void*)(rsData.c_str()), pszExtraInfo);
} // SetOpt(const std::string&)


tbool CXloader::SetOpt(CURLoption iOption, const tchar* pszOption, tbool bData, const tchar* pszExtraInfo /*= ""*/)
{
	tint64 iData = bData ? 1 : 0;
	return SetOpt(iOption, pszOption, (const void*)iData, pszExtraInfo); 
} // SetOpt(tbool)


tbool CXloader::OpenConnection()
{
	// No longer - will use libCURL instead
	//return OpenConnection_OSSpecific();

	// Get an "easy" handle
	if (mpCURLEasyHandle == NULL) {
		mpCURLEasyHandle = curl_easy_init();
		if (!SetOpt(CURLOPT_PRIVATE, "CURLOPT_PRIVATE", this, " => this"))
			return false;

	}

	// Set buffer for human readble error
	*mpszErrorBuffer = '\0';
	if (!SetOpt(CURLOPT_ERRORBUFFER, "CURLOPT_ERRORBUFFER", mpszErrorBuffer))
		return false;

	EVerbType eVerb = GetActuallyUsedVerb();

	// Assemble URL string
	std::string sURL = std::string("http://") + msHost;
	if (miPort != 80) {
		// Custom port
		tchar pszPort[16];
		sprintf(pszPort, ":%d", miPort);
		sURL += pszPort;
	}
	sURL += msPage;
	if ((eVerb == VERB_GET) && (miParamsAssembledLen > 0)) {
		// Add URL-encoded params as part of the URL
		sURL += "?";
		sURL += std::string(mpszParamsAssembled, miParamsAssembledLen);
	}

	// Set URL
	if (!SetOpt(CURLOPT_URL, "CURLOPT_URL", sURL))
		return false;

	// Set VERB for x-Loader
	if (mbIsUploader) {
		// This is an IUploader

		// Signal to use a POST verb + use multi-part form message body + append message body
		if (!SetOpt(CURLOPT_HTTPPOST, "CURLOPT_HTTPPOST", mpFormPost_First))
			return false;
		
		// Set callbacks for upload
		{
			// Set callback function for streaming upload file
			if (!SetOpt(CURLOPT_READFUNCTION, "CURLOPT_READFUNCTION", (void*)(&Static_ReadFunction_ForUpload)))
				return false;
			// Set pointer so callback function for streaming upload file can find correct contents
			if (!SetOpt(CURLOPT_READDATA, "CURLOPT_READDATA", this))
				return false;

			// Set callback function for seeking upload file
			if (!SetOpt(CURLOPT_SEEKFUNCTION, "CURLOPT_SEEKFUNCTION", (void*)(&Static_SeekFunction_ForUpload)))
				return false;
			// Set pointer so callback function for seeking upload file can find correct contents
			if (!SetOpt(CURLOPT_SEEKDATA, "CURLOPT_SEEKDATA", this))
				return false;
		}

		// To use the PUT verb we must set it explicitly
		if (eVerb == VERB_PUT) {
			if (!SetOpt(CURLOPT_CUSTOMREQUEST, "CURLOPT_CUSTOMREQUEST", "PUT", " := \"PUT\""))
				return false;
		}
	}
	else {
		// This is an IDownloader

		// Set URL-encoded message body (only for verb POST style IDownloader)
		if (eVerb == VERB_GET) {
			// Signal to use GET verb
			if (!SetOpt(CURLOPT_HTTPGET, "CURLOPT_HTTPGET", true))
				return false;
		}
		else { //if (eVerb == VERB_POST) {
			// Signal to use POST verb with a URL-encoded message body
			if (!SetOpt(CURLOPT_POST, "CURLOPT_POST", true))
				return false;

			// Prepare size of URL-encoded message body
			if (!SetOpt(CURLOPT_POSTFIELDSIZE, "CURLOPT_POSTFIELDSIZE", miParamsAssembledLen))
				return false;

			// Add URL-encoded message body
			if (!SetOpt(CURLOPT_POSTFIELDS, "CURLOPT_POSTFIELDS", mpszParamsAssembled))
				return false;
		}
	}

	// Setup receiver for reply / download
	{
		// Set callback function for saving download file / reply data
		if (!SetOpt(CURLOPT_WRITEFUNCTION, "CURLOPT_WRITEFUNCTION", (void*)(&Static_WriteFunction_ForReply)))
			return false;
		// Set pointer so callback function for receiving download / reply can find correct contents
		if (!SetOpt(CURLOPT_WRITEDATA, "CURLOPT_WRITEDATA", this))
			return false;
	}

	// Set user for authentication
	if (msUser.length() > 0) {
		if (!SetOpt(CURLOPT_USERNAME, "CURLOPT_USERNAME", msUser))
			return false;
	}

	// Set password for authentication
	if (msPassword.length() > 0) {
		if (!SetOpt(CURLOPT_PASSWORD, "CURLOPT_PASSWORD", msPassword))
			return false;
	}

	// Set time-out - but not the usual brain-dead way
	{
		// Set "nothing happening" time-out period
		tchar pszInfo[32];
		sprintf(pszInfo, " := %u seconds", muiTimeOutSecs);
		if (!SetOpt(CURLOPT_LOW_SPEED_TIME, "CURLOPT_LOW_SPEED_TIME", muiTimeOutSecs, pszInfo))
			return false;

		// Set "nothing happening" time-out trigger level: < 1 byte per second
		if (!SetOpt(CURLOPT_LOW_SPEED_LIMIT, "CURLOPT_LOW_SPEED_LIMIT", 1))
			return false;
	}

	// Set "get connected" time-out at 3 seconds (that's quite enough even for slow analogue modems)
	if (!SetOpt(CURLOPT_CONNECTTIMEOUT, "CURLOPT_CONNECTTIMEOUT", 3))
		return false;

	// Maybe set auto-redirect
	{
		// Allow/prohibit redirects
		if (!SetOpt(CURLOPT_FOLLOWLOCATION, "CURLOPT_FOLLOWLOCATION", mbAllowRedirects))
			return false;

		// Set referrer document when redirecting
		if (!SetOpt(CURLOPT_AUTOREFERER, "CURLOPT_AUTOREFERER", mbAllowRedirects))
			return false;

		// Allow/prohibit forward of username and password on redirect
		if (!SetOpt(CURLOPT_UNRESTRICTED_AUTH, "CURLOPT_UNRESTRICTED_AUTH", mbAllowRedirects))
			return false;
	}

	// Set MIME for reply (additional header)
	{ //if (meMIMEType != MIME_TYPE_NONE) {
		std::string sAccept = "Accept: ";
		sAccept += GetMIMEString();
		mpSList_ExtraHeaders = curl_slist_append(mpSList_ExtraHeaders, sAccept.c_str());
		if (!SetOpt(CURLOPT_HTTPHEADER, "CURLOPT_HTTPHEADER", mpSList_ExtraHeaders))
			return false;
	}

	// Make sure we fail on status codes indicating error
	if (!SetOpt(CURLOPT_FAILONERROR, "CURLOPT_FAILONERROR", true))
		return false;

	// Success
	return true;
} // OpenConnection


void CXloader::CloseConnection()
{
	// No longer - will use libCURL instead
	//CloseConnection_OSSpecific();
	
	// Remove handle from "multi" handler, effectively kills any transfers
	if (mpCURLEasyHandle) {
		std::string sErr;
		if (!CXloader_MultiWrapper::Remove(this, &sErr)) {
			SetError(sErr.c_str());
		}
	}

	// Release extra headers chain
	if (mpSList_ExtraHeaders) {
		curl_slist_free_all(mpSList_ExtraHeaders);
		mpSList_ExtraHeaders = NULL;
	}

	ZapReplyBuffer();

	CloseFile_IgnoreError();
} // CloseConnection


tbool CXloader::Start(IFile* pfileForDownload /*= NULL*/)
{
	CAutoLock Lock(mMutex_Connection);

	if (IsFailed()) {
		//SetError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		SetError("Not initialized");
		return false;
	}
	
	if (IsTransfering()) {
		SetError("Already transfering");
		return false;
	}

	CloseConnection();
	if (!AssembleParams(GetActuallyUsedVerb())) return false;
	if (!OpenConnection()) {
		CloseConnection();
		return false;
	}
	mpfileForReply = pfileForDownload;
	SetIsTransfering();

	/*
	// Start transfer
	{
		// Get lock
		GetLockForMultiInstance();

		// Add easy handle to multi
		CURLMcode rc = curl_multi_add_handle(gpCURLMulti, mpCURLEasyHandle);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_multi_add_handle(..) returned %d", rc);
			SetError(pszErr);
			// Release lock
			ReleaseLockForMultiInstance();
			return false;
		}

		// Start all 
		tint32 iRunningHandles_Dummy = 0;
		do {
			rc = curl_multi_perform(gpCURLMulti, &iRunningHandles_Dummy);
		} while (rc == CURLM_CALL_MULTI_PERFORM);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_multi_perform(..) returned %d", rc);
			SetError(pszErr);
			// Release lock
			ReleaseLockForMultiInstance();
			return false;
		}

		// Release lock
		ReleaseLockForMultiInstance();
	}
	*/

	std::string sErr;
	if (!CXloader_MultiWrapper::Add(this, &sErr)) {
		SetError(sErr.c_str());
		return false;
	}

	// Success
	return true;
} // Start


tbool CXloader::GetReplyPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize)
{
	*piPortionSize = 0;
	
	if (IsFailed()) {
		//SetError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		SetError("Not initialized");
		return false;
	}
	
	if ((!IsTransfering()) && (!IsDone())) {
		SetError("Not transfering and not done");
		return false;
	}

	if (mpfileForReply != NULL) {
		SetError("Download / reply goes directly to a file, so you can't use GetReplyPortion(..)");
		return false;
	}

	// Get data
	{
		// Make sure we're the only thread working on data
		//CAutoLock Lock(mMutex_ForReplyBuffer);
		// Get lock
		if (++miLockLevel_ForReplyBuffer == 1) {
			tchar* pszDst = pszBuffer;
			tuint32 uiBytesToGet = iBufferSize;
			while (uiBytesToGet > 0) {
				if (mlist_pReplyChain.size() > 0) {
					CXloader_ReplyChainLink* pLink = *(mlist_pReplyChain.begin());
					tuint32 uiGot = pLink->GetBytes(pszDst, uiBytesToGet);
					if (uiGot == 0) {
						// That one's empty
						delete pLink;
						mlist_pReplyChain.erase(mlist_pReplyChain.begin());
					}
					else {
						// Remember how much we got
						uiBytesToGet -= uiGot;
						pszDst += uiGot;
						*piPortionSize += uiGot;
					}
				}
				else {
					// Not enough buffered data to fill receiver - but it's still a success
					uiBytesToGet = 0;
				}
			}
			
			if (mlist_pReplyChain.size() == 0) {
				// No more data in buffer
				// Are we done?
				if (mbIsMultiDone) {
					// Yes, multi handler already told us before that we're done,
					// and now all data has been delivered too, so:
					SetIsDone();
				}
			}
		}
		// Release lock
		miLockLevel_ForReplyBuffer--;
	}

	// Success (and returned all bytes requested)
	return true;
} // GetReplyPortion


/*
tbool CXloader::UploadPortion(tuint64* puiUploadProgress, tchar* pszReplyBuffer, tint32 iReplyBufferSize, tint32* piReplyPortionSize, tuint64* puiReplyTotalSize)
{
	*piReplyPortionSize = 0;
	
	if (IsFailed()) {
		//SetError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		SetError("Not initialized");
		return false;
	}
	
	tbool bFirstTime = (!IsTransfering());
	SetIsTransfering();
	if (bFirstTime) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		if (!AssembleParams()) return false;
		if (!OpenConnection()) {
			CloseConnection();
			return false;
		}
		// We're alive
		RefreshAlive();
	}
	
	return DoPortion_OSSpecific(puiUploadProgress, pszReplyBuffer, iReplyBufferSize, piReplyPortionSize, puiReplyTotalSize);
} // UploadPortion
*/


tbool CXloader::Abort()
{
	if (IsTransfering()) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		SetIsDone();
	}
	
	WipeParams();
	
	CloseFile_IgnoreError();	
	
	return true;
} // Abort


tbool CXloader::GetProgress(tint64* piUploadProgress, tint64* piUploadSize, tint64* piDownloadProgress, tint64* piDownloadSize)
{
	if (IsFailed()) {
		//SetError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		SetError("Not initialized");
		return false;
	}
	
	if (piUploadProgress) *piUploadProgress = muiUploadProgress;
	if (piUploadSize) *piUploadSize = (mpfileToUpload) ? mpfileToUpload->GetSizeWhenOpened() : 0;
	if (piDownloadProgress) *piDownloadProgress = muiReplyProgress;
	if (piDownloadSize) *piDownloadSize = max(muiReplySize, 1);

	return true;
} // GetProgress(for IUploader)


tbool CXloader::GetProgress(tint64* piDownloadProgress, tint64* piDownloadSize)
{
	return GetProgress(NULL, NULL, piDownloadProgress, piDownloadSize);
} // GetProgress(for IDownloader)


void CXloader::CloseFile_IgnoreError()
{
	if (mpfileToUpload) {
		//mpfileToUpload->Destroy(); - will be Destroy'ed from calling app
		mpfileToUpload = NULL;
	}
} // CloseFile_IgnoreError


void CXloader::SetIsUninitialized()
{
	mbIsInitialized = mbIsTransfering = mbIsMultiDone = mbIsDone = mbIsFailed = false;
} // SetIsUninitialized


void CXloader::SetIsInitialized()
{
	mbIsInitialized = true;
} // SetIsInitialized


void CXloader::SetIsTransfering()
{
	mbIsFailed = mbIsDone = false;
	mbIsTransfering = true;
} // SetIsTransfering


void CXloader::SetMultiSaysDone(CURLcode status)
{
	if (!IsFailed()) {
		if (status != 0) {
			// There's an error
			tchar pszErr[512];
			*pszErr = '\0';
			if (status == CURLE_HTTP_RETURNED_ERROR) {
				// An error was reported by web server
				tint32 iResponse = 0;
				CURLcode rc = curl_easy_getinfo(
					mpCURLEasyHandle,
					CURLINFO_RESPONSE_CODE,
					&iResponse
					);
				if ((rc == 0) && (iResponse != 0)) {
					sprintf(pszErr, "%s responded with error %d", msHost.c_str(), iResponse);
				}
			}
			if (*pszErr == '\0') {
				// Some other error
				sprintf(pszErr, "curl_multi_info_read(..) gave status DONE but code %d", status);
			}
			SetError(pszErr);
		}
		else {
			// External transfer is done
			mbIsMultiDone = true;
			if (mpfileForReply != NULL) {
				// No internal buffer - set all transfer done
				SetIsDone();
			}
		}
	}
} // SetMultiSaysDone


void CXloader::SetIsDone()
{
	if (!IsFailed()) {
		mbIsDone = true;
		mbIsTransfering = false;
	}
} // SetIsDone


void CXloader::SetIsFailed()
{
	mbIsFailed = true;
	mbIsTransfering = false;
} // SetIsFailed


tbool CXloader::IsTransfering()
{
	return mbIsTransfering;
} // IsTransfering


tbool CXloader::IsInitialized()
{
	return mbIsInitialized;
} // IsInitialized


tbool CXloader::IsDone()
{
	return mbIsDone;
} // IsDone


tbool CXloader::IsFailed()
{
	return mbIsFailed;
} // IsFailed


/*
void CXloader::RefreshAlive()
{
	muiAliveMs = ITime::GetTimeMS();
} // RefreshAlive


tbool CXloader::IsAlive()
{
	// (lasse) Are we really needing this? Anyway not used now
	
	if ((!mbIsInitialized) || (!mbIsTransfering)) {
		SetError("Not initialized or not downloading");
		return false;
	}
	
	if (mbIsFailed) {
		//SetError("Previously failed");
		return false;
	}
	
	tuint32 uiElapsedMs = ITime::GetTimeMS() - muiAliveMs;
	
	if (muiTimeOutSecs == 0) {
		// Special case for zero time-out: Actually use 500 ms
		return (uiElapsedMs < 500);
	}
	
	tuint32 uiSecsElapsed = uiElapsedMs / 1000;
	return (uiSecsElapsed <= muiTimeOutSecs);
} // IsAlive
*/


void CXloader::SetError(const tchar* pszError)
{
	CAutoLock Lock(mMutex_ForErrors);
	
	if (IsFailed()) {
		// Append error
		msLastError += std::string("\n") + pszError;
	}
	else {
		// New error
		msLastError = pszError;
	}
	SetIsFailed();
} // SetError


tbool CXloader::GetError(tchar* pszErrBuff, tint32 iErrBuffSize)
{
	CAutoLock Lock(mMutex_ForErrors);
	
	if ((pszErrBuff == NULL) || (iErrBuffSize <= 1)) {
		// Invalid input - can't do anything!
		return false;
	}
	
	if (!mbIsFailed) {
		// No error
		*pszErrBuff = '\0';
		return true;
	}
	
	tchar* pszMsg = (tchar*)(msLastError.c_str());
	tint32 iLenMsg = msLastError.length();
	if (iLenMsg == 0) {
		// No error description available
		pszMsg = "Unspecified error";
		iLenMsg = strlen(pszMsg);
	}
	tbool bRoomEnough = iLenMsg < iErrBuffSize;
	if (!bRoomEnough) {
		// Cut off
		iLenMsg = iErrBuffSize - 1;
	}
	memcpy(pszErrBuff, pszMsg, iLenMsg);
	pszErrBuff[iLenMsg] = '\0';
	
	return bRoomEnough;
} // GetError
