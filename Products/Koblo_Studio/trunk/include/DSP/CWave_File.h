// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.





class CWave_File : 
public virtual CLoad_Error
{
public:
	CWave_File() {
//		mpPlugIn = pPlugIn;
//		mpChunk = NULL;
		mpFile = NULL;
		miSampleCount = 0;
		mpfData = NULL;
		mfBaseFreq = 0;
		miSoundSize = 0;
		mbIsEmpty = false;
	}
	virtual ~CWave_File() {

		if (mpfData) {
			if (mbIsEmpty == false) {
				delete[] mpfData;
			}
		}
		if (mpFile) {
			mpFile->Destroy();
		}

	}


	tbool LoadSound(tint32 iSampleRate, IChunk* pChunk);


	tbool LoadSoundStream(tint32 iBufferSize, const tchar* pszPathName);

	void SetBaseFreq(float fNew) {
		mfBaseFreq = fNew;
	}

	tint				GetSampleCount()	const	{return miSampleCount;}
	tfloat64			GetCyclesPerLoop()	const	{return mfCycles;}
	tfloat64			GetCycles()			const	{return mfCyclesSound;}
	const tfloat*	GetData()			const	{return mpfData;}
	void GetDataStream(tfloat32* pfData, tint32 iOffset, tint32 iSamples);

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

	tint32 miBitDepth;
	tint32 miChannels;

	CMutex mMutex;
};








