

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

	iBufferSize = 512;

	tint32 iTotalSize = 512 * 1024;
	miStreamBufferSize = iBufferSize;
	miStreamBuffers = iTotalSize / iBufferSize;


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



void CWave_File::Timer()
{

}


void CWave_File::GetStreamInfo(IFile*& rpFile, tint32& riOffset, tint32& riLength, tint32* piBitWidth, tint32* piChannels)
{
	rpFile = mpFile;

	riOffset = miDataOffset;
	riLength = miSampleCount;
	*piBitWidth = miBitDepth;
	*piChannels = miChannels;
}





