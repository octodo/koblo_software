
#include "ineInternalOS.h"


//

CXloader::CXloader(tbool bIsUploader)
{
	mbIsUploader = bIsUploader;
	mbUseAuthentication = false;

	mpfileForReply = NULL;
	mpszParamsAssembled = NULL;
	miParamsAssembledLen = 0;
	mbIsInitialized = mbIsTransfering = mbIsFailed = mbIsMultiDone = mbIsDone = false;
	meReplyMIMEType = MIME_TYPE_NONE;
	meSpecificVerb = VERB_DEFAULT;
	mbAllowRedirects = true;

	miLockLevel_ForReplyBuffer = 0;
	
	// Make sure we only close what's been opened
	mpCURLEasyHandle = NULL;
	mpFormPost_First = mpFormPost_Last = NULL;
	mpSList_ExtraHeaders = NULL;
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


tbool CXloader::Init(const tchar* pszFullURL, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	SetIsFullBlownURL(true);
	return _Init(pszFullURL, NULL, NULL, 0, pszUser, pszPassword, iTimeOutSecs);
} // Init

tbool CXloader::Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	SetIsFullBlownURL(false);
	return _Init(NULL, pszHost, pszPage, iPort, pszUser, pszPassword, iTimeOutSecs);
} // Init


tbool CXloader::_Init(const tchar* pszFullURL, const tchar* pszHost, const tchar* pszPage, tint32 iPort /*= 80*/, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	Abort();
	WipeParams();
	ZapReplyBuffer();
	//mbIsInitialized = false;
	SetIsUninitialized();

	if (mbIsInit_FullBlownURL) {
		// Loosely verify sanity of full blown URL (libCURL itself will verify it more elaborately)
		if ((pszFullURL == NULL) || (*pszFullURL == '\0')) {
			AppendError("No URL");
			return false;
		}
		for (tchar* pc = (tchar*)pszFullURL; *pc; pc++) {
			tchar c = *pc;
			if (
				((c < 'A') || (c > 'Z'))
				&&
				((c < 'a') || (c > 'z'))
				&&
				((c < '0') || (c > '9'))
				&&
				(
					(c != '.') && (c != '-')
					&&
					(c != ':') && (c != '/')
					&&
					(c != '_') && (c != '%')
				)
			) {
				std::string sErr = std::string("Not valid URL:\n  ") + pszFullURL;
				AppendError(sErr.c_str());
				return false;
			}
		}
	}
	else {
		// Verify sanity of host
		{
			if ((pszHost == NULL) || (*pszHost == '\0')) {
				AppendError("No URL-host");
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
					((c != '.') && (c != '-'))
				) {
					AppendError("Not valid URL-host");
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
						((c != '.') && (c != '-'))
						&&
						(c != '/')
						) {
						AppendError("Not valid url-document");
						return false;
					}
				}
			}
		}

		// Verify sanity of port
		if (iPort <= 0) {
			AppendError("Not a valid port");
			return false;
		}
	}

	// Verify sanity of time-out
	if (iTimeOutSecs < 0) {
		AppendError("Negative time-out");
		return false;
	}
	
	msURL = (pszFullURL == NULL) ? "" : pszFullURL;
	msHost = (pszHost == NULL) ? "" : pszHost;
	msPage = (pszPage == NULL) ? "" : pszPage;
	miPort = iPort;
	msUser = (pszUser == NULL) ? "" : pszUser;
	msPassword = (pszPassword == NULL) ? "" : pszPassword;
	muiTimeOutSecs = (tuint32)iTimeOutSecs;

	meReplyMIMEType = MIME_TYPE_NONE;
	meSpecificVerb = VERB_DEFAULT;
	mbAllowRedirects = true;
	mbUseStreamingUpload = true;
	mpfileForReply = NULL;

	muiUploadProgress = muiUploadSize = muiReplyProgress = muiReplySize = 0;
	miHttpStatus = 0;

	mbFailImmediatelyOnStatus = false;
	msDelayedStatusError = "";

	mbIsInitialized = true;
	return true;
} // _Init


tbool CXloader::SetReplyMIMEType(E_MIME_Type eMIME)
{
	if (mbIsFailed) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't set media type as we've already begun transfer");
		return false;
	}
	
	// Success
	meReplyMIMEType = eMIME;
	return true;
} // SetReplyMIMEType


const tchar* CXloader::GetReplyMIMEString()
{
	return GetMIMEString(meReplyMIMEType);
} // GetReplyMIMEString


