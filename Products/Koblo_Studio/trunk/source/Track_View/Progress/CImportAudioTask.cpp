
#include "KSOS.h"

#ifdef _DEBUG
const tint32 kiPortionSize = 128 * 1024;
#else _DEBUG
const tint32 kiPortionSize = 1024 * 1024;
#endif // _DEBUG

CImportAudioTask::CImportAudioTask() {
	
	
	meCodec = ac::geAudioCodecUndefined;
	
	mpfSrc = mpfDstL = mpfDstR = NULL;
	mpDecoder = NULL;
	miAudioImportOrder = geAudioImport_Start;
	
	mbDstIsAlreadyThere = false;
	mbCopyLossyOriginal = false;
	
	miRegion_TrackID = -1;
	miRegion_TrackPos = -1;
	mbDeleteNonLossyOriginal = false;
}

CImportAudioTask::~CImportAudioTask() {
	tbool bWasAborted = false;
	if (mpDecoder) {
		bWasAborted = true;

		mpDecoder->Destroy();
		mpDecoder = NULL;
	}
	if (mpfSrc) {
		mpfSrc->Destroy();
		mpfSrc = NULL;
	}

	if (mpfDstL) {
		mpfDstL->Destroy();
		mpfDstL = NULL;
		if ((bWasAborted) && (msDstPathNameL.length())) {
			IFile::DeleteFile(msDstPathNameL.c_str());
		}
	}
	if (mpfDstR) {
		mpfDstR->Destroy();
		mpfDstR = NULL;
		if ((bWasAborted) && (msDstPathNameR.length())) {
			IFile::DeleteFile(msDstPathNameR.c_str());
		}
	}
} // destructor


void CImportAudioTask::Destroy()
{
	delete dynamic_cast<CImportAudioTask*>(this);
} // Destroy

