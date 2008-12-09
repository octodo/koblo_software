/*!	\file CFFT.cpp
	\author Michael Olsen
	\date 13/Dec/2005
	\date 13/Dec/2005
*/


#include "geInternalOS.h"


CFFT::CFFT()
	: mpPrimitives(NULL), mpFFT(NULL), mpWindow(NULL), mpBandInfo(NULL),
	muiTimeLast(0)
{
	tint iBuffer;
	for (iBuffer = 0; iBuffer < guiFFTNrOfQueueBuffers; iBuffer++) {
		mppfQueueLeft[iBuffer] = NULL;
		mppfQueueRight[iBuffer] = NULL;
	}

	mpfLeft = NULL;
	mpfRight = NULL;
}

CFFT::~CFFT()
{
	if (mpBandInfo) {
		delete[] mpBandInfo;
	}

	if (mpFFT) {
		mpFFT->Destroy();
	}
	if (mpWindow) {
		mpWindow->Destroy();
	}

	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}

	if (mppfQueueLeft[0]) {
		tint iBuffer;
		for (iBuffer = 0; iBuffer < guiFFTNrOfQueueBuffers; iBuffer++) {
			delete[] mppfQueueLeft[iBuffer];
			delete[] mppfQueueRight[iBuffer];
		}
	}

	if (mpfLeft) {
		delete[] mpfLeft;
	}
	if (mpfRight) {
		delete[] mpfRight;
	}
}

IFFT* IFFT::Create()
{
	return dynamic_cast<IFFT*>(new CFFT());
}

void CFFT::Destroy()
{
	delete dynamic_cast<CFFT*>(this);
}

void CFFT::Init(tint32 iID, const SRGB& Colour, IWindow* pParentWindow, tuint32 uiSampleRate, tuint uiChannels)
{
	CControl::Init(iID, NULL);

	mColour = Colour;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pParentWindow);

	mpFFT = dspe::IFFT::Create();

	// Min size needed to measure 20 Hz.
	tuint uiMinFFTSize = (uiSampleRate / 2) / 20;
	// Some more precision is needed for the low end
	uiMinFFTSize *= 4;
	// Calculate the power
	tuint uiPower = 1;
	muiFFTSize = 2;
	while (muiFFTSize < uiMinFFTSize) {
		uiPower++;
		muiFFTSize *= 2;
	}

	mpFFT->Init(uiPower);

	muiNyquist = uiSampleRate / 2;
	muiChannels = uiChannels;

	tint iBuffer;
	for (iBuffer = 0; iBuffer < guiFFTNrOfQueueBuffers; iBuffer++) {
		mppfQueueLeft[iBuffer] = new tfloat32[muiFFTSize];
		mppfQueueRight[iBuffer] = new tfloat32[muiFFTSize];
	}

	mpfLeft = new tfloat32[muiFFTSize];
	mpfRight = new tfloat32[muiFFTSize];

	muiQueueBufferWrite = 0;
	muiQueueBufferRead = 0;
	muiQueueBufferWriteIndex = 0;

	muiNrOfBands = 30;
	mpBandInfo = new SBandInfo[muiNrOfBands];

	tfloat64 fMul = pow(2, 1.0 / 3);
	tfloat64 fCurLowFreq = 20.0;
	tfloat64 fCurHighFreq = fCurLowFreq * fMul;
	tuint uiBand;
	for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
		mpBandInfo[uiBand].fMinFreq = fCurLowFreq;
		mpBandInfo[uiBand].fMaxFreq = fCurHighFreq;
		fCurLowFreq = fCurHighFreq;
		fCurHighFreq *= fMul;
	}

	mpfMagsHist.Allocate(muiNrOfBands);
	mpfMaxHist.Allocate(muiNrOfBands);
	mpfMaxCounter.Allocate(muiNrOfBands);

	mpWindow = dspe::IWindow::Create();
	mpWindow->Init(dspe::IWindow::WindowHann, 0, muiFFTSize);
}

