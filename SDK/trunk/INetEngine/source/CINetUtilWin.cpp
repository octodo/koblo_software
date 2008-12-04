
#include "ineInternalOS.h"


void GetGetLastError(tchar pszMsg[256])
{
	DWORD dwLastErr = ::GetLastError();
	tint iLen = sprintf(pszMsg, "GetLastError() == %d (0x%08X) :  ", dwLastErr, dwLastErr);
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastErr, 0, pszMsg + iLen, 256 - iLen, NULL);
}


#define POST_GET(post, get) ((Parameters == NULL)?get:post)
#define POST_CONTENT_TYPE "Content-Type: application/x-www-form-urlencoded"
void CINetUtil::GetWebFile(tchar* Parameters, tchar* FileName, tint32* OutputLength, tchar** OutputBuffer)
{
	*OutputBuffer = NULL;
	*OutputLength = 0;
	HINTERNET Initialize = NULL;
	HINTERNET Connection = NULL;
    HINTERNET File = NULL;
	tchar* szFullFileName = NULL;
	try {
		//combine path and filename
		szFullFileName = new tchar[strlen(INTERFACE_PATH)+strlen((const char *) FileName)+1];
		sprintf((char *) szFullFileName,"%s%s",INTERFACE_PATH,FileName);
		/*initialize the wininet library*/
		// Lasse, modified 2007-09-10 - 1: correctly name client, 2: allow for proxy, 3: flag connection as asyncronous
		//if (NULL == (Initialize = InternetOpen("INE",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0)))
		if (NULL == (Initialize = InternetOpen("KeyToSound INet Engine 1.1", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)))//INTERNET_FLAG_ASYNC)))
		// .. Lasse
		{
			// Lasse, modified 2007-09-10
			//throw IException::Create(IException::TypeNetwork,
			//	IException::ReasonNetworkCannotOpen,
			//	EXCEPTION_INFO);
			tchar pszMsg[256];
			GetGetLastError(pszMsg);
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonNetworkCannotOpen,
				EXCEPTION_INFO,
				pszMsg);
			// .. Lasse
		}
		//Set timeout
		int timeout = CONNECT_TIMEOUT * 1000;
		// Lasse, modified 2007-09-10 - check for return value
		//InternetSetOption(Initialize, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, 4);
		if (!InternetSetOption(Initialize, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, 4)) {
			tchar pszMsg[256];
			GetGetLastError(pszMsg);
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonNetworkGeneric,
				EXCEPTION_INFO,
				pszMsg);
		}

		// Lasse, added 2007-09-10 - context identifier for application
		DWORD dwContext = 0;
		DWORD_PTR pdwContext = (DWORD_PTR)&dwContext;
		// .. Lasse

		/*connect to the server*/
		// Lasse, modified 2007-09-10 - avoid risky NULL pointer read; use context identifier correctly
		//if (NULL == (Connection = InternetConnect(Initialize,WEB_SERVER,INTERNET_DEFAULT_HTTP_PORT,
		//	NULL,NULL,INTERNET_SERVICE_HTTP,0,0)))
		if (NULL == (Connection = InternetConnect(Initialize, WEB_SERVER, INTERNET_DEFAULT_HTTP_PORT,
			NULL, NULL, INTERNET_SERVICE_HTTP, 0, pdwContext)))
		// .. Lasse
		{
			// Lasse, modified 2007-09-10
			//throw IException::Create(IException::TypeNetwork,
			//	IException::ReasonCouldntConnectToServer,
			//	EXCEPTION_INFO);
			tchar pszMsg[256];
			GetGetLastError(pszMsg);
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonCouldntConnectToServer,
				EXCEPTION_INFO,
				pszMsg);
			// .. Lasse
		}
		/*open up an HTTP request*/
		// Lasse, modified 2007-09-10 - 1: avoid risky NULL pointer read; use context identifier correctly, 2: avoid using cache
		//if (NULL == (File = HttpOpenRequest(Connection,POST_GET("POST","GET"),(const char*) szFullFileName,NULL,NULL,NULL,0,0)))
		if (NULL == (File = HttpOpenRequest(Connection, POST_GET("POST","GET"), (const char*)szFullFileName, NULL, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD, pdwContext)))
		// .. Lasse
		{
			// Lasse, modified 2007-09-10
			//throw IException::Create(IException::TypeNetwork,
			//	IException::ReasonCouldntConnectToAppl,
			//	EXCEPTION_INFO);
			tchar pszMsg[256];
			GetGetLastError(pszMsg);
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonCouldntConnectToAppl,
				EXCEPTION_INFO,
				pszMsg);
			// .. Lasse
		}
		/*Read the file*/
		if(HttpSendRequest(File,POST_GET(POST_CONTENT_TYPE,NULL),POST_GET(strlen(POST_CONTENT_TYPE),0),POST_GET(Parameters,NULL),POST_GET(strlen((const char *) Parameters),0)))
		{
			*OutputBuffer = new tchar[MAX_PAGE_SIZE];
			// Lasse, changed 2007-09-11 - fix for prematurely return before all data have been received
			//InternetReadFile(File,*OutputBuffer,MAX_PAGE_SIZE, (unsigned long *) OutputLength);
			DWORD dwLen = 0;
			*OutputLength = 0;
			tuint32 uiRemainingBuffer = MAX_PAGE_SIZE;
			tchar* pszBuff = *OutputBuffer;
			// This is the correct approach: Read until 0 bytes are received
			do {
				if (!InternetReadFile(File, pszBuff, uiRemainingBuffer, &dwLen)) {
					// Time-out reading page
					tchar pszMsg[256];
					GetGetLastError(pszMsg);
					throw IException::Create(IException::TypeNetwork,
						IException::ReasonNetworkTimeOut,
						EXCEPTION_INFO,
						pszMsg);
				}
				// Prepare for more data - advance buffer pointer etc.
				pszBuff += dwLen;
				*OutputLength += dwLen;
				uiRemainingBuffer -= dwLen;
			} while (dwLen > 0);
			// .. Lasse
			if (*OutputLength == MAX_PAGE_SIZE) {
				throw IException::Create(IException::TypeNetwork, IException::ReasonPageToLarge, EXCEPTION_INFO);
			}
            (*OutputBuffer)[*OutputLength] = NULL; //zero termination
		} else {
			// Lasse, modified 2007-09-10
			//throw IException::Create(IException::TypeNetwork,
			//	IException::ReasonErrorReadingFromAppl,
			//	EXCEPTION_INFO);
			tchar pszMsg[256];
			GetGetLastError(pszMsg);
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonErrorReadingFromAppl,
				EXCEPTION_INFO,
				pszMsg);
			// .. Lasse
		}
	}
	catch (...) {
	   /*close file , terminate server connection and deinitialize the wininet library*/
		if (File != NULL) InternetCloseHandle(File);
		if (Connection != NULL) InternetCloseHandle(Connection);
		if (Initialize != NULL) InternetCloseHandle(Initialize);
		throw;
	}
	if (File != NULL) InternetCloseHandle(File);
	if (Connection != NULL) InternetCloseHandle(Connection);
	if (Initialize != NULL) InternetCloseHandle(Initialize);
}
