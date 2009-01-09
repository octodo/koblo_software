

#include "KSOS.h"

CRegion_Controller::CRegion_Controller()
{
	
}

CRegion_Controller::~CRegion_Controller()
{
	
}

tint32 CRegion_Controller::Create_Region(const std::string& sSoundListItemName, 
 tint32 iTrack, 
 tuint64 uiTrack_Pos, 
 tuint64 uiSample_Start, 
 tuint64 uiSample_Duration,
 tuint64 uiFade_In_Duration,
 tuint64 uiFade_Out_Duration,
 tfloat32 fVolume)
{
	static int iID = 0;
	tint32 iRegionID = iID++;
 
	// DSP
	gpDSPEngine->GetTrack(iTrack)->CreateRegion(iRegionID, 
												sSoundListItemName, 
												uiTrack_Pos, 
												uiSample_Start, 
												uiSample_Duration,
												uiFade_In_Duration,
												uiFade_Out_Duration,
												fVolume);
 
	// GUI		
	mpRegionCallback->InsertRegion(iRegionID, iTrack, uiTrack_Pos, uiSample_Start, uiSample_Duration, sSoundListItemName.c_str());
	mpRegionCallback->SelectRegion(iRegionID, iTrack);
	gpDSPEngine->Select_Region(iRegionID);
 
	gpDSPEngine->Sanitize_Session_End_Sample();	
 
	return iRegionID;
}
 
void  CRegion_Controller::Get_Region_Info(SRegionInfo& RegionInfo, tuint32 uiRegionID)
{
	CDSP::RegionSearchHandle Search = gpDSPEngine->GetFirstRegion(RegionInfo);
	while ((Search != gpDSPEngine->mRegionSearchEnd) &&
		   (RegionInfo.uiRegionID != uiRegionID)) {
		gpDSPEngine->GetNextRegion(Search, RegionInfo);
	}
	
	if (Search == gpDSPEngine->mRegionSearchEnd) {
		// We didn't find a matching region. Make sure we don't crash
		RegionInfo.uiRegionID = 0;
		RegionInfo.uiTrack = 0;
		RegionInfo.uiStartPos = 0;
		RegionInfo.uiEndPos = 0;
		return;
	}
	gpDSPEngine->FinishRegionSearch(Search);
	
}

void CRegion_Controller::Set_Region_Volume(tuint32 uiRegionID, tfloat32 fVolume)
{ 
	Prepare(uiRegionID);
	mpRegion_DSP->Set_Volume(fVolume);	
	
}

void CRegion_Controller::Cut_Region(tuint32 uiTrack, tuint32 uiRegionID, tuint64 uiCutPos)
{ 

	Prepare(uiRegionID);
	
	std::string sClipName				=	mpRegion_DSP->GetSoundListItemName();
	tint64 iRegion_Pos					=	mpTrack_DSP->GetRegionPosOnTrack(uiRegionID);	
	tint64 iSample_Offset				=	mpRegion_DSP->Get_Sample_Offset();
	tint64 iSample_Duration				=	mpRegion_DSP->GetDuration();
	tuint64 iFade_Out_Duration			=	mpRegion_DSP->Get_Fade_Out_Duration();
	tfloat32 fVolume					=	mpRegion_DSP->Get_Volume();
	
	// Trim end of curent region
	mpTrack_DSP->Trim_Region(uiRegionID, iRegion_Pos, iSample_Offset,  uiCutPos);
	mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	
	
	iSample_Offset		+=	uiCutPos;
	iRegion_Pos			+=	uiCutPos;
	iSample_Duration	-=	uiCutPos;
	
	uiRegionID = Create_Region(sClipName, 
							   uiTrack, 
							   iRegion_Pos, 
							   iSample_Offset, 
							   iSample_Duration,
							   0, 
							   iFade_Out_Duration,
							   fVolume);
	/*
	 Create_Region(const std::string& sSoundListItemName, 
	 tint32 iChannel, 
	 tuint64 uiTrack_Pos,
	 tuint64 uiSample_Offset, 
	 tuint64 uiSample_Duration = -1,
	 tuint64 uiFade_In_Duration = 0,
	 tuint64 uiFade_Out_Duration = 0,
	 tfloat32 fVolume = 1.0);
*/	 
	
	
	
	
}

