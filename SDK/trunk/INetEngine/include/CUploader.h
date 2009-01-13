class CUploader : public virtual IUploader, public virtual CUpAndDownloader_Common
{
public:
	CUploader();
	virtual ~CUploader();
	
	//! IDestructable override
	virtual void Destroy();

	//! IUploader implementation
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10);
	//! IUploader implementation
	virtual tbool SetDesiredMIMEType(E_MIME_Type eType);
	//! IUploader implementation
	virtual tbool SetSpecificVerb(EVerbType eVerb);
	//! IUploader implementation
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen);
	//! IUploader implementation
	virtual tbool UploadPortion(const tchar* pszData, tint32 iDataLen, tint32* piActuallySent);
	//! IUploader implementation
	virtual tbool UploadFinish();
	//! IUploader implementation
	virtual tbool Abort();

	//! IUploader implementation
	virtual tbool IsDone();
	//! IUploader implementation
	virtual tbool IsFailed();

	//! IUploader implementation
	virtual tbool GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize);

protected:
	void Constructor_OSSpecific();
	void Destructor_OSSpecific();

	CMutex mMutex_Connection;
	tbool OpenConnection();
	tbool OpenConnection_OSSpecific();
	void CloseConnection();
	void CloseConnection_OSSpecific();
	
	tbool UploadPortion_OSSpecific(const tchar* pszData, tint32 iDataLen, tint32* piActuallySent);
	

#ifdef _WIN32
	#include "CUploaderWin.h"
#endif // _WIN32
#ifdef _Mac
	#include "CUploaderOSX.h"
#endif // _Mac

};