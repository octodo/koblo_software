
#include "ineInternalOS.h"

CUploader::CUploader()
{
	Constructor_OSSpecific();
} // constructor


CUploader::~CUploader()
{
	Abort();
	WipeParams();
	CloseConnection();
	//
	Destructor_OSSpecific();
} // destructor


IUploader* IUploader::Create()
{
	return dynamic_cast<IUploader*>(new CUploader());
} // Create


void CUploader::Destroy()
{
	delete dynamic_cast<CUploader*>(this);
} // Destroy


tbool CUploader::Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort /*= 80*/, const tchar* pszUser /*= NULL*/, const tchar* pszPassword /*= NULL*/, tint32 iTimeOutSecs /*= 10*/)
{
	Abort();

	return CUpAndDownloader_Common::Init(pszHost, pszPage, iPort, pszUser, pszPassword, iTimeOutSecs);
} // Init


tbool CUploader::SetDesiredMIMEType(E_MIME_Type eType)
{
	return CUpAndDownloader_Common::SetMIMEType(eType);
} // SetDesiredMIMEType


tbool CUploader::SetSpecificVerb(EVerbType eVerb)
{
	return CUpAndDownloader_Common::SetSpecificVerb(eVerb, false);
} // SetSpecificVerb


tbool CUploader::AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen)
{
	return CUpAndDownloader_Common::AddParam(pszParamName, pcParamData, iParamDataLen);
} // AddParam


tbool CUploader::OpenConnection()
{
	return OpenConnection_OSSpecific();
} // OpenConnection


void CUploader::CloseConnection()
{
	CloseConnection_OSSpecific();
} // CloseConnection


tbool CUploader::UploadPortion(const tchar* pszData, tint32 iDataLen, tint32* piActuallySent)
{
	*piActuallySent = 0;
	
	if (IsFailed()) {
		//SetError("Previous error");
		return false;
	}
	
	if (!IsInitialized()) {
		SetError("Not initialized");
		return false;
	}
	
	tbool bFirstTime = (!IsTransfering());
	SetIsTransfering();
	if (bFirstTime) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		if (!AssembleParams()) return false;
		if (!OpenConnection()) {
			CloseConnection();
			return false;
		}
		// We're alive
		RefreshAlive();
	}
	
	return UploadPortion_OSSpecific(pszData, iDataLen, piActuallySent);
} // UploadPortion


tbool CUploader::UploadFinish()
{
	SetError("Not implemented yet");
	return false;
} // UploadFinish


tbool CUploader::Abort()
{
	if (IsTransfering()) {
		CAutoLock Lock(mMutex_Connection);
		
		CloseConnection();
		SetIsDone();
	}
	
	WipeParams();
	
	return true;
} // Abort


tbool CUploader::IsDone()
{
	return CUpAndDownloader_Common::IsDone();
} // IsDone


tbool CUploader::IsFailed()
{
	return CUpAndDownloader_Common::IsFailed();
} // IsFailed


tbool CUploader::GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize)
{
	return CUpAndDownloader_Common::GetLatestError(pszErrBuff, iErrBuffSize);
}
