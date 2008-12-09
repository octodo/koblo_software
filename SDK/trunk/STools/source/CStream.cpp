/*!	\file CStream.cpp
	\author Michael Olsen
	\date 19/Jun/2007
*/

#include "SToolsInternalOS.h"

CStream::CStream()
{
	mpFile = NULL;
	miFileOffset = 0;
	miSamples = 0;

	miBuffersToFill = 0;
	miInCriticalCode_Level = 0;

//	mpBuffers.resize(giNrOfBuffers);
	tint32 iBuffer;
	for (iBuffer = 0; iBuffer < giNrOfBuffers; iBuffer++) {
		mpBuffers[iBuffer] = NULL;
	}

	mbStutter = false;
} // constructor


CStream::~CStream()
{
	if (mpBuffers[0]) {
		delete[] mpBuffers[0];
	}
} // destructor


st::IStream* st::CStream::Create()
{
	return dynamic_cast<st::IStream*>(new CStream());
} // Create


void CStream::Destroy()
{
	delete this;
} // Destroy


void CStream::Reset(IFile* pFile, tint32 iOffset, tint32 iLength, tbool bLoop, tint16* piInitialData, tint32 iBitWidth /*= 24*/, tint32 iChannels /*= 1*/)
{
	// Take lock
	miInCriticalCode_Level++;
	while (miInCriticalCode_Level != 1) {
		// We can't do this while OnTimer is running - release some time
		ITime::SleepMS(1);
	}

	mpFile = pFile;

	miFileOffset = iOffset;
	miSamples = iLength;
	miCurIndex = 0;

	mbLoop = bLoop;

	// Create buffers
	// The buffers are allocated as 1 large buffer, so we only need to delete the first
	if (mpBuffers[0]) {
		delete[] mpBuffers[0];
	}

	// Allocate 1 large buffer, then distrubute the data
	SSample24* pBuffer = new SSample24[giBufferSize * giNrOfBuffers];
	if (iBitWidth == 24) {
		tint32 iBuffer;
		for (iBuffer = 0; iBuffer < giNrOfBuffers; iBuffer++) {
			mpBuffers[iBuffer] = pBuffer;
			pBuffer += giBufferSize;
		}
	}
	else { // if (iBitWidth == 16) {
		tint16* pi16 = (tint16*)pBuffer;
		for (tint32 iBuffer = 0; iBuffer < giNrOfBuffers; iBuffer++) {
			mpBuffers[iBuffer] = (SSample24*)pi16;
			pi16 += giBufferSize;
		}
	}

	mbEmpty = false;

	miBitWidth = iBitWidth;
	miByteWidth = iBitWidth / 8;

	mbStutter = false;

	// Fill the buffers
/*	tint32 iToRead = miSamples - miCurIndex;
	if (iToRead >= giBufferSize) {
		memcpy(mpBuffers[0], piInitialData, giBufferSize * sizeof(tint16));

		miCurIndex += giBufferSize;
	}
	else {
		memset(mpBuffers[0], 0, giBufferSize * sizeof(tint16));

		memcpy(mpBuffers[0], piInitialData, iToRead * sizeof(tint16));

		mbEmpty = true;
	}*/

	miBufferCur = 0;
	miBufferIndex = 0;
//	miNextBufferToFill = 1;
//	miBuffersToFill = giNrOfBuffers - 1;
	miNextBufferToFill = 0;

	if (((miBitWidth != 16) && (miBitWidth != 24)) || (iChannels != 1)) {
		miBuffersToFill = 0;
		mbEmpty = true;
		mpFile = NULL;
		miByteWidth = 3;
		miBitWidth = 24;
	}
	else {
		miBuffersToFill = giNrOfBuffers;
	}

	// Release
	miInCriticalCode_Level--;
} // Reset


tbool CStream::GetStutter()
{
	if (!mbStutter)
		return false;

	mbStutter = false;
	return true;
} // GetStutter