const tchar* CXloader::GetMIMEString(E_MIME_Type eMIME)
{
	switch (eMIME) {
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
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't change http-verb as we've already begun transfer");
		return false;
	}
	
	switch (eVerb) {
		case VERB_DEFAULT:
			meSpecificVerb = eVerb;
			return true;
			
		case VERB_GET:
		{
			// Only IDownloader may use GET
			if (!mbIsUploader)
				return true;
		}
			break;
			
		case VERB_POST:
			meSpecificVerb = eVerb;
			return true;
			
		case VERB_PUT:
		{
			// Only IUploader may use PUT
			if (mbIsUploader)
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
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't change redirect behaviour as we've already begun transfer");
		return false;
	}

	mbAllowRedirects = bAllow;
	return true;
} // SetAllowRedirects


tbool CXloader::DisableAutoRedirects()
{ return SetAllowRedirects(false); }


tbool CXloader::EnableAutoRedirects()
{ return SetAllowRedirects(true); }


tbool CXloader::SetFailOnHttpStatus(tbool bFailOnStatus)
{
	if (mbIsFailed) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't change http status behaviour as we've already begun transfer");
		return false;
	}

	mbFailImmediatelyOnStatus = bFailOnStatus;
	return true;
} // SetFailOnHttpStatus


tbool CXloader::SetStreamingUpload(tbool bUseStreaming)
{
	if (mbIsFailed) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't change streaming behaviour as we've already begun transfer");
		return false;
	}

	mbUseStreamingUpload = bUseStreaming;
	return true;
} // SetStreamingUpload


tbool CXloader::AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen)
{
	CAutoLock Lock(mMutex_ForParams);
	
	if (mbIsFailed) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't add more parameters as we've already begun transfer");
		return false;
	}
	
	// Verify sanity of param name
	if (!VerifyParamName(pszParamName))
		return false;
	
	if (mbIsUploader) {
		tchar* pszData = NULL;
		tint32 iDataLen = 0;
		// No need to verify data for uploader - it'll eat anything!
		// Just a special case for NULL pointer
		if (pcParamData) {
			if (iParamDataLen >= 0)
				iDataLen = iParamDataLen;
			else
				iDataLen = strlen(pcParamData);
			// Copy to a buffer we own (so we don't need to rely on calling app keeping data around)
			pszData = new tchar[iDataLen + 1];
			memcpy(pszData, pcParamData, iDataLen);
			pszData[iDataLen] = '\0';
		}
		// Add param name and raw value to lists
		try {
			mlist_sParamNames.push_back(pszParamName);
			mlist_iParamDataLen.push_back(iDataLen);
			mlist_pszParamData.push_back(pszData);
		}
		catch (...) {
			// Unspecified error - probably out of memory
			return false;
		}

		// Success
		return true;
	}

	
	// For downloader we must verify and convert to URL-encoded etc.

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
				AppendError("Out of memory");
				return false;
			}
			tint32 iActuallyEncoded = IINetUtil::URLEncode(pcParamData, iParamDataLen, pszUrlEncoded);
			if (iActuallyEncoded != iUrlEncodedLen) {
				AppendError("Insane output length");
				delete[] pszUrlEncoded;
				return false;
			}
		}
	}
	
	// Add param name and encoded value to lists
	try {
		mlist_sParamNames.push_back(pszParamName);
		mlist_iParamDataLen.push_back(iUrlEncodedLen);
		mlist_pszParamData.push_back(pszUrlEncoded);
	}
	catch (...) {
		// Unspecified error - probably out of memory
		delete[] pszUrlEncoded;
		return false;
	}
	
	// Success
	return true;
} // AddParam


tbool CXloader::AddFileParam(const tchar* pszParamName, IFile* pfileToUpload, tchar* pszDestinationName /*= NULL*/, E_MIME_Type eMIME /*= MIME_TYPE_DEFAULT*/)
{
	CAutoLock Lock(mMutex_ForParams);
	
	if (mbIsFailed) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if (mbIsTransfering) {
		AppendError("We can't add more parameters as we've already begun transfer");
		return false;
	}
	
	// Verify sanity of param name
	if (!VerifyParamName(pszParamName))
		return false;

	if (!pfileToUpload) {
		AppendError("File to upload is NULL");
		return false;
	}

	// Get name of destination file (on upload server)
	std::string sNameAndExt;
	if ((pszDestinationName) && (*pszDestinationName)) {
		// We have a custom name, use that
		sNameAndExt = pszDestinationName;
	}
	else {
		// Extract path and name from upload IFile object
		if ((dynamic_cast<IFileMemory*>(pfileToUpload)) != NULL) {
			// This is a memory file - no name
			sNameAndExt = "(no name)";
		}
		else {
			tchar pszFilePathName[1024];
			pfileToUpload->GetPathName(pszFilePathName);
			tchar* pszColon = strrchr(pszFilePathName, ':');
			if (pszColon == NULL) {
				AppendError("Internal error: Unexpected format of file path");
				return false;
			}
			sNameAndExt = pszColon + 1;
		}
	}

	// Add file to param list
	SUploadStream* pS = new SUploadStream();
	pS->mpThis = this;
	pS->msParamName = pszParamName;
	pS->mpfile = pfileToUpload;
	pS->meMIME = eMIME;
	pS->msNameAndExtOnly = sNameAndExt;
	mlistUploadFiles.push_back(pS);

	// Success
	return true;
} // AddFileParam


