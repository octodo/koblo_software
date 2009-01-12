class CDownloader : public virtual IDownloader
{
public:
	CDownloader();
	virtual ~CDownloader();
	
	//! IDestructable override
	virtual void Destroy();

	//! IDownloader implementation
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IDownloader implementation
	virtual tbool SetDesiredMIMEType(EDesiredMIMEType eType);
	//! IDownloader implementation
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen);
	//! IDownloader implementation
	virtual tbool DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize);
	//! IDownloader implementation
	virtual tbool Abort();

	//! IDownloader implementation
	virtual tbool IsDone();
	//! IDownloader implementation
	virtual tbool IsFailed();

	//! IDownloader implementation
	virtual tbool GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize);

protected:
	std::string msHost;
	std::string msPage;
	tint32 miPort;
	std::string msUser;
	std::string msPassword;
	tuint32 muiTimeOutSecs;

	EDesiredMIMEType meDesiredMIMEType;

	std::list<std::string> mlist_sParamNames;
	std::list<tint32> mlist_iParamDataLen;
	std::list<tchar*> mlist_pszParamDataUrlEncoded;
	tint32 miParamsAssembledLen;
	tchar* mpszParamsAssembled;
	tbool AssembleParams();
	void WipeParams();
	CMutex mMutex_ForParams;

	volatile tbool mbIsInitialized;
	volatile tbool mbIsDownloading;
	volatile tbool mbIsFailed;
	volatile tbool mbIsDone;

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
	
	tbool DownloadPortion_OSSpecific(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize);
	

#ifdef _WIN32
	#include "CDownloaderWin.h"
#endif // _WIN32
#ifdef _Mac
	#include "CDownloaderOSX.h"
#endif // _Mac

};