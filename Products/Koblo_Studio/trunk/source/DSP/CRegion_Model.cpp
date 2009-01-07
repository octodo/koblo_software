

#include "KSOS.h"

CRegion_Model::CRegion_Model()
{
	
}

CRegion_Model::~CRegion_Model()
{
	
}

tint32 CRegion_Model::CreateRegion(const std::string& sSoundListItemName, 
 tint32 iTrack, 
 tuint64 uiTrack_Pos, 
 tuint64 uiSample_Start, 
 tuint64 uiSamplePosEnd,
 tuint64 uiFadeInLength,
 tuint64 uiFadeOutLength,
 tfloat32 fRegionVolume)
{
 // (mo) Temporary id code:
 static int iID = 0;
 tint32 iRegionID = iID++;
 
 // DSP
 gpDSPEngine->GetTrack(iTrack)->CreateRegion(iRegionID, 
 sSoundListItemName, 
 uiTrack_Pos, 
 uiSample_Start, 
 uiSamplePosEnd,
 uiFadeInLength,
 uiFadeOutLength,
 fRegionVolume);
 
 // GUI		
 tuint64 uiSample_Duration = uiSamplePosEnd - uiSample_Start;
 
 mpRegionCallback->InsertRegion(iRegionID, iTrack, uiTrack_Pos, uiSample_Start, uiSample_Duration, sSoundListItemName.c_str());
 mpRegionCallback->SelectRegion(iRegionID, iTrack);
 gpDSPEngine->SelectRegion(iRegionID);
 
 //Set_Session_Size( (uiSamplePosEnd - uiSample_Start) + uiTrack_Pos + 32);
 gpDSPEngine->Sanitize_Session_End_Sample();	
 
 
 return iRegionID;
}
 
void  CRegion_Model::Get_Region_Info(SRegionInfo& RegionInfo, tuint32 uiRegionID)
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

void CRegion_Model::Set_Region_Volume(tuint32 uiRegionID, tfloat32 fRegion_Volume)
{ 
	
	SRegionInfo RegionInfo;
	Get_Region_Info( RegionInfo, uiRegionID);
	CRegion_DSP* pRegion	=	gpDSPEngine->GetTrack(RegionInfo.uiTrack)->GetRegion_DSP(uiRegionID);
	pRegion->SetRegionVolume(fRegion_Volume);	
	
}