tbool CXloader::VerifyParamName(const tchar* pszParamName)
{
	if ((pszParamName == NULL) || (*pszParamName == '\0')) {
		AppendError("No param name");
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
			AppendError("Not valid paramname");
			return false;
		}
	}

	return true;
} // VerifyParamName


tbool CXloader::AssembleParams()
{
	CAutoLock Lock(mMutex_ForParams);
	
	if (!mbIsInitialized) {
		AppendError("Not initialized");
		return false;
	}
	
	if ((mpszParamsAssembled) || (mpFormPost_First)) {
		AppendError("Already assembled parameters");
		return false;
	}
	
	// Verify sanity of lists
	tint32 iNames = mlist_sParamNames.size();
	tint32 iLens = mlist_iParamDataLen.size();
	tint32 iDatas = mlist_pszParamData.size();
	if ((iNames != iLens) || (iLens != iDatas)) {
		AppendError("List lengths aren't all the same");
		return false;
	}

	if (mbIsUploader)
		return AssembleParams_ForMultiPartForm();
	else
		return AssembleParams_ForUrlEncoded();
} // AssembleParams
	

tbool CXloader::AssembleParams_ForUrlEncoded()
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
		AppendError("Out of memory");
		return false;
	}
	
	// And action! Assemble the parameter string
	{
		tchar* pszDst = mpszParamsAssembled;
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		std::list<tchar*>::iterator itParamData = mlist_pszParamData.begin();
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


tbool CXloader::AssembleParams_ForMultiPartForm()
{
	// First add any non-file parameters
	{
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		std::list<tchar*>::iterator itParamData = mlist_pszParamData.begin();
		while (itName != mlist_sParamNames.end()) {
			// Get elements
			std::string& rsName = *itName;
			tint32 iNameLen = rsName.length();
			tchar* pcData = *itParamData;
			tint32 iDataLen = *itDataLen;

			CURLFORMcode rc = (CURLFORMcode)0;

			// Add param
			rc = curl_formadd(&mpFormPost_First, &mpFormPost_Last,
				CURLFORM_COPYNAME, rsName.c_str(),
				CURLFORM_NAMELENGTH, iNameLen,
				CURLFORM_COPYCONTENTS, pcData,
				CURLFORM_CONTENTSLENGTH, iDataLen,
				CURLFORM_END
				);

			if (rc != 0) {
				tchar pszErr[512];
				sprintf(pszErr, "curl_formadd(..) returned %d for param %s", rc, rsName.c_str());
				AppendError(pszErr);
				return false;
			}

			// Advance to next
			itName++;
			itDataLen++;
			itParamData++;
		}
	}

	// Finally add file(s)
	std::list<SUploadStream*>::iterator it = mlistUploadFiles.begin();
	for ( ; it != mlistUploadFiles.end(); it++) {
		SUploadStream* pS = *it;
		//
		std::string& rsParamName = pS->msParamName;
		IFile* pfile = pS->mpfile;
		std::string& rsNameAndExtOnly = pS->msNameAndExtOnly;
		tint32 iTotalSize = (tint32)(pS->mpfile->GetSizeWhenOpened());

		// Array for upload of file
		curl_forms formsArrayForAdd[10];
		tint32 iAryIx = 0;

		// Add common data
		formsArrayForAdd[iAryIx].option = CURLFORM_PTRNAME;
		formsArrayForAdd[iAryIx++].value = rsParamName.c_str();
		formsArrayForAdd[iAryIx].option = CURLFORM_FILENAME;
		formsArrayForAdd[iAryIx++].value = rsNameAndExtOnly.c_str();
		formsArrayForAdd[iAryIx].option = CURLFORM_CONTENTSLENGTH;
		formsArrayForAdd[iAryIx++].value = (const char*)iTotalSize;
		if (pS->meMIME != MIME_TYPE_DEFAULT) {
			formsArrayForAdd[iAryIx].option = CURLFORM_CONTENTTYPE;
			formsArrayForAdd[iAryIx++].value = GetMIMEString(pS->meMIME);
		}

		// Add upload file in correct way
		if (mbUseStreamingUpload) {
			// Upload as stream (avoid loading all of file in memory at once)
			formsArrayForAdd[iAryIx].option = CURLFORM_STREAM;
			formsArrayForAdd[iAryIx++].value = (const char*)pS;
		}
		else {
			// Load entire upload file into memory
			IFileMemory* pMem = dynamic_cast<IFileMemory*>(pS->mpfile);
			if (pMem) {
				AppendError("Non-streaming IFileMemory isn't supported yet");
				return false;
			}
			else {
				// Add file path, so CURL can load it itself
				tchar pszFilePath[1024];
				pfile->GetPathName(pszFilePath);
				IFile::PathToOS2(pszFilePath, pszFilePath);
				formsArrayForAdd[iAryIx].option = CURLFORM_FILE;
				formsArrayForAdd[iAryIx++].value = pszFilePath;
			}
		}

		// Mark end of array
		formsArrayForAdd[iAryIx].option = CURLFORM_END;

		// Perform addition of array
		CURLFORMcode rc = curl_formadd(
			&mpFormPost_First, &mpFormPost_Last,
			CURLFORM_ARRAY, formsArrayForAdd,
			CURLFORM_END
			);
		if (rc != 0) {
			tchar pszErr[128];
			sprintf(pszErr, "curl_formadd(..) returned %d for file %s", rc, rsNameAndExtOnly.c_str());
			AppendError(pszErr);
			return false;
		}
	}

	return true;
} // AssembleParams_ForMultiPartForm


void CXloader::WipeParams()
{
	CAutoLock Lock(mMutex_ForParams);
	
	mlist_sParamNames.clear();
	
	if (!mbIsUploader) {
		std::list<tchar*>::iterator it = mlist_pszParamData.begin();
		while (it != mlist_pszParamData.end()) {
			tchar* psz = *it;
			if (psz) delete[] psz;
			mlist_pszParamData.erase(it);
			it = mlist_pszParamData.begin();
		}
	}
	mlist_iParamDataLen.clear();
	
	while (mlist_pszParamData.size() > 0) {
		tchar* pszData = mlist_pszParamData.front();
		mlist_pszParamData.pop_front();
		if (pszData) {
			delete[] pszData;
			pszData = NULL;
		}
	}
	
	if (mpszParamsAssembled) {
		delete[] mpszParamsAssembled;
		mpszParamsAssembled = NULL;
	}

	if (mpFormPost_First) {
		// Free entire chain of multi-part form params by freeing the first
		curl_formfree(mpFormPost_First);
		mpFormPost_First = mpFormPost_Last = NULL;
	}

	while (mlistUploadFiles.size()) {
		SUploadStream* p = mlistUploadFiles.front();
		delete p;
		mlistUploadFiles.pop_front();
	}
} // WipeParams


size_t Static_ReadFunction_ForUpload(void *ptr, size_t size, size_t nmemb, void *stream)
{
	CXloader::SUploadStream* pS = (CXloader::SUploadStream*)stream;
	return pS->mpThis->ReadFunction_ForUpload(pS->mpfile, ptr, size, nmemb);
} // Static_ReadFunction_ForUpload


size_t CXloader::ReadFunction_ForUpload(IFile* pfile, void *ptr, size_t size, size_t nmemb)
{
	if (mbIsFailed) {
		// We can't continue
		return CURL_READFUNC_ABORT;
	}

	tuint64 uiMaxNumberOfBytes = (tuint64)size * (tuint64)nmemb;

	tuint64 uiFileIx = pfile->GetCurrentFilePosition();
	tuint64 uiFileSize = pfile->GetSizeWhenOpened();
	tuint64 uiBytesAvailable = uiFileSize - uiFileIx;

	tuint64 uiBytesWanted = (uiBytesAvailable < uiMaxNumberOfBytes) ? uiBytesAvailable : uiMaxNumberOfBytes;
	tuint64 uiActuallyRead = pfile->Read((tchar*)ptr, uiBytesWanted);
	if (uiActuallyRead != uiBytesWanted) {
		AppendError("Unable to read file for upload");
		return CURL_READFUNC_ABORT;
	}

	return (size_t)uiActuallyRead;
} // ReadFunction_ForUpload


//int Static_SeekFunction_ForUpload(void *instream, curl_off_t offset, int origin)
//{
//	CXloader::SUploadStream* pS = (CXloader::SUploadStream*)instream;
//	return pS->mpThis->SeekFunction_ForUpload(pS->mpfile, offset, origin);
//} // Static_SeekFunction_ForUpload


//int CXloader::SeekFunction_ForUpload(IFile* pfile, curl_off_t offset, int origin)
//{
//	if (mbIsFailed) {
//		// We can't continue
//		return CURL_READFUNC_ABORT;
//	}
//	tuint64 uiWantedPos = origin;
//	uiWantedPos += offset;
//	tuint64 uiActualPos = pfile->Seek(uiWantedPos);
//	if (uiActualPos != uiWantedPos) {
//		AppendError("SeekFunction_ForUpload failed");
//		// We can't continue
//		return CURL_READFUNC_ABORT;
//	}
//	// Success
//	return 0;
//} // SeekFunction_ForUpload


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


int Static_ProgressFunction(void* p, double dDownloadSize, double dDownloaded, double dUploadSize, double dUploaded)
{
	CXloader* pthis = (CXloader*)p;
	return pthis->ProgressFunction(dDownloadSize, dDownloaded, dUploadSize, dUploaded);
} // Static_ProgressFunction


int CXloader::ProgressFunction(double dDownloadSize, double dDownloaded, double dUploadSize, double dUploaded)
{
	muiUploadProgress	= (tuint64)dUploaded;
	muiUploadSize		= (tuint64)dUploadSize;
	muiReplyProgress	= (tuint64)dDownloaded;
	muiReplySize		= (tuint64)dDownloadSize;
	
	// Success
	return 0;
} // ProgressFunction


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
		AppendError(pszErr);
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
	// We must first check to see if we need to kill previous authentication
	tbool bUseAuthentication_ThisTime = ((msUser.length() > 0) || (msPassword.length() > 0));
	if (mbUseAuthentication && !bUseAuthentication_ThisTime) {
		// We have to kill the "easy" handle to reset authentication
		if (mpCURLEasyHandle) {
			curl_easy_cleanup(mpCURLEasyHandle);
			mpCURLEasyHandle = NULL;
		}
	}
	mbUseAuthentication = bUseAuthentication_ThisTime;

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

	// Do we need to assemble URL string ourselves?
	if (!mbIsInit_FullBlownURL) {
		// Assemble URL string
		msURL = std::string("http://") + msHost;
		if (miPort != 80) {
			// Custom port
			tchar pszPort[16];
			sprintf(pszPort, ":%d", miPort);
			msURL += pszPort;
		}
		msURL += msPage;
	}

	// Maybe add parameters as part of URL string
	EVerbType eVerb = GetActuallyUsedVerb();
	if ((eVerb == VERB_GET) && (miParamsAssembledLen > 0)) {
		// Add URL-encoded params as part of the URL
		msURL += "?";
		msURL += std::string(mpszParamsAssembled, miParamsAssembledLen);
	}

	// Set URL
#ifdef _DEBUG
	std::string sTest = msURL;
#endif // _DEBUG
	if (!SetOpt(CURLOPT_URL, "CURLOPT_URL", msURL)) {
		AppendError((std::string("Unable to set URL:  ") + msURL).c_str());
		return false;
	}

	// Set VERB for x-Loader
	if (mbIsUploader) {
		// This is an IUploader

		// Set callbacks for upload
		{
			// Set callback function for streaming upload file
			if (!SetOpt(CURLOPT_READFUNCTION, "CURLOPT_READFUNCTION", (void*)(&Static_ReadFunction_ForUpload)))
				return false;
			// Set pointer so callback function for streaming upload file can find correct contents
			if (!SetOpt(CURLOPT_READDATA, "CURLOPT_READDATA", this))
				return false;

			/*
			// Set callback function for seeking upload file
			if (!SetOpt(CURLOPT_SEEKFUNCTION, "CURLOPT_SEEKFUNCTION", (void*)(&Static_SeekFunction_ForUpload)))
				return false;
			// Set pointer so callback function for seeking upload file can find correct contents
			if (!SetOpt(CURLOPT_SEEKDATA, "CURLOPT_SEEKDATA", this))
				return false;
			*/

			// Set callback function for getting progress info
			if (!SetOpt(CURLOPT_PROGRESSFUNCTION, "CURLOPT_PROGRESSFUNCTION", (void*)(&Static_ProgressFunction)))
				return false;
			// Set pointer so callback function for progress info can find correct contents
			if (!SetOpt(CURLOPT_PROGRESSDATA, "CURLOPT_PROGRESSDATA", this))
				return false;
			// Enable progress info (notice the upside/down bool...)
			if (!SetOpt(CURLOPT_NOPROGRESS, "CURLOPT_NOPROGRESS", false))
				return false;
		}

		// Signal to use a POST verb + use multi-part form message body + append message body
		if (!SetOpt(CURLOPT_HTTPPOST, "CURLOPT_HTTPPOST", mpFormPost_First))
			return false;
		
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

	if (mbUseAuthentication) {
		// Set user for authentication
		if (!SetOpt(CURLOPT_USERNAME, "CURLOPT_USERNAME", msUser))
			return false;

		// Set password for authentication
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

	// (lasse) connect time-out up'ed to 30 seconds as reality proves it will fail for 10 secs some times
	// Set "get connected" time-out at 10 seconds (that's quite enough even for slow analogue modems)
	if (!SetOpt(CURLOPT_CONNECTTIMEOUT, "CURLOPT_CONNECTTIMEOUT", 30))//10))
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
	{
		std::string sAccept = "Accept: ";
		sAccept += GetReplyMIMEString();
		mpSList_ExtraHeaders = curl_slist_append(mpSList_ExtraHeaders, sAccept.c_str());
		if (!SetOpt(CURLOPT_HTTPHEADER, "CURLOPT_HTTPHEADER", mpSList_ExtraHeaders))
			return false;
	}

	// Make sure we fail on status codes indicating error
	if (mbFailImmediatelyOnStatus) {
		if (!SetOpt(CURLOPT_FAILONERROR, "CURLOPT_FAILONERROR", true))
			return false;
	}

	// Success
	return true;
} // OpenConnection


void CXloader::CloseConnection()
{
	// Remove handle from "multi" handler, effectively kills any transfers
	if (mpCURLEasyHandle) {
		std::string sErr;
		if (!CXloader_MultiWrapper::Remove(this, &sErr)) {
			AppendError(sErr.c_str());
		}
	}

	// Release extra headers chain
	if (mpSList_ExtraHeaders) {
		curl_slist_free_all(mpSList_ExtraHeaders);
		mpSList_ExtraHeaders = NULL;
	}

	ZapReplyBuffer();
} // CloseConnection


tbool CXloader::Start(IFile* pfileForDownload /*= NULL*/)
{
	CAutoLock Lock(mMutex_Connection);

	if (IsFailed()) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		AppendError("Not initialized");
		return false;
	}
	
	if (IsTransfering()) {
		AppendError("Already transfering");
		return false;
	}

	CloseConnection();
	if (!AssembleParams()) return false;
	if (!OpenConnection()) {
		CloseConnection();
		return false;
	}
	mpfileForReply = pfileForDownload;
	SetIsTransfering();

	std::string sErr;
	if (!CXloader_MultiWrapper::Add(this, &sErr)) {
		AppendError(sErr.c_str());
		return false;
	}

	// Success
	return true;
} // Start


tbool CXloader::GetReplyPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize)
{
	*piPortionSize = 0;
	
	if (IsFailed()) {
		//AppendError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		AppendError("Not initialized");
		return false;
	}
	
	if ((!IsTransfering()) && (!IsDone())) {
		AppendError("Not transfering and not done");
		return false;
	}

	if (mpfileForReply != NULL) {
		AppendError("Download / reply goes directly to a file, so you can't use GetReplyPortion(..)");
		return false;
	}

	// Get data
	{
		// Make sure we're the only thread working on data
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


tbool CXloader::Abort()
{
	if (IsTransfering()) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		SetIsDone();
	}
	
	WipeParams();
	
	return true;
} // Abort


tbool CXloader::GetProgress(tint64* piUploadProgress, tint64* piUploadSize, tint64* piDownloadProgress, tint64* piDownloadSize)
{
	if ((!IsFailed()) && (!IsInitialized())) {
		AppendError("Not initialized");
		return false;
	}
	
	if (piUploadProgress) *piUploadProgress = muiUploadProgress;
	if (piUploadSize) *piUploadSize = max(muiUploadSize, 1);
	if (piDownloadProgress) *piDownloadProgress = muiReplyProgress;
	if (piDownloadSize) *piDownloadSize = max(muiReplySize, 1);

	return true;
} // GetProgress(for IUploader)


tbool CXloader::GetProgress(tint64* piDownloadProgress, tint64* piDownloadSize)
{
	return GetProgress(NULL, NULL, piDownloadProgress, piDownloadSize);
} // GetProgress(for IDownloader)


void CXloader::SetIsUninitialized()
{
	mbIsInitialized = mbIsTransfering = mbIsMultiDone = mbIsDone = mbIsFailed = false;
} // SetIsUninitialized


void CXloader::SetIsFullBlownURL(tbool bFullBlownURL)
{
	mbIsInit_FullBlownURL = bFullBlownURL;
} // SetIsFullBlownURL


void CXloader::SetIsInitialized()
{
	mbIsInitialized = true;
} // SetIsInitialized


void CXloader::SetIsTransfering()
{
	mbIsFailed = mbIsDone = false;
	mbIsTransfering = true;
} // SetIsTransfering


void CXloader::SetMultiSaysDone(CURLcode code)
{
	if (!IsFailed()) {
		// External transfer is done
		mbIsMultiDone = true;
		CURLcode rc;
		rc = curl_easy_getinfo(mpCURLEasyHandle, CURLINFO_RESPONSE_CODE, &miHttpStatus);
		if (miHttpStatus >= 400) {
			CAutoLock Lock(mMutex_ForErrors);
			tchar pszErr[128];
			sprintf(pszErr, "%s returned status %d", msHost.c_str(), miHttpStatus);
			msDelayedStatusError = pszErr;
		}
		else if (code != 0) {
			// There's an error
			tchar* p = "(unknown)";
			switch (code) {
				case CURLE_UNSUPPORTED_PROTOCOL: p = "CURLE_UNSUPPORTED_PROTOCOL"; break;
				case CURLE_FAILED_INIT: p = "CURLE_FAILED_INIT"; break;
				case CURLE_URL_MALFORMAT: p = "CURLE_URL_MALFORMAT"; break;
				case CURLE_COULDNT_RESOLVE_PROXY: p = "CURLE_COULDNT_RESOLVE_PROXY"; break;
				case CURLE_COULDNT_RESOLVE_HOST: p = "CURLE_COULDNT_RESOLVE_HOST"; break;
				case CURLE_COULDNT_CONNECT: p = "CURLE_COULDNT_CONNECT"; break;
				case CURLE_FTP_WEIRD_SERVER_REPLY: p = "CURLE_FTP_WEIRD_SERVER_REPLY"; break;
				case CURLE_REMOTE_ACCESS_DENIED: p = "CURLE_REMOTE_ACCESS_DENIED"; break;
				case CURLE_FTP_WEIRD_PASS_REPLY: p = "CURLE_FTP_WEIRD_PASS_REPLY"; break;
				case CURLE_FTP_WEIRD_PASV_REPLY: p = "CURLE_FTP_WEIRD_PASV_REPLY"; break;
				case CURLE_FTP_WEIRD_227_FORMAT: p = "CURLE_FTP_WEIRD_227_FORMAT"; break;
				case CURLE_FTP_CANT_GET_HOST: p = "CURLE_FTP_CANT_GET_HOST"; break;
				case CURLE_FTP_COULDNT_SET_TYPE: p = "CURLE_FTP_COULDNT_SET_TYPE"; break;
				case CURLE_PARTIAL_FILE: p = "CURLE_PARTIAL_FILE"; break;
				case CURLE_FTP_COULDNT_RETR_FILE: p = "CURLE_FTP_COULDNT_RETR_FILE"; break;
				case CURLE_QUOTE_ERROR: p = "CURLE_QUOTE_ERROR"; break;
				case CURLE_HTTP_RETURNED_ERROR: p = "CURLE_HTTP_RETURNED_ERROR"; break;
				case CURLE_WRITE_ERROR: p = "CURLE_WRITE_ERROR"; break;
				case CURLE_UPLOAD_FAILED: p = "CURLE_UPLOAD_FAILED"; break;
				case CURLE_READ_ERROR: p = "CURLE_READ_ERROR"; break;
				case CURLE_OUT_OF_MEMORY: p = "CURLE_OUT_OF_MEMORY"; break;
				case CURLE_OPERATION_TIMEDOUT: p = "CURLE_OPERATION_TIMEDOUT"; break;
				case CURLE_FTP_PORT_FAILED: p = "CURLE_FTP_PORT_FAILED"; break;
				case CURLE_FTP_COULDNT_USE_REST: p = "CURLE_FTP_COULDNT_USE_REST"; break;
				case CURLE_RANGE_ERROR: p = "CURLE_RANGE_ERROR"; break;
				case CURLE_HTTP_POST_ERROR: p = "CURLE_HTTP_POST_ERROR"; break;
				case CURLE_SSL_CONNECT_ERROR: p = "CURLE_SSL_CONNECT_ERROR"; break;
				case CURLE_BAD_DOWNLOAD_RESUME: p = "CURLE_BAD_DOWNLOAD_RESUME"; break;
				case CURLE_FILE_COULDNT_READ_FILE: p = "CURLE_FILE_COULDNT_READ_FILE"; break;
				case CURLE_LDAP_CANNOT_BIND: p = "CURLE_LDAP_CANNOT_BIND"; break;
				case CURLE_LDAP_SEARCH_FAILED: p = "CURLE_LDAP_SEARCH_FAILED"; break;
				case CURLE_FUNCTION_NOT_FOUND: p = "CURLE_FUNCTION_NOT_FOUND"; break;
				case CURLE_ABORTED_BY_CALLBACK: p = "CURLE_ABORTED_BY_CALLBACK"; break;
				case CURLE_BAD_FUNCTION_ARGUMENT: p = "CURLE_BAD_FUNCTION_ARGUMENT"; break;
				case CURLE_INTERFACE_FAILED: p = "CURLE_INTERFACE_FAILED"; break;
				case CURLE_TOO_MANY_REDIRECTS: p = "CURLE_TOO_MANY_REDIRECTS"; break;
				case CURLE_UNKNOWN_TELNET_OPTION: p = "CURLE_UNKNOWN_TELNET_OPTION"; break;
				case CURLE_TELNET_OPTION_SYNTAX: p = "CURLE_TELNET_OPTION_SYNTAX"; break;
				case CURLE_PEER_FAILED_VERIFICATION: p = "CURLE_PEER_FAILED_VERIFICATION"; break;
				case CURLE_GOT_NOTHING: p = "CURLE_GOT_NOTHING"; break;
				case CURLE_SSL_ENGINE_NOTFOUND: p = "CURLE_SSL_ENGINE_NOTFOUND"; break;
				case CURLE_SSL_ENGINE_SETFAILED: p = "CURLE_SSL_ENGINE_SETFAILED"; break;
				case CURLE_SEND_ERROR: p = "CURLE_SEND_ERROR"; break;
				case CURLE_RECV_ERROR: p = "CURLE_RECV_ERROR"; break;
				case CURLE_SSL_CERTPROBLEM: p = "CURLE_SSL_CERTPROBLEM"; break;
				case CURLE_SSL_CIPHER: p = "CURLE_SSL_CIPHER"; break;
				case CURLE_SSL_CACERT: p = "CURLE_SSL_CACERT"; break;
				case CURLE_BAD_CONTENT_ENCODING: p = "CURLE_BAD_CONTENT_ENCODING"; break;
				case CURLE_LDAP_INVALID_URL: p = "CURLE_LDAP_INVALID_URL"; break;
				case CURLE_FILESIZE_EXCEEDED: p = "CURLE_FILESIZE_EXCEEDED"; break;
				case CURLE_USE_SSL_FAILED: p = "CURLE_USE_SSL_FAILED"; break;
				case CURLE_SEND_FAIL_REWIND: p = "CURLE_SEND_FAIL_REWIND"; break;
				case CURLE_SSL_ENGINE_INITFAILED: p = "CURLE_SSL_ENGINE_INITFAILED"; break;
				case CURLE_LOGIN_DENIED: p = "CURLE_LOGIN_DENIED"; break;
				case CURLE_TFTP_NOTFOUND: p = "CURLE_TFTP_NOTFOUND"; break;
				case CURLE_TFTP_PERM: p = "CURLE_TFTP_PERM"; break;
				case CURLE_REMOTE_DISK_FULL: p = "CURLE_REMOTE_DISK_FULL"; break;
				case CURLE_TFTP_ILLEGAL: p = "CURLE_TFTP_ILLEGAL"; break;
				case CURLE_TFTP_UNKNOWNID: p = "CURLE_TFTP_UNKNOWNID"; break;
				case CURLE_REMOTE_FILE_EXISTS: p = "CURLE_REMOTE_FILE_EXISTS"; break;
				case CURLE_TFTP_NOSUCHUSER: p = "CURLE_TFTP_NOSUCHUSER"; break;
				case CURLE_CONV_FAILED: p = "CURLE_CONV_FAILED"; break;
				case CURLE_CONV_REQD: p = "CURLE_CONV_REQD"; break;
				case CURLE_SSL_CACERT_BADFILE: p = "CURLE_SSL_CACERT_BADFILE"; break;
				case CURLE_REMOTE_FILE_NOT_FOUND: p = "CURLE_REMOTE_FILE_NOT_FOUND"; break;
				case CURLE_SSH: p = "CURLE_SSH"; break;
				case CURLE_SSL_SHUTDOWN_FAILED: p = "CURLE_SSL_SHUTDOWN_FAILED"; break;
				case CURLE_AGAIN: p = "CURLE_AGAIN"; break;
				case CURLE_SSL_CRL_BADFILE: p = "CURLE_SSL_CRL_BADFILE"; break;
				case CURLE_SSL_ISSUER_ERROR: p = "CURLE_SSL_ISSUER_ERROR"; break;
 			}
			tchar pszErr[512];
			sprintf(pszErr, "curl_multi_info_read(..) gave status DONE but code %d %s", code, p);
			AppendError(pszErr);
		}

		if (mpfileForReply != NULL) {
			// No internal buffer - set all transfer done
			SetIsDone();
		}
	}
} // SetMultiSaysDone


void CXloader::SetIsDone()
{
	// First see if we have any delayed errors that we need to report now
	{
		CAutoLock Lock(mMutex_ForErrors);
		if (msDelayedStatusError.length() > 0) {
			AppendError(msDelayedStatusError.c_str());
			msDelayedStatusError = "";
			return;
		}
	}

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


tbool CXloader::IsFinished()
{
	return mbIsDone || mbIsFailed;
} // IsFinished


void CXloader::AppendError(const tchar* pszError)
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
} // AppendError


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
