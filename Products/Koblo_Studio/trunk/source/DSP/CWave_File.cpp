

#include "KSOS.h"


/*
	Error reporting
*/

void SoundLoadError(const tchar* psz)
{
#ifdef WIN32
	::MessageBox(NULL, (char*)psz, NULL, MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#endif	// WIN32
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





