

#include "KSOS.h"

//tint16* gpiData = NULL;
//tfloat32* gpfData2 = NULL;

/*
	Error reporting
*/

void SoundLoadError(const tchar* psz)
{
#ifdef WIN32
	::MessageBox(NULL, (char*)psz, NULL, MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#endif	// WIN32
}

void SoundLoadGeneralParsingError(const tchar* psz)
{
	tchar psz2[512];
	sprintf((char*)psz2, "General parsing error in sound: %s", psz);
	SoundLoadError(psz2);
}


/*
	CWaveParser
*/

CWaveParser::CWaveParser()
{
	miSampleRate = 0;
	miLoopStart = -1;
	miLoopEnd = -1;
	mfBaseFreq = 0;
	miSampleCount = 0;
	mpData = NULL;
}

CWaveParser::~CWaveParser()
{
	if (mpData) {
		delete[] mpData;
	}
}

// Used for wave parsing
#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SChunkHeader {
	long chunkID;
	long chunkSize;
} PACKED;
struct SFmtChunk {
	short          wFormatTag;
	unsigned short wChannels;
	unsigned long  dwSamplesPerSec;
	unsigned long  dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
} PACKED;
struct SSmplLoop {
  long  dwIdentifier;
  long  dwType;
  long  dwStart;
  long  dwEnd;
  long  dwFraction;
  long  dwPlayCount;
} PACKED;
// Note, the specs says SSmplLoop Loops[], but that's a non-standard extension,
// so we just use any arbitrary number
struct SSmplChunk {
	long           dwManufacturer;
	long           dwProduct;
	long           dwSamplePeriod;
	long           dwMIDIUnityNote;
	long           dwMIDIPitchFraction;
	long           dwSMPTEFormat;
	long           dwSMPTEOffset;
	long           cSampleLoops;
	long           cbSamplerData;
	SSmplLoop      Loops[256];
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

// Converts .wav ('smpl') based info to frequency
inline tfloat Smpl2Freq(long lUnityNote, unsigned long lPitchFraction)
{
	const tfloat fBaseFreq = 440.0f;
	const tint iBaseNote = 57;

	tfloat fRet = fBaseFreq * (tfloat)pow(1.0594630943592952645618252949463, lUnityNote - iBaseNote);

/*	if (lPitchFraction > 0) {
		tfloat f1Up = fBaseFreq * (tfloat)pow(1.0594630943592952645618252949463, lUnityNote + 1 - iBaseNote);
		tfloat fCent = (tfloat)((tfloat64)lPitchFraction / 2147483647) * 0.5f;
		fRet += ((f1Up - fRet) * fCent);
	}
	if (lPitchFraction < 0) {
		tfloat f1Down = fBaseFreq * (tfloat)pow(1.0594630943592952645618252949463, lUnityNote - 1 - iBaseNote);
		tfloat fCent = (tfloat)((tfloat64)lPitchFraction / -2147483647) * 0.5f;
		fRet += ((f1Down - fRet) * fCent);
	}*/
	tfloat f1Down = fBaseFreq * (tfloat)pow(1.0594630943592952645618252949463, lUnityNote - 1 - iBaseNote);
	tfloat fCent = (tfloat)((tfloat64)lPitchFraction / -2147483647) * 0.5f;
	fRet += ((fRet - f1Down) * fCent);

	return fRet;
}

//bool CWaveParser::Load(const tchar* pszPathName)
bool CWaveParser::Load(IFile* pFile, tbool bLoadData)
{
//	CAutoDelete<IFile> pFile(IFile::Create());
//	IFile* pFile = dynamic_cast<IFile*>(pChunk);

/*	if (pFile->Open((const tchar*)pszPathName, IFile::FileRead) == false) {
		// Cannot open file
		char psz[512];
		sprintf(psz, "Cannot load sound: %s", pszPathName);
		SoundLoadError((tchar*)psz);
		return false;
	}*/

	char pszID[4];
	if (pFile->Read((tchar*)pszID, 4) != 4) {
		// File too small
		SoundLoadGeneralParsingError((tchar*)"Unknown");
		return false;
	}
	if (pszID[0] != 'R' || pszID[1] != 'I' || pszID[2] != 'F' || pszID[3] != 'F') {
		// Wrong file format
		SoundLoadGeneralParsingError((tchar*)"Unknown");
		return false;
	}

	// read size
	tint32 iSize;
	pFile->ReadCP(&iSize, 1);

	if (pFile->Read((tchar*)pszID, 4) != 4) {
		// File too small
		SoundLoadGeneralParsingError((tchar*)"Unknown");
		return false;
	}
	iSize -= 4;
	if (pszID[0] != 'W' || pszID[1] != 'A' || pszID[2] != 'V' || pszID[3] != 'E') {
		// Wrong file format
		SoundLoadGeneralParsingError((tchar*)"Unknown");
		return false;
	}

	// Iterate through chunks
	bool bDone = false;
	while (bDone == false) {
		SChunkHeader Header;
		if (pFile->Read((tchar*)&Header, sizeof(SChunkHeader)) != sizeof(SChunkHeader)) {
			// No more to read, break out
			bDone = true;
		}
		iSize -= sizeof(SChunkHeader);

		// Reverse for endianess
#ifdef WIN32
		tuint32 ui = Header.chunkID;
		Header.chunkID = ((ui << 24) | ((ui & 0xff00) << 8) | ((ui & 0xff0000) >> 8) | (ui >> 24));
#endif	// WIN32
#ifdef _Mac_Intel
		tuint32 ui = Header.chunkID;
		Header.chunkID = ((ui << 24) | ((ui & 0xff00) << 8) | ((ui & 0xff0000) >> 8) | (ui >> 24));
#endif	// _Mac_Intel
		Header.chunkSize = CPSwap((tint32)Header.chunkSize);

		// For debugging it's nice to be able to see the characters
		char p[4];
		p[0] = (char)(Header.chunkID >> 24);
		p[1] = (char)((Header.chunkID & 0xff0000) >> 16);
		p[2] = (char)((Header.chunkID & 0xff00) >> 8);
		p[3] = (char)(Header.chunkID & 0xff);

		if (bDone == false) {
			switch(Header.chunkID) {
				case 'fmt ':
					{
						SFmtChunk* pChunk;
						tchar* p = new tchar[Header.chunkSize];
						if (pFile->Read((tchar*)p, Header.chunkSize) != Header.chunkSize) {
							// File too small
							delete[] p;
							return false;
						}
						iSize -= Header.chunkSize;
						pChunk = (SFmtChunk*)p;
						pChunk->wChannels = CPSwap(pChunk->wChannels);
						pChunk->wBitsPerSample = CPSwap(pChunk->wBitsPerSample);
						pChunk->wFormatTag = CPSwap(pChunk->wFormatTag);
						pChunk->dwSamplesPerSec = CPSwap((tuint32)pChunk->dwSamplesPerSec);

						miBitDepth = pChunk->wBitsPerSample;
						miSampleRate = pChunk->dwSamplesPerSec;
						miChannels = pChunk->wChannels;

						// Check some info
						// Mono only
						if (pChunk->wChannels != 1) {
							char psz[512];
							sprintf(psz, "File is not mono: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
							delete[] p;
							return false;
						}
						// 16 & 24-bit only
						if ((pChunk->wBitsPerSample != 24) && (pChunk->wBitsPerSample != 16)) {
							char psz[512];
							sprintf(psz, "File is not 16 or 24-bit: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
							delete[] p;
							return false;
						}
						// Integer only
						if (pChunk->wFormatTag != 1) {
							char psz[512];
							sprintf(psz, "File is not integer: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
							delete[] p;
							return false;
						}

						if (miSampleRate < 11025 || miSampleRate > 96000) {
							char psz[512];
							sprintf(psz, "File is probably outside samplerate safety limits: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
						}

						delete[] p;
					}
					break;
				case 'data':
					{
						miSampleCount = Header.chunkSize / (miBitDepth / 8);
						// If the chunk size is uneven it is padded
//						if (Header.chunkSize % 2 != 0) {
//							Header.chunkSize++;
//						}
						if (bLoadData) {
							mpData = new tchar[Header.chunkSize];
							memset(mpData,0,Header.chunkSize);
							if (pFile->Read((tchar*)mpData, Header.chunkSize) != Header.chunkSize) {
								// File too small
								SoundLoadGeneralParsingError((tchar*)"Unknown");
								return false;
							}
						}
						else {
							miDataOffset = (tint32)pFile->GetCurrentFilePosition();
							if (pFile->Seek(miDataOffset + Header.chunkSize) != miDataOffset + Header.chunkSize) {
								SoundLoadGeneralParsingError((tchar*)"Unknown");
								return false;
							}
						}
						iSize -= Header.chunkSize;
					}
					break;
				case 'smpl':
					{
						tchar* p = new tchar[Header.chunkSize];
						if (pFile->Read((tchar*)p, Header.chunkSize) != Header.chunkSize) {
							// File too small
							SoundLoadGeneralParsingError((tchar*)"Unknown");
							delete[] p;
							return false;
						}
						iSize -= Header.chunkSize;

						SSmplChunk* pChunk = (SSmplChunk*)p;
						pChunk->dwMIDIUnityNote = CPSwap((tint32)pChunk->dwMIDIUnityNote);
						pChunk->dwMIDIPitchFraction = CPSwap((tint32)pChunk->dwMIDIPitchFraction);
						pChunk->cSampleLoops = CPSwap((tint32)pChunk->cSampleLoops);

						mfBaseFreq = Smpl2Freq(pChunk->dwMIDIUnityNote, pChunk->dwMIDIPitchFraction);

						if (mfBaseFreq == 0) {
							char psz[512];
							sprintf(psz, "File base freq is 0: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
						}

//						if (pChunk->cSampleLoops != 1) {
						if (pChunk->cSampleLoops > 1) {
							char psz[512];
//							sprintf(psz, "File contains !=1 loops: %s", (tchar*)"Unknown");
							sprintf(psz, "File contains >1 loops: %s", (tchar*)"Unknown");
							SoundLoadError((tchar*)psz);
						}
						if (pChunk->cSampleLoops > 0) {
							miLoopStart = CPSwap((tuint32)pChunk->Loops[0].dwStart);
							miLoopEnd = CPSwap((tuint32)pChunk->Loops[0].dwEnd);
						}

						delete[] p;
					}
					break;
				default:
					// Unknown chunk
					{
						tchar* p = new tchar[Header.chunkSize];
						if (pFile->Read((tchar*)p, Header.chunkSize) != Header.chunkSize) {
							// File too small
//							SoundLoadGeneralParsingError((tchar*)"Unknown");
							delete[] p;
//							return false;
							return true;
						}
						iSize -= Header.chunkSize;
						delete[] p;
					}
					break;
			}
		}

		if (iSize <= 0) {
			bDone = true;
		}
	}

	if (mfBaseFreq == 0) {
		mfBaseFreq = 440.0;
	}

	// Check that we got the info we needed, and if not bail
	if (miSampleRate == 0 ||
//		miLoopStart == 0 ||
//		miLoopEnd == 0 ||
//		mfBaseFreq == 0 ||
		miSampleCount == 0) {

		tchar psz[512];
		sprintf((char*)psz, "Unknown error, probably file didn't have sampler chunk: %s", (tchar*)"Unknown");
		SoundLoadError((tchar*)psz);
		return false;
	}

	return true;
}


/*
	CWave_File
*/

#ifdef WIN32
#pragma pack(push, 1)
#endif	// WIN32
struct SSample24
{
	unsigned char a;
	unsigned char b;
	char c;
} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

//tbool CWave_File::LoadSound(tint32 iSampleRate, const tchar* pszPathName)
tbool CWave_File::LoadSound(tint32 iSampleRate, IChunk* pChunk)
{
	CWaveParser Parser;
	if (Parser.Load(pChunk)) {
	

		miLoopStart = Parser.GetLoopStart();
		miLoopEnd	= Parser.GetLoopEnd();

//		miSampleCount = iLoopEnd - iLoopStart + 1;
		if (miLoopEnd == -1) {
			// No loop
			miSampleCount = Parser.GetSampleCount();
		}
		else {
			// Loop present
			miSampleCount = miLoopEnd + 1;
		}
		miBitDepth = Parser.GetBitDepth();
		miChannels = Parser.GetChannels();

		// For optimization reasons we allocate 1 more sample
		mpfData = new tfloat[miSampleCount + 2];
		miSoundSize = (miSampleCount + 2) * sizeof(tfloat);

		const tchar* pSrc = Parser.GetData();

		tint i;
		for (i = 0; i < miSampleCount; i++) {
			const tint16* pSrc2 = (tint16*)(pSrc + i * 2);
#ifdef _Mac_PowerPC
			tint16 i2 = CPSwap(*pSrc2);
			mpfData[i] = (tfloat)(i2 / 32767.0) * 2.0f;
#else	// _Mac_PowerPC
			mpfData[i] = (tfloat)(*pSrc2 / 32767.0) * 2.0f;
#endif	// _Mac_PowerPC
		}

		if (miLoopEnd == -1) {
			// No loop
			mpfData[miSampleCount] = mpfData[miSampleCount - 1];
			mpfData[miSampleCount + 1] = mpfData[miSampleCount - 1];
		}
		else {
			// Loop present
			mpfData[miSampleCount] = mpfData[miLoopStart];
			mpfData[miSampleCount + 1] = mpfData[miLoopStart + 1];
		}

		// Set base freq, and compensate for sample frequency
		mfBaseFreq = (tfloat)((tfloat64)Parser.GetBaseFreq() * (tfloat64)iSampleRate / Parser.GetSampleRate());
		if (mfBaseFreq == 0) {
			char psz[512];
			sprintf(psz, "Converted base freq is 0: %s", (tchar*)"Unknown");
			SoundLoadError((tchar*)psz);
		}

	//	float fSamplesPerCycle = Parser.GetSampleRate() / Parser.GetBaseFreq();
		mfCycles = 0;
		if (miLoopEnd != -1) {
			tfloat64 fSamplesPerCycle = (tfloat64)iSampleRate / mfBaseFreq;
			mfCycles = (miLoopEnd - miLoopStart + 1) / fSamplesPerCycle;
		}

		tfloat64 fSamplesPerSound = (tfloat64)iSampleRate / mfBaseFreq;
		mfCyclesSound = miSampleCount / fSamplesPerSound;

		return true;
	}
	return false;
}

tbool gbCheck = true;

//tbool CWave_File::LoadSoundStream(tint32 iSampleRate, IChunk* pChunk, tint32 iBufferSize, const std::string& sPathName)
tbool CWave_File::LoadSoundStream(tint32 iBufferSize, const tchar* pszPathName)
{
	if ((pszPathName == NULL) || (*pszPathName == '\0'))
		return false;

	mpFile = IFile::Create();
	mpFile->Open(pszPathName, IFile::FileRead);
	CWaveParser Parser;
	if (Parser.Load(mpFile, false) == false) {
		return false;
	}

/*	{
		CAutoDelete<IFile> pFile(IFile::Create());
		pFile->Open((tchar*)"c:\\file.wav", IFile::FileCreate);

		tchar* p = new tchar[pChunk->GetSize()];
		pChunk->Seek(0);
		pChunk->Read(p, pChunk->GetSize());
		pFile->Write(p, pChunk->GetSize());
		delete[] p;
	}*/

	miLoopStart = Parser.GetLoopStart();
	miLoopEnd	= Parser.GetLoopEnd();

//	miSampleCount = iLoopEnd - iLoopStart + 1;
	if (miLoopEnd == -1) {
		// No loop
		miSampleCount = Parser.GetSampleCount();
	}
	else {
		// Loop present
		miSampleCount = miLoopEnd + 1;
	}

//	mpfData = new tfloat[iBufferSize];
	miSoundSize = (miSampleCount + 2) * sizeof(tfloat);

	miChannels = Parser.GetChannels();
	miBitDepth = Parser.GetBitDepth();

	// Set base freq, and compensate for sample frequency
//	mfBaseFreq = (tfloat)((tfloat64)Parser.GetBaseFreq() * (tfloat64)iSampleRate / Parser.GetSampleRate());
//	if (mfBaseFreq == 0) {
//		char psz[512];
//		sprintf(psz, "Converted base freq is 0: %s", (tchar*)"Unknown");
//		SoundLoadError((tchar*)psz);
//	}

//	float fSamplesPerCycle = Parser.GetSampleRate() / Parser.GetBaseFreq();
//	mfCycles = 0;
//	if (miLoopEnd != -1) {
//		tfloat64 fSamplesPerCycle = (tfloat64)iSampleRate / mfBaseFreq;
//		mfCycles = (miLoopEnd - miLoopStart + 1) / fSamplesPerCycle;
//	}

//	tfloat64 fSamplesPerSound = (tfloat64)iSampleRate / mfBaseFreq;
//	mfCyclesSound = miSampleCount / fSamplesPerSound;

//	mpChunk = pChunk;
	miDataOffset = Parser.GetDataOffset();
/*#ifdef WIN32
	miDataOffset += mpChunk->GetOffset();
	mpChunk->Destroy();
	mpChunk = NULL;

	mhFile = ::CreateFile(sPathName.c_str(),
		FILE_READ_DATA,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING /*FILE_FLAG_RANDOM_ACCESS*//*,
		NULL);
	ASSERT(mhFile != INVALID_HANDLE_VALUE);
#endif	// WIN32*/

/*#ifdef WIN32
	std::string sRootName = sPathName.substr(0, 3);
	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;
	DWORD dwNrOfClusters;
	::GetDiskFreeSpace(sRootName.c_str(),
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwNrOfClusters);

	miSectorSize = dwBytesPerSector;
	tint32 iSamplesPerSector = dwBytesPerSector / 2;
	miStreamBufferSize = iSamplesPerSector;
	miStreamBuffers = 65536 / miStreamBufferSize;
#else	// WIN32*/
//	miStreamBufferSize = 256 * 2;
//	miStreamBuffers = 256;
	// / 2 is to convert bytes to samples (16-bit)
/*	iBufferSize = mpPlugIn->GetGlobalParm(giSystemBufferSize, giSectionSystem) / 2;
	switch(iBufferSize) {
		case 0:
			iBufferSize = 256;
			break;
		case 1:
			iBufferSize = 512;
			break;
		case 2:
			iBufferSize = 1024;
			break;
		case 3:
			iBufferSize = 2048;
			break;
		case 4:
			iBufferSize = 4096;
			break;
		case 5:
			iBufferSize = 8192;
			break;
	}*/
	iBufferSize = 512;
/*	tint32 iTotalSize = mpPlugIn->GetGlobalParm(giSystemCacheSize, giSectionSystem) / 2;
	switch(iTotalSize) {
		case 0:
			iTotalSize = 64 * 1024;
			break;
		case 1:
			iTotalSize = 128 * 1024;
			break;
		case 2:
			iTotalSize = 256 * 1024;
			break;
		case 3:
			iTotalSize = 512 * 1024;
			break;
		case 4:
			iTotalSize = 1024 * 1024;
			break;
		case 5:
			iTotalSize = 2048 * 1024;
			break;
	}*/
	tint32 iTotalSize = 512 * 1024;
	miStreamBufferSize = iBufferSize;
	miStreamBuffers = iTotalSize / iBufferSize;
/*#endif	// WIN32*/

/*	gbCheck = false;
	miStreamBlocks = miSampleCount / miStreamBufferSize + 1;
	tint32 iBlock;
	miBlockUsageCounter = 0;
//	for (iBlock = 0; iBlock < giStreamBlocks + giStreamBlocksPredictive; iBlock++) {
	for (iBlock = 0; iBlock < miStreamBuffers; iBlock++) {
//		mppStreamBlock[iBlock] = new tfloat32[miStreamBufferSize];
		mppiStreamBlock[iBlock] = new tint16[miStreamBufferSize];
		FillStreamBlock(iBlock, iBlock);
	}
	gbCheck = true;*/

	return true;
}

void CWave_File::CreateEmpty()
{
	mbIsEmpty = true;

	miLoopStart = -1;
	miLoopEnd	= -1;

	miSampleCount = 1000;
	miSoundSize = (miSampleCount + 2) * sizeof(tfloat);

	mfBaseFreq = 440.0f;

	mfCycles = 0;

	tfloat64 fSamplesPerSound = 44100 / mfBaseFreq;
	mfCyclesSound = miSampleCount / fSamplesPerSound;

/*	mpfData = gpfBufferEmpty;
	memset(mpfData, 0, 1000 * sizeof(tfloat32));*/
}

/*void CWave_File::CheckBlock()
{
	return;
	if (gbCheck == false) {
		return;
	}

//	const tint32 iBuffers = giStreamBlocks + giStreamBlocksPredictive;
	const tint32 iBuffers = miStreamBuffers;
	const tint32 iBlocks = 2000;

	tbool pbBlock[iBlocks];
	tint32 iBlock;
	for (iBlock = 0; iBlock < iBlocks; iBlock++) {
		pbBlock[iBlock] = false;
	}

	tint32 iBuffer;
	for (iBuffer = 0; iBuffer < iBuffers; iBuffer++) {
		tint32 iBlock = mpiBlocks[iBuffer];
		ASSERT(pbBlock[iBlock] == false);
		pbBlock[iBlock] = true;
		std::map<tint32, tint32>::iterator it = mStreamMap.find(iBlock);
		ASSERT(it != mStreamMap.end());
	}

	tbool pbBuffer[giMaxBufferBlocks];
	for (iBuffer = 0; iBuffer < iBuffers; iBuffer++) {
		pbBuffer[iBuffer] = false;
	}

	for (iBlock = 0; iBlock < iBlocks; iBlock++) {
		std::map<tint32, tint32>::iterator it = mStreamMap.find(iBlock);
		if (it != mStreamMap.end()) {
			tint32 iBuffer = it->second;

			ASSERT(pbBuffer[iBuffer] == false);
			pbBuffer[iBuffer] = true;

			it++;
//			ASSERT(it == mStreamMap.end());

			std::map<tint32, tint32>::iterator it = mStreamMap.begin();
			while (it != mStreamMap.end()) {
				tint32 a = it->first;
				tint32 b = it->second;

				it++;
			}
		}
	}
}

void CWave_File::AddToList(tint32 iBlock)
{
	tbool bFoundIt = false;
	std::list<tint32>::const_iterator it = mToFillList.begin();
	for (; bFoundIt == false && it != mToFillList.end(); it++) {
		if (*it == iBlock) {
			bFoundIt = true;
		}
	}

	if (bFoundIt == false) {
		mToFillList.push_back(iBlock);
	}
}

void CWave_File::GetDataStream(tfloat32* pfData, tint32 iOffset, tint32 iSamples)
{
	ASSERT(iOffset >= 0);
	ASSERT(iSamples >= 0);
	ASSERT(iSamples <= giMaxStreamBufferSize);

	CAutoLock Lock(mMutex);

	if (mbIsEmpty) {
		memset(pfData, 0, iSamples * sizeof(tfloat32));
		return;
	}

//	ASSERT(mpChunk);

	tint iToGo = iSamples;
	tint iIndex = 0;
	while (iToGo) {
		tint32 iIndexSrc = iOffset + iIndex;
		tint32 iIndexSrcMod = iIndexSrc % miStreamBufferSize;
		tint32 iStreamBlock = iIndexSrc / miStreamBufferSize;

		// Samples left in buffer
		tint iSizeThis = iToGo;
		// Make sure we don't copy too much
		if (iSizeThis > miStreamBufferSize - iIndexSrcMod) {
			iSizeThis = miStreamBufferSize - iIndexSrcMod;
		}

		std::map<tint32, tint32>::const_iterator it = mStreamMap.find(iStreamBlock);
		if (it == mStreamMap.end()) {
//			AddToList(iStreamBlock);
			AddToList(iStreamBlock + 1);

			tint32 iFilePos = miDataOffset + (iOffset + iIndex) * 2;
			int iPos = mpChunk->Seek(iFilePos);
			ASSERT (iPos == iFilePos);

			mpChunk->Read((tchar*)gpfData2, iSizeThis * 2);
#ifdef _Mac_PowerPC
			{
				tint32 iIndex;
				for (iIndex = 0; iIndex < iSizeThis; iIndex++) {
					(((tint16*)gpfData2)[iIndex]) = CPSwap((((tint16*)gpfData2)[iIndex]));
				}
			}
#endif	// _Mac_PowerPC

			tint i;
			for (i = 0; i < iSizeThis; i++) {
//				const tchar* pSrc2 = ((tchar*)gpfData2) + i * 3;
				const tchar* pSrc2 = ((tchar*)gpfData2) + i * 2;
				tint16 sample = *((tint16*)pSrc2);
				tfloat f = (tfloat)(sample / 32767.0);

				pfData[i + iIndex] = f * 2.0f;
			}

			// Load from hd
		}
		else {
			tint32 iBuffer = it->second;

//			memcpy(pfData + iIndex, mppStreamBlock[iBuffer] + iIndexSrcMod, iSizeThis * sizeof(tfloat32));
			tint16* pSrc = mppiStreamBlock[iBuffer] + iIndexSrcMod;
			tfloat* pDest = pfData + iIndex;
			tint i;
			for (i = 0; i < iSizeThis; i++) {
				pDest[i] = (tfloat)(pSrc[i] / 32767.0);
			}

			mpiBlockUsage[iBuffer] = miBlockUsageCounter;
			miBlockUsageCounter++;

			AddToList(iStreamBlock + 1);
			AddToList(iStreamBlock + 2);
			AddToList(iStreamBlock + 3);
			AddToList(iStreamBlock + 4);
//			AddToList(iStreamBlock + 5);
//			AddToList(iStreamBlock + 6);
		}

		iIndex += iSizeThis;
		iToGo -= iSizeThis;
	}
}

inline tint32 ReadFromFile(IChunk* pChunk, tint32 iFilePos, tchar* pfData, tint32 iSamples, tint32 iSectorSize, CMutex* pMutex)
{
	int iPos = pChunk->Seek(iFilePos);
	ASSERT (iPos == iFilePos);

	{
		CLockReleaser Releaser(pMutex);

		pChunk->Read(pfData, iSamples * 2);
#ifdef _Mac_PowerPC
		{
			tint32 iIndex;
			for (iIndex = 0; iIndex < iSamples; iIndex++) {
				(((tint16*)pfData)[iIndex]) = CPSwap((((tint16*)pfData)[iIndex]));
			}
		}
#endif	// _Mac_PowerPC
	}

	return 0;
}

void CWave_File::FillStreamBlock(tint32 iBuffer, tint32 iBlock)
{
	mpiBlockUsage[iBuffer] = miBlockUsageCounter;
	miBlockUsageCounter++;

	mStreamMap.insert(std::pair<tint32, tint32>(iBlock, iBuffer));
	mpiBlocks[iBuffer] = iBlock;

//	tfloat32* pfData = mppStreamBlock[iBuffer];
	tint16* piData = mppiStreamBlock[iBuffer];
	const tint32 iOffset = iBlock * miStreamBufferSize;
	const tint32 iSamples = miStreamBufferSize;

//	ASSERT(mpChunk);

	if (iBlock == miStreamBlocks - 1) {
		// Last block, so fill with zeroes first, in case we don't have enough samples
//		memset(gpfData, 0, iSamples * 2);
		memset(piData, 0, iSamples * 2);
	}
	if (iBlock >= miStreamBlocks) {
//		memset(gpfData, 0, iSamples * 2);

		CheckBlock();

		return;
	}

//	int iOffset2 = ReadFromFile(/mpChunk, miDataOffset + iOffset * 2, (tchar*)gpfData, iSamples, miSectorSize, &mMutex);
	int iOffset2 = ReadFromFile(/mpChunk, miDataOffset + iOffset * 2, (tchar*)piData, iSamples, miSectorSize, &mMutex);

	CheckBlock();
}

void CWave_File::LoadStreamBlock(tint32 iBlock)
{
	// First find buffer to clear
	// Note, we never clear the first buffers
	tint32 iBufferStart = miStreamBuffers / 8;
	if (iBufferStart <= 1) {
		iBufferStart = 1;
	}
	tint32 iBuffer = iBufferStart;
	tint32 iCounterValue = mpiBlockUsage[iBuffer];
	tint32 iBuf;
//	for (iBuf = 2; iBuf < giStreamBlocks + giStreamBlocksPredictive; iBuf++) {
	for (iBuf = iBufferStart; iBuf < miStreamBuffers; iBuf++) {
		if (mpiBlockUsage[iBuf] < iCounterValue) {
			iBuffer = iBuf;
			iCounterValue = mpiBlockUsage[iBuf];
		}
	}

	// Remove from map
	std::map<tint32, tint32>::iterator it = mStreamMap.find(mpiBlocks[iBuffer]);
	ASSERT(it != mStreamMap.end());
	mStreamMap.erase(it);

	// Do it
	FillStreamBlock(iBuffer, iBlock);

	CheckBlock();

}

void CWave_File::LoadOne()
{
	CAutoLock Lock(mMutex);

	std::list<tint32> ToDelete;

	std::list<tint32>::iterator it = mToFillList.begin();
	while (it != mToFillList.end()) {
		std::map<tint32, tint32>::iterator it2 = mStreamMap.find(*it);
		if (it2 == mStreamMap.end()) {
			// Not loaded, so load it
			{
//				CLockReleaser Releaser(mMutex);

				LoadStreamBlock(*it);
			}
			ToDelete.push_back(*it);
			break;
		}
		else {
			// Already loaded
			ToDelete.push_back(*it);
			it++;
		}
	}

	std::list<tint32>::iterator it2 = ToDelete.begin();
	for (; it2 != ToDelete.end(); it2++) {
		mToFillList.remove(*it2);
	}
	ToDelete.clear();
}*/

void CWave_File::Timer()
{
//	while (mToFillList.size() != 0) {
//		LoadOne();
//	}

/*	CheckBlock();

	LoadOne();
//	LoadOne();
//	LoadOne();

	CheckBlock();*/
}


/*void CWave_File::GetDataStreamAsync(tfloat32* pfData, tint32 iOffset, tint32 iSamples)
{
#ifdef WIN32
	if (mbIsEmpty) {
		memset(pfData, 0, iSamples * sizeof(tfloat32));
		return;
	}

	ASSERT(mpChunk);

	tint32 iFilePos = miDataOffset + iOffset * 3;
	int iPos = mpChunk->Seek(iFilePos);
	ASSERT (iPos == iFilePos);

	mpChunk->Read((tchar*)mpfData, iSamples * 3);

	tint i;
	for (i = 0; i < iSamples; i++) {
		const tchar* pSrc2 = ((tchar*)mpfData) + i * 3;
		SSample24 sample = *((SSample24*)pSrc2);
		tint32 ii = (sample.c << 24) >> 8;
		ii		|=	(sample.b << 8);
		ii		|=	sample.a;
		tfloat f = (tfloat)(ii / 8388607.0);

		pfData[i] = f * 2.0f;
	}
#endif	// WIN32
}*/

/*void CWave_File::CompleteAsyncTransfer()
{
#ifdef WIN32
	if (mbHasAsyncTransfer == false) {
		return;
	}
#endif	// WIN32
}*/

void CWave_File::GetStreamInfo(IFile*& rpFile, tint32& riOffset, tint32& riLength, tint32* piBitWidth, tint32* piChannels)
{
	rpFile = mpFile;

	riOffset = miDataOffset;
	riLength = miSampleCount;
	*piBitWidth = miBitDepth;
	*piChannels = miChannels;
}





