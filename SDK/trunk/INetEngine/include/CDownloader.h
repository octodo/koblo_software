class CDownloader : public virtual IDownloader
{
public:
	CDownloader();
	virtual ~CDownloader();
	
	//! IDestructable override
	virtual void Destroy();

	//! IDownloader implementation
	virtual tbool Init(tchar* pszHost, tchar* pszPage, tint32 iPort = 80);
	//! IDownloader implementation
	virtual tbool AddParam(tchar* pszParamName, tchar* pcParamData, tint32 iParamDataLen);
	//! IDownloader implementation
	virtual tbool DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize);
	//! IDownloader implementation
	virtual tbool Abort();
	//! IDownloader implementation
	virtual tbool IsDone();

protected:
	std::string msHost;
	std::string msPage;
	tint32 miPort;

	std::list<std::string> mlist_sParamNames;
	std::list<tint32> mlist_iParamDataLen;
	std::list<tchar*> mlist_pszParamDataUrlEncoded;
	tint32 iParamsAssembledLen;
	tchar* mpszParamsAssembled;
	tbool AssembleParams();
	void WipeParams();

	tbool mbIsDownloading;
	tbool mbIsDone;

};