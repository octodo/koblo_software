
#include "KSOS.h"

#ifdef _DEBUG
const tint32 kiPortionSize = 128 * 1024;
#else _DEBUG
const tint32 kiPortionSize = 1024 * 1024;
#endif // _DEBUG

CImportAudioTask::CImportAudioTask() {
	
	
	meCodec = ac::geAudioCodecUndefined;
	
	mpSrc_File = mpAudio_File_L = mpAudio_File_R = NULL;
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
	
	if (mpSrc_File) {
		mpSrc_File->Destroy();
		mpSrc_File = NULL;
	}

	if (mpAudio_File_L) {
		mpAudio_File_L->Destroy();
		
		mpAudio_File_L = NULL;
		
		/*
		if ((bWasAborted) && (NameL().length())) {
			IFile::DeleteFile(NameL().c_str());
		}
		 */
	}
	if (mpAudio_File_R) {
		mpAudio_File_R->Destroy();
		mpAudio_File_R = NULL;
		
		/*
		if ((bWasAborted) && (NameR().length())) {
			IFile::DeleteFile(NameR().c_str());
		}
		 */
	}
} // destructor


void CImportAudioTask::Destroy()
{
	delete dynamic_cast<CImportAudioTask*>(this);
} // Destroy
/*
std::string Get_File_Name_From_Path(std::string sFile_Path)
{
	// Extract the file name from path
	tint iPos = sFile_Path.find_last_of(":");
	return  sFile_Path.substr(iPos + 1);
}
 */
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
tbool CImportAudioTask::Init(std::string sSource_Path, tbool bDoesWaveAlreadyExist , EStereoBehavior eStereoBehavior , tbool bForceOriginalIsLossy )
{
	// prepare all the different filenames and paths
	if( mFile_Item.Import( sSource_Path ) == false)
	   return false; 
	
//	sprintf(" Source path called from CImportAudioTask::Init %s \n", mFile_Item.Source_Path().c_str());
	
	std::string sDebug = mFile_Item.Source_Path();
	
	// Build target path
//	std::string sOldTargetPath = gpApplication->GetProjDir_Clips();
	
	
	// Determine file format and number of channels
	/*
	if (bDoesWaveAlreadyExist) {
		
		mbDstIsAlreadyThere = true;
		meCodec = ac::geAudioCodecWave;
		meSrcQuality = ac::geQualityUnknown;
		mbSrcLossyCompressed = bForceOriginalIsLossy;
		mFile_Item.Stereo(eStereoBehavior > geStereoDoAsk);
		miBitWidth = 0;
	}
	 */
	if(true) {
		mpSrc_File = IFile::Create();
		if (mpSrc_File == NULL) {
			msExtendedError = std::string("IFile::Create() => NULL for '") + mFile_Item.Name() + "' (out of memory?).";
			return false;
		}
		if (!mpSrc_File->Open(mFile_Item.Source_Path().c_str(), IFile::FileRead)) {
			msExtendedError = std::string("Can't open file '") + mFile_Item.Name() + "'.";
			return false;
		}
		
		tchar pszErrMsgBuff[1024];
		*pszErrMsgBuff = '\0';
		ac::IDecoder* pDecoder = ac::IDecoder::Create(mpSrc_File, pszErrMsgBuff, 1024);
		if (!pDecoder) {
			if (*pszErrMsgBuff != '\0')
				msExtendedError = pszErrMsgBuff;
			else
				msExtendedError = std::string("Unknown format file '") + mFile_Item.Name() + "'.";
			return false;
		}
		// We have to do a TestFile here - even though it will fail if it causes mp3
		// LAME engine to be invoked from two different threads
		if (!pDecoder->TestFile(mpSrc_File)) {
			tchar pszErrMsg[1024];
			pDecoder->GetErrMsg(pszErrMsg, 1024, true);
			msExtendedError = std::string("Error testing file: ") + pszErrMsg;
			return false;
		}
		meCodec = pDecoder->GetAudioCodec();
		mbSrcLossyCompressed = pDecoder->mbIsLossyCompression;
		meSrcQuality = pDecoder->meLowestInputQuality;
//		msDstNameL = msClipName;
//		msDstNameR = msClipName;
		mFile_Item.Stereo(pDecoder->miLastInputChannels == 2); //
		if (mbSrcLossyCompressed)
			miBitWidth = 24; //pDecoder->miOutputBitWidth;
		else
			miBitWidth = (meSrcQuality == ac::geQualityLossless16) ? 16 : 24;
		// First close decoder, so it won't crash later
		pDecoder->Destroy();
		pDecoder = NULL;
	}
	
	mbSplit = false;
	/*
	if (mbStereo) {
//		msDstNameL += "-1";
//		msDstNameR += "-2";
		
		
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
					mFile_Item.Name().c_str()
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
	 */
	// This can take some time - so display hour-glass cursor
	//CAutoDelete<ge::IWaitCursor> pWaitCursor(ge::IWaitCursor::Create());
/* 
	
//	std::string sTaget_FileL = Source_File();
//	std::string sTaget_FileR = Source_File();
	
	
	
	


	
	// Build target path name
	if (bDoesWaveAlreadyExist) {
		sTaget_FileL = gpApplication->GetFromWaveName_ClipWave_Safe(sTaget_FileL.c_str());
		sTaget_FileR = gpApplication->GetFromWaveName_ClipWave_Safe(sTaget_FileR.c_str());
	}
	else {
		if (mbSrcLossyCompressed) {
			sTaget_FileL = gpApplication->GetFromWaveName_ClipWaveDecomp(sTaget_FileL.c_str());
			sTaget_FileR = gpApplication->GetFromWaveName_ClipWaveDecomp(sTaget_FileR.c_str());
		}
		else {
			sTaget_FileL = gpApplication->GetFromWaveName_ClipWave(sTaget_FileL.c_str());
			sTaget_FileR = gpApplication->GetFromWaveName_ClipWave(sTaget_FileR.c_str());
		}
	}
	
	NameL(sTaget_FileL);
	NameR(sTaget_FileR);
	
	printf("sTaget_FileL is : \%s \n", NameL().c_str());
	printf("sTaget_FileR is : \%s \n", NameR().c_str());
	
*/	
	
	return true;
} // Init