void CRegion_Controller::TrimRegion(tuint32 uiTrack, tuint32 uiRegionID, tbool bStart, tint64 iSamplePos)
{ 

	Prepare(uiRegionID);
	
	std::string sClipName				=	mpRegion_DSP->GetSoundListItemName();
	tint64 iTrackStartPos				=	mpTrack_DSP->GetRegionPosOnTrack(uiRegionID);	
	tint64 iSample_Duration				=	mpRegion_DSP->GetDuration();
	tint64 iSample_Offset				=	mpRegion_DSP->Get_Sample_Offset();
//	tint64 iSampleEnd					=	mpRegion_DSP->GetDuration() + iSample_Offset - 1;
//	tint64 iSoundDuration				=	mpRegion_DSP->GetSound(0)->GetLength();
	
	//--------------------------------------
	// Trim start
	if(bStart){
		
		 
		iSamplePos
		
		mpTrack_DSP->Trim_Region(uiRegionID, iTrackStartPos+iSamplePos, iSample_Offset+iSamplePos, iSample_Duration-iSamplePos);
		
		// Update fade in
	//	Fade_In(uiRegionID, mpRegion_DSP->Get_Fade_In_Duration());
	}
	else{
	//--------------------------------------
	// Trim end-
		
		iSample_Duration = iSamplePos;
		mpTrack_DSP->Trim_Region(uiRegionID, iTrackStartPos, iSample_Offset, iSample_Duration);
		
	
	}
	
	mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	
}

void CRegion_Controller::DuplicateRegion()
{ 
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		
		STrackSelectionInfo pTrack_Selection_Info	=	gpDSPEngine->GetTrackSelection(uiTrack);
		
		if(pTrack_Selection_Info.uiSelection_Type = giSelect_Region &&  pTrack_Selection_Info.iRegionID != -1)
		{
			tuint32 uiRegionID = pTrack_Selection_Info.iRegionID ;
			
			SRegionInfo RegionInfo;
			Get_Region_Info( RegionInfo, uiRegionID);
			CTrack_DSP*		pTrack_DSP			=	gpDSPEngine->GetTrack(RegionInfo.uiTrack);
			CRegion_DSP*	pRegion_DSP			=	pTrack_DSP->GetRegion_DSP(uiRegionID);
			std::string sClipName				=	pRegion_DSP->GetSoundListItemName();
			tint64 iPossition					=	pTrack_DSP->GetRegionPosOnTrack(uiRegionID);	
			tint64 iSample_Duration				=	pRegion_DSP->GetDuration();
			tint64 iSample_Offset				=	pRegion_DSP->Get_Sample_Offset();
			
			// Create new region
			tuint32 uiTrack_Pos					=	iPossition + iSample_Duration -1;
			tuint32 muiRegionID = Create_Region(sClipName, uiTrack, uiTrack_Pos , iSample_Offset, iSample_Duration);
			gpDSPEngine->Select_Region(muiRegionID);
			mpRegionCallback->SelectRegion(muiRegionID,uiTrack);
			return;
		}
	}
}

void CRegion_Controller::NormaliseRegion()
{ 
	
	
}

void CRegion_Controller::InverseRegion()
{ 
	
	
}

tuint64 CRegion_Controller::Fade_In(tuint32 uiRegionID, tuint64 uiFadeInLength)
{ 
	Prepare(uiRegionID);
	tuint64 iSample_Duration			=	mpRegion_DSP->GetDuration();
	tuint64 uiFadeOutLength				=	mpRegion_DSP->Get_Fade_Out_Duration();
	
	if(uiFadeInLength > iSample_Duration)
		uiFadeInLength = iSample_Duration;
	
	if( uiFadeInLength + uiFadeOutLength > iSample_Duration){
		uiFadeOutLength = iSample_Duration - uiFadeInLength;
		mpRegion_DSP->Set_Fade_Out_Duration(uiFadeOutLength);
	}
	
	
	mpRegion_DSP->Set_Fade_In_Duration(uiFadeInLength);
	return uiFadeInLength;
	
}

