// CUploader OS X only code

#include "ineInternalOS.h"


void CUploader::Constructor_OSSpecific()
{
	//mURLRef = NULL;
	mMessageRef = NULL;
	mReadStreamRef = NULL;
	mParametersDataRef = NULL;
} // Constructor_OSSpecific

void CUploader::Destructor_OSSpecific()
{
	// Nothing here
} // Destructor_OSSpecific


tbool CUploader::OpenConnection_OSSpecific()
{
	// Form action string (http verb)
	tbool bHasParameters = (miParamsAssembledLen > 0);
	EVerbType eVerbDefault = VERB_PUT;
	EVerbType eVerbToUse = GetVerb(eVerbDefault);
	const tchar* pszActionStr = GetVerbString(eVerbDefault);
    CFStringRef vActionStr = CFStringCreateWithCStringNoCopy(NULL, pszActionStr, kCFStringEncodingMacRoman, NULL);
	if (vActionStr == NULL) {
		SetError("Unable to create http-verb string");
		return false;
	}
	
	// Combined URL string
	std::string sFormatedURL = std::string("http://") + msHost + msPage;
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

	// Set desired MIME type for download
	if (meMIMEType != MIME_TYPE_NONE) {
		CFStringRef cfsMIME = CFStringCreateWithCStringNoCopy(NULL, GetMIMEString(), kCFStringEncodingMacRoman, NULL);
		CFHTTPMessageSetHeaderFieldValue(mMessageRef, CFSTR("Accept"), cfsMIME);
	}

	// Insert parameters into message body
	if (bHasParameters) {
		CFHTTPMessageSetHeaderFieldValue(mMessageRef, CFSTR("Content-Type"), CFSTR("application/x-www-form-urlencoded"));
		mParametersDataRef = CFDataCreate(kCFAllocatorDefault, (unsigned char*)mpszParamsAssembled, miParamsAssembledLen);
		if (mParametersDataRef == NULL) {
			SetError("Unable to create parameters ref");
			return false;
		}
		CFHTTPMessageSetBody(mMessageRef, mParametersDataRef);
	}

	// Allocate stream ref
	mReadStreamRef = CFReadStreamCreateForHTTPRequest(kCFAllocatorDefault, mMessageRef);
	if (mReadStreamRef == NULL)
	{
		SetError("Unable to create read-stream ref");
		return false;
	}
	if (!CFReadStreamSetProperty (mReadStreamRef,
								  kCFStreamPropertyHTTPShouldAutoredirect, 
								  kCFBooleanTrue))
	{
		SetError("Unable to set auto-redirect property");
		return false;
	}
	
	// And action!
	return CFReadStreamOpen( mReadStreamRef );
} // OpenConnection_OSSpecific


void CUploader::CloseConnection_OSSpecific()
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
} // CloseConnection_OSSpecific


tbool CUploader::UploadPortion_OSSpecific(const tchar* pszData, tint32 iDataLen, tint32* piActuallySent)
{
	SetError("Not implemented yet!");
	return false;
	/*
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
	*/
} // UploadPortion_OSSpecific



