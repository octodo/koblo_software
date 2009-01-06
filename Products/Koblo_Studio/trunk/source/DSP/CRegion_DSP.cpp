

#include "KSOS.h"

CRegion_DSP::CRegion_DSP(tint32 iUniqueID, const std::string& sSoundPathNameL, const std::string& sSoundPathNameR, const std::string& sSoundListItemName, tuint64 uiSamplePosStart, tuint64 uiSamplePosEnd)
{
	muiFadeInLength = 0;
	muiFadeOutLength = 0;

	mppPeakFile[0] = NULL;
	mppPeakFile[1] = NULL;
	mppPeakFile[2] = NULL;
	mppPeakFile[3] = NULL;

	muiUniqueID = iUniqueID;

	muiStartPos = uiSamplePosStart;
	muiEndPos	= uiSamplePosEnd;

	miChannels = 1;
	if (sSoundPathNameR.size() > 0) {
		miChannels = 2;
	}

	if (miChannels == 1) {
		mppSample	= new CSample_DSP*[1];
		mppSample[0] = new CSample_DSP(sSoundPathNameL);
	}
	else {
		mppSample = new CSample_DSP*[2];
		mppSample[0] = new CSample_DSP(sSoundPathNameL);
		mppSample[1] = new CSample_DSP(sSoundPathNameR);
	}

	msSoundListItemName = sSoundListItemName;

	if (muiEndPos == (tuint64)-1) {
		muiEndPos = (mppSample[0]->GetLength() - uiSamplePosStart) -1;
	}

#ifdef _Mac_PowerPC
	tchar kpsk1024[] = ".kspk1024_ppc";
	tchar kpsk64[] = ".kspk64_ppc";
#else _Mac_PowerPC
	tchar kpsk1024[] = ".kspk1024";
	tchar kpsk64[] = ".kspk64";
#endif // _Mac_PowerPC
	std::string sPeakFileName = sSoundPathNameL + kpsk1024;
	mppPeakFile[0] = IFile::Create();
	if (!mppPeakFile[0]->Open(sPeakFileName.c_str(), IFile::FileRead)) {
		mppPeakFile[0]->Destroy();
		mppPeakFile[0] = NULL;
	}

	sPeakFileName = sSoundPathNameL + kpsk64;
	mppPeakFile[1] = IFile::Create();
	mppPeakFile[1]->Open(sPeakFileName.c_str(), IFile::FileRead);

	if (miChannels > 1) {
		sPeakFileName = sSoundPathNameR + kpsk1024;
		mppPeakFile[2] = IFile::Create();
		mppPeakFile[2]->Open(sPeakFileName.c_str(), IFile::FileRead);

		sPeakFileName = sSoundPathNameR + kpsk64;
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
	muiEndPos = (muiStartPos + iDuration) -1;
	
	 ASSERT( (muiEndPos!=0) && (muiEndPos >= muiStartPos) );
 }


void CRegion_DSP::GetSamples(tfloat32** ppfData, tint32 iSamples)
{
	tuint64 uiPosBufferStart = mppSample[0]->GetPos();
	tint32 iSample;

	if (miChannels == 1) {
		mppSample[0]->GetSamples(ppfData[0], iSamples);

		if (muiFadeInLength) {
			if (muiFadeInLength > uiPosBufferStart) {
				tfloat32 fOneDivFadeInLength = 1.0f / muiFadeInLength;
				tfloat32* pfData = ppfData[0];
				
				tint32 iSamplesToProcess = iSamples;
				if (iSamplesToProcess > muiFadeInLength - uiPosBufferStart) {
					iSamplesToProcess = (tint32)(muiFadeInLength - uiPosBufferStart);
				}
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					tfloat32 fIndex = ((tint32)uiPosBufferStart + iSample) * fOneDivFadeInLength;
					fIndex *= fIndex;

					pfData[iSample] *= fIndex;
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
					tfloat32 fIndex = ((tint32)muiEndPos - uiPosBufferStart + iSample) * fOneDivFadeOutLength;
					fIndex *= fIndex;

					pfData[iSample] *= fIndex;
				}
			}
		}
		for (iSample = 0; iSample < iSamples; iSample++) {
			tfloat32* pfData = ppfData[0];
			pfData[iSample] *= mfRegionVolume;
		}
	}
	else {
		mppSample[0]->GetSamples(ppfData[0], iSamples);
		mppSample[1]->GetSamples(ppfData[1], iSamples);

		if (muiFadeInLength) {
			if (muiFadeInLength > uiPosBufferStart) {
				tfloat32 fOneDivFadeInLength = 1.0f / muiFadeInLength;
				tfloat32* pfDataL = ppfData[0];
				tfloat32* pfDataR = ppfData[1];
				tint32 iSamplesToProcess = iSamples;
				if (iSamplesToProcess > muiFadeInLength - uiPosBufferStart) {
					iSamplesToProcess = (tint32)(muiFadeInLength - uiPosBufferStart);
				}
				for (iSample = 0; iSample < iSamplesToProcess; iSample++) {
					tfloat32 fIndex = ((tint32)uiPosBufferStart + iSample) * fOneDivFadeInLength;
					fIndex *= fIndex;

					
					pfDataL[iSample] *= fIndex;
					pfDataR[iSample] *= fIndex;
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
					tfloat32 fIndex = ((tint32)muiEndPos - uiPosBufferStart + iSample) * fOneDivFadeOutLength;
					fIndex *= fIndex;

					pfDataL[iSample] *= fIndex;
					pfDataR[iSample] *= fIndex;
				}
			}
		}
		for (iSample = 0; iSample < iSamples; iSample++) {
			
			tfloat32* pfDataL = ppfData[0];
			tfloat32* pfDataR = ppfData[1];
			
			
			pfDataL[iSample] *= mfRegionVolume;
			pfDataR[iSample] *= mfRegionVolume;
		}
	}
} // GetSamples


void CRegion_DSP::SetPos(tuint64 uiPos)
{
	mppSample[0]->SetPos(muiStartPos + uiPos);

	if (miChannels > 1) {
		mppSample[1]->SetPos(muiStartPos + uiPos);
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