tuint64 CRegion_Controller::Fade_Out(tuint32 uiRegionID, tuint64 uiFadeOutLength)
{ 
	Prepare(uiRegionID);
	tuint64 iSample_Duration				=	mpRegion_DSP->GetDuration();
	tuint64 uiFadeInLength				=	mpRegion_DSP->Get_Fade_In_Duration();
	
	if(uiFadeOutLength > iSample_Duration)
		uiFadeOutLength = iSample_Duration;
	
	if(uiFadeOutLength + uiFadeInLength > iSample_Duration){
		uiFadeInLength = iSample_Duration - uiFadeOutLength;
		mpRegion_DSP->Set_Fade_In_Duration(uiFadeInLength);
	}
	
	mpRegion_DSP->Set_Fade_Out_Duration(uiFadeOutLength);
	
	return uiFadeOutLength;
}

void CRegion_Controller::Move_Region(tuint32 uiRegionID, tint32 iChannelNew, tuint64 uiTrackPosStartNew)
{
	
	Prepare(uiRegionID);
	std::string sClipName				=	mpRegion_DSP->GetSoundListItemName();
	tuint64 iSample_Offset				=	mpRegion_DSP->Get_Sample_Offset();
	tuint64 iSample_Duration				=	mpRegion_DSP->GetDuration();
//	tuint64 uiSamplePosEnd				=	iSample_Offset + iSample_Duration-1;
	tuint64 uiFadeInLength				=	mpRegion_DSP->Get_Fade_In_Duration();
	tuint64 uiFadeOutLength				=	mpRegion_DSP->Get_Fade_Out_Duration();
	tfloat32 fRegionVolume				=	mpRegion_DSP->Get_Volume();
	
	gpDSPEngine->Delete_Region(iChannelNew, uiRegionID);
	
	uiRegionID = Create_Region(sClipName, 
							  iChannelNew, 
							  uiTrackPosStartNew, 
							  iSample_Offset, 
							  iSample_Duration,
							  uiFadeInLength, 
							  uiFadeOutLength,
							  fRegionVolume);
	gpDSPEngine->Deselect_All_Tracks();
	gpDSPEngine->Select_Region(uiRegionID);
}



