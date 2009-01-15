
#include "ineInternalOS.h"

#define POST_GET(post, get) ((Parameters == NULL)?get:post)

#define POST_CONTENT_TYPE_TAG "Content-Type"
#define POST_CONTENT_TYPE_DESCRIPTION "application/x-www-form-urlencoded"
#define POST_CONTENT_TYPE POST_CONTENT_TYPE_TAG ": " POST_CONTENT_TYPE_DESCRIPTION

// ---------------------------------------------------------------------------------------------------------------

#if 1

void IINetUtil::GetWebFile(const tchar* Parameters, const tchar* pszServer, const tchar* pszFileName, tint32* OutputLength, tchar** OutputBuffer)
{
    char                    vFormattedUrl[2000];
    bool                    vSuccess;
    CFURLRef                vUrlRef;
	CFHTTPMessageRef        vMessageRef;
	CFReadStreamRef         vReadStreamRef;
	CFStringRef             vUrlStr;
	CFStringRef             vActionStr;
	CFDataRef               vParametersData;
	bool                    vDone;
	CFStreamError           vStreamError;
    CFIndex                 vBytesRead;
    unsigned char*          vpDest;
    long                    vMaxDataLen;
    
    //printf("Parameters: %s\n",Parameters);
    //printf("FileName: %s\n",FileName);
    //printf("WEB_SERVER: %s\n",WEB_SERVER);
    //printf("POST_GET(...): %s\n",POST_GET("POST","GET"));
    
    *OutputLength = 0;
	*OutputBuffer = new tchar[MAX_PAGE_SIZE+1];
	
	vSuccess = false;
	
	//snprintf((char*)vFormattedUrl,2000,"http://%s%s%s",WEB_SERVER,INTERFACE_PATH,(char*)FileName);
	snprintf((char*)vFormattedUrl, 2000, "http://%s%s", pszServer, pszFileName);
	
	//
	// format CF url and prepare stream connection
	//
	
    vUrlStr = CFStringCreateWithCStringNoCopy(NULL,vFormattedUrl,kCFStringEncodingMacRoman,NULL);

    vActionStr = CFStringCreateWithCStringNoCopy(NULL,(char*)POST_GET("POST","GET"),kCFStringEncodingMacRoman,NULL);
	
//	vParametersData = CFDataCreate(kCFAllocatorDefault, (unsigned char*) Parameters, strlen((char*)Parameters));
	vParametersData = NULL;
	if (Parameters) {
		vParametersData = CFDataCreate(kCFAllocatorDefault, (unsigned char*) Parameters, strlen((char*)Parameters));
	}
	
//	if ((vUrlStr != NULL) && (vActionStr != NULL) && (vParametersData != NULL))
	if ((vUrlStr != NULL) && (vActionStr != NULL))
	{
    	vUrlRef = CFURLCreateWithString (
    	            kCFAllocatorDefault, 
    	            vUrlStr, 
    	            NULL );
    	
    	if (vUrlRef != NULL)
    	{
        	vMessageRef = CFHTTPMessageCreateRequest(
                    	    kCFAllocatorDefault,  
                    	    vActionStr, 
                    	    vUrlRef, 
                    	    kCFHTTPVersion1_1);
	        
	        if (vMessageRef != NULL)
	        {
	            CFHTTPMessageSetHeaderFieldValue(
	                vMessageRef, 
	                CFSTR(POST_CONTENT_TYPE_TAG),
				    CFSTR(POST_CONTENT_TYPE_DESCRIPTION));
				
	            if (vParametersData != NULL)
    	        {
    	            
    	            CFHTTPMessageSetBody(
    	                vMessageRef, 
    	                vParametersData);
    	        }
	        
	            vReadStreamRef = CFReadStreamCreateForHTTPRequest( 
            	                    kCFAllocatorDefault, 
            	                    vMessageRef );
	        }
	    }
    }
    
    //
    // stream is ready, connect and wait for data
    //
    
    if (vReadStreamRef != NULL)
    {	
	    CFReadStreamSetProperty ( 
	        vReadStreamRef, 
	        kCFStreamPropertyHTTPShouldAutoredirect, 
	        kCFBooleanTrue);
	        
	    vSuccess = CFReadStreamOpen( vReadStreamRef );
	
	    if (vSuccess == true)
	    {
	        //printf("connected...\n");
	        
    	    vDone = false;
    	    vMaxDataLen = MAX_PAGE_SIZE;
    	    vpDest = (unsigned char*) (*OutputBuffer);
    	    
    	    while (!vDone) 
    	    {     
    	        if (CFReadStreamHasBytesAvailable(vReadStreamRef)) 
    	        {   
    	            if (vMaxDataLen <= 0)
    	            {
    	                vDone = true;    
                        vSuccess = false;  
    	            }
    	            else
    	            {
        	            //printf("aMaxDataLen: %d\n",aMaxDataLen);
        	            
        	            vBytesRead = CFReadStreamRead(vReadStreamRef, vpDest, vMaxDataLen);         
        	            
        	            //printf("vBytesRead: %d\n",vBytesRead);
        	            
        	            if (vBytesRead < 0) 
        	            { 
                            vStreamError = CFReadStreamGetError(vReadStreamRef);             
                            
                            vDone = true;    
                            vSuccess = false;  
                        } 
                        else if (vBytesRead == 0) 
                        {             
                            vDone = true;     
                        } 
                        else 
                        {             
            	            vpDest += vBytesRead;
            	            vMaxDataLen -= vBytesRead;
            	            
            	            *OutputLength += vBytesRead;
                        }     
                    }
                } 
				//!!! max
				// Prevent hanging if no Internet avaliable 
			//	else vDone = true;
                
                CFStreamStatus status = CFReadStreamGetStatus(vReadStreamRef);
				if (status == kCFStreamStatusAtEnd)
                {                 
                    vDone = true;             
                }
				else if ((status == kCFStreamStatusError) || (status == kCFStreamStatusClosed)) {
					vDone = true;
					vSuccess = false;
				}
            } 
    	
    	    CFReadStreamClose( vReadStreamRef );
    	}

        CFRelease( vReadStreamRef );
    }
    
    if (vMessageRef)
        CFRelease( vMessageRef );
    
    if (vParametersData)
        CFRelease( vParametersData );
    
	
	if (vSuccess)
	{
	    (*OutputBuffer)[*OutputLength] = 0;
	    
	    //printf("ok: downloaded data for %d bytes\n",*OutputLength);
	    //printf("content follows\n");
	    //printf("%s\n",*OutputBuffer);
	    //printf("content end\n");
	    
	}
	else
	{
	    //printf("failed: error %d\n",vErr);
	    
	    delete[] *OutputBuffer;
	    
	    *OutputBuffer = NULL;
	    *OutputLength = 0;
	}
       
}

