

#include "KSOS.h"

//tint16* gpiData = NULL;
//tfloat32* gpfData2 = NULL;

/*
	Error reporting
*/
/*
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
*/

/*
	CWave_Parser
*/

CWave_Parser::CWave_Parser()
{
	miSampleRate = 0;
	miLoopStart = -1;
	miLoopEnd = -1;
	mfBaseFreq = 0;
	miSampleCount = 0;
	mpData = NULL;
}

CWave_Parser::~CWave_Parser()
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

//bool CWave_Parser::Load(const tchar* pszPathName)
bool CWave_Parser::Load(IFile* pFile, tbool bLoadData)
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





