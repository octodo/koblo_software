
#include "KSOS.h"


// Private enum for state machine
enum EDownloadOrder {
	geDownload_Uninitialized,

	geDownload_GetProject_Before,
	geDownload_GetProject_Action,
	geDownload_GetProject_After,

	geDownload_Take_Download_Before,
	geDownload_Take_Download_Action,
	geDownload_Take_Download_After,

	geDownload_Done
}; // EDownloadOrder


CDownloadTask::CDownloadTask()
{
	miActionOrder = 0;

	mpDownloader = NULL;

	mpTakeCurr = NULL;
	mpfileOgg = NULL;
	mpfileXML = NULL;
} // constructor


CDownloadTask::~CDownloadTask()
{
	if (mpDownloader) {
		mpDownloader->Destroy();
		mpDownloader = NULL;
	}
	//
	std::list<CTake_Data*>::iterator it = mlistpTakes.begin();
	for ( ; it != mlistpTakes.end(); it++) {
		CTake_Data* pTake = *it;
		delete pTake;
		mlistpTakes.pop_back();
	}
	if (mpTakeCurr) {
		delete mpTakeCurr;
		mpTakeCurr = NULL;
	}
	//
	if (mpfileOgg) {
		// This download file was not complete - it must be deleted!

		// Save path of file
		tchar pszPath[1024];
		*pszPath = '\0';
		mpfileOgg->GetPathName(pszPath);

		// Close object - release handle
		mpfileOgg->Destroy();
		mpfileOgg = NULL;

		// Delete file
		if (*pszPath != '\0')
			IFile::DeleteFile(pszPath);

	}
	//
	if (mpfileXML) {
		mpfileXML->Destroy();
		mpfileXML = NULL;
	}
} // destructor


void CDownloadTask::Destroy()
{
	delete dynamic_cast<CDownloadTask*>(this);
} // Destroy


tbool CDownloadTask::Init_DownloadXML(
	const tchar* pszUser, const tchar* pszPassword,
	const tchar* pszBranchUUID)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= pszUser;
	msPassword				= pszPassword;

	msBranchUUID			= pszBranchUUID;

	if (!Init_Helper(NULL))
		return false;

	mpfileXML = IFileMemory::Create();
	if (mpfileXML == NULL)
		return false;
	if (mpfileXML->Open(IFile::FileCreate))
		return false;

	miActionOrder			= geDownload_GetProject_Before;
	return true;
} // Init_DownloadXML


tbool CDownloadTask::Init_Update(
	const tchar* pszUser,
	const tchar* pszPassword,
	std::list<CTake_Data*>* plistpTakes)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= pszUser;
	msPassword				= pszPassword;

	if (!Init_Helper(plistpTakes))
		return false;

	if (mlistpTakes.size() == 0) {
		msExtendedError = "No takes to download";
		return false;
	}

	miActionOrder = geDownload_Take_Download_Before;
	return true;
} // Init_Update


tbool CDownloadTask::Init_Helper(std::list<CTake_Data*>* plistpTakes)
{
	// Note: Unfortunately we NEED TO COPY the takes!!!
	// Can't use the originals as that won't be thread safe
	if (plistpTakes) {
		std::list<CTake_Data*>::iterator it = plistpTakes->begin();
		for ( ; it != plistpTakes->end(); it++) {
			CTake_Data* pTake = *it;
			CTake_Data* pTake_Copy = new CTake_Data(*pTake);
			mlistpTakes.push_back(pTake_Copy);
		}
	}

	if (mpDownloader == NULL) {
		mpDownloader = ine::IDownloader::Create();
	}

	return (mpDownloader != NULL);
} // Init_Helper


