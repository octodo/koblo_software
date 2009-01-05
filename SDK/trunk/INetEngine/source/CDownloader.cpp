
#include "ineInternalOS.h"

CDownloader::CDownloader()
{
	mpszParamsAssembled = NULL;
	mbIsDownloading = false;
	mbIsDone = false;
} // constructor


CDownloader::~CDownloader()
{
	WipeParams();
} // destructor


IDownloader* IDownloader::Create()
{
	return dynamic_cast<IDownloader*>(new CDownloader());
} // Create


void CDownloader::Destroy()
{
	delete dynamic_cast<CDownloader*>(this);
} // Destroy


tbool CDownloader::Init(tchar* pszHost, tchar* pszPage, tint32 iPort /*= 80*/)
{
	msHost = pszHost;
	msPage = pszPage;
	miPort = iPort;

	WipeParams();

	return true;
} // Init


tbool CDownloader::AddParam(tchar* pszParamName, tchar* pcParamData, tint32 iParamDataLen)
{
	if (mbIsDownloading) {
		// Error: We can't add more parameters as we've already started downloading
		return false;
	}

	// Verify sanity of param name
	{
		if ((pszParamName == NULL) || (*pszParamName == '\0')) {
			// Error: No param name
			return false;
		}
		for (tchar* pc = pszParamName; *pc; pc++) {
			tchar c = *pc;
			if (
				((c < 'A') || (c > 'Z'))
				&&
				((c < 'a') || (c > 'z'))
				&&
				((c < '0') || (c > '9'))
			) {
				// Error: Not US-ASCII character
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
				// Error: Out of memory
				return false;
			}
			tint32 iActuallyEncoded = IINetUtil::URLEncode(pcParamData, iParamDataLen, pszUrlEncoded);
			if (iActuallyEncoded != iUrlEncodedLen) {
				// Error: Insane output length
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
	if (mpszParamsAssembled) {
		// Error: Already assembled
		return false;
	}

	// Verify sanity of lists
	tint32 iNames = mlist_sParamNames.size();
	tint32 iLens = mlist_iParamDataLen.size();
	tint32 iDatas = mlist_pszParamDataUrlEncoded.size();
	if ((iNames != iLens) || (iLens != iDatas)) {
		// Error: List lengths aren't all the same
		return false;
	}

	// Calculate space needed
	{
		iParamsAssembledLen = 0;

		// First accumulate lengths of parameter names
		std::list<std::string>::iterator itName = mlist_sParamNames.begin();
		for ( ; itName != mlist_sParamNames.end(); itName++) {
			std::string& rsName = *itName;
			iParamsAssembledLen += 1 + rsName.length();
		}

		// Then accumulate length of non-empty paramater data
		std::list<tint32>::iterator itDataLen = mlist_iParamDataLen.begin();
		for ( ; itDataLen != mlist_iParamDataLen.end(); itDataLen++) {
			tint32 iLen = *itDataLen;
			if (iLen > 0) {
				iParamsAssembledLen += 1 + iLen;
			}
		}
	}

	// Attempt to allocate space for params + traling zero
	mpszParamsAssembled = new tchar[iParamsAssembledLen + 1];
	if (mpszParamsAssembled == NULL) {
		// Error: Out of memory
		return false;
	}

	// And action! Assemble the parameter string
	{
		tchar* pszDst = mpszParamsAssembled;
		tchar cNameDelimiter = '?';
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
			*pszDst++ = cNameDelimiter;
			memcpy(pszDst, rsName.c_str(), iNameLen);
			pszDst += iNameLen;
			// Maybe copy param data
			if (iDataLen > 0) {
				*pszDst++ = '=';
				memcpy(pszDst, pcData, iDataLen);
				pszDst += iDataLen;
			}
			// Advance to next
			itName++;
			itDataLen++;
			itParamData++;
			cNameDelimiter = '&';
		}
		// Append zero-termination
		*pszDst = '\0';
	}
	
	return true;
} // AssembleParams


void CDownloader::WipeParams()
{
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


tbool CDownloader::IsDone()
{
	return mbIsDone;
} // IsDone
