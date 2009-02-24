
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
	
	mRegion_Data.Track_ID( -1);
	
	/*
	
	miRegion_TrackID = -1;
	miRegion_TrackPos = -1;
	 */
	
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
	//	gpApplication->Takes_Decompressed();
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


tbool CImportAudioTask::Init(CKSFile_Item* pFile_Item )
{
	// prepare all the different filenames and paths
	if( mFile_Item.Import( pFile_Item->Source_Path() ) == false)
		return false; 
	
	mFile_Item.Set_UUID( pFile_Item->Get_UUID() );
	mFile_Item.Screen_Name(pFile_Item->Screen_Name());
	
	mpSrc_File = IFile::Create();
	if (mpSrc_File == NULL) {
		msExtendedError = std::string("IFile::Create() => NULL for '") + mFile_Item.Screen_Name() + "' (out of memory?).";
		return false;
	}
	if (!mpSrc_File->Open(pFile_Item->Source_Path().c_str(), IFile::FileRead)) {
		msExtendedError = std::string("Can't open file '") + mFile_Item.Screen_Name() + "'.";
		return false;
	}
	
	tchar pszErrMsgBuff[1024];
	*pszErrMsgBuff = '\0';
	ac::IDecoder* pDecoder = ac::IDecoder::Create(mpSrc_File, pszErrMsgBuff, 1024);
	if (!pDecoder) {
		if (*pszErrMsgBuff != '\0')
			msExtendedError = pszErrMsgBuff;
		else
			msExtendedError = std::string("Unknown format file '") + mFile_Item.Screen_Name() + "'.";
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
	
	mFile_Item.Stereo(pDecoder->miLastInputChannels == 2); //
	if (mbSrcLossyCompressed)
		miBitWidth = 24; //pDecoder->miOutputBitWidth;
	else
		miBitWidth = (meSrcQuality == ac::geQualityLossless16) ? 16 : 24;
	// First close decoder, so it won't crash later
	pDecoder->Destroy();
	pDecoder = NULL;
	
	
	//	mbSplit = false;
	
	return true;
} // Init

tbool CImportAudioTask::Init(CTake_Data* pTake_Data )
{
	mbDstIsAlreadyThere = true;
	mFile_Item.Set_UUID(pTake_Data->Get_UUID() );
	mFile_Item.Screen_Name(pTake_Data->Screen_Name() );

	
	CAutoDelete<IFile> pfile_L(IFile::Create());
	CAutoDelete<IFile> pfile_R(IFile::Create());
	tbool bStereo = pTake_Data->Channels() == 2;
	std::string s_L = pTake_Data->Left_Wave_File_Path();
	std::string s_R = pTake_Data->Right_Wave_File_Path();
	if (!pfile_L->Open(s_L.c_str(), IFile::FileRead)) {
		msExtendedError = std::string("Can't open file '") + s_L.c_str() + "'.";
		return false;
	}
	if (bStereo) {
		if (!pfile_R->Open(s_R.c_str(), IFile::FileRead)) {
			msExtendedError = std::string("Can't open file '") + s_R.c_str() + "'.";
			return false;
		}
	}
	
	tchar pszErrMsgBuff[1024];
	*pszErrMsgBuff = '\0';
	ac::IDecoder* pDecoder = ac::IDecoder::Create(mpSrc_File, pszErrMsgBuff, 1024);
	if (!pDecoder) {
		if (*pszErrMsgBuff != '\0')
			msExtendedError = pszErrMsgBuff;
		else
			msExtendedError = std::string("Unknown format file '") + mFile_Item.Screen_Name() + "'.";
		return false;
	}
	if (!pDecoder->TestFile(pfile_L)) {
		tchar pszErrMsg[1024];
		pDecoder->GetErrMsg(pszErrMsg, 1024, true);
		msExtendedError = std::string("Error testing file: ") + pszErrMsg;
		return false;
	}
	if (bStereo) {
		if (!pDecoder->TestFile(pfile_R)) {
			tchar pszErrMsg[1024];
			pDecoder->GetErrMsg(pszErrMsg, 1024, true);
			msExtendedError = std::string("Error testing file: ") + pszErrMsg;
			return false;
		}
	}
	meCodec = ac::geAudioCodecWave;
	mbSrcLossyCompressed = false;
	meSrcQuality = pDecoder->meLowestInputQuality;
	miBitWidth = (meSrcQuality == ac::geQualityLossless16) ? 16 : 24;
	
	mFile_Item.Stereo(bStereo); //

	// First close decoder, so it won't crash later
	pDecoder->Destroy();
	pDecoder = NULL;
		
	//	mbSplit = false;
	
	return true;
} // Init


void CImportAudioTask::Init_InsertAsRegionAfterImport(tint32 iTrackID, tint64 iTrackPos)
{
	if (iTrackID >= 0) {
		
		mRegion_Data.Track_ID(iTrackID);
		mRegion_Data.Possition(iTrackPos);
		//miRegion_TrackID = iTrackID;
		//miRegion_TrackPos = iTrackPos;
		mbDeleteNonLossyOriginal = true;
	}
} // Init_InsertAsRegionAfterImport

void CImportAudioTask::Init_InsertAsRegionAfterImport(CRegion_Data* pRegion_Data)
{
	mRegion_Data = *pRegion_Data;
	
} // Init_InsertAsRegionAfterImport


tbool CImportAudioTask::Open()
{
	tbool bOpenSuccess = false;

	
	if (mbDstIsAlreadyThere) {
		return true;
	}
	 
	
	if (gpApplication->Take_Is_In_Use(mFile_Item.Disk_Name()) ) return false;

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
	
//	printf("pathname is : \%s \n", mFile_Item.Path().c_str() );

	std::string sLeft_File_Name			= mFile_Item.Left_Path();
	std::string sRight_File_Name		= mFile_Item.Right_Path();
	
	
	

	
	
	
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
				msProgress += std::string(" '") + mFile_Item.Screen_Name() + "'";
				//msProgress += std::string(" '") + mFile_Item.Disk_Name() + "'";
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
					IFile::DeleteFile(gpApplication->Wave_File_Folder().c_str()); //!!! destination path????
					if (IFile::CopyFile(gpApplication->Wave_File_Folder().c_str(), mFile_Item.Path().c_str(), (mFile_Item.Get_UUID() + mFile_Item.Extencion()).c_str())) {
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

		case geAudioImport_Peak_Before: //7
			{
				
				msProgress = "Creating/verifying peak files";
				muiProgressIx = 0;
				muiProgressTarget = (mFile_Item.Disk_Name().size()) ? mpSrc_File->GetSizeWhenOpened() : 1;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_Action://8
			{
				tbool bForceCreate = !mbDstIsAlreadyThere;
				gpApplication->VerifyCreatePeakFiles( &mFile_Item , bForceCreate);
				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Peak_After://9
			{
				if (mpDecoder) {
					mpDecoder->Destroy();
					mpDecoder = NULL;
				}

				gpApplication->AddClipToList(this);

				msProgress = "Peak files done";
				muiProgressIx = muiProgressTarget = 1;
				
				if (mRegion_Data.Track_ID() >= 0)
					miAudioImportOrder++;
				else {
				 
					// Done
					miAudioImportOrder = geAudioImport_Region_After;
					miAudioImportOrder++;
				 }
			}
			break;
			

		case geAudioImport_Region_Before: //9
			{
				msProgress = "Inserting as region";
				muiProgressIx = 0;
				muiProgressTarget = 2;

				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Region_Action: //10
			{
				muiProgressIx++;

				if(mRegion_Data.Track_ID() != -1){
					gpDSPEngine->CreateRegion( mFile_Item.Screen_Name(),  mRegion_Data.Track_ID(),  mRegion_Data.Possition(),  0 );
				}
				//!!! fix this l8tr
				/*
				if (!mbSrcLossyCompressed) {
					if (mbDeleteNonLossyOriginal) {
						if (mpfSrc) {
							mpfSrc->Destroy();
							mpfSrc = NULL;
						}
					//	IFile::DeleteFile(msSrcPathName.c_str());
					}
				}
				*/
				miAudioImportOrder++;
			}
			break;

		case geAudioImport_Region_After://11
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