/*
void CImportAudioTask::Init_InsertAsRegionAfterImport(tint32 iTrackID, tint64 iTrackPos)
{
	if (iTrackID >= 0) {
		miRegion_TrackID = iTrackID;
		miRegion_TrackPos = iTrackPos;
		mbDeleteNonLossyOriginal = true;
	}
} // Init_InsertAsRegionAfterImport
*/

tbool CImportAudioTask::Open()
{
	tbool bOpenSuccess = false;

	
	if (mbDstIsAlreadyThere) {
		return true;
	}
	 
	
	if (gpApplication->Take_Is_In_Use(mFile_Item.Name()) ) return false;

/*
	// Verify that file hasn't already been imported
	if (gpApplication->IsClipNameInUse(msClipName.c_str(), msDstNameL.c_str(), msDstNameR.c_str(), &msExtendedError)) {
		return false;
	}
*/
	mpDecoder = ac::IDecoder::Create(mpSrc_File);

	/*
	// Kill any previous wave file(s) - ignore error
	IFile::DeleteFile(mFile_Item.Name().c_str());
	IFile::DeleteFile(NameR().c_str());
	 */

	// Decode file into project
	//tbool bDecodeSuccess = false;
	mpAudio_File_L = IFile::Create();
	mpAudio_File_R = IFile::Create();
	
	printf("pathname is : \%s \n", mFile_Item.Path().c_str() );

	std::string sLeft_File_Name = mFile_Item.Destination_Path() + mFile_Item.Name() + "-1.wav";
	std::string sRight_File_Name = mFile_Item.Destination_Path() + mFile_Item.Name() + "-2.wav";
	
	
	

	
	
	
	if (mpAudio_File_L->Open(sLeft_File_Name.c_str(), IFile::FileCreate)) {
		if ((!mFile_Item.Stereo()) || (mpAudio_File_R->Open(sRight_File_Name.c_str(), IFile::FileCreate))) {
			if (!mFile_Item.Stereo()) {
				if (mpDecoder->Init(mpAudio_File_L, true, miBitWidth, 1, NULL)) {
					bOpenSuccess = true;
				}
			}
			else {
				if (mpDecoder->Init(mpAudio_File_L, true, miBitWidth, 1, mpAudio_File_R)) {
					bOpenSuccess = true;
				}
			}
		}
	}
	 

	if (bOpenSuccess) {
		mpSrc_File->Seek(0);
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
				msProgress += std::string(" '") + mFile_Item.Name() + "'";
				muiProgressIx = 0;
				muiProgressTarget = mpSrc_File->GetSizeWhenOpened();

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Decode_Action:
			{
				tint32 iSamplesProcessed = 0;
				bWorkError = !mpDecoder->Process(mpSrc_File, miDecodeIx, 128 * 1024, &iSamplesProcessed);
				if (!bWorkError) {
					miDecodeIx += iSamplesProcessed;
					muiProgressIx = mpSrc_File->GetCurrentFilePosition();
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
				muiProgressTarget = mpSrc_File->GetSizeWhenOpened();

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Copy_Action:
			{
				/*
			//	std::string sCompressedDest = gpApplication->GetProjDir_Clips();
				if (stricmp(mFile_Item.Destination_Path().c_str(), mFile_Item.c_str()) == 0) {
					// Already inside bundle - can't copy onto itself

					// Go to next task
					miAudioImportOrder++;
				}
				else
				 */{
				//	msDstPathName_ForCopy = sCompressedDest + msClipName + msExt;
					IFile::DeleteFile(mFile_Item.Destination_Path().c_str());
					if (IFile::CopyFile(mFile_Item.Destination_Path().c_str(), mFile_Item.Path().c_str(), (mFile_Item.Name() + mFile_Item.Extencion()).c_str())) {
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
				muiProgressTarget = (mFile_Item.Name().size()) ? mpSrc_File->GetSizeWhenOpened() : 1;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_Action:
			{
				tbool bForceCreate = !mbDstIsAlreadyThere;
				
				//gpApplication->VerifyCreatePeakFiles(mFile_Item.Left_Peak_File_Name().c_str(), mFile_Item.Stereo() ? mFile_Item.Right_Peak_File_Name().c_str() : "", bForceCreate);
				gpApplication->VerifyCreatePeakFiles( &mFile_Item , bForceCreate);

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
/*
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
*/			break;

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
