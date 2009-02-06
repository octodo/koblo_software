

#include "KSOS.h"

CRegion_Controller::CRegion_Controller()
{
	for(tint32 i = 0; i< 64; i++)
		mpRegion_Clipboard[i].bEmpty =  true;
}

CRegion_Controller::~CRegion_Controller()
{
	
}

tint32 CRegion_Controller::CreateRegion(
const std::string& sSoundListItemName, 
 tint32 iTrack, 
 tuint64 uiTrack_Pos, 
 tuint64 uiSample_Offset, 
 tuint64 uiSample_Duration,
 tuint64 uiFade_In_Duration,
 tuint64 uiFade_Out_Duration,
 tfloat32 fVolume)
{
	static int iID = 0;
	tint32 iRegionID = iID++;
	
	CTrack_DSP* pTrack_DSP = gpDSPEngine->GetTrack(iTrack);
	pTrack_DSP->CreateRegion(iRegionID, 
							sSoundListItemName, 
							uiTrack_Pos, 
							uiSample_Offset, 
							uiSample_Duration,
							uiFade_In_Duration,
							uiFade_Out_Duration,
							fVolume);
 

	mpRegionCallback->InsertRegion(iRegionID, iTrack, uiTrack_Pos, uiSample_Offset, uiSample_Duration, sSoundListItemName.c_str());
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
	mpRegion_DSP->Volume(fVolume);	
	
}

void CRegion_Controller::Cut_Region(tuint32 uiTrack, tuint32 uiRegionID, tuint64 uiCutPos)
{ 

	Prepare(uiRegionID);
	
	std::string sSample_Name			=	mpRegion_DSP->Get_Sample_Name();
	tfloat64 uiTrack_Pos				=	mpTrack_DSP->Get_Region_Pos(uiRegionID);	
	tuint64 uiSample_Offset				=	mpRegion_DSP->Get_Sample_Offset();
	tuint64 uiSample_Duration			=	mpRegion_DSP->Get_Duration();

	tfloat32 fVolume					=	mpRegion_DSP->Volume();
	tuint64 uiFade_Out_Duration			=	mpRegion_DSP->Fade_Out_Duration();
	
	// trim end of curent region
	mpTrack_DSP->Resize_Region(uiRegionID, uiTrack_Pos, uiSample_Offset, uiCutPos-1);
	mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	
	// create new region
	
	CreateRegion(sSample_Name,
				 uiTrack, 
				 uiTrack_Pos + uiCutPos , 
				 uiSample_Offset + uiCutPos, 
				 uiSample_Duration - uiCutPos , 
				 0, 
				 uiFade_Out_Duration, 
				 fVolume );
	
}

void CRegion_Controller::Trim_Region(tuint32 uiTrack, tuint32 uiRegionID, tbool bStart, tint64 iDelta)
{

	Prepare(uiRegionID);
	
	std::string sSample_Name		=	mpRegion_DSP->Get_Sample_Name();
	tuint64 uiRegion_Pos		=	mpTrack_DSP->Get_Region_Pos(uiRegionID);	
	tint64 iDuration			=	mpRegion_DSP->Get_Duration();
	tint64 iSample_Offset		=	mpRegion_DSP->Get_Sample_Offset();
	
	// trim start
	if(bStart){
		tint64 iFade_In_Duration	=	mpRegion_DSP->Fade_In_Duration();
		
		// prevent sample offset being less than 0
		if(iSample_Offset + iDelta < 0 ) iDelta = -iSample_Offset;
		// prevent iDuration from beinging less than 1 sample
		else if( iDuration - iDelta <= 1)  iDelta = iDuration - 1;

		mpTrack_DSP->Resize_Region(uiRegionID, uiRegion_Pos + iDelta, iSample_Offset+iDelta, iDuration - iDelta);
		Fade_In(uiRegionID, iFade_In_Duration);
	}
	// trim end
	else{
		tint64 iFade_Out_Duration	=	mpRegion_DSP->Fade_Out_Duration();
		tint64 iSample_Duration				=	mpRegion_DSP->Get_Sample_Duration();
		
		
		// prevent duration from beion greater than iSample_Duration
		if( iDelta > iSample_Duration - iSample_Offset) iDelta = iSample_Duration - iSample_Offset;
		// prevent duration from being less than 1 sample
		else if (iDelta < 1) iDelta = 1;
		
		mpTrack_DSP->Resize_Region(uiRegionID, uiRegion_Pos, iSample_Offset, iDelta);
		Fade_Out(uiRegionID, iFade_Out_Duration);
	}
	
	mpRegionCallback->Refresh_Region_GUI(uiRegionID, uiTrack);
	
}

