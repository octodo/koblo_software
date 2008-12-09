

#include "SToolsInternalOS.h"

//#include <process.h>

IWaveOscStream* IWaveOscStream::Create()
{
	return dynamic_cast<IWaveOscStream*>(new CWaveOscStream());
}

void CWaveOscStream::Destroy()
{
	delete dynamic_cast<CWaveOscStream*>(this);
}

CWaveOscStream::CWaveOscStream()
	: /*mppfBuffers(0),*/ mpCallback(0),
	mpfTmpBuffer(0)
{
	mpResampler = IResampler::Create();
}

CWaveOscStream::~CWaveOscStream()
{
	DeInit();

	mpResampler->Destroy();

/*	if (mppfBuffers) {
		int iBuffer;
		for (iBuffer = 0; iBuffer < miBufferCount; iBuffer++) {
			delete[] mppfBuffers[iBuffer];
		}

		delete[] mppfBuffers;
	}*/

	if (mpfTmpBuffer) {
		delete[] mpfTmpBuffer;
	}
}

void CWaveOscStream::Reset()
{
	CAutoLock Lock(Mutex);

	mpResampler->Reset();

	mpfTmpBuffer[0] = mpfTmpBuffer[1] = mpfTmpBuffer[2] = mpfTmpBuffer[3] = 0;

	mpCallback->Reset(miCallbackID);

	// Reset, and fill the buffers
/*	ASSERT(mpCallback);
	miBufferCurWrite = 0;
//	tint iBuffer;
	mpCallback->Reset(miCallbackID);*/
/*	for (iBuffer = 0; iBuffer < miBufferCount; iBuffer++) {
		mpCallback->FillBuffer(miCallbackID, mppfBuffers[iBuffer], miBufferSize);
	}*/

/*	miBufferCurRead = 0;
	miBufferReadIndex = 0;
	mpbBufferFilled[0] = mpbBufferFilled[1] =
	mpbBufferFilled[2] = mpbBufferFilled[3] =
	mpbBufferFilled[4] = mpbBufferFilled[5] =
	mpbBufferFilled[6] = mpbBufferFilled[7] = false;*/
}

void CWaveOscStream::SetCallback(IWaveOscStreamCallback* pCallback, int iID)
{
	mpCallback = pCallback;
	miCallbackID = iID;
}

void CWaveOscStream::SetBufferSize(int iBufferSize)
{
	miBufferSize = iBufferSize;
}

void CWaveOscStream::SetBufferCount(int iBufferCount)
{
	miBufferCount = iBufferCount;

	// Create memory
/*	ASSERT(mppfBuffers == 0);
	mppfBuffers = new sample*[miBufferCount];
	int iBuffer;
	for (iBuffer = 0; iBuffer < miBufferCount; iBuffer++) {
		mppfBuffers[iBuffer] = new sample[miBufferSize];
	}

	// Fill the buffers
	ASSERT(mpCallback);
	miBufferCurWrite = 0;
	for (iBuffer = 0; iBuffer < miBufferCount; iBuffer++) {
//		mpCallback->FillBuffer(miCallbackID, mppfBuffers[iBuffer], miBufferSize);
	}*/

/*	miBufferCurRead = 0;
	miBufferReadIndex = 0;*/
}

void CWaveOscStream::SetMaxBufferSize(int iMaxOutputBufferSize)
{
	miMaxOutputBufferSize = iMaxOutputBufferSize;
}

void CWaveOscStream::SetMaxPitch(float fMaxPitch)
{
	mfMaxPitch = fMaxPitch;

	ASSERT(mpfTmpBuffer == 0);
	int iSize = (int)(miMaxOutputBufferSize * fMaxPitch + 8 + 4);
	mpfTmpBuffer = new tfloat32[iSize];

	Reset();

	Init();
}

