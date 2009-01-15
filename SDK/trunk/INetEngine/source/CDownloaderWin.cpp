// CDownloader MS-Windows only code

#include "ineInternalOS.h"


static DWORD dwContextID_Seed = 0;



void CDownloader::Constructor_OSSpecific()
{
	// Make sure we only close what's been opened
	mhInitialize = mhConnection = mhFile = NULL;
} // Constructor_OSSpecific

void CDownloader::Destructor_OSSpecific()
{
	CloseConnection();
} // Destructor_OSSpecific


tbool CDownloader::OpenConnection_OSSpecific()
{
	// First verify that computer is connected to the Internet
	DWORD dwConnectedState = 0;
	if (
		(!::InternetGetConnectedState(&dwConnectedState, 0))
		||
		(dwConnectedState & INTERNET_CONNECTION_OFFLINE)
	) {
		SetError("No Internet connection");
		return false;
	}

	mhInitialize = ::InternetOpen("Koblo INet Engine 1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (mhInitialize == NULL) {
		SetError("InternetOpen(..) failed");
		return false;
	}
	// 3 seconds of *connecting* should be quite enough - even for very slow analogue modems...
	tint32 iConnectTimeOutMs = 3000; 
	if (!InternetSetOption(mhInitialize, INTERNET_OPTION_CONNECT_TIMEOUT, &iConnectTimeOutMs, sizeof(iConnectTimeOutMs))) {
		SetError("Unable to set connection time-out");
		return false;
	}

	// Create pseudo-unique context identifier for the connection
	mdwContextID = ++dwContextID_Seed;
	DWORD_PTR pdwContextID = (DWORD_PTR)&mdwContextID;
	// Connect to server
	mhConnection = InternetConnect(
		mhInitialize,
		msHost.c_str(), miPort,	msUser.c_str(), msPassword.c_str(),
		INTERNET_SERVICE_HTTP, 0, pdwContextID);
	if (mhConnection == NULL) {
		SetError("InternetConnect(..) failed");
		return false;
	}

	// Prepere media-type
	tchar* apszTypes[2];
	apszTypes[0] = apszTypes[1] = NULL;
	switch (meDesiredMIMEType) {
		case DESIRED_TYPE_HTML:		apszTypes[0] = "text/html";		break;
		case DESIRED_TYPE_XML:		apszTypes[0] = "text/xml";		break;
		case DESIRED_TYPE_BINARY:	apszTypes[0] = "application/octet-stream"; break;
		case DESIRED_TYPE_OGG:		apszTypes[0] = "audio/ogg";		break;
		case DESIRED_TYPE_MP3:		apszTypes[0] = "audio/mpeg";	break;
		
		default:
		case DESIRED_TYPE_TEXT:		apszTypes[0] = NULL;		break;
	}
	// Open document
	mhFile = ::HttpOpenRequest(
		mhConnection,
		(miParamsAssembledLen > 0) ? "POST" : "GET",
		msPage.c_str(),
		NULL, NULL, (LPCTSTR*)apszTypes,
		INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD,
		pdwContextID);

	if (mhFile == NULL) {
		SetError("HttpOpenRequest(..) failed");
		return false;
	}

	tchar* pszAdditionalHeaders = NULL;
	tint32 iAdditionHeadersLen = 0;
	if (miParamsAssembledLen > 0) {
		pszAdditionalHeaders = "Content-Type: application/x-www-form-urlencoded";
		iAdditionHeadersLen = strlen(pszAdditionalHeaders);
	}
	if (!::HttpSendRequest(mhFile,
		pszAdditionalHeaders, iAdditionHeadersLen,
		mpszParamsAssembled, miParamsAssembledLen)
	){
		SetError("HttpSendRequest(..) failed");
		return false;
	}

	muiTotalSize = 0;

	return true;
} // OpenConnection_OSSpecific


void CDownloader::CloseConnection_OSSpecific()
{
	if (mhFile) {
		::InternetCloseHandle(mhFile);
		mhFile = NULL;
	}
	if (mhConnection) {
		::InternetCloseHandle(mhConnection);
		mhConnection = NULL;
	}
	if (mhInitialize) {
		::InternetCloseHandle(mhInitialize);
		mhInitialize = NULL;
	}
} // CloseConnection_OSSpecific


tbool CDownloader::DownloadPortion_OSSpecific(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize)
{
	/* hm... ::InternetReadFileEx(..) is too complicated - will attempt InternetReadFile(..)
	INTERNET_BUFFERS buffers;
	// Initialize empty so we don't get confused
	buffers.dwStructSize = sizeof(buffers);
	buffers.Next = NULL;
	buffers.lpcszHeader = NULL;
	buffers.dwHeadersLength = 0;
	buffers.dwHeadersTotal = 0;
	buffers.lpvBuffer = NULL;
	buffers.dwBufferLength = 0;
	buffers.dwBufferTotal = 0;
	buffers.dwOffsetLow = buffers.dwOffsetHigh = 0;
	::InternetReadFileEx(mhFile
	*/
	DWORD dwReturned = 0;
	if (::InternetReadFile(mhFile, pszBuffer, (tuint32)iBufferSize, &dwReturned)) {
		// Read portion success

		// We're alive
		RefreshAlive();

		// Tell how much we got in this portion
		*piPortionSize = (tint32)dwReturned;
		if (dwReturned == 0) {
			// No more data
			mbIsDone = true;
			mbIsDownloading = false;
		}
		else {
			// Check status code
			{
				tchar pszBuff[128];
				DWORD dwBuffSize = 128;
				DWORD dwHeaderIndex_Dummy = 0;
				BOOL bQueryOK = ::HttpQueryInfo(
					mhFile,
					HTTP_QUERY_STATUS_CODE, pszBuff, &dwBuffSize,
					&dwHeaderIndex_Dummy);
				if (bQueryOK) {
					tint32 iStatus = atol(pszBuff);
					tbool bStatusOK = (iStatus == 200);
					if (!bStatusOK) {
						SetError(pszBuff);
						return false;
					}
				}
				else {
					SetError("Can't get http status");
					return false;
				}
			}

			// Get total data size
			tchar pszBuff[32];
			DWORD dwBuffSize = 32;
			muiTotalSize = 0;
			DWORD dwHeaderIndex_Dummy = 0;
			BOOL bQueryOK = ::HttpQueryInfo(
				mhFile,
				HTTP_QUERY_CONTENT_LENGTH, pszBuff, &dwBuffSize,
				&dwHeaderIndex_Dummy);
			if (bQueryOK) {
				muiTotalSize = atol(pszBuff);
			}
			else {
				// Fall back to largest possible value
				muiTotalSize = !((tuint64)0);
			}
		}
	}
	else {
		// Read portion failed!
		tchar pszErr[1024];
		pszErr[0] = '\0';
		DWORD dwErrBuffSize = 1024;
		DWORD dwErr_Dummy = 0;
		if (::InternetGetLastResponseInfo(&dwErr_Dummy, pszErr, &dwErrBuffSize)) {
			std::string sErr = "InternetReadFile(..) failed:\n\n";
			sErr += pszErr;
			SetError(sErr.c_str());
		}
		else {
			// Message too long for our buffer
			SetError("InternetReadFile(..) failed");
		}

		return false;
	}

	// Success!
#ifdef _DEBUG
	tint64 iTotalSize_Debug = muiTotalSize;
#endif // _DEBUG
	return true;
} // DownloadPortion_OSSpecific