tbool CRegion_Controller::SaveTrackRegionDataToChunk(tint32 iTrack, IChunk* pChunk)
{
	CTrack_DSP*		pTrack_DSP			=	gpDSPEngine->GetTrack(iTrack);
	std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator it = pTrack_DSP->GetRegionList().begin();
	std::string sRc = "";
	
#ifdef _WIN32
	const tchar pszUI64Format[] = "%I64u|";
#else _WIN32
	const tchar pszUI64Format[] = "%llu|";
#endif // #else _WIN32
	
	const tint32 iSaveVersion = 2;
	if (iSaveVersion == 1) {
		sRc = "0001|"; // Version
		for (; it != pTrack_DSP->GetRegionList().end(); it++) {
			CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;
			CRegion_DSP* pRegion_DSP = pRegionInfo->pRegion;
			
			tint64 iSample_Offset = pRegion_DSP->Get_Sample_Offset();
			tuint64 iSample_Duration = pRegion_DSP->GetDuration();
			std::string sPath = pRegion_DSP->GetSound(0)->GetPathName();
			tchar pszPathOnly_Dummy[1024];
			tchar pszNameOnly[1024];
			if (IFile::SplitPathToPathAndName(sPath.c_str(), pszPathOnly_Dummy, pszNameOnly)) {
				tchar* pszLastDot = strrchr(pszNameOnly, '.');
				if (pszLastDot) {
					// Remove extension
					*pszLastDot = '\0';
				}
				
				tchar pszBuff[1024];
				tchar* psz = pszBuff;
				//psz += sprintf(psz, "%u|", uiID);uiTrackPosStart
				psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrackPosStart);
				psz += sprintf(psz, pszUI64Format, iSample_Offset);
				psz += sprintf(psz, pszUI64Format, iSample_Duration);
				psz += sprintf(psz, "%s|", pszNameOnly);
				
				sRc += pszBuff;
			}
		}
		
		// Delete ending '|' char
		sRc.erase(sRc.length() - 1);
	}
	else if (iSaveVersion == 2) {
		//sRc = "0002|4|"; // Version number and record length
		sRc = "0002|7|"; // Version number and record length
		for (; it != pTrack_DSP->GetRegionList().end(); it++) {
			CTrack_DSP::SChannelRegionInfo* pRegionInfo = *it;
			CRegion_DSP* pRegion_DSP = pRegionInfo->pRegion;
			const tchar* pszListItemName = pRegion_DSP->GetSoundListItemName();
			tuint64 iSample_Offset = pRegion_DSP->Get_Sample_Offset();
			tuint64 iSample_Duration = pRegion_DSP->GetDuration();
			//
			tuint64 uiFadeIn = pRegion_DSP->Get_Fade_In_Duration();
			tuint64 uiFadeOut = pRegion_DSP->Get_Fade_Out_Duration();
			tfloat32 fVolume = pRegion_DSP->Get_Volume();
			tuint64 uiVol1000000 = Float2Int(fVolume * (tuint64)1000000);
			
			tchar pszBuff[1024];
			tchar* psz = pszBuff;
			//psz += sprintf(psz, "%u|", uiID);uiTrackPosStart
			psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrackPosStart);
			psz += sprintf(psz, pszUI64Format, iSample_Offset);
			psz += sprintf(psz, pszUI64Format, iSample_Duration);
			psz += sprintf(psz, "%s|", pszListItemName);
			//
			psz += sprintf(psz, pszUI64Format, uiFadeIn);
			psz += sprintf(psz, pszUI64Format, uiFadeOut);
			psz += sprintf(psz, pszUI64Format, uiVol1000000);
			
			sRc += pszBuff;
		}
		
		// Delete ending '|' char
		sRc.erase(sRc.length() - 1);
	}
	
	pChunk->Write(sRc.c_str(), sRc.size());
	
	return true;
} // SaveTrackRegionDataToChunk


