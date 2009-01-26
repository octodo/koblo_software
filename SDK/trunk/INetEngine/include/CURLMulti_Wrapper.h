
#ifndef _curl_multi_wrapper_h
#define _curl_multi_wrapper_h

// Forward definition
class CXloader;

	
/*! \class CURLMulti_Wrapper
 * \brief Handles timed events for all allocated IUploader and IDownloader instances
 *
 * Wraps CURLMulti (a.k.a. "multi" handle), which holds several CURL class instances (a.k.a. "easy" handles).
 * This is done so we can run web-access in the background and don't need to wait for every "easy" handle to complete its entire upload or download operation before returning control to application.
 * (lasse)
*/
class CURLMulti_Wrapper : public virtual ITimerCallback
{
protected:
	//! Constructor
	CURLMulti_Wrapper();
	//! Destructor
	virtual ~CURLMulti_Wrapper();
public:
	//! Add a new IUploader or IDownloader to handler. If a handler doesn't exist it will be created
	static tbool Add(CXloader* pXloader, std::string* psError);
	//! Remove a IUploader or IDownloader from handler. When the last Xloader has been removed the handler is auto-destroyed
	static tbool Remove(CXloader* pXloader, std::string* psError);

	//! ITimerCallback implementation - timer handles data i/o for all active IDownloader and IUploader
	virtual void OnTimer(tint32 iID);

protected:
	//! Pointer to CURL multi instance. The instance holds a number of active download/upload threads (every IUploader and IDownloader has 0 or 1 active thread)
	CURLM* mpCURLMulti;
	//! Timer ID
	tuint32 miTimerID;
	//! Timer
	ITimer* mpTimer;

	static void GetLockForMultiInstance();
	static void ReleaseLockForMultiInstance();

	void SetAllError(const tchar* pszErr);

	std::list<CXloader*> mlistXloaders;
	static tbool Find(CXloader* pXloader);
	static tbool Find(CXloader* pXloader, std::list<CXloader*>::iterator* pit);
};


#endif // _curl_multi_wrapper_h