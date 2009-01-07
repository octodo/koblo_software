
#include "KSOS.h"

CSample_DSP::CSample_DSP(const std::string& sPathName)
{
	muiPos = 0;

	msPathName = sPathName;

	CWave_File* pWaveFile = new CWave_File();
	pWaveFile->LoadSoundStream(512, msPathName.c_str());


	mppStream = gpStreamManager->GetStream();

	IFile* pFile;
	tint32 iOffset;
	tint32 iLength;
	tint32 iBitWidth;
	tint32 iChannels;
	pWaveFile->GetStreamInfo(pFile, iOffset, iLength, &iBitWidth, &iChannels);


	mppStream->Reset(pFile, iOffset, iLength, false, NULL, iBitWidth, iChannels);


	muiLength = iLength;
}




