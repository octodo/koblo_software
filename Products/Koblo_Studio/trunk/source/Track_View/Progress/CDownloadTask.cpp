
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

	geDownload_PresetSettings_Before,
	geDownload_PresetSettings_Action,
	geDownload_PresetSettings_After,

	geDownload_QueueInsertRegions,

	geDownload_Done
}; // EDownloadOrder


CDownloadTask::CDownloadTask()
{
	miActionOrder = 0;

	mpDownloader = NULL;

	mpTakeCurr = NULL;
	mpfileOgg = NULL;
	mpfileXML = NULL;
	mpfilePresetData = NULL;
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
		// This is a memory file - no delete
		mpfileXML->Destroy();
		mpfileXML = NULL;
	}
	//
	if (mpfilePresetData) {
		// This download file was not complete - it must be deleted!

		// Save path of file
		tchar pszPath[1024];
		*pszPath = '\0';
		mpfilePresetData->GetPathName(pszPath);

		// Close object - release handle
		mpfilePresetData->Destroy();
		mpfilePresetData = NULL;

		// Delete file
		if (*pszPath != '\0')
			IFile::DeleteFile(pszPath);
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


tbool CDownloadTask::Init_Update(std::list<CTake_Data*>* plistpTakes,
	const tchar* pszPresetDataUUID /*= ""*/, const tchar* pszFilePresetData /*= ""*/)
{
	if (miActionOrder != 0) {
		msExtendedError = "Double initialization";
		return false;
	}

	msUser					= "";//pszUser;
	msPassword				= "";//pszPassword;

	if (!Init_Helper(plistpTakes))
		return false;

	msPresetDataUUID = pszPresetDataUUID;
	if (msPresetDataUUID.size() > 0) {
		mpfilePresetData = IFile::Create();
		if (!mpfilePresetData->Open(pszFilePresetData, IFile::FileCreate)) {
			msExtendedError = "Unable to write-open file:\n ";
			msExtendedError += pszFilePresetData;
			msPresetDataUUID = "";
			mpfilePresetData = NULL;
			return false;
		}
	}

	if (mlistpTakes.size() == 0) {
		// Skip past download - all are there already
		miActionOrder = geDownload_QueueInsertRegions;
	}
	else {
		// Start with download
		miActionOrder = geDownload_Take_Download_Before;
	}
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
			pTake_Copy->Set_UUID( pTake->Get_UUID() );
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
				// Done with takes
				if (msPresetDataUUID.size() == 0) {
					// Don't get preset data - queue insert regions
					miActionOrder = geDownload_QueueInsertRegions;
				}
				else {
					// Go to Get preset data
					miActionOrder++;
				}
				gpApplication->Takes_Downloaded();
			}
			else {
				// Download next take
				miActionOrder = geDownload_Take_Download_Before;
			}
			break;

		case geDownload_PresetSettings_Before:
			bSuccess = DoGetPresetData_Before();
			miActionOrder++;
			break;
		case geDownload_PresetSettings_Action:
			bSuccess = DoGetPresetData_Action(&bActionDone);
			if (bActionDone) miActionOrder++;
			break;
		case geDownload_PresetSettings_After:
			bSuccess = DoGetPresetData_After();
			miActionOrder++;
			break;

		case geDownload_QueueInsertRegions:
			bSuccess = DoQueueInsertRegions();
			miActionOrder++;
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
	//!!! TODO: Insert functionality
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

	std::string sURL = mpTakeCurr->URL();
	if (
		(!mpDownloader->Init(sURL.c_str(), "", ""))
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
	// Close ogg file so it doesn't get auto-deleted when task is destroyed
	if (mpfileOgg) {
		// Destroy object (will close file handle, not delete file)
		mpfileOgg->Destroy();
		mpfileOgg = NULL;
	}

	// Queue decompress of take
	if (!gpApplication->Decompress_Take(mpTakeCurr)) {
		std::string sReason = gpApplication->Extended_Error();
		std::string sErr = "Unable to queue decomression of take:\n  ";
		sErr += mpTakeCurr->Screen_Name();
		sErr += "\nReason:\n  ";
		sErr += sReason;
		gpApplication->ShowMessageBox_NonModal(sErr.c_str(), "Download failed");
	}

	// Done yet?
	if (mlistpTakes.size() == 0) {
		// No more takes
		*pbNoMoreTakes = true;
	}
	return true;
} // DoTake_Download_After


tbool CDownloadTask::DoQueueInsertRegions()
{
	// Queue insertion of regions
	CInsertRegionsTask* pInsertRegionsTask = new CInsertRegionsTask();
	pInsertRegionsTask->Init();
	{
		// (lasse) no need - CAutoLock Lock(gpApplication->mMutex_Progress);
		gpApplication->mpProgressTasks->Add(pInsertRegionsTask);
		// (lasse) no need - gpApplication->Playback_InProgressTask();
	}

	return true;
} // DoQueueInsertRegions


tbool CDownloadTask::DoGetPresetData_Before()
{
	std::string sURI = std::string("/plugindata/") + msPresetDataUUID + ".xml";
	if ((!mpDownloader->Init("koblo.com", sURI.c_str(), 80))
		||
		(!mpDownloader->Start(mpfilePresetData))
	) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = std::string("Init+start of download preset data failed:\n") + pszErr;
		return false;
	}

	muiProgressIx = 0;
	muiProgressTarget = 1;
	msProgress = std::string("Downloading preset data");

	return true;
} // DoGetPresetData_Before

tbool CDownloadTask::DoGetPresetData_Action(tbool* pbActionDone)
{
	if (mpDownloader->IsFailed()) {
		tchar pszErr[1024];
		mpDownloader->GetError(pszErr, 1024);
		msExtendedError = std::string("Download preset data failed:\n") + pszErr;
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
} // DoGetPresetData_Action

tbool CDownloadTask::DoGetPresetData_After()
{
	// Close preset data file so it doesn't get auto-deleted when task is destroyed
	mpfilePresetData->Destroy();
	mpfilePresetData = NULL;

	return true;
} // DoGetPresetData_After

