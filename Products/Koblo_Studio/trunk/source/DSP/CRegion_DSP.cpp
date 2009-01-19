

#include "KSOS.h"

CRegion_DSP::CRegion_DSP(tint32 iUniqueID, 
						 CSample_Data* pSample_Data,
						 CTake_Data* pTake_Data,
						 tuint64 uiSample_Offset, 
						 tuint64 uiSample_Duration)
{
	muiUniqueID			= iUniqueID;
	mpSample_Data		= pSample_Data;
	mpTake_Data			= pTake_Data;
	muiSample_Offset	= uiSample_Offset;
	muiEndPos			= uiSample_Offset + uiSample_Duration - 1;
	muiFadeInLength		= 0;
	muiFadeOutLength	= 0;
	mppPeakFile[0]		= NULL;
	mppPeakFile[1]		= NULL;
	mppPeakFile[2]		= NULL;
	mppPeakFile[3]		= NULL;
	
	
	if (pTake_Data->sWavePathNameR.size() == 0) {
		miChannels = 1;
		mppSample	= new CSample_DSP*[1];
		mppSample[0] = new CSample_DSP(mpTake_Data->sWavePathNameL);
	}
	else {
		miChannels = 2;
		mppSample = new CSample_DSP*[2];
		mppSample[0] = new CSample_DSP(mpTake_Data->sWavePathNameL);
		mppSample[1] = new CSample_DSP(mpTake_Data->sWavePathNameR);
	}
	
	msSample_Name = mpSample_Data->Get_Name();
	
	if (uiSample_Duration == (tuint64)-1) {
		muiEndPos = (mppSample[0]->GetLength() - uiSample_Offset) -1;
	}
	
#ifdef _Mac_PowerPC
	tchar kpsk1024[] = ".kspk1024_ppc";
	tchar kpsk64[] = ".kspk64_ppc";
#else _Mac_PowerPC
	tchar kpsk1024[] = ".kspk1024";
	tchar kpsk64[] = ".kspk64";
#endif // _Mac_PowerPC
	
	
	
	std::string sPeakFileName = mpTake_Data->sWavePathNameL + kpsk1024;
	mppPeakFile[0] = IFile::Create();
	
	if (!mppPeakFile[0]->Open(sPeakFileName.c_str(), IFile::FileRead)) {
		mppPeakFile[0]->Destroy();
		mppPeakFile[0] = NULL;
	}
	
	sPeakFileName = mpTake_Data->sWavePathNameL + kpsk64;
	mppPeakFile[1] = IFile::Create();
	mppPeakFile[1]->Open(sPeakFileName.c_str(), IFile::FileRead);
	
	if (miChannels > 1) {
		sPeakFileName = mpTake_Data->sWavePathNameR + kpsk1024;
		mppPeakFile[2] = IFile::Create();
		mppPeakFile[2]->Open(sPeakFileName.c_str(), IFile::FileRead);
		
		sPeakFileName = mpTake_Data->sWavePathNameR + kpsk64;
		mppPeakFile[3] = IFile::Create();
		mppPeakFile[3]->Open(sPeakFileName.c_str(), IFile::FileRead);
	}
	
	
}

CRegion_DSP::~CRegion_DSP()
{
	mppPeakFile[0]->Destroy();
	mppPeakFile[1]->Destroy();
	if (mppPeakFile[2]) {
		mppPeakFile[2]->Destroy();
	}
	if (mppPeakFile[3]) {
		mppPeakFile[3]->Destroy();
	}
}

void CRegion_DSP::SetEndPos(tuint64 iPos)
{
	if(iPos > mppSample[0]->GetLength())
		muiEndPos = mppSample[0]->GetLength();
	else
		muiEndPos = iPos;
}


void CRegion_DSP::SetDuration(tuint64 iDuration)
{
	muiEndPos = (muiSample_Offset + iDuration) -1;
	
	ASSERT( (muiEndPos!=0) && (muiEndPos >= muiSample_Offset) );
}