/*
 // old one
tbool CImportAudioTask::Init(const tchar* pszSrcPath, tbool bDoesWaveAlreadyExist , EStereoBehavior eStereoBehavior , tbool bForceOriginalIsLossy )
{
//	mpPlugIn = pPlugIn;
	msSrcPathName = pszSrcPath;

	// Extract the file name from path
	tint iPos = msSrcPathName.find_last_of(":");
	msClipName = msSrcPathName.substr(iPos + 1);
	msPathOnly = msSrcPathName.substr(0, iPos + 1);
	tint iPosDot = msClipName.find_last_of(".");
	msExt = "";
	// Note: '>' is intented; not '>=' 
	if (iPosDot > 0) {
		msExt = msClipName.substr(iPosDot, msClipName.length() - iPosDot);
		msClipName.erase(iPosDot, msExt.length());
	}

	// Build target path
	std::string sTargetPath = gpApplication->GetProjDir_Clips();

	// Determine file format and number of channels
	if (bDoesWaveAlreadyExist) {
		mbDstIsAlreadyThere = true;

		meCodec = ac::geAudioCodecWave;
		meSrcQuality = ac::geQualityUnknown;
		mbSrcLossyCompressed = bForceOriginalIsLossy;
		msDstNameL = msClipName;
		msDstNameR = msClipName;
		mbStereo = (eStereoBehavior > geStereoDoAsk);
		miBitWidth = 0;
	}
	else {
		mpfSrc = IFile::Create();
		if (mpfSrc == NULL) {
			msExtendedError = std::string("IFile::Create() => NULL for '") + msClipName + "' (out of memory?).";
			return false;
		}
		if (!mpfSrc->Open(pszSrcPath, IFile::FileRead)) {
			msExtendedError = std::string("Can't open file '") + msClipName + "'.";
			return false;
		}

		tchar pszErrMsgBuff[1024];
		*pszErrMsgBuff = '\0';
		ac::IDecoder* pDecoder = ac::IDecoder::Create(mpfSrc, pszErrMsgBuff, 1024);
		if (!pDecoder) {
			if (*pszErrMsgBuff != '\0')
				msExtendedError = pszErrMsgBuff;
			else
				msExtendedError = std::string("Unknown format file '") + msClipName + "'.";
			return false;
		}
		// We have to do a TestFile here - even though it will fail if it causes mp3
		// LAME engine to be invoked from two different threads
		if (!pDecoder->TestFile(mpfSrc)) {
			tchar pszErrMsg[1024];
			pDecoder->GetErrMsg(pszErrMsg, 1024, true);
			msExtendedError = std::string("Error testing file: ") + pszErrMsg;
			return false;
		}
		meCodec = pDecoder->GetAudioCodec();
		mbSrcLossyCompressed = pDecoder->mbIsLossyCompression;
		meSrcQuality = pDecoder->meLowestInputQuality;
		msDstNameL = msClipName;
		msDstNameR = msClipName;
		mbStereo = (pDecoder->miOutputChannels > 1);
		if (mbSrcLossyCompressed)
			miBitWidth = 24; //pDecoder->miOutputBitWidth;
		else
			miBitWidth = (meSrcQuality == ac::geQualityLossless16) ? 16 : 24;
		// First close decoder, so it won't crash later
		pDecoder->Destroy();
		pDecoder = NULL;
	}

	mbSplit = false;
	if (mbStereo) {
		msDstNameL += "-1";
		msDstNameR += "-2";

		if (eStereoBehavior == geStereoDoSplit)
			mbSplit = true;
		else if (eStereoBehavior == geStereoDoKeep)
			mbSplit = false;
		else {
			tchar pszMsg[1024];
			sprintf(
				pszMsg,
				"Importing clip '%s'\n"
				"\n"
				"This is a stereo file. Do you want to use it like that?\n"
				"\n"
				" Stereo:\tImport as a stereo stream\n"
				" Split:\tSplit file into two mono streams\n"
				" Cancel:\tDon't import file(s) anyway",
				msClipName.c_str()
				);

			ge::IWindow::EMsgBoxReturn eRet;
			eRet = ge::IWindow::ShowMessageBox(pszMsg, "Stereo file", ge::IWindow::MsgBoxSplitStereo);
			switch (eRet) {
				case ge::IWindow::MsgBoxRetYes:
					mbSplit = false;
					break;
				case ge::IWindow::MsgBoxRetNo:
					mbSplit = true;
					break;
				case ge::IWindow::MsgBoxRetCancel: 
					// User aborted - not an error
					return false; // true;
			}
		}
	}
	// This can take some time - so display hour-glass cursor
	//CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
	

	// Build target path name
	if (bDoesWaveAlreadyExist) {
		msDstPathNameL = gpApplication->GetFromWaveName_ClipWave_Safe(msDstNameL.c_str());
		msDstPathNameR = gpApplication->GetFromWaveName_ClipWave_Safe(msDstNameR.c_str());
	}
	else {
		if (mbSrcLossyCompressed) {
			msDstPathNameL = gpApplication->GetFromWaveName_ClipWaveDecomp(msDstNameL.c_str());
			msDstPathNameR = gpApplication->GetFromWaveName_ClipWaveDecomp(msDstNameR.c_str());
		}
		else {
			msDstPathNameL = gpApplication->GetFromWaveName_ClipWave(msDstNameL.c_str());
			msDstPathNameR = gpApplication->GetFromWaveName_ClipWave(msDstNameR.c_str());
		}
	}

	return true;
} // Init
*/
// new one
tbool CImportAudioTask::Init(const tchar* pszSrcPath, tbool bDoesWaveAlreadyExist , EStereoBehavior eStereoBehavior , tbool bForceOriginalIsLossy )
{
	//	mpPlugIn = pPlugIn;
	msSrcPathName = pszSrcPath;
	
	// Extract the file name from path
	tint iPos = msSrcPathName.find_last_of(":");
	msClipName = msSrcPathName.substr(iPos + 1);
	msPathOnly = msSrcPathName.substr(0, iPos + 1);
	tint iPosDot = msClipName.find_last_of(".");
	
	
	msExt = "";
	// Note: '>' is intented; not '>=' 
	if (iPosDot > 0) {
		msExt = msClipName.substr(iPosDot, msClipName.length() - iPosDot);
		msClipName.erase(iPosDot, msExt.length());
	}
	
	// Build target path
	std::string sOldTargetPath = gpApplication->GetProjDir_Clips();
	// Build target path
	std::string sTargetPath = gpApplication->Project_Folder() + ":Wave Files:";
	
	// Determine file format and number of channels
	if (bDoesWaveAlreadyExist) {
		mbDstIsAlreadyThere = true;
		
		meCodec = ac::geAudioCodecWave;
		meSrcQuality = ac::geQualityUnknown;
		mbSrcLossyCompressed = bForceOriginalIsLossy;
		msDstNameL = msClipName;
		msDstNameR = msClipName;
		mbStereo = (eStereoBehavior > geStereoDoAsk);
		miBitWidth = 0;
	}
	else {
		mpfSrc = IFile::Create();
		if (mpfSrc == NULL) {
			msExtendedError = std::string("IFile::Create() => NULL for '") + msClipName + "' (out of memory?).";
			return false;
		}
		if (!mpfSrc->Open(pszSrcPath, IFile::FileRead)) {
			msExtendedError = std::string("Can't open file '") + msClipName + "'.";
			return false;
		}
		
		tchar pszErrMsgBuff[1024];
		*pszErrMsgBuff = '\0';
		ac::IDecoder* pDecoder = ac::IDecoder::Create(mpfSrc, pszErrMsgBuff, 1024);
		if (!pDecoder) {
			if (*pszErrMsgBuff != '\0')
				msExtendedError = pszErrMsgBuff;
			else
				msExtendedError = std::string("Unknown format file '") + msClipName + "'.";
			return false;
		}
		// We have to do a TestFile here - even though it will fail if it causes mp3
		// LAME engine to be invoked from two different threads
		if (!pDecoder->TestFile(mpfSrc)) {
			tchar pszErrMsg[1024];
			pDecoder->GetErrMsg(pszErrMsg, 1024, true);
			msExtendedError = std::string("Error testing file: ") + pszErrMsg;
			return false;
		}
		meCodec = pDecoder->GetAudioCodec();
		mbSrcLossyCompressed = pDecoder->mbIsLossyCompression;
		meSrcQuality = pDecoder->meLowestInputQuality;
		msDstNameL = msClipName;
		msDstNameR = msClipName;
		mbStereo = (pDecoder->miOutputChannels > 1);
		if (mbSrcLossyCompressed)
			miBitWidth = 24; //pDecoder->miOutputBitWidth;
		else
			miBitWidth = (meSrcQuality == ac::geQualityLossless16) ? 16 : 24;
		// First close decoder, so it won't crash later
		pDecoder->Destroy();
		pDecoder = NULL;
	}
	
	mbSplit = false;
	if (mbStereo) {
		msDstNameL += "-1";
		msDstNameR += "-2";
		
		if (eStereoBehavior == geStereoDoSplit)
			mbSplit = true;
		else if (eStereoBehavior == geStereoDoKeep)
			mbSplit = false;
		else {
			tchar pszMsg[1024];
			sprintf(
					pszMsg,
					"Importing clip '%s'\n"
					"\n"
					"This is a stereo file. Do you want to use it like that?\n"
					"\n"
					" Stereo:\tImport as a stereo stream\n"
					" Split:\tSplit file into two mono streams\n"
					" Cancel:\tDon't import file(s) anyway",
					msClipName.c_str()
					);
			
			ge::IWindow::EMsgBoxReturn eRet;
			eRet = ge::IWindow::ShowMessageBox(pszMsg, "Stereo file", ge::IWindow::MsgBoxYesNoCancel);
			switch (eRet) {
				case ge::IWindow::MsgBoxRetYes:
					mbSplit = false;
					break;
				case ge::IWindow::MsgBoxRetNo:
					mbSplit = true;
					break;
				case ge::IWindow::MsgBoxRetCancel: 
					// User aborted - not an error
					return false; // true;
			}
		}
	}
	// This can take some time - so display hour-glass cursor
	//CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
	
	msDstPathNameL = sTargetPath;
	msDstPathNameR = sTargetPath;
	
	/*
	// Build target path name
	if (bDoesWaveAlreadyExist) {
		msDstPathNameL = gpApplication->GetFromWaveName_ClipWave_Safe(msDstNameL.c_str());
		msDstPathNameR = gpApplication->GetFromWaveName_ClipWave_Safe(msDstNameR.c_str());
	}
	else {
		if (mbSrcLossyCompressed) {
			msDstPathNameL = gpApplication->GetFromWaveName_ClipWaveDecomp(msDstNameL.c_str());
			msDstPathNameR = gpApplication->GetFromWaveName_ClipWaveDecomp(msDstNameR.c_str());
		}
		else {
			msDstPathNameL = gpApplication->GetFromWaveName_ClipWave(msDstNameL.c_str());
			msDstPathNameR = gpApplication->GetFromWaveName_ClipWave(msDstNameR.c_str());
		}
	}*/
	
	return true;
} // Init