void CStream::OnTimer()
{
	// Don't catch up with ourselves
	if (++miInCriticalCode_Level != 1) {
		// Release and exit
		miInCriticalCode_Level--;
		return;
	}

	// Note: Is it possible that 2 streams with same file can read at same time? That might mess up the seeking
	//CAutoLock Lock(mMutex);

	if (miBuffersToFill > 0) {
		// Buffers this time
		tint32 iBuffersThisTime = miBuffersToFill;
		if (iBuffersThisTime > giNrOfBuffersToReadMax) {
			iBuffersThisTime = giNrOfBuffersToReadMax;
		}
		// Make sure we don't "wrap around"
		if (giNrOfBuffers - miNextBufferToFill < iBuffersThisTime) {
			iBuffersThisTime = giNrOfBuffers - miNextBufferToFill;
		}

		if (mbEmpty) {
			memset(mpBuffers[miNextBufferToFill], 0, iBuffersThisTime * giBufferSize * miByteWidth);
		}
		else {
			tint64 iToRead = miSamples - miCurIndex;
			if (iToRead >= giBufferSize * iBuffersThisTime) {
				if (mpFile) {
					tint64 iWantedPos = miCurIndex * miByteWidth + miFileOffset;
					if (mpFile->GetCurrentFilePosition() != iWantedPos)
						mpFile->Seek(iWantedPos);
					mpFile->Read((tchar*)(mpBuffers[miNextBufferToFill]), iBuffersThisTime * giBufferSize * miByteWidth);
				}
				else {
					memset(mpBuffers[miNextBufferToFill], 0, iBuffersThisTime * giBufferSize * miByteWidth);
				}

				miCurIndex += (giBufferSize * iBuffersThisTime);
			}
			else {
				if (mbLoop) {
					// How many we still need to read
					tint32 iToGo = iBuffersThisTime * giBufferSize;
					tint32 iRead = 0;
					tint32 iBytesRead = 0;
					while (iToGo) {
						// How many we do this round of the loop
						tint32 iThisTime = iToGo;
						if (miSamples - miCurIndex < iThisTime) {
							iThisTime = (tint32)(miSamples - miCurIndex);
						}

						if (mpFile) {
							tint64 iWantedPos = miCurIndex * miByteWidth + miFileOffset;
							if (mpFile->GetCurrentFilePosition() != iWantedPos)
								mpFile->Seek(iWantedPos);
							mpFile->Read(((tchar*)(mpBuffers[miNextBufferToFill])) + iBytesRead, iThisTime * miByteWidth);
						}
						else {
							memset(mpBuffers[miNextBufferToFill], 0, iThisTime * miByteWidth);
						}

						iRead += iThisTime;
						iBytesRead += iThisTime * miByteWidth;
						iToGo -= iThisTime;

						miCurIndex += iThisTime;
						ASSERT(miCurIndex <= miSamples);
						if (miCurIndex == miSamples) {
							miCurIndex = 0;
						}
					}
				}
				else {
					memset(mpBuffers[miNextBufferToFill], 0, iBuffersThisTime * giBufferSize * miByteWidth);

					if (mpFile) {
						tint64 iWantedPos = miCurIndex * miByteWidth + miFileOffset;
						if (mpFile->GetCurrentFilePosition() != iWantedPos)
							mpFile->Seek(iWantedPos);
						mpFile->Read((tchar*)(mpBuffers[miNextBufferToFill]), iToRead * miByteWidth);
					}

					mbEmpty = true;
				}
			}
		}

		tint32 iBuffer;
		for (iBuffer = 0; iBuffer < iBuffersThisTime; iBuffer++) {
			miNextBufferToFill++;
			if (miNextBufferToFill == giNrOfBuffers) {
				miNextBufferToFill = 0;
			}

			miBuffersToFill--;
		}
	}

	// Release
	miInCriticalCode_Level--;
} // OnTimer