void CRegion_DSP::GetSamples(tfloat32** ppfData, tint32 iSamples)
{
	tuint64 uiPosBufferStart = mppSample[0]->GetPos();
	tint32 iSample;
	
	if (miChannels == 1) {
		mppSample[0]->GetSamples(ppfData[0], iSamples);
		
		if (muiFadeInLength) {
			if (muiSample_Offset + muiFadeInLength > uiPosBufferStart) {
				tfloat32 fOneDivFadeInLength = 1.0f / muiFadeInLength;
				tfloat32* pfData = ppfData[0];
				
				tint32 iSamplesToProcess = iSamples;
				if (iSamplesToProcess > muiSample_Offset + muiFadeInLength - uiPosBufferStart) {
					iSamplesToProcess = (tint32)(muiSample_Offset + muiFadeInLength  - uiPosBufferStart);
				}
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					tfloat32 fVolume = ((tint32)uiPosBufferStart - muiSample_Offset + iSample) * fOneDivFadeInLength;
					fVolume *= fVolume;
					
					pfData[iSample] *= fVolume;
				}
			}
		}
		if (muiFadeOutLength) {
			if (muiEndPos - muiFadeOutLength < uiPosBufferStart + iSamples) {
				tfloat32 fOneDivFadeOutLength = 1.0f / muiFadeOutLength;
				tfloat32* pfData = ppfData[0];
				tint32 iSample;
				tint32 iSamplesToProcess = iSamples;
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					tfloat32 fVolume = ((tint32)muiEndPos - uiPosBufferStart + iSample) * fOneDivFadeOutLength;
					fVolume *= fVolume;
					
					pfData[iSample] *= fVolume;
				}
			}
		}
		for (iSample = 0; iSample < iSamples; iSample++) {
			tfloat32* pfData = ppfData[0];
			pfData[iSample] *= mfVolume;
		}
	}
	else {
		mppSample[0]->GetSamples(ppfData[0], iSamples);
		mppSample[1]->GetSamples(ppfData[1], iSamples);
		
		
		if (muiFadeInLength) {
			if ( muiSample_Offset + muiFadeInLength > uiPosBufferStart) {
				tfloat32 fOneDivFadeInLength = 1.0f / muiFadeInLength;
				tfloat32* pfDataL = ppfData[0];
				tfloat32* pfDataR = ppfData[1];
				tint32 iSamplesToProcess = iSamples;
				if (iSamplesToProcess > muiSample_Offset + muiFadeInLength - uiPosBufferStart) {
					iSamplesToProcess = (tint32)(muiSample_Offset + muiFadeInLength  - uiPosBufferStart);
				}
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					// apply fade in volume
					tfloat32 fVolume = ((tint32)uiPosBufferStart - muiSample_Offset + iSample) * fOneDivFadeInLength;
					fVolume *= fVolume;
					
					
					pfDataL[iSample] *= fVolume;
					pfDataR[iSample] *= fVolume;
				}
			}
		}
		
		if (muiFadeOutLength) {
			if (muiEndPos - muiFadeOutLength < uiPosBufferStart + iSamples) {
				tfloat32 fOneDivFadeOutLength = 1.0f / muiFadeOutLength;
				tfloat32* pfDataL = ppfData[0];
				tfloat32* pfDataR = ppfData[1];
				tint32 iSample;
				tint32 iSamplesToProcess = iSamples;
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					tfloat32 fVolume = ((tint32)muiEndPos - uiPosBufferStart + iSample) * fOneDivFadeOutLength;
					fVolume *= fVolume;
					
					pfDataL[iSample] *= fVolume;
					pfDataR[iSample] *= fVolume;
				}
			}
		}
		for (iSample = 0; iSample < iSamples; iSample++) {
			
			tfloat32* pfDataL = ppfData[0];
			tfloat32* pfDataR = ppfData[1];
			
			
			pfDataL[iSample] *= mfVolume;
			pfDataR[iSample] *= mfVolume;
		}
	}
} // GetSamples


void CRegion_DSP::SetPos(tuint64 uiPos)
{
	mppSample[0]->SetPos(muiSample_Offset + uiPos);
	
	if (miChannels > 1) {
		mppSample[1]->SetPos(muiSample_Offset + uiPos);
	}
}

void CRegion_DSP::GetPeakFile(IFile** ppFile, tint32 iChannel, tint32 iSize)
{
	ASSERT(mppPeakFile[0]);
	ASSERT(mppPeakFile[1]);
	if (iChannel > 1) {
		ASSERT(mppPeakFile[2]);
		ASSERT(mppPeakFile[3]);
	}
	
	*ppFile = mppPeakFile[iSize + iChannel * 2];
}

tbool CRegion_DSP::Set_Region_UUID()
{ 
	// only generate the uuid once
	if(msUUID.size() == 0){
		msUUID = gpApplication->Get_UUID();
		return true;
	}
	return false;
}

std::string CRegion_DSP::Get_Region_UUID()
{ 
	Set_Region_UUID();	
	return msUUID;
}



