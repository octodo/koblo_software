// CXloader OS X only code
// Is included into CXloader.h

protected:

//CFURLRef                mURLRef;
CFHTTPMessageRef        mMessageRef;
CFReadStreamRef         mReadStreamRef;
CFReadStreamRef         mReadStreamRef_FileToUpload;
CFURLRef				mURLRef_FileToUpload;
CFDataRef mParametersDataRef;