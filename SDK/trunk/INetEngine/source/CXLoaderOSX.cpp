// CXloader OS X only code

#include "ineInternalOS.h"

#include "curl/curl.h"


void CXloader::Constructor_OSSpecific()
{
	//mURLRef = NULL;
	mMessageRef = NULL;
	mReadStreamRef = mReadStreamRef_FileToUpload = NULL;
	mURLRef_FileToUpload = NULL;
	mParametersDataRef = NULL;
} // Constructor_OSSpecific

void CXloader::Destructor_OSSpecific()
{
	// Nothing here
} // Destructor_OSSpecific


tbool CXloader::OpenConnection_OSSpecific()
{
	// Form action string (http verb)
	tbool bHasParameters = (miParamsAssembledLen > 0);
	EVerbType eVerbDefault;
	if (mbIsUploader) {
		eVerbDefault = VERB_PUT;
	}
	else {
		eVerbDefault = (bHasParameters) ? VERB_POST : VERB_GET;
	}
	EVerbType eVerbToUse = GetVerb(eVerbDefault);
	const tchar* pszActionStr = GetVerbString(eVerbDefault);
    CFStringRef vActionStr = CFStringCreateWithCStringNoCopy(NULL, pszActionStr, kCFStringEncodingMacRoman, NULL);
	if (vActionStr == NULL) {
		SetError("Unable to create http-verb string");
		return false;
	}
	
	// Combined URL string
	std::string sFormatedURL = std::string("http://") + msHost + msPage;
	if ((eVerbToUse == VERB_GET) && (bHasParameters)) {
		// Add parameters as part of URI (rather than as part of message body)
		sFormatedURL += mpszParamsAssembled;
	}
    CFStringRef vUrlStr = CFStringCreateWithCStringNoCopy(NULL, sFormatedURL.c_str(), kCFStringEncodingMacRoman, NULL);
	if (vUrlStr == NULL) {
		SetError("Unable to create URL string");
		return false;
	}

    CFURLRef vUrlRef = CFURLCreateWithString (kCFAllocatorDefault, vUrlStr, NULL);
   	if (vUrlRef == NULL) {
		SetError("Unable to create URL ref");
		return false;
	}

	mMessageRef = CFHTTPMessageCreateRequest(kCFAllocatorDefault,  vActionStr, vUrlRef, kCFHTTPVersion1_1);
	if (mMessageRef == NULL)
	{
		SetError("Unable to create message ref");
		return false;
	}
	
	// Set desired MIME type for download/reply
	if (meMIMEType != MIME_TYPE_NONE) {
		CFStringRef vMIME = CFStringCreateWithCStringNoCopy(NULL, GetMIMEString(), kCFStringEncodingMacRoman, NULL);
		CFHTTPMessageSetHeaderFieldValue(mMessageRef, CFSTR("Accept"), vMIME);
	}

	// Insert parameters into message body (POST + PUT verbs only)
	if (eVerbToUse != VERB_GET) {
		CFHTTPMessageSetHeaderFieldValue(mMessageRef, CFSTR("Content-Type"), CFSTR("application/x-www-form-urlencoded"));
		// Are there any parameters?
		if (bHasParameters) {
			mParametersDataRef = CFDataCreate(kCFAllocatorDefault, (unsigned char*)mpszParamsAssembled, miParamsAssembledLen);
			if (mParametersDataRef == NULL) {
				SetError("Unable to create parameters ref");
				return false;
			}
			CFHTTPMessageSetBody(mMessageRef, mParametersDataRef);
		}
	}

	// Prepare file for upload (IUploader only)
	if ((mbIsUploader) && (mpfileToUpload)) {
		tchar pszFilePath_InternalFormat[1024];
		*pszFilePath_InternalFormat = '\0';
		mpfileToUpload->GetPathName(pszFilePath_InternalFormat);
		tchar pszFilePath_OSFormat[1024];
		if (!IFile::PathToOS2(pszFilePath_InternalFormat, pszFilePath_OSFormat)) {
			tchar pszErr[2048];
			sprintf(pszErr, "Unable to convert file path \"%s\" to OS format", pszFilePath_InternalFormat);
			SetError(pszErr);
			return false;
		}
		CFStringRef cfstrFilePath = CFStringCreateWithCStringNoCopy(NULL, pszFilePath_OSFormat, kCFStringEncodingMacRoman, NULL);
		if (cfstrFilePath == NULL) {
			SetError("Unable to create CFStringRef from upload file path (out of memory?)");
			return false;
		}
		mURLRef_FileToUpload = CFURLCreateWithFileSystemPath(NULL, cfstrFilePath, kCFURLPOSIXPathStyle, false);
		if (mURLRef_FileToUpload == NULL) {
			tchar pszErr[2048];
			sprintf(pszErr, "Unable to create a file:// URL from upload file path \"%s\"", pszFilePath_OSFormat);
			SetError(pszErr);
			return false;
		}
		mReadStreamRef_FileToUpload = CFReadStreamCreateWithFile(NULL, mURLRef_FileToUpload);
		if (mReadStreamRef_FileToUpload == NULL) {
			tchar pszErr[2048];
			sprintf(pszErr, "Unable to create CFReadStreamRef from upload file path \"%s\"", pszFilePath_OSFormat);
			SetError(pszErr);
			return false;
		}
	}
	
	// Allocate stream ref
	// (Create for streamed http because large data can't be kept in memory)
	//mReadStreamRef = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, mMessageRef);
	mReadStreamRef = CFReadStreamCreateForStreamedHTTPRequest(kCFAllocatorDefault, mMessageRef, mReadStreamRef_FileToUpload);
	if (mReadStreamRef == NULL)
	{
		SetError("Unable to create read-stream ref");
		return false;
	}
	// (lasse) Can't set auto-redirect property for streamed http - won't work
	/*
	if (!CFReadStreamSetProperty (mReadStreamRef,
								  kCFStreamPropertyHTTPShouldAutoredirect, 
								  kCFBooleanTrue))
	{
		SetError("Unable to set auto-redirect property");
		return false;
	}
	*/
	// .. (lasse)
	
	// And action!
	return CFReadStreamOpen( mReadStreamRef );
} // OpenConnection_OSSpecific


