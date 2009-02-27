
/*! \class CXloader
 * \brief Implements IUploader and IDownloader
 *
 * Supplies more complex and buffered method for posting/putting web-data
 * (lasse)
*/
class CXloader : public virtual IDownloader, public virtual IUploader
{
	friend class CXloader_MultiWrapper;
public:
	CXloader(tbool bIsUploader);
	virtual ~CXloader();
	
	//! IDestructable override
	virtual void Destroy();

	//! IDownloader implementation
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IDownloader implementation
	virtual tbool Init(const tchar* pszFullURL, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IDownloader implementation
	virtual tbool SetReplyMIMEType(E_MIME_Type eMIME);
	//! IDownloader implementation
	virtual tbool SetSpecificVerb(EVerbType eVerb);
	//! IDownloader implementation
	virtual tbool SetFailOnHttpStatus(tbool bFailOnStatus);
	//! IUploader implementation
	virtual tbool SetStreamingUpload(tbool bUseStreaming);
	//! IDownloader implementation
	virtual tbool DisableAutoRedirects();
	//! IDownloader implementation
	virtual tbool EnableAutoRedirects();
	//! IDownloader implementation
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen);
	//! IUploader implementation
	virtual tbool AddFileParam(const tchar* pszParamName, IFile* pfileToUpload, tchar* pszDestinationName = NULL, E_MIME_Type eMIME = MIME_TYPE_DEFAULT);
	//! IDownloader implementation
	virtual tbool Start(IFile* pfileForDownload = NULL);
	//! IDownloader implementation
	virtual tbool GetReplyPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize);
	//! IDownloader implementation
	virtual tbool Abort();
	//! IUploader implementation
	virtual tbool GetProgress(tint64* piUploadProgress, tint64* piUploadSize, tint64* piDownloadProgress, tint64* piDownloadSize);
	//! IDownloader implementation
	virtual tbool GetProgress(tint64* piDownloadProgress, tint64* piDownloadSize);

	//! IDownloader implementation
	virtual tbool IsDone();
	//! IDownloader implementation
	virtual tbool IsFailed();
	//! IDownloader implementation
	virtual tbool IsFinished();

	//! IDownloader implementation
	virtual tbool GetError(tchar* pszErrBuff, tint32 iErrBuffSize);
	//! IDownloader implementation
	virtual tint32 GetHttpStatusCode() { return miHttpStatus; };

	// Callbacks
	size_t ReadFunction_ForUpload(IFile* pfile, void *ptr, size_t size, size_t nmemb);
	//int SeekFunction_ForUpload(IFile* pfile, curl_off_t offset, int origin);
	size_t WriteFunction_ForReply(void *ptr, size_t size, size_t nmemb);
	int ProgressFunction(double dDownloadSize, double dDownloaded, double dUploadSize, double dUploaded);

	struct SUploadStream {
		CXloader* mpThis;
		//
		std::string msParamName;
		IFile* mpfile;
		E_MIME_Type meMIME;
		std::string msNameAndExtOnly;
	};

protected:
	tbool mbIsUploader;

	tbool _Init(const tchar* pszFullURL, const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);

	std::string msURL;
	std::string msHost;
	std::string msPage;
	tint32 miPort;
	std::string msUser;
	std::string msPassword;
	tbool mbUseAuthentication;
	tuint32 muiTimeOutSecs;

	E_MIME_Type meReplyMIMEType;
	const tchar* GetReplyMIMEString();
	const tchar* GetMIMEString(E_MIME_Type eMIME);
	
	EVerbType meSpecificVerb;
	EVerbType GetActuallyUsedVerb();
	const tchar* GetVerbString(EVerbType eVerb);

	tbool mbAllowRedirects;
	tbool SetAllowRedirects(tbool bAllow);

	tbool mbUseStreamingUpload;
	
	std::list<SUploadStream*> mlistUploadFiles;

	std::list<std::string> mlist_sParamNames;
	std::list<tint32> mlist_iParamDataLen;
	std::list<tchar*> mlist_pszParamData;
	tint32 miParamsAssembledLen;
	tchar* mpszParamsAssembled;
	tbool AssembleParams();
	void WipeParams();
	CMutex mMutex_ForParams;
	tbool VerifyParamName(const tchar* pszParamName);

	// File for writing reply directly into
	IFile* mpfileForReply;

	std::list<CXloader_ReplyChainLink*> mlist_pReplyChain;
	volatile tint32 miLockLevel_ForReplyBuffer;
	void ZapReplyBuffer();

	tuint64 muiUploadProgress;
	tuint64 muiUploadSize;
	tuint64 muiReplyProgress;
	tuint64 muiReplySize;

	tint32 miHttpStatus;

	std::string msLastError;
	void AppendError(const tchar* pszError);
	CMutex mMutex_ForErrors;

	//! If true we never get body of message with error status (default false)
	tbool mbFailImmediatelyOnStatus;
	//! Temporary error string used when http status indicates error but we're not done with receival of body
	std::string msDelayedStatusError;

	
	CMutex mMutex_Connection;
	tbool OpenConnection();
	void CloseConnection();
	
	void SetIsUninitialized();
	void SetIsFullBlownURL(tbool bFullBlownURL);
	void SetIsInitialized();
	void SetIsTransfering();
	void SetMultiSaysDone(CURLcode code);
	void SetIsDone();
	void SetIsFailed();

	tbool IsInitialized();
	tbool IsTransfering();
	
	
private:
	volatile tbool mbIsInitialized;
	volatile tbool mbIsInit_FullBlownURL;
	volatile tbool mbIsTransfering;
	volatile tbool mbIsFailed;
	volatile tbool mbIsMultiDone;
	volatile tbool mbIsDone;

	tchar mpszErrorBuffer[CURL_ERROR_SIZE];

	CURL* mpCURLEasyHandle;
	curl_httppost* mpFormPost_First;
	curl_httppost* mpFormPost_Last;
	curl_slist* mpSList_ExtraHeaders;
	
	tbool AssembleParams_ForUrlEncoded();
	tbool AssembleParams_ForMultiPartForm();

	tbool SetOpt(CURLoption iOption, const tchar* pszOption, const void* pData, const tchar* pszExtraInfo = "");
	tbool SetOpt(CURLoption iOption, const tchar* pszOption, tint32 iData, const tchar* pszExtraInfo = "");
	tbool SetOpt(CURLoption iOption, const tchar* pszOption, tuint32 uiData, const tchar* pszExtraInfo = "");
	tbool SetOpt(CURLoption iOption, const tchar* pszOption, const tchar* pszData, const tchar* pszExtraInfo = "");
	tbool SetOpt(CURLoption iOption, const tchar* pszOption, const std::string& rsData, const tchar* pszExtraInfo = "");
	tbool SetOpt(CURLoption iOption, const tchar* pszOption, tbool bData, const tchar* pszExtraInfo = "");

	tbool GetInfo(CURLINFO eInfo, const tchar* pszInfo, void* pvoidData);
	tbool GetInfo(CURLINFO eInfo, const tchar* pszInfo, tint32* piData);
	tbool GetInfo(CURLINFO eInfo, const tchar* pszInfo, double* pfData);
	
};