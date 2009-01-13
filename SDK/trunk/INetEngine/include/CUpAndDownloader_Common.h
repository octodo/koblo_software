class CUpAndDownloader_Common
{
public:
	CUpAndDownloader_Common();
	virtual ~CUpAndDownloader_Common();
	
	tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen);

	void SetIsUninitialized();
	void SetIsInitialized();
	void SetIsTransfering();
	void SetIsDone();
	void SetIsFailed();

	tbool IsInitialized();
	tbool IsTransfering();
	tbool IsDone();
	tbool IsFailed();
	tbool GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize);

protected:
	std::string msHost;
	std::string msPage;
	tint32 miPort;
	std::string msUser;
	std::string msPassword;
	tuint32 muiTimeOutSecs;

	E_MIME_Type meMIMEType;
	tbool SetMIMEType(E_MIME_Type eMIME);
	const tchar* GetMIMEString();

	EVerbType meSpecificVerb;
	tbool SetSpecificVerb(EVerbType eVerb, tbool bIsUploader);
	const tchar* GetVerbString(EVerbType eVerbDefault);

	std::list<std::string> mlist_sParamNames;
	std::list<tint32> mlist_iParamDataLen;
	std::list<tchar*> mlist_pszParamDataUrlEncoded;
	tint32 miParamsAssembledLen;
	tchar* mpszParamsAssembled;
	tbool AssembleParams();
	void WipeParams();
	CMutex mMutex_ForParams;

	volatile tuint32 muiAliveMs;
	void RefreshAlive();
	tbool IsAlive();

	std::string msLastError;
	void SetError(const tchar* pszError);
	CMutex mMutex_ForErrors;

private:
	volatile tbool mbIsInitialized;
	volatile tbool mbIsTransfering;
	volatile tbool mbIsFailed;
	volatile tbool mbIsDone;

};