void CImportAudioTask::Init_InsertAsRegionAfterImport(tint32 iTrackID, tint64 iTrackPos)
{
	if (iTrackID >= 0) {
		miRegion_TrackID = iTrackID;
		miRegion_TrackPos = iTrackPos;
		mbDeleteNonLossyOriginal = true;
	}
} // Init_InsertAsRegionAfterImport


tbool CImportAudioTask::Open()
{
	tbool bOpenSuccess = false;

	if (mbDstIsAlreadyThere) {
		return true;
	}

	// Verify that file hasn't already been imported
	if (gpApplication->IsClipNameInUse(msClipName.c_str(), msDstNameL.c_str(), msDstNameR.c_str(), &msExtendedError)) {
		return false;
	}

	mpDecoder = ac::IDecoder::Create(mpfSrc);

	// Kill any previous wave file(s) - ignore error
	IFile::DeleteFile(msDstPathNameL.c_str());
	IFile::DeleteFile(msDstPathNameR.c_str());

	// Decode file into project
	//tbool bDecodeSuccess = false;
	mpfDstL = IFile::Create();
	mpfDstR = IFile::Create();

	if (mpfDstL->Open(msDstPathNameL.c_str(), IFile::FileCreate)) {
		if ((!mbStereo) || (mpfDstR->Open(msDstPathNameR.c_str(), IFile::FileCreate))) {
			if (!mbStereo) {
				if (mpDecoder->Init(mpfDstL, true, miBitWidth, 1, NULL)) {
					bOpenSuccess = true;
				}
			}
			else {
				if (mpDecoder->Init(mpfDstL, true, miBitWidth, 1, mpfDstR)) {
					bOpenSuccess = true;
				}
			}
		}
	}

	if (bOpenSuccess) {
		mpfSrc->Seek(0);
		miDecodeIx = 0;
		miAudioImportOrder = geAudioImport_Start;
	}

	return bOpenSuccess;
} // Open