void CFFT::Input(const tfloat32** ppfSamples, tuint32 uiSampleCount)
{
	const tfloat32* pfLeft = ppfSamples[0];
	const tfloat32* pfRight = NULL;
	if (muiChannels > 1) {
		pfRight = ppfSamples[1];
	}

	tuint32 uiSamplesLeft = uiSampleCount;
	tuint32 uiBufferOffset = 0;
	while (uiSamplesLeft) {
		tuint32 uiSizeThis = uiSamplesLeft;
		if (uiSizeThis > muiFFTSize - muiQueueBufferWriteIndex) {
			// Cannot write more into buffer than it has space for
			uiSizeThis = muiFFTSize - muiQueueBufferWriteIndex;
		}

		memcpy(mppfQueueLeft[muiQueueBufferWrite] + muiQueueBufferWriteIndex,
			pfLeft + uiBufferOffset,
			uiSizeThis * sizeof(tfloat32));
		if (muiChannels > 1) {
			memcpy(mppfQueueRight[muiQueueBufferWrite] + muiQueueBufferWriteIndex,
				pfRight + uiBufferOffset,
				uiSizeThis * sizeof(tfloat32));
		}

		uiSamplesLeft -= uiSizeThis;
		uiBufferOffset += uiSizeThis;

		muiQueueBufferWriteIndex += uiSizeThis;
		if (muiQueueBufferWriteIndex == muiFFTSize) {
			muiQueueBufferWriteIndex = 0;

			// Buffer full
			muiQueueBufferWrite++;
			if (muiQueueBufferWrite == guiFFTNrOfQueueBuffers) {
				muiQueueBufferWrite = 0;
			}

			if (muiQueueBufferWrite == muiQueueBufferRead) {
				// Read and write buffers are now the same, which in this case means we didn't process fast enough.
				// Skip one input buffer to minimize problem
				muiQueueBufferRead++;
				if (muiQueueBufferRead == guiFFTNrOfQueueBuffers) {
					muiQueueBufferRead = 0;
				}
			}
		}
	}
}

void CFFT::Process()
{
//	const tfloat64 fRollOffPerMS = 0.995;
	const tfloat64 fRollOffPerMS = 0.997;
//	tfloat64 fMSPerBuffer = muiFFTSize * 1000.0f / (muiNyquist * 2);
//	tfloat32 fRollOff = (tfloat32)pow(fRollOffPerMS, fMSPerBuffer);
	tuint32 uiTime = ITime::GetTimeMS();
	tuint32 uiTimeDiff = uiTime - muiTimeLast;
	muiTimeLast = uiTime;
	tfloat32 fRollOff = 1.0f;
	if (uiTimeDiff != 0) {
		fRollOff = (tfloat32)pow((tfloat64)fRollOffPerMS, (tint)uiTimeDiff);
	}

	const tfloat64 fMaxRollOffPerMS = 0.999;
	tfloat32 fRollOffMax = 1.0f;
	if (uiTimeDiff != 0) {
		fRollOffMax = (tfloat32)pow((tfloat64)fMaxRollOffPerMS, (tint)uiTimeDiff);
	}

	// Time in ms that the max bar stays up until it starts to drop
	const tfloat fMaxTimeMS = 2000.0f;

	tuint uiBand;
	for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
		mpfMagsHist[uiBand] *= fRollOff;

		mpfMaxCounter[uiBand] += uiTimeDiff;
		if (mpfMaxCounter[uiBand] >= fMaxTimeMS) {
			mpfMaxCounter[uiBand] = fMaxTimeMS;

			mpfMaxHist[uiBand] *= fRollOffMax;
		}
	}

	// As long as we have buffers, process
	tbool bFirstBuffer = true;
	while (muiQueueBufferWrite != muiQueueBufferRead) {
//		tfloat32 fRollOff = 1.0f;
		if (bFirstBuffer) {
//			fRollOff = 0.9f;
			bFirstBuffer = false;
		}

		memcpy(mpfLeft, mppfQueueLeft[muiQueueBufferRead], muiFFTSize * sizeof(tfloat32));
		if (muiChannels > 1) {
			memcpy(mpfRight, mppfQueueRight[muiQueueBufferRead], muiFFTSize * sizeof(tfloat32));
		}

		muiQueueBufferRead++;
		if (muiQueueBufferRead == guiFFTNrOfQueueBuffers) {
			muiQueueBufferRead = 0;
		}

		mpWindow->Apply(mpfLeft);
		mpFFT->GetMagnitude(mpfLeft);
		if (muiChannels > 1) {
			mpWindow->Apply(mpfRight);
			mpFFT->GetMagnitude(mpfRight);

			tuint uiIndex;
			for (uiIndex = 0; uiIndex < muiFFTSize / 2 + 1; uiIndex++) {
				mpfLeft[uiIndex] = (mpfLeft[uiIndex] + mpfRight[uiIndex]) * 0.5f;
			}
		}

		tuint uiBand;
		for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
			tfloat64 fIndexLow = mpBandInfo[uiBand].fMinFreq / muiNyquist;
			if (fIndexLow <= 1) {
				tint32 iIndexLow = Float2Int(fIndexLow * (muiFFTSize / 2 + 1));
				iIndexLow = max(iIndexLow, 0);
				tfloat64 fIndexHigh = mpBandInfo[uiBand].fMaxFreq / muiNyquist;
				fIndexHigh = min(fIndexHigh, 1.0);
				tint32 iIndexHigh = (tint32)(fIndexHigh * (muiFFTSize / 2 + 1));

				tfloat fMag = 0.0f;
				tint32 iIndex;
				for (iIndex = iIndexLow; iIndex <= iIndexHigh; iIndex++) {
					fMag += mpfLeft[iIndex];
				}
				fMag /= (iIndexHigh - iIndexLow + 1);

//				mpfMagsHist[uiBand] = max(fMag, mpfMagsHist[uiBand] * fRollOff);
				mpfMagsHist[uiBand] = max(fMag, mpfMagsHist[uiBand]);

				if (mpfMagsHist[uiBand] >= mpfMaxHist[uiBand]) {
					// New maximum
					mpfMaxHist[uiBand] = mpfMagsHist[uiBand];
					mpfMaxCounter[uiBand] = 0;
				}
			}
			else {
				mpfMagsHist[uiBand] = 0.0f;
			}
		}
	}

