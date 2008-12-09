/*
 *  untitled.cpp
 *  
 *
 *  Created by Michael Olsen on 4/7/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "KSOS.h"

CSound::CSound(const std::string& sPathName)
{
	muiPos = 0;

	msPathName = sPathName;

	CWaveFile* pWaveFile = new CWaveFile();
	pWaveFile->LoadSoundStream(512, msPathName.c_str());

	mppStream[0] = gpStreamManager->GetStream();

	IFile* pFile;
	tint32 iOffset;
	tint32 iLength;
	tint32 iBitWidth;
	tint32 iChannels;
	pWaveFile->GetStreamInfo(pFile, iOffset, iLength, &iBitWidth, &iChannels);

	//mppStream[0]->Reset(pFile, iOffset, iLength, false, NULL);
	mppStream[0]->Reset(pFile, iOffset, iLength, false, NULL, iBitWidth, iChannels);
//	mppStream[0]->SetPosition(mpPlugIn->GetSongPos());

	muiLength = iLength;
}




