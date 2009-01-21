//! Static pointer to CURL multi instance. The instance holds a number of active download/upload threads (every IUploader and IDownloader has 0 or 1 active thread)
static CURLM* gpCURLMulti = NULL; //curl_multi_init( );
//! Keeps track of how many IUploader and IDownloader objects are using this CURL multi instance
static volatile tint32 giCURLMultiHooks = 0;
//! Lock for modifying pointer to CURL multi instance
static volatile tint32 giCURLMulti_Level = 0;

//! Implement of IUploader and IDownloader
class CXloader : public virtual IDownloader, public virtual IUploader
{
public:
	CXloader(tbool bIsUploader);
	virtual ~CXloader();
	
	//! IDestructable override
	virtual void Destroy();

	//! IDownloader implementation
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IUploader implementation
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, IFile* pfileToUpload, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IDownloader implementation
	virtual tbool SetReplyMIMEType(E_MIME_Type eMIME);
	//! IDownloader implementation
	virtual tbool SetSpecificVerb(EVerbType eVerb);
	//! IDownloader implementation
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen);
	//! IDownloader implementation
	virtual tbool DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize);
	//! IUploader implementation
	virtual tbool UploadPortion(tuint64* puiUploadProgress, tchar* pszReplyBuffer, tint32 iReplyBufferSize, tint32* piReplyPortionSize, tuint64* puiReplyTotalSize);
	//! IDownloader implementation
	virtual tbool Abort();

	//! IDownloader implementation
	virtual tbool IsDone();
	//! IDownloader implementation
	virtual tbool IsFailed();

	//! IDownloader implementation
	virtual tbool GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize);

protected:
	tbool mbIsUploader;

	void GetLockForMultiInstance();
	void ReleaseLockForMultiInstance();
	
	std::string msHost;
	std::string msPage;
	IFile* mpfileToUpload;
	tint32 miPort;
	std::string msUser;
	std::string msPassword;
	tuint32 muiTimeOutSecs;
	
	E_MIME_Type meMIMEType;
	const tchar* GetMIMEString();
	
	EVerbType meSpecificVerb;
	EVerbType GetVerb(EVerbType eVerbDefault);
	const tchar* GetVerbString(EVerbType eVerbDefault);
	
	std::list<std::string> mlist_sParamNames;
	std::list<tint32> mlist_iParamDataLen;
	std::list<tchar*> mlist_pszParamDataUrlEncoded;
	tint32 miParamsAssembledLen;
	tchar* mpszParamsAssembled;
	tbool AssembleParams();
	void WipeParams();
	CMutex mMutex_ForParams;
	void CloseFile_IgnoreError();

	
	volatile tuint32 muiAliveMs;
	void RefreshAlive();
	tbool IsAlive();
	
	std::string msLastError;
	void SetError(const tchar* pszError);
	CMutex mMutex_ForErrors;

	
	void Constructor_OSSpecific();
	void Destructor_OSSpecific();

	CMutex mMutex_Connection;
	tbool OpenConnection();
	tbool OpenConnection_OSSpecific();
	void CloseConnection();
	void CloseConnection_OSSpecific();
	
	tbool DoPortion_OSSpecific(tuint64* puiUploadProgress, tchar* pszReplyBuffer, tint32 iReplyBufferSize, tint32* piReplyPortionSize, tuint64* puiReplyTotalSize);
	
	void SetIsUninitialized();
	void SetIsInitialized();
	void SetIsTransfering();
	void SetIsDone();
	void SetIsFailed();
	
	tbool IsInitialized();
	tbool IsTransfering();
	
	
private:
	volatile tbool mbIsInitialized;
	volatile tbool mbIsTransfering;
	volatile tbool mbIsFailed;
	volatile tbool mbIsDone;
	
	
protected:
	
#ifdef _WIN32
	#include "CXloaderWin.h"
#endif // _WIN32
#ifdef _Mac
	#include "CXloaderOSX.h"
#endif // _Mac

};