void CXloader::CloseConnection_OSSpecific()
{
	if (mReadStreamRef) {
		CFReadStreamClose( mReadStreamRef );
		CFRelease( mReadStreamRef );
		mReadStreamRef = NULL;
	}
	
	if (mMessageRef) {
		CFRelease( mMessageRef );
		mMessageRef = NULL;
	}
	
	if (mParametersDataRef) {
		CFRelease( mParametersDataRef );
		mParametersDataRef = NULL;
	}
	
	if (mReadStreamRef_FileToUpload) {
		CFRelease( mReadStreamRef_FileToUpload );
		mReadStreamRef_FileToUpload = NULL;
	}
	
	if (mURLRef_FileToUpload) {
		CFRelease( mURLRef_FileToUpload );
		mURLRef_FileToUpload = NULL;
	}
} // CloseConnection_OSSpecific


tbool CXloader::DoPortion_OSSpecific(tuint64* puiUploadProgress, tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize)
{
	if (CFReadStreamHasBytesAvailable(mReadStreamRef)) 
	{
		CFIndex vBytesRead = CFReadStreamRead(mReadStreamRef, (unsigned char*)pszBuffer, iBufferSize);         
		
		if (vBytesRead < 0) 
		{ 
			CFStreamError vStreamError = CFReadStreamGetError(mReadStreamRef);             
			tchar pszError[128];
			sprintf(pszError, "CFReadStreamRead returned %d", vStreamError.error);
			SetError(pszError);
			return false;
		} 
		else if (vBytesRead > 0) 
		{
			// We're alive
			RefreshAlive();
			
			*piPortionSize = vBytesRead;	
			CFTypeRef vContentLength = CFReadStreamCopyProperty(mReadStreamRef, CFSTR("Content-Length"));
			tchar* pszContentLength = (tchar*)vContentLength;
			if (pszContentLength) {
				*puiTotalSize = atol(pszContentLength);
			}
		}
	} 

	CFStreamStatus status = CFReadStreamGetStatus(mReadStreamRef);
	if (status == kCFStreamStatusAtEnd)
	{                 
		SetIsDone();
	}
	else if ((status == kCFStreamStatusError) || (status == kCFStreamStatusClosed)) {
		SetIsFailed();
	}
	
	// Success
	return true;
} // DownloadPortion_OSSpecific