//	if (bFirstBuffer == false) {
		// This means we processed at least one buffer
		Redraw();
//	}
}

void CFFT::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	SRect Rect = GetRect();

	// Width per band
	tfloat64 fBandWidth = Rect.iCX / (tfloat64)muiNrOfBands;

	tfloat64 fPosXCur = Rect.iX;
	tuint uiBand;
	for (uiBand = 0; uiBand < muiNrOfBands; uiBand++) {
		// Calculate bands rect
		SRect RectBand(SPos(Float2Int(fPosXCur), Rect.iY), SSize(Float2Int(fBandWidth), Rect.iCY));

		// Clear rect
		mpPrimitives->DrawRect(rUpdate, RectBand, SRGB(0, 0, 0));

		// Calculate rect to draw
		SRect RectDraw(RectBand);
//		const tfloat32 fdBMax = -12.0f;
//		const tfloat32 fdBMin = -60.0f;
		const tfloat32 fdBMax = -36.0f;
		const tfloat32 fdBMin = -72.0f;
		tfloat32 fdB = -1000.0f;
		if (mpfMagsHist[uiBand] != 0) {
			fdB = log10(mpfMagsHist[uiBand]) * 20;
		}
		tfloat32 fIndex = (fdB - fdBMin) / (fdBMax - fdBMin);
		fIndex = min(fIndex, 1);
		fIndex = max(fIndex, 0);
		RectDraw.iY += (tint)(RectDraw.iCY * (1 - fIndex));
		RectDraw.iCY -= (tint)(RectDraw.iCY * (1 - fIndex));

		// Draw rect
		mpPrimitives->DrawRect(rUpdate, RectDraw, mColour);

		// Calculate max rect
		SRect RectDrawMax(RectBand);
		fdB = -1000.0f;
		if (mpfMaxHist[uiBand] != 0) {
			fdB = log10(mpfMaxHist[uiBand]) * 20;
		}
		fIndex = (fdB - fdBMin) / (fdBMax - fdBMin);
		fIndex = min(fIndex, 1);
		fIndex = max(fIndex, 0);
		RectDrawMax.iY += (tint)(RectDrawMax.iCY * (1 - fIndex));
		if (RectDrawMax.iCY * (1 - fIndex) == RectDrawMax.iCY) {
			RectDrawMax.iY -= 1;
		}
		RectDrawMax.iCY = 1;

		// Draw max rect
		mpPrimitives->DrawRect(rUpdate, RectDrawMax, SRGB(192, 128, 128));

		fPosXCur += fBandWidth;
	}
}

tbool CFFT::OnMouse(EMouseMsg /*MouseMsg*/, const SPos& /*Pos*/)
{
	return false;
}

tbool CFFT::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CFFT::OnTimer()
{
	Process();

	CControl::OnTimer();
}