tbool CRegion_Controller::CreateRegionFromChunkData(tint32 iTrack, IChunk* pChunk)
{

	
	tchar* pszBuff = new tchar[pChunk->GetSize() + 1];
	pChunk->Read(pszBuff, pChunk->GetSize());
	pszBuff[pChunk->GetSize()] = '\0';
	
	std::list<std::string> asItems;
	PszEnumString2List(pszBuff, '|', asItems);
	std::list<std::string>::iterator it = asItems.begin();
	
#ifdef _WIN32
	const tchar pszUI64Format[] = "%I64u";
#else _WIN32
	const tchar pszUI64Format[] = "%llu";
#endif // #else _WIN32
	
	tint32 iItems = asItems.size();
	if (iItems > 0) {
		std::string sVersion = *it++;
		iItems--;
		tint32 iVersion = 0;
		sscanf(sVersion.c_str(), "%d", &iVersion);
		tint32 iItemsPerRecord = -1;
		if (iVersion > 1) {
			std::string sItem = *it++;
			sscanf(sItem.c_str(), "%d", &iItemsPerRecord);
		}
		switch (iVersion) {
			case 1:
			{
				iItemsPerRecord = 4;
				while (iItems >= iItemsPerRecord) {
					tuint64 uiTrackPosStart;
					// No support for trimming in version 1 saved files
					//tuint64 uiStart;
					//tuint64 uiEnd;
					
					std::string sItem = *it++;
					sscanf(sItem.c_str(), pszUI64Format, &uiTrackPosStart);
					sItem = *it++;
					//sscanf(sItem.c_str(), pszUI64Format, &uiStart);
					sItem = *it++;
					//sscanf(sItem.c_str(), pszUI64Format, &uiEnd);
					sItem = *it++;
					Create_Region(sItem.c_str(), iTrack, uiTrackPosStart, 0, (tuint64)-1);//, uiStart, uiEnd);
					
					iItems -= iItemsPerRecord;
				}
			}
				break;
				
			case 2:
			{
				if (iItemsPerRecord >= 4) {
					// How many items the current version of KS expects
					tint32 iItemsPerRecord_backCompat = 7;
					while (iItems >= iItemsPerRecord) {
						std::string sClipName;
						tuint64 uiTrackPosStart;
						tuint64 uiRegionStartPos;
						tuint64 uiRegionDuration;
						//
						tuint64			uiFadeIn		= 0;
						tuint64			uiFadeOut		= 0;
						tfloat32		fRegionVolume	= 1.0f;
						
						std::string sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiTrackPosStart);
						sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiRegionStartPos);
						sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiRegionDuration);
						sClipName = *it++;
					//	tuint64 uiRegionStopPos = uiRegionStartPos + uiRegionDuration - 1;
						//
						if (iItemsPerRecord >= 6) {
							sItem = *it++;
							sscanf(sItem.c_str(), pszUI64Format, &uiFadeIn);
							sItem = *it++;
							sscanf(sItem.c_str(), pszUI64Format, &uiFadeOut);
						}
						//
						if (iItemsPerRecord >= 7) {
							sItem = *it++;
							tuint64 uiVol1000000;
							sscanf(sItem.c_str(), pszUI64Format, &uiVol1000000);
							fRegionVolume = (tfloat32)(uiVol1000000 / 1000000.0);
						}
						//
						Create_Region(sClipName.c_str(), 
									  iTrack,
									  uiTrackPosStart,
									  uiRegionStartPos, 
									  uiRegionDuration,
									 uiFadeIn, 
									  uiFadeOut, 
									  fRegionVolume);
						//
						
						
						// Ignore extra items not supported yet (save-file from newer version of KS)
						while (iItemsPerRecord_backCompat < iItemsPerRecord) {
							iItemsPerRecord_backCompat++;
							it++;
						}
						
						iItems -= iItemsPerRecord;
					}
				} // >= 4
			}
				break;
				
			default:
				return false;
		}
	}
	
	return true;
} // CreateRegionFromChunkData

void CRegion_Controller::Refresh_Region_GUI(tint32 iRegion, tuint32 iTrack)
{	
	
	mpRegionCallback->Refresh_Region_GUI(iRegion, iTrack);
	
}

void CRegion_Controller::Delete_Region(tuint32 uiTrack, tuint32 uiRegionID)
{
	Prepare(uiRegionID);
	// Delete region on channel
	mpTrack_DSP->DeleteRegion(uiRegionID);
	// Find new end of song
	gpDSPEngine->Sanitize_Session_End_Sample();
	// Delete region on GUI
	Delete_Region_View(mRegionInfo.uiTrack, uiRegionID);
	
}

void CRegion_Controller::Delete_Region_View(tuint32 uiTrack, tuint32 uiRegionID)
{
	gpDSPEngine->Set_Track_Selection_Type(uiTrack, giSelect_Off );
	gpDSPEngine->Set_Track_Selection_Region( uiTrack, -1);
	mpRegionCallback->DeleteRegion(uiRegionID, uiTrack);
}

