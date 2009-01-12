
#include "ineInternalOS.h"

CDownloader::CDownloader()
{
	mpszParamsAssembled = NULL;
	mbIsInitialized = mbIsDownloading = mbIsFailed = mbIsDone = false;
	//
	Constructor_OSSpecific();
} // constructor


CDownloader::~CDownloader()
{
	Abort();
	WipeParams();
	CloseConnection();
	//
	Destructor_OSSpecific();
} // destructor


IDownloader* IDownloader::Create()
{
	return dynamic_cast<IDownloader*>(new CDownloader());
} // Create


void CDownloader::Destroy()
{
	delete dynamic_cast<CDownloader*>(this);
} // Destroy


tbool CDownloader::Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort /*= 80*/, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	Abort();
	WipeParams();
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
	miPort = iPort;
	msUser = (pszUser == NULL) ? "" : pszUser;
	msPassword = (pszPassword == NULL) ? "" : pszPassword;
	muiTimeOutSecs = (tuint32)iTimeOutSecs;

	mbIsInitialized = true;
	meDesiredMIMEType = DESIRED_TYPE_TEXT;

	return true;
} // Init


tbool CDownloader::SetDesiredMIMEType(EDesiredMIMEType eType)
{
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}

	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}

	if (mbIsDownloading) {
		SetError("We can't set media type as we've already started downloading");
		return false;
	}

	// Success
	return true;
} // SetDesiredMIMEType


tbool CDownloader::AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen)
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

	if (mbIsDownloading) {
		SetError("We can't add more parameters as we've already started downloading");
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


tbool CDownloader::AssembleParams()
{
	CAutoLock Lock(mMutex_ForParams);

	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}

	if (mpszParamsAssembled) {
		SetError("Already assembled");
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

	// Calculate space needed
	{
		miParamsAssembledLen = 0;

		// First accumulate lengths of parameter names
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		for ( ; itName != mlist_sParamNames.end(); itName++) {
			std::string& rsName = *itName;
			miParamsAssembledLen += rsName.length();
		}

		// Then accumulate length of non-empty paramater data
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		for ( ; itDataLen != mlist_iParamDataLen.end(); itDataLen++) {
			tint32 iLen = *itDataLen;
			if (iLen > 0) {
				// An equation-sign and then data
				miParamsAssembledLen += 1 + iLen;
			}
		}

		// Lastly a new-line sequence (always DOS style due to w3c spec)
		miParamsAssembledLen += 2;
	}

	// Attempt to allocate space for params + traling zero
	mpszParamsAssembled = new tchar[miParamsAssembledLen + 1];
	if (mpszParamsAssembled == NULL) {
		SetError("Out of memory");
		return false;
	}

	// And action! Assemble the parameter string
	{
		tchar* pszDst = mpszParamsAssembled;
		//tchar cNameDelimiter = '?';
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
			//*pszDst++ = cNameDelimiter;
			memcpy(pszDst, rsName.c_str(), iNameLen);
			pszDst += iNameLen;
			// Maybe copy param data
			if (iDataLen > 0) {
				*pszDst++ = '=';
				memcpy(pszDst, pcData, iDataLen);
				pszDst += iDataLen;
			}
			// Newline sequence
			memcpy(pszDst, "\r\n", 2);
			pszDst += 2;
			// Advance to next
			itName++;
			itDataLen++;
			itParamData++;
			//cNameDelimiter = '&';
		}
		// Append zero-termination
		*pszDst = '\0';
	}
	
	return true;
} // AssembleParams


void CDownloader::WipeParams()
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
} // WipeParams


tbool CDownloader::OpenConnection()
{
	return OpenConnection_OSSpecific();
} // OpenConenction


void CDownloader::CloseConnection()
{
	CloseConnection_OSSpecific();
} // CloseConenction


tbool CDownloader::DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize)
{
	*piPortionSize = 0;
	
	if (mbIsFailed) {
		//SetError("Previous error");
		return false;
	}
	
	if (!mbIsInitialized) {
		SetError("Not initialized");
		return false;
	}
	
	tbool bFirstTime = (!mbIsDownloading);
	mbIsDownloading = true;
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
	
	return DownloadPortion_OSSpecific(pszBuffer, iBufferSize, piPortionSize, puiTotalSize);
} // DownloadPortion


tbool CDownloader::Abort()
{
	if (mbIsDownloading) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		mbIsDownloading = false;
	}
	
	WipeParams();
	
	return true;
} // Abort


tbool CDownloader::IsDone()
{
	return mbIsDone;
} // IsDone


tbool CDownloader::IsFailed()
{
	return mbIsFailed;
} // IsFailed


void CDownloader::RefreshAlive()
{
	muiAliveMs = ITime::GetTimeMS();
} // RefreshAlive


tbool CDownloader::IsAlive()
{
	// (lasse) Are we really needing this? Anyway not used now

	if ((!mbIsInitialized) || (!mbIsDownloading)) {
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


void CDownloader::SetError(const tchar* pszError)
{
	CAutoLock Lock(mMutex_ForErrors);

	msLastError = pszError;
	mbIsFailed = true;
}

tbool CDownloader::GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize)
{
	CAutoLock Lock(mMutex_ForErrors);

	if ((pszErrBuff == NULL) || (iErrBuffSize <= 0)) return false;

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
}