void CWaveOscStream::Process(tfloat32* pfOut, tfloat32 fPitch, int iC)
{
	CAutoLock Lock(Mutex);

	ASSERT(fPitch <= mfMaxPitch);

	int iSamplesNeeded = mpResampler->GetNrOfInputSamplesNeeded(fPitch, iC);

	ASSERT(mpCallback);
	mpCallback->FillBuffer(miCallbackID, mpfTmpBuffer + 4, iSamplesNeeded);

	mpResampler->Resample(iC, iSamplesNeeded, fPitch, pfOut, (float*)mpfTmpBuffer);

//	if (miBufferCurRead == miBufferCurWrite) {
//	if (miBufferCurRead != miBufferCurWrite) {
		// Need to fill one more buffer
//		FillBuffer();
//		ASSERT(miBufferCurRead != miBufferCurWrite);
//	}

/*	int iSamplesNeeded = mpResampler->GetNrOfInputSamplesNeeded(fPitch, iC);*/

/*	if (miBufferSize - miBufferReadIndex >= iSamplesNeeded) {
		// Easy and fast version, we have a non-fragmented memory we can use
		sample* pfBuf = mppfBuffers[miBufferCurRead];
		pfBuf += miBufferReadIndex;

		mpResampler->Resample(iC, iSamplesNeeded, fPitch, pfOut, (float*)pfBuf);

		miBufferReadIndex += iSamplesNeeded;

		if (miBufferReadIndex == miBufferSize) {
			// Done with buffer, advance to next
			miBufferReadIndex = 0;
			miBufferCurRead++;
			if (miBufferCurRead == miBufferCount) {
				miBufferCurRead = 0;
			}
		}
	}
	else {*/
		// Slower and more complicated version. The memory is fragmented, so we need to copy it first
/*		tint iToGo = iSamplesNeeded;
		tint iIndexDest = 0;
		while (iToGo) {
			if (mpbBufferFilled[miBufferCurRead] == false) {
//			if (miBufferCurRead == miBufferCurWrite) {
//			if (miBufferCurRead != miBufferCurWrite) {
				// Need to fill one more buffer
				FillBuffer();
//				ASSERT(miBufferCurRead != miBufferCurWrite);
			}

//			if ((miBufferCurRead + 1 == miBufferCurWrite) ||
//				((miBufferCurRead == miBufferCount - 1) && (miBufferCurWrite == 0))) {
//				FillBuffer();
//			}

			// Samples left in buffer
			tint iSizeThis = miBufferSize - miBufferReadIndex;
			// Make sure we don't copy too much
			if (iSizeThis > iToGo) {
				iSizeThis = iToGo;
			}

			memcpy(mpfTmpBuffer + 4 + iIndexDest, mppfBuffers[miBufferCurRead] + miBufferReadIndex, iSizeThis * sizeof(sample));

			miBufferReadIndex += iSizeThis;

			if (miBufferReadIndex == miBufferSize) {
				// Done with buffer, advance to next
				miBufferReadIndex = 0;
				mpbBufferFilled[miBufferCurRead] = false;
				miBufferCurRead++;
				if (miBufferCurRead == miBufferCount) {
					miBufferCurRead = 0;
				}
			}

			iIndexDest += iSizeThis;
			iToGo -= iSizeThis;
		}

		mpResampler->Resample(iC, iSamplesNeeded, fPitch, pfOut, (float*)mpfTmpBuffer);*/
/*	}*/
}

void CWaveOscStream::FillBuffer()
{
/*	ASSERT(mpCallback);

	CAutoLock Lock(Mutex);

	ASSERT(mpbBufferFilled[miBufferCurWrite] == false);

//	memset(mppfBuffers[miBufferCurWrite], 0, miBufferSize * sizeof(float));
	mpCallback->FillBuffer(miCallbackID, mppfBuffers[miBufferCurWrite], miBufferSize);

	mpbBufferFilled[miBufferCurWrite] = true;

	miBufferCurWrite++;
	if (miBufferCurWrite == miBufferCount) {
		miBufferCurWrite = 0;
	}*/
}

void CWaveOscStream::Timer()
{
/*	CAutoLock Lock(Mutex);

//	if (miBufferCurRead != miBufferCurWrite) {
//	if (miBufferCurRead == miBufferCurWrite) {
	if (mpbBufferFilled[miBufferCurWrite] == false) {
		// Need to fill one more buffer
		FillBuffer();
	}*/

/*	tint32 iBufferWrite = miBufferCurWrite + 1;
	if (iBufferWrite >= miBufferCount) {
		iBufferWrite -= miBufferCount;
	}
	if (iBufferWrite != miBufferCurRead) {
		// Need to fill one more buffer
		FillBuffer();
	}*/
}

void CWaveOscStream::Init()
{
//	_beginthread(ThreadWrap, 8192, dynamic_cast<CWaveOscStream*>(this));
}

void CWaveOscStream::DeInit()
{
}

void CWaveOscStream::Thread()
{
/*	while (1) {
		{
			CAutoLock Lock(Mutex);

			if (miBufferCurRead != miBufferCurWrite) {
				// Need to fill one more buffer
				FillBuffer();
			}
		}

//		Sleep(5);
	}*/
}

void CWaveOscStream::ThreadWrap(void* p)
{
	CWaveOscStream* pStream = (CWaveOscStream*)p;
	pStream->Thread();
}