tbool CDownloadTask::DoWork()
{
	tbool bSuccess = false;
	tbool bActionDone = false;
	tbool bNoMoreTakes = false;

	switch (miActionOrder) {
		case geDownload_Uninitialized:
			msExtendedError = "Internal error: Uninitialized";
			break;

		case geDownload_GetProject_Before:
			bSuccess = DoGetBranchXml_Before();
			miActionOrder++;
			break;
		case geDownload_GetProject_Action:
			bSuccess = DoGetBranchXml_Action(&bActionDone);
			if (bActionDone) miActionOrder++;
			break;
		case geDownload_GetProject_After:
			bSuccess = DoGetBranchXml_After(&bNoMoreTakes);
			// Where to go?
			if (bNoMoreTakes) {
				// No takes - done
				miActionOrder = geDownload_Done;
			}
			else {
				// Download takes
				miActionOrder = geDownload_Take_Download_Before;
			}
			break;

		case geDownload_Take_Download_Before:
			bSuccess = DoTake_Download_Before();
			miActionOrder++;
			break;
		case geDownload_Take_Download_Action:
			bSuccess = DoTake_Download_Action(&bActionDone);
			if (bActionDone) miActionOrder++;
			break;
		case geDownload_Take_Download_After:
			bSuccess = DoTake_Download_After(&bNoMoreTakes);
			// Where to go?
			if (bNoMoreTakes) {
				// Done
				miActionOrder = geDownload_Done;
			}
			else {
				// Download next take
				miActionOrder = geDownload_Take_Download_Before;
			}
			break;

		default:
			// Why are we here?
			break;
	}

	if (!bSuccess) {
		miActionOrder = geDownload_Done;
	}
	return bSuccess;
} // DoWork


tbool CDownloadTask::IsDone()
{
	return (miActionOrder >= geDownload_Done);
} // IsDone


tbool CDownloadTask::DoGetBranchXml_Before()
{
	std::string sURI = std::string("/branches/") + msBranchUUID + "/commits/latest/markup.xml";
	if (
		(!mpDownloader->Init("koblo.com", sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpDownloader->Start(mpfileXML))
	) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = "Init+start of download project xml failed:\n";
		msExtendedError += pszErr;
		return false;
	}

	muiProgressIx = 0;
	muiProgressTarget = 1;
	msProgress = std::string("Downloading project xml");

	return true;
} // DoGetBranchXml_Before

tbool CDownloadTask::DoGetBranchXml_Action(tbool* pbActionDone)
{
	if (mpDownloader->IsFailed()) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = std::string("Download project xml failed:\n") + pszErr;
		tint32 iReplySize = (tint32)mpfileXML->GetMemorySize();
		if (iReplySize > 1) {
			msExtendedError += "\n\n";
			msExtendedError += std::string((tchar*)(mpfileXML->GetMemoryPtr()), iReplySize);
		}
		return false;
	}

	tint64 iProgress = 0;
	tint64 iTotal = 1;
	mpDownloader->GetProgress(&iProgress, &iTotal);
	muiProgressIx = (tuint64)(iProgress);
	muiProgressTarget = (tuint64)(iTotal);

	if (mpDownloader->IsDone())
		*pbActionDone = true;

	return true;
} // DoGetBranchXml_Action

tbool CDownloadTask::DoGetBranchXml_After(tbool* pbNoTakes)
{
	// Get project xml parsed
	//gpApplication->Set_Branch_Name(msBranchName);

	if (mlistpTakes.size() == 0) {
		// No takes to download - we're done
		*pbNoTakes = true;
	}

	return true;
} // DoGetBranchXml_After