#endif

// ---------------------------------------------------------------------------------------------------------------

#if 0

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
		if (NULL == (Initialize = InternetOpen("INE",INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0)))
		{
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonNetworkCannotOpen);
		}
		//Set timeout
		int timeout = CONNECT_TIMEOUT*1000;
		InternetSetOption(Initialize, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, 4);
		/*connect to the server*/
		if (NULL == (Connection = InternetConnect(Initialize,WEB_SERVER,INTERNET_DEFAULT_HTTP_PORT,
			NULL,NULL,INTERNET_SERVICE_HTTP,0,0)))
		{
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonCouldntConnectToServer);
		}
		/*open up an HTTP request*/
		if (NULL == (File = HttpOpenRequest(Connection,POST_GET("POST","GET"),(const char*) szFullFileName,NULL,NULL,NULL,0,0)))
		{
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonCouldntConnectToAppl);
		}
		/*Read the file*/
		if(HttpSendRequest(File,
                POST_GET(POST_CONTENT_TYPE,NULL),
                POST_GET(strlen(POST_CONTENT_TYPE),0),
                POST_GET(Parameters,NULL),
                POST_GET(strlen((const char *) Parameters),0)))
		{
			*OutputBuffer = new tchar[MAX_PAGE_SIZE];
			InternetReadFile(File,*OutputBuffer,MAX_PAGE_SIZE, (unsigned long *) OutputLength);
			if (*OutputLength == MAX_PAGE_SIZE) {
				throw IException::Create(IException::TypeNetwork, IException::ReasonPageToLarge);
			}
            (*OutputBuffer)[*OutputLength] = NULL; //zero termination
		}else {
			throw IException::Create(IException::TypeNetwork,
				IException::ReasonErrorReadingFromAppl);
		}
	}

	catch (exception e) {
	   /*close file , terminate server connection and deinitialize the wininet library*/
		if (File != NULL) InternetCloseHandle(File);
		if (Connection != NULL) InternetCloseHandle(Connection);
		if (Initialize != NULL) InternetCloseHandle(Initialize);
		throw(e);
	}
	if (File != NULL) InternetCloseHandle(File);
	if (Connection != NULL) InternetCloseHandle(Connection);
	if (Initialize != NULL) InternetCloseHandle(Initialize);
}

#endif