void CRegion_Controller::Duplicate_Region()
{ 
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		
		STrackSelectionInfo pTrack_Selection_Info	=	gpDSPEngine->GetTrackSelection(uiTrack);
		
		if(pTrack_Selection_Info.uiSelection_Type = giSelect_Region &&  pTrack_Selection_Info.iRegionID != -1)
		{
			tuint32 uiRegionID					= pTrack_Selection_Info.iRegionID ;
			
			SRegionInfo RegionInfo;
			Get_Region_Info( RegionInfo, uiRegionID);
			CTrack_DSP*		pTrack_DSP			=	gpDSPEngine->GetTrack(RegionInfo.uiTrack);
			CRegion_DSP*	pRegion_DSP			=	pTrack_DSP->GetRegion_DSP(uiRegionID);
			std::string sSample_Name			=	pRegion_DSP->Get_Sample_Name();
			tuint64 uiPossition					=	pTrack_DSP->Get_Region_Pos(uiRegionID);	
			tuint64 uiDuration					=	pRegion_DSP->Get_Duration();
			tuint64 uiSample_Offset				=	pRegion_DSP->Get_Sample_Offset();
			
			tuint64	uiFade_In_Duration			=	pRegion_DSP->Fade_In_Duration();
			tuint64	uiFade_Out_Duration			=	pRegion_DSP->Fade_Out_Duration();
			tfloat32 fVolume					=	pRegion_DSP->Volume();

			
			// Create new region
			tuint32 muiRegionID = CreateRegion(sSample_Name, 
											   uiTrack, 
											   uiPossition + uiDuration , 
											   uiSample_Offset, 
											   uiDuration,
											   uiFade_In_Duration,
											   uiFade_Out_Duration,
											   fVolume);
			
			gpDSPEngine->Select_Region(muiRegionID);
			mpRegionCallback->SelectRegion(muiRegionID,uiTrack);
			return;
		}
	}
}

void CRegion_Controller::Copy_Region()
{
	for(tint32 uiTrack=0; uiTrack<giNumber_Of_Tracks; uiTrack++){
		
		STrackSelectionInfo pTrack_Selection_Info	=	gpDSPEngine->GetTrackSelection(uiTrack);
		
		if(pTrack_Selection_Info.uiSelection_Type = giSelect_Region &&  pTrack_Selection_Info.iRegionID != -1)
		{
			tuint32 uiRegionID					= pTrack_Selection_Info.iRegionID ;
			
			SRegionInfo RegionInfo;
			Get_Region_Info( RegionInfo, uiRegionID);
			CTrack_DSP*		pTrack_DSP			=	gpDSPEngine->GetTrack(RegionInfo.uiTrack);
			CRegion_DSP*	pRegion_DSP			=	pTrack_DSP->GetRegion_DSP(uiRegionID);
			
			
			
			mpRegion_Clipboard[0].bEmpty					=	false;
			mpRegion_Clipboard[0].sClipName					=	pRegion_DSP->Get_Sample_Name();
			mpRegion_Clipboard[0].uiDuration				=	pRegion_DSP->Get_Duration();
			mpRegion_Clipboard[0].uiSample_Offset			=	pRegion_DSP->Get_Sample_Offset();
			mpRegion_Clipboard[0].uiPossition				=	pTrack_DSP->Get_Region_Pos(uiRegionID) + mpRegion_Clipboard[0].uiDuration;	
			mpRegion_Clipboard[0].uiFade_In_Duration		=	pRegion_DSP->Fade_In_Duration();
			mpRegion_Clipboard[0].uiFade_Out_Duration		=	pRegion_DSP->Fade_Out_Duration();
			mpRegion_Clipboard[0].fVolume					=	pRegion_DSP->Volume();
			

			return;
		}
	}
}
void CRegion_Controller::Copy_Selection()
{
	
}

