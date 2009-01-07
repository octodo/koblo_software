
class CWaveParser
{
public:
	CWaveParser();
	virtual ~CWaveParser();

//	bool Load(const tchar* pszPathName);
//	bool Load(IChunk* pChunk, tbool bLoadData = true);
	bool Load(IFile* pFile, tbool bLoadData = true);

	tint			GetSampleRate()		const	{return miSampleRate;}
	// Returns -1 if no loop
	tint			GetLoopStart()		const	{return miLoopStart;}
	// Returns -1 if no loop
	tint			GetLoopEnd()		const	{return miLoopEnd;}
	tfloat		GetBaseFreq()		const	{return mfBaseFreq;}
	tint			GetSampleCount()	const	{return miSampleCount;}
	const tchar*	GetData()			const	{return mpData;}
	tint32 GetDataOffset() const {return miDataOffset;}

	tint32 GetBitDepth() {return miBitDepth;}
	tint32 GetSampleRate() {return miSampleRate;}
	tint32 GetChannels() {return miChannels;}

protected:
	// This is the info we want to read
	tint		miSampleRate;
	tint		miLoopStart;
	tint		miLoopEnd;
	tfloat	mfBaseFreq;
	tint		miSampleCount;
	tchar*	mpData;
	tint32 miDataOffset;

	tint32 miBitDepth;
//	tint32 miSampleRate;
	tint32 miChannels;
};


class CWave_File
{
public:
	CWave_File(/*CINetSynth3PlugIn* pPlugIn*/) {
//		mpPlugIn = pPlugIn;
//		mpChunk = NULL;
		mpFile = NULL;
		miSampleCount = 0;
		mpfData = NULL;
		mfBaseFreq = 0;
		miSoundSize = 0;
		mbIsEmpty = false;
/*		for (tint iBlock = 0; iBlock < giMaxBufferBlocks; iBlock++) {
//			mppStreamBlock[iBlock] = NULL;
			mppiStreamBlock[iBlock] = NULL;
		}*/
//		mbHasAsyncTransfer = false;
/*#ifdef WIN32
		mhFile = INVALID_HANDLE_VALUE;
#endif	// WIN32*/
	}
	virtual ~CWave_File() {
//		if (mppStreamBlock[0]) {
/*		if (mppiStreamBlock[0]) {
			tint32 iBlock;
//			for (iBlock = 0; iBlock < giStreamBlocks + giStreamBlocksPredictive; iBlock++) {
			for (iBlock = 0; iBlock < giMaxBufferBlocks; iBlock++) {
				if (mppiStreamBlock[iBlock]) {
					delete[] mppiStreamBlock[iBlock];
				}
			}
		}*/
		if (mpfData) {
			if (mbIsEmpty == false) {
				delete[] mpfData;
			}
		}
		if (mpFile) {
			mpFile->Destroy();
		}
/*#ifdef WIN32
		if (mhFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(mhFile);
		}
#endif	// WIN32*/
	}

//	tbool LoadSound(tint32 iSampleRate, const tchar* pszPathName);
	tbool LoadSound(tint32 iSampleRate, IChunk* pChunk);

	// Loads headers, but not data (used for streaming). The last parameter is the max number of sample which will be requested at any given call.
//	tbool LoadSoundStream(tint32 iSampleRate, IChunk* pChunk, tint32 iBufferSize, const std::string& sPathName);

	tbool LoadSoundStream(tint32 iBufferSize, const tchar* pszPathName);

	void SetBaseFreq(float fNew) {
		mfBaseFreq = fNew;
	}

	tint				GetSampleCount()	const	{return miSampleCount;}
	tfloat64			GetCyclesPerLoop()	const	{return mfCycles;}
	tfloat64			GetCycles()			const	{return mfCyclesSound;}
	const tfloat*	GetData()			const	{return mpfData;}
	void GetDataStream(tfloat32* pfData, tint32 iOffset, tint32 iSamples);
//	void GetDataStreamAsync(tfloat32* pfData, tint32 iOffset, tint32 iSamples);
//	void CompleteAsyncTransfer();
	tfloat			GetBaseFreq()		const	{return mfBaseFreq;}
	// Returns -1 if no loop
	tint			GetLoopStart()		const	{return miLoopStart;}
	// Returns -1 if no loop
	tint			GetLoopEnd()		const	{return miLoopEnd;}

	//! Returns the sounds loaded size in bytes. Only works if the sound has been loaded (otherwise 0 will be returned)
	tint32		GetSoundSize() const {return miSoundSize;}

	void CreateEmpty();

	void Timer();
	void LoadOne();
	void AddToList(tint32 iBlock);
/*#ifdef WIN32
	HANDLE mhFile;
#endif	// WIN32*/

	tint32 miStreamBufferSize;
	tint32 miStreamBuffers;
	// Sector size in bytes
	tint32 miSectorSize;

	virtual void GetStreamInfo(IFile*& pFile, tint32& riOffset, tint32& riLength, tint32* piBitWidth, tint32* piChannels);

protected:
	// Number of samples in sound
	tint			miSampleCount;
	tfloat*		mpfData;

	// Nr of cycles in 1 "loop"
	tfloat64		mfCycles;

	// Nr of cycles in sound
	tfloat64		mfCyclesSound;

	// Frequency sound was recorded at
	tfloat		mfBaseFreq;

	tint		miLoopStart;
	tint		miLoopEnd;

	tint32		miSoundSize;

	tint32 miDataOffset;
//	IChunk* mpChunk;
	IFile* mpFile;

	tbool mbIsEmpty;

//	tbool mbHasAsyncTransfer;

/*	tint32 miStreamBlocks;
//	tfloat32* mppStreamBlock[giStreamBlocks + giStreamBlocksPredictive];
//	tfloat32* mppStreamBlock[giMaxBufferBlocks];
	tint16* mppiStreamBlock[giMaxBufferBlocks];
	// First value: Stream block, second value: Stream buffer
	std::map<tint32, tint32> mStreamMap;
	// Reverse lookup
//	tint32 mpiBlocks[giStreamBlocks + giStreamBlocksPredictive];
	tint32 mpiBlocks[giMaxBufferBlocks];
//	tint32 mpiBlockUsage[giStreamBlocks + giStreamBlocksPredictive];
	tint32 mpiBlockUsage[giMaxBufferBlocks];
	tint32 miBlockUsageCounter;

	void FillStreamBlock(tint32 iBuffer, tint32 iBlock);
	void LoadStreamBlock(tint32 iBlock);
	void CheckBlock();


	std::list<tint32> mToFillList;

	CINetSynth3PlugIn* mpPlugIn;*/

	tint32 miBitDepth;
	tint32 miChannels;

	CMutex mMutex;
};