void CStream::GetSamplesFrom1Buffer(tfloat32* pfBuffer, tint32 iCount)
{
	//while (miBuffersToFill >= giNrOfBuffers) {
	tbool bLockTaken = false;
	tint32 iToFillNow = 0;

	tint32 iSamplesMissing;
	while (1) {
		tint32 iBuffersReady = giNrOfBuffers - miBuffersToFill;
		tint32 iSamplesReady = iBuffersReady * giBufferSize;
		iSamplesReady -= miBufferIndex;
		iSamplesMissing = iCount - iSamplesReady;
		if ((iSamplesMissing <= 0) || (mbEmpty)) {
			// We don't need to read any extra samples - or there are no more
			break;
		}
		if (bLockTaken) {
			// We have already taken lock

			// Is it ok to do a disk read now?
			if (miInCriticalCode_Level == 1) {
				// Yes. We're alone (OnTimer has finished)

				// Go for it
				break;
			}
		}
		else {
			// It seems we need to read - take lock
			miInCriticalCode_Level++;
			bLockTaken = true;
		}
		tint32 iStutterWarningCounter = 0;
		while (miInCriticalCode_Level != 1) {
			// We need to read more samples, but OnTimer is running - release some time
			// After delay OnTimer may have filled buffers, so we'll calculate again
			ITime::SleepMS(1);
			// Maybe warn about stutter
			if (++iStutterWarningCounter > 50)
				mbStutter = true;
		}
	}

	if (iSamplesMissing > 0) {
		// We need more data

		// This is bad - there will be a stutter (silent period)
		mbStutter = true;

		// How many buffers
		tint32 iBuffersMissing = (iSamplesMissing + giBufferSize - 1) / giBufferSize;
		if (iBuffersMissing > miBuffersToFill) {
			// That's too bad - we're asking for more than we can get - handle gracefully
			iToFillNow = miBuffersToFill;
		}
		else if (iBuffersMissing > giNrOfBuffersToReadMax) {
			// Mustn't read that many buffers at a time (disk access pauses other operations)
			iToFillNow = giNrOfBuffersToReadMax;
		}
		else {
			// Load exactly what we need
			iToFillNow = iBuffersMissing;
		}
	}
	while (iToFillNow > 0) {
		// We need to read first
		if (mbEmpty) {
			memset(mpBuffers[miNextBufferToFill], 0, giBufferSize * miByteWidth);
		}
		else {
			tint64 iToRead = miSamples - miCurIndex;
			if (iToRead >= giBufferSize) {
				tint64 iWantedPos = miCurIndex * miByteWidth + miFileOffset;
				if (mpFile->GetCurrentFilePosition() != iWantedPos)
					mpFile->Seek(iWantedPos);
				mpFile->Read((tchar*)(mpBuffers[miNextBufferToFill]), giBufferSize * miByteWidth);
				miCurIndex += giBufferSize;
			}
			else {
				memset(mpBuffers[miNextBufferToFill], 0, giBufferSize * miByteWidth);

				tint64 iWantedPos = miCurIndex * miByteWidth + miFileOffset;
				if (mpFile->GetCurrentFilePosition() != iWantedPos)
					mpFile->Seek(iWantedPos);
				mpFile->Read((tchar*)(mpBuffers[miNextBufferToFill]), iToRead * miByteWidth);

				mbEmpty = true;
			}
		}

		miNextBufferToFill++;
		if (miNextBufferToFill == giNrOfBuffers) {
			miNextBufferToFill = 0;
		}

		miBuffersToFill--;
		iToFillNow--;

/*		static int a = 0;
		if (a == 0) {
			a = 1;
			::MessageBox(NULL, "Missed one", NULL, MB_OK);
		}*/
	}

	//SSample24* pSrc = mpBuffers[miBufferCur];
	//pSrc += miBufferIndex;
	SSample24* pSrc = (SSample24*)(((tchar*)mpBuffers[miBufferCur]) + (miBufferIndex * miByteWidth));

	tint32 iToCopy = iCount;
	if (iToCopy > giBufferSize - miBufferIndex) {
		iToCopy = giBufferSize - miBufferIndex;
	}

//	tint32 i;
//	for (i = 0; i < iToCopy; i++) {
//		pfBuffer[i] = (tfloat)(pSrc[i] / 32767.0) * 2.0f;
//	}

	if (miBitWidth == 24) {
		tint32 i;
		for (i = 0; i < iToCopy; i++) {
			const tchar* pSrc2 = ((tchar*)pSrc) + i * miByteWidth;
			SSample24 sample = *((SSample24*)pSrc2);
			tint32 ii = (sample.c << 24) >> 8;
			ii		|=	(sample.b << 8);
			ii		|=	sample.a;
			tfloat f = (tfloat)(ii / 8388607.0);

			pfBuffer[i] = f;
		}
	}
	else { //if (miBitWidth == 16) {
		const tfloat32 fRangeMax = 1.0f;
		const tfloat32 fRangeMin = -1.0f;
		tint32 iSamples = iToCopy;

		tchar* pcPCM16Mono = (tchar*)pSrc;
		tchar* pcOutput = (tchar*)pfBuffer;

		// We go from range -32768 => 32767
		const tint32 kiAdditor = 0x8000;
		tfloat32 fRangeFull = fRangeMax - fRangeMin;
		tfloat32 kfDivisor = (((tfloat32)0xFFFF) / fRangeFull);

		for ( ; iSamples > 0; iSamples--) {
			tint32 i32 = CPSwap(*((tint16*)pcPCM16Mono));
			pcPCM16Mono += 2;

			// Convert range -32768 => 32767 to range -1.0 => 1.0
			i32 += kiAdditor;
			tfloat32 f32 = (tfloat32)i32;
			f32 /= kfDivisor;
			f32 += fRangeMin;

			*((tfloat32*)pcOutput) = f32;
			pcOutput += 4;
		}
	}

	miBufferIndex += iToCopy;
	if (miBufferIndex > giBufferSize) {
		ASSERT(0);
	}
	if (miBufferIndex == giBufferSize) {
		miBufferCur++;
		if (miBufferCur == giNrOfBuffers) {
			miBufferCur = 0;
		}

		miBuffersToFill++;

		miBufferIndex = 0;
	}

	// Release
	if (bLockTaken) {
		miInCriticalCode_Level--;
	}
} // GetSamplesFrom1Buffer