tbool CImportAudioTask::DoWork()
{
	tbool bWorkError = false;

	switch (miAudioImportOrder) {
		case geAudioImport_Start:
			{
				if (IsOpened()) {
					// That's bad
					bWorkError = true;
				}
				else {
					if (!Open())
						bWorkError = true;
					else {
						if (mbDstIsAlreadyThere) {
							miAudioImportOrder = geAudioImport_Copy_After;
							miAudioImportOrder++;
						}
						else {
							miAudioImportOrder++;
						}
					}
				}
			}
			break;

		case geAudioImport_Decode_Before: 
			{
				if (mbSrcLossyCompressed)
					msProgress = "Decoding";
				else
					msProgress = "Importing";
				msProgress += std::string(" '") + msClipName + "'";
				muiProgressIx = 0;
				muiProgressTarget = mpfSrc->GetSizeWhenOpened();

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Decode_Action:
			{
				tint32 iSamplesProcessed = 0;
				bWorkError = !mpDecoder->Process(mpfSrc, miDecodeIx, 128 * 1024, &iSamplesProcessed);
				if (!bWorkError) {
					miDecodeIx += iSamplesProcessed;
					muiProgressIx = mpfSrc->GetCurrentFilePosition();
					if (iSamplesProcessed == 0) {
						// No more samples - go on to next task
						miAudioImportOrder++;
					}
				}
				// Close decoder?
				if ((bWorkError) || (miAudioImportOrder > geAudioImport_Decode_Action)) {
					if (!mpDecoder->Finalize()) {
						bWorkError = true;
					}
				}
				// Report error?
				if (bWorkError) {
					tchar pszErrMsg[1024];
					mpDecoder->GetErrMsg(pszErrMsg, 1024, true);
					msExtendedError = std::string("Error importing audio: ") + pszErrMsg;
				}
			}
			break;

		case geAudioImport_Decode_After:
			{
				msProgress = "Decode done";
				muiProgressIx = muiProgressTarget = 1;

				if ((mbSrcLossyCompressed) && (mbCopyLossyOriginal))
					miAudioImportOrder++;
				else {
					miAudioImportOrder = geAudioImport_Copy_After;
					miAudioImportOrder++;
				}
			}
			break;

		case geAudioImport_Copy_Before:
			{
				msProgress = "Importing compressed original";
				muiProgressIx = 0;
				muiProgressTarget = mpfSrc->GetSizeWhenOpened();

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Copy_Action:
			{
				std::string sCompressedDest = gpApplication->GetProjDir_Clips();
				if (stricmp(sCompressedDest.c_str(), msPathOnly.c_str()) == 0) {
					// Already inside bundle - can't copy onto itself

					// Go to next task
					miAudioImportOrder++;
				}
				else {
					msDstPathName_ForCopy = sCompressedDest + msClipName + msExt;
					IFile::DeleteFile(msDstPathName_ForCopy.c_str());
					if (IFile::CopyFile(sCompressedDest.c_str(), msPathOnly.c_str(), (msClipName + msExt).c_str())) {
						// Go to next task
						miAudioImportOrder++;
					}
					else {
						msExtendedError = "Compressed file copy operation failed.";
						bWorkError = true;
					}
				}
			}
			break;

		case geAudioImport_Copy_After:
			{
				msProgress = "Copying done";
				muiProgressIx = muiProgressTarget = 1;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_Before:
			{
				msProgress = "Creating/verifying peak files";
				muiProgressIx = 0;
				muiProgressTarget = (mpfSrc) ? mpfSrc->GetSizeWhenOpened() : 1;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_Action:
			{
				tbool bForceCreate = !mbDstIsAlreadyThere;
				gpApplication->VerifyCreatePeakFiles(msDstPathNameL.c_str(), mbStereo ? msDstPathNameR.c_str() : "", bForceCreate);

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_After:
			{
				if (mpDecoder) {
					mpDecoder->Destroy();
					mpDecoder = NULL;
				}

				gpApplication->AddClipToList(this);

				msProgress = "Peak files done";
				muiProgressIx = muiProgressTarget = 1;

				if (miRegion_TrackID >= 0)
					miAudioImportOrder++;
				else {
					// Done
					miAudioImportOrder = geAudioImport_Region_After;
					miAudioImportOrder++;
				}
			}
			break;

		case geAudioImport_Region_Before:
			{
				msProgress = "Inserting as region";
				muiProgressIx = 0;
				muiProgressTarget = 2;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Region_Action:
			{
				muiProgressIx++;

				gpDSPEngine->CreateRegion(msClipName, miRegion_TrackID, miRegion_TrackPos, 0);
				if (!mbSrcLossyCompressed) {
					if (mbDeleteNonLossyOriginal) {
						if (mpfSrc) {
							mpfSrc->Destroy();
							mpfSrc = NULL;
						}
						IFile::DeleteFile(msSrcPathName.c_str());
					}
				}

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Region_After:
			{
				muiProgressIx = muiProgressTarget;

				miAudioImportOrder++;
			}
			break;

		default:
			// That's weird - what are we doing here?
			bWorkError = true;
			break;

	}

	if (bWorkError) {
		miAudioImportOrder = geAudioImport_Done;
	}
	return !bWorkError;
} // DoWork


tbool CImportAudioTask::IsDone()
{
	return (miAudioImportOrder >= geAudioImport_Done);
} // IsDone