tbool CDownloadTask::DoTake_Download_Before()
{
	if (mlistpTakes.size() == 0) {
		msExtendedError = "Internal error: Where did upload takes in list go?";
		return false;
	}
	if (mpTakeCurr) {
		// Release this take - it has already been handled
		delete mpTakeCurr;
		mpTakeCurr = NULL;
	}
	mpTakeCurr = mlistpTakes.front();
	mlistpTakes.pop_front();

	if (mpfileOgg) {
		msExtendedError = "Internal error: Previous download file not closed";
		return false;
	}
	mpfileOgg = IFile::Create();
	if (mpfileOgg == NULL) {
		return false;
	}
	std::string sOgg = mpTakeCurr->GetOggFilePath();
	if (!mpfileOgg->Open(sOgg.c_str(), IFile::FileCreate)) {
		msExtendedError = std::string("Can't create file:\n") + sOgg;
		mpfileOgg->Destroy();
		mpfileOgg = NULL;
		return false;
	}

#ifdef _WIN32
	std::string sURL = mpTakeCurr->URL();
	tchar pszURL_Lower[1024];
	strncpy(pszURL_Lower, sURL.c_str(), 1023);
	pszURL_Lower[1023] = '\0';
	strlwr(pszURL_Lower);
	const tchar* pszSlash = strchr(sURL.c_str(), '/');
	const tchar* pszHTTP = strstr(pszURL_Lower, "http://");
	std::string sHost, sURI;
	if (pszSlash == NULL) {
		// There is no document/URI after host/server
		sURI = "/";
		if (pszHTTP != pszURL_Lower) {
			// Doesn't start with "http://"
			sHost = sURL;
		}
		else {
			// Skip "http://"
			sHost = sURL.substr(7);
		}
	}
	else {
		// First extract document/URI part of URL
		sURI = pszSlash;
		tint32 iHostPartLen = sURL.length() - sURI.length();
		sHost = sURL.substr(0, iHostPartLen);
		if (pszHTTP == pszURL_Lower) {
			// Remove "http://"
			sHost = sHost.substr(7);
		}
	}
#endif // _WIN32
#ifdef _Mac
	std::string sURL = mpTakeCurr->URL();
	const tchar* pszSlash = strchr(sURL.c_str(), '/');
	tint32 iHTTPIx = strncasecmp(sURL.c_str(), "http://", 7);
	std::string sHost, sURI;
	if (pszSlash == NULL) {
		// There is no document/URI after host/server
		sURI = "/";
		if (iHTTPIx != 0) {
			// Doesn't start with "http://"
			sHost = sURL;
		}
		else {
			// Skip "http://"
			sHost = sURL.substr(7);
		}
	}
	else {
		// First extract document/URI part of URL
		sURI = pszSlash;
		tint32 iHostPartLen = sURL.length() - sURI.length();
		sHost = sURL.substr(0, iHostPartLen);
		if (iHTTPIx == 0) {
			// Remove "http://"
			sHost = sHost.substr(7);
		}
	}
#endif //_Mac
	
	if (
		(!mpDownloader->Init(sHost.c_str(), sURI.c_str(), 80, msUser.c_str(), msPassword.c_str()))
		||
		(!mpDownloader->Start(mpfileOgg))
	) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = "Init+start download take:\n";
		msExtendedError += pszErr;
		return false;
	}

	muiProgressIx = 0;
	muiProgressTarget = 1;
	msProgress = std::string("Downloading ") + mpTakeCurr->Screen_Name();

	return true;
} // DoTake_Download_Before

tbool CDownloadTask::DoTake_Download_Action(tbool* pbActionDone)
{
	if (mpDownloader->IsFailed()) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = std::string("Download of take failed:\n") + pszErr;
		return false;
	}

	tint64 iProgress = 0;
	tint64 iTotal = 1;
	mpDownloader->GetProgress(&iProgress, &iTotal);
	muiProgressIx = (tuint64)(iProgress);
	muiProgressTarget = (tuint64)(iTotal);

	if (mpDownloader->IsDone())
		*pbActionDone = true;

	return true;
} // DoTake_Download_Action

tbool CDownloadTask::DoTake_Download_After(tbool* pbNoMoreTakes)
{
	// Close file so it won't get auto-deleted
	if (mpfileOgg) {
		// Destroy object (will close file handle, not delete file)
		mpfileOgg->Destroy();
		mpfileOgg = NULL;
	}

	// Done yet?
	if (mlistpTakes.size() == 0) {
		// No more takes
		*pbNoMoreTakes = true;
	}
	return true;
} // DoTake_Download_After