void CStream::GetSamples(tfloat32* pfBuffer, tint32 iCount)
{
	/*
	// Wait for OnTimer thread to conclude
	while (++miInCriticalCode_Level != 1) {
		ITime::SleepMS(0);
		miInCriticalCode_Level--;
	}
	**/
	//miInCriticalCode_Level++;

	//CAutoLock Lock(mMutex);

	tint32 iToGo = iCount;
	tint32 iIndex = 0;
	while (iToGo) {
		tint32 iSamplesToGet = iToGo;
		if (giBufferSize - miBufferIndex < iSamplesToGet) {
			// Not enough samples in the buffer, so split it
			iSamplesToGet = giBufferSize - miBufferIndex;
		}

		GetSamplesFrom1Buffer(pfBuffer + iIndex, iSamplesToGet);

		iIndex += iSamplesToGet;
		iToGo -= iSamplesToGet;
	}

	// Release
	//miInCriticalCode_Level--;
} // GetSamples


void CStream::SetPosition(tuint64 uiPos)
{
	/*
	// Don't flush unless forced
	if (uiPos == miCurIndex)
		return;
	*/

	// Take lock
	miInCriticalCode_Level++;
	while (miInCriticalCode_Level != 1) {
		// We can't do this while OnTimer is running - release some time
		ITime::SleepMS(1);
	}

	miCurIndex = uiPos;

	miBufferCur = 0;
	miBufferIndex = 0;
	miNextBufferToFill = 0;
	miBuffersToFill = giNrOfBuffers;

	mbEmpty = false;

	mbStutter = false;

	// Release lock
	miInCriticalCode_Level--;

	// Now forcefully run OnTimer to get samples (avoid stutter)
	OnTimer();
} // SetPosition