void CRegion_Controller::Paste_Region(tuint32 uiTrack, tuint64 uiPosition)
{ 
	
		//	tuint64 uiPosition = mpRegion_Clipboard[0].uiPossition;
	if(!mpRegion_Clipboard[0].bEmpty) {	
	
			// Create new region
		tuint32 uiID = CreateRegion(mpRegion_Clipboard[0].sClipName, 
											uiTrack, 
											uiPosition , 
											mpRegion_Clipboard[0].uiSample_Offset, 
											mpRegion_Clipboard[0].uiDuration,
											mpRegion_Clipboard[0].uiFade_In_Duration,
											mpRegion_Clipboard[0].uiFade_Out_Duration,
											mpRegion_Clipboard[0].fVolume);
			
		gpDSPEngine->Select_Region(uiID);
		mpRegionCallback->SelectRegion(uiID,uiTrack);
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
	tuint64 uiSoundDuraion				=	mpRegion_DSP->Get_Duration();
	tuint64 uiFadeOutLength				=	mpRegion_DSP->Fade_Out_Duration();
	
	if(uiFadeInLength > uiSoundDuraion)
		uiFadeInLength = uiSoundDuraion;
	
	if( uiFadeInLength + uiFadeOutLength > uiSoundDuraion){
		uiFadeOutLength = uiSoundDuraion - uiFadeInLength;
		mpRegion_DSP->Fade_Out_Duration(uiFadeOutLength);
	}
	
	
	mpRegion_DSP->Fade_In_Duration(uiFadeInLength);
	return uiFadeInLength;
	
}

tuint64 CRegion_Controller::Fade_Out(tuint32 uiRegionID, tuint64 uiFadeOutLength)
{ 
	Prepare(uiRegionID);
	tuint64 uiSoundDuraion				=	mpRegion_DSP->Get_Duration();
	tuint64 uiFadeInLength				=	mpRegion_DSP->Fade_In_Duration();
	
	if(uiFadeOutLength > uiSoundDuraion)
		uiFadeOutLength = uiSoundDuraion;
	
	if(uiFadeOutLength + uiFadeInLength > uiSoundDuraion){
		uiFadeInLength = uiSoundDuraion - uiFadeOutLength;
		mpRegion_DSP->Fade_In_Duration(uiFadeInLength);
	}
	
	mpRegion_DSP->Fade_Out_Duration(uiFadeOutLength);
	
	return uiFadeOutLength;
}

void CRegion_Controller::Move_Region(tuint32 uiRegionID, tint32 iTrack, tuint64 uiPossition)
{
	
	Prepare(uiRegionID);
	
	
	std::string sClipName				=	mpRegion_DSP->Get_Sample_Name();
	tuint64 uiSample_Offset				=	mpRegion_DSP->Get_Sample_Offset();
	tuint64 uiSample_Duration			=	mpRegion_DSP->Get_Duration();
	tuint64 uiFade_In_Duration			=	mpRegion_DSP->Fade_In_Duration();
	tuint64 uiFade_Out_Duration			=	mpRegion_DSP->Fade_Out_Duration();
	
	tfloat32 fVolume					=	mpRegion_DSP->Volume();
	
	gpDSPEngine->Delete_Region(miTrack, uiRegionID);
	
	uiRegionID = CreateRegion(sClipName, 
							  iTrack, 
							  uiPossition, 
							  uiSample_Offset, 
							  uiSample_Duration,
							  uiFade_In_Duration,
							  uiFade_Out_Duration, 
							  fVolume);
	
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
			
			tuint64 uiSample_Offset		=	pRegion_DSP->Get_Sample_Offset();
			tuint64 uiDuration			=	pRegion_DSP->Get_Duration();
			std::string sPath			=	pRegion_DSP->GetSound(0)->GetPathName();
			
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
				
				psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrack_Pos);
				psz += sprintf(psz, pszUI64Format, uiSample_Offset);
				psz += sprintf(psz, pszUI64Format, uiDuration);
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
			const tchar* pszListItemName = pRegion_DSP->Get_Sample_Name();
			tuint64 uiStart = pRegion_DSP->Get_Sample_Offset();
			tuint64 uiDurat = pRegion_DSP->Get_Duration();
			//
			tuint64 uiFadeIn = pRegion_DSP->Fade_In_Duration();
			tuint64 uiFadeOut = pRegion_DSP->Fade_Out_Duration();
			tfloat32 fVolume = pRegion_DSP->Volume();
			tuint64 uiVol1000000 = Float2Int(fVolume * (tuint64)1000000);
			
			tchar pszBuff[1024];
			tchar* psz = pszBuff;
			//psz += sprintf(psz, "%u|", uiID);uiTrack_Pos
			psz += sprintf(psz, pszUI64Format, pRegionInfo->uiTrack_Pos);
			psz += sprintf(psz, pszUI64Format, uiStart);
			psz += sprintf(psz, pszUI64Format, uiDurat);
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
					tuint64 uiTrack_Pos;
					// No support for trimming in version 1 saved files
					//tuint64 uiStart;
					//tuint64 uiEnd;
					
					std::string sItem = *it++;
					sscanf(sItem.c_str(), pszUI64Format, &uiTrack_Pos);
					sItem = *it++;
					//sscanf(sItem.c_str(), pszUI64Format, &uiStart);
					sItem = *it++;
					//sscanf(sItem.c_str(), pszUI64Format, &uiEnd);
					sItem = *it++;
					CreateRegion(sItem.c_str(), iTrack, uiTrack_Pos, 0, (tuint64)-1);//, uiStart, uiEnd);
					
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
						tuint64 uiTrack_Pos;
						tuint64 uiSample_Offset;
						tuint64 uiSample_Duration;
						//
						tuint64			uiFadeIn		= 0;
						tuint64			uiFadeOut		= 0;
						tfloat32		fRegionVolume	= 1.0f;
						
						std::string sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiTrack_Pos);
						sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiSample_Offset);
						sItem = *it++;
						sscanf(sItem.c_str(), pszUI64Format, &uiSample_Duration);
						sClipName = *it++;
						//tuint64 uiRegionStopPos = uiSample_Offset + uiSample_Duration - 1;
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
						CreateRegion(sClipName.c_str(), 
									 iTrack,
									 uiTrack_Pos,
									 uiSample_Offset, 
									 uiSample_Duration,
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
	tuint64 uiStart_Sample				=	mpTrack_DSP->Get_Region_Pos(iRegionID);	
	tuint64 uiDuration					=	(mpRegion_DSP ? mpRegion_DSP->Get_Duration() : 0);
	
	tint32 iNewSelectType = giSelect_Region;
	if (ge::IWindow::ShiftPressed()) {
		
		STrackSelectionInfo sinfo = gpDSPEngine->GetTrackSelection(miTrack);
		
		tint32 iOldType = sinfo.uiSelection_Type;
		
		if ((iOldType == giSelect_On_Track) || (iOldType == giSelect_Region)) {
			iNewSelectType = giSelect_On_Track;
			tuint64 uiOldStart = sinfo.uiSelection_Pos;
			if (uiOldStart < uiStart_Sample) {
				uiDuration += (uiStart_Sample - uiOldStart);
				uiStart_Sample = uiOldStart;
			}
			tuint64 uiOldDurat = sinfo.uiSelection_Duration;
			tuint64 uiOldFinal = uiOldStart + uiOldDurat - 1;
			tuint64 uiNewFinal = uiStart_Sample + uiDuration - 1;
			if (uiOldFinal > uiNewFinal) {
				uiDuration = uiOldFinal - uiStart_Sample + 1;
			}
		}
	}
	gpDSPEngine->SetTrackSelection(mRegionInfo.uiTrack,  uiStart_Sample,  uiDuration, iNewSelectType, iRegionID);
	
	return gpDSPEngine->GetTrackSelection(miTrack);
}

SRegion_Drawing_Info CRegion_Controller::Get_Region_Drawing_Info(tuint iRegionID)
{
	SRegion_Drawing_Info info;
	
	Prepare(iRegionID);
	
	if (mpRegion_DSP) {
		info.uiSound_Start		=	mpRegion_DSP->Get_Sample_Offset();
		info.uiSound_Duration	=	mpRegion_DSP->Get_Duration();
		info.uiTrack_Start		=	mRegionInfo.uiStartPos;
		info.uiFade_In			=	mpRegion_DSP->Fade_In_Duration();
		info.uiFade_Out			=	mpRegion_DSP->Fade_Out_Duration();
		info.fRegion_Volume		=	mpRegion_DSP->Volume();
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
			return mpRegion_DSP->Get_Sample_Name();
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

tuint64 CRegion_Controller::GetRegionSize(tuint32 iRegionID )
{
	Prepare(iRegionID);	
	return	mpRegion_DSP->Get_Duration();
}


