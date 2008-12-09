

#include "DPhaserOS.h"

/*
	CDPhaserDSP
*/

CDPhaserDSP::CDPhaserDSP()
	: muiChannels(0)
{
}

CDPhaserDSP::~CDPhaserDSP()
{
}

void CDPhaserDSP::Destroy()
{
	delete dynamic_cast<CDPhaserDSP*>(this);
}

void CDPhaserDSP::Start()
{
	mfCurIndex = 0;

	mfFeedbackL = mfFeedbackR = 0;

	tint i;
	for (i = 0; i < 10; i++) {
		mfHistoryL[i] = mfHistoryR[i] = 0;
	}
}

void CDPhaserDSP::Stop()
{
}

void CDPhaserDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CDPhaserDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CDPhaserDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CDPhaserDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn = ppfIn[0];
	float* pfOut = ppfOut[0];

	if (mbBypass) {
		// Output meter
		tfloat32 fMeter = mpfMeterSignal[0];

		for (long l = 0; l < lC; l++) {
			pfOut[l] = pfIn[l];

			tfloat32 fOut = fabsf(pfOut[l]);

			if (fOut > fMeter) {
				fMeter = fOut;
			}
		}

		mpfMeterSignal[0] = fMeter;
		return;
	}


	// TODO: Replace with something meaningful
	{
		for (long l = 0; l < lC; l++) {
			pfOut[l] = pfIn[l];
		}
	}


	// Output meter
	tfloat32 fMeter = mpfMeterSignal[0];

	for (long l = 0; l < lC; l++) {
		tfloat32 fOut = fabsf(pfOut[l]);

		if (fOut > fMeter) {
			fMeter = fOut;
		}
	}

	mpfMeterSignal[0] = fMeter;
}

void CDPhaserDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0	= ppfOut[0];
	float* pfOut1 = ppfOut[1];
	

	if (mbBypass) {
		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];

		}
		return;
	}


	for (long uiSample = 0; uiSample < lC; uiSample++) {

		// Calc sin
		mfCurIndex += mfOneOverRate;
		if (mfCurIndex >= 1) {
			mfCurIndex -= 1;
		}

		tfloat fIndex = mfCurIndex * guiSinTableSize;
		// Interpolate
		tint iIndex = (tint)fIndex;
		tfloat fDelta = fIndex - iIndex;

		tfloat fSin = gpfSinTable[iIndex] * (1.0f - fDelta);
		fSin += gpfSinTable[iIndex + 1] * fDelta;
		fSin *= 0.5f;
		fSin += 1;
		ASSERT(fSin >= 0);
		ASSERT(fSin <= 1.5f);
//		fSin += 0.5f;

		// Calc delay, based on range
		tfloat fRangeMin = mfHz - (mfHz * 0.8f * (mfRange + 0.1f));
		fRangeMin = fRangeMin / (muiSampleRate / 2);
		tfloat fRangeMax = mfHz + (mfHz * (mfRange * 3 + 0.1));
		fRangeMax = fRangeMax / (muiSampleRate / 2);
		tfloat fDelay = fRangeMin + (fRangeMax - fRangeMin) * fSin;

		// Input + inverted feedback
		tfloat fLeft = pfIn0[uiSample];
		fLeft += (mfFeedbackL * mfFeedback);
		tfloat fRight = pfIn1[uiSample];
		fRight += (mfFeedbackR * mfFeedback);

		tfloat fAlpha = 1 - fDelay;

		// Do allpassing
		for (tint i = 0; i < 10; i++) {
			tfloat fTmp	= fLeft * -fAlpha + mfHistoryL[i];
			mfHistoryL[i] = fTmp * fAlpha + fLeft;
			mfHistoryL[i] += gfAnti_denormal;
			fLeft = fTmp;

			fTmp = fRight * -fAlpha + mfHistoryR[i];
			mfHistoryR[i] = fTmp * fAlpha + fRight;
			mfHistoryR[i] += gfAnti_denormal;
			fRight = fTmp;
		}

		mfFeedbackL = fLeft + gfAnti_denormal;
		mfFeedbackR = fRight + gfAnti_denormal;

		// Output
		pfOut0[uiSample] = fLeft	* fMix_Wet;
		pfOut1[uiSample] = fRight	* fMix_Wet;
		
		pfOut0[uiSample] += pfIn0[uiSample]	* fMix_Dry;
		pfOut1[uiSample] += pfIn1[uiSample]	* fMix_Dry;
		
			
	}
}

void CDPhaserDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}


void CDPhaserDSP::SetFreq(tint32 iValue)
{
	mfHz = iValue;
}

void CDPhaserDSP::SetRange(tint32 iValue)
{
	mfRange = iValue * 0.01f;
}

void CDPhaserDSP::SetRate(tint32 iValue)
{
	mfOneOverRate = (iValue * 0.01f) / muiSampleRate;
}

void CDPhaserDSP::SetFeedback(tint32 iValue)
{
	mfFeedback = iValue * 0.01f * -0.9f;
}

void CDPhaserDSP::SetMix(tint32 iValue)
{
	fMix_Wet	=	(tfloat32)iValue / (tfloat32)128;
	fMix_Dry	=	1.0f - fMix_Wet;
}