STrackSelectionInfo CRegion_Controller::Select_Region(tint32 iRegionID)
{ 
	Prepare(iRegionID);
	// Trim selection from start
	tuint64 uiStart_Sample				=	mpTrack_DSP->GetRegionPosOnTrack(iRegionID);	
	tint64 iSample_Duration					=	(mpRegion_DSP ? mpRegion_DSP->GetDuration() : 0);
	
	tint32 iNewSelectType = giSelect_Region;
	if (ge::IWindow::ShiftPressed()) {
		
		STrackSelectionInfo sinfo = gpDSPEngine->GetTrackSelection(miTrack);
		
		tint32 iOldType = sinfo.uiSelection_Type;
		
		if ((iOldType == giSelect_On_Track) || (iOldType == giSelect_Region)) {
			iNewSelectType = giSelect_On_Track;
			tuint64 uiOldStart = sinfo.uiSelection_Pos;
			if (uiOldStart < uiStart_Sample) {
				iSample_Duration += (uiStart_Sample - uiOldStart);
				uiStart_Sample = uiOldStart;
			}
			tuint64 uiOldDurat = sinfo.uiSelection_Duration;
			tuint64 uiOldFinal = uiOldStart + uiOldDurat - 1;
			tuint64 uiNewFinal = uiStart_Sample + iSample_Duration - 1;
			if (uiOldFinal > uiNewFinal) {
				iSample_Duration = uiOldFinal - uiStart_Sample + 1;
			}
		}
	}
	gpDSPEngine->SetTrackSelection(mRegionInfo.uiTrack,  uiStart_Sample,  iSample_Duration, iNewSelectType, iRegionID);
	
	return gpDSPEngine->GetTrackSelection(miTrack);
}

tuint64 CRegion_Controller::GetRegionSize(const std::string& sSoundPathName,tuint64 uiSamplePosStart, tint64 uiSamplePosEnd)
{
	CTrack_DSP*		pTrack_DSP			=	gpDSPEngine->GetTrack(0);
	tint64 iSize = pTrack_DSP->GetRegionSize(0, sSoundPathName, uiSamplePosStart, uiSamplePosEnd);
	return iSize;
}

tuint64 CRegion_Controller::GetRegionSize(tuint32 iRegionID )
{
	Prepare(iRegionID);	
	return	mpRegion_DSP->GetDuration();
}

SRegion_Drawing_Info CRegion_Controller::Get_Region_Drawing_Info(tuint iRegionID)
{
	SRegion_Drawing_Info info;
	
	Prepare(iRegionID);
	
	if (mpRegion_DSP) {
		info.uiSound_Start		=	mpRegion_DSP->Get_Sample_Offset();
		info.uiSound_Duration	=	mpRegion_DSP->GetDuration();
		info.uiTrack_Start		=	mRegionInfo.uiStartPos;
		info.uiFade_In			=	mpRegion_DSP->Get_Fade_In_Duration();
		info.uiFade_Out			=	mpRegion_DSP->Get_Fade_Out_Duration();
		info.fRegion_Volume		=	mpRegion_DSP->Get_Volume();
	}
	else {
		info.uiSound_Start		=	0;
		info.uiSound_Duration	=	0;
		info.uiTrack_Start		=	mRegionInfo.uiStartPos;
		info.uiFade_In			=	0;
		info.uiFade_Out			=	0;
		info.fRegion_Volume		=	0;
	}
	
	return info;
}

const tchar* CRegion_Controller::GetClipNameOfSelectedRegion()
{
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		
		STrackSelectionInfo pTrack_Selection_Info	=	gpDSPEngine->GetTrackSelection(uiTrack);
		
		if(pTrack_Selection_Info.uiSelection_Type = giSelect_Region &&  pTrack_Selection_Info.iRegionID != -1)
		{
			Prepare(pTrack_Selection_Info.iRegionID);
			return mpRegion_DSP->GetSoundListItemName();
		}
	}
	return "";
} 



void CRegion_Controller::GetRegionPeakFile(tuint32 uiRegionID, IFile** ppFile, tint32 iTrack, tint32 iSize)
{
//	SRegionInfo RegionInfo;
//	GetRegionInfo(RegionInfo, uiRegionID);
	Prepare( uiRegionID);	
	

	if (mpRegion_DSP)
		mpRegion_DSP->GetPeakFile(ppFile, iTrack, iSize);
	else
		*ppFile = NULL;
}


void CRegion_Controller::Prepare(tuint32 uiRegionID)
{

	Get_Region_Info( mRegionInfo, uiRegionID);
	mpTrack_DSP			=	gpDSPEngine->GetTrack(mRegionInfo.uiTrack);
	mpRegion_DSP		=	mpTrack_DSP->GetRegion_DSP(uiRegionID);
	miTrack				=	mRegionInfo.uiTrack;
	
}


