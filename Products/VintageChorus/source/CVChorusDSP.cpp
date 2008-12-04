

#include "VChorusOS.h"

/*
	CVChorusDSP
*/

CVChorusDSP::CVChorusDSP()
	: muiChannels(0), mpfBuffer1(NULL), mpfBuffer2(NULL)
{
	mpfBuffer1 = new tfloat[EFFECT_CHORUSVINTAGE_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_CHORUSVINTAGE_BUFFER_SIZE];
}

CVChorusDSP::~CVChorusDSP()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;
}

void CVChorusDSP::Destroy()
{
	delete dynamic_cast<CVChorusDSP*>(this);
}

void CVChorusDSP::Start()
{
	memset(mpfBuffer1, 0, EFFECT_CHORUSVINTAGE_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_CHORUSVINTAGE_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mfIndexCur = 0;

	mfLPHistoryL = 0;
	mfLPHistoryR = 0;
	mfHPHistoryL = 0;
	mfHPHistoryR = 0;

	// Could be moved to SetSampleRate();
	mfLPCoeff = (float)(1 - (exp(-6.283 * (6000.0 / muiSampleRate))));

	// Could be moved to SetSampleRate();
	mfHPCoeff = (float)(1 - (exp(-6.283 * (200.0 / muiSampleRate))));
}

void CVChorusDSP::Stop()
{
}

void CVChorusDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CVChorusDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CVChorusDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CVChorusDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
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

void CVChorusDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfL = ppfIn[0];
	const float* pfR = ppfIn[1];
	/*
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
*/
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];
/*
	if (mbBypass) {
		// Output meter
		tfloat32 fMeterL = mpfMeterSignal[0];
		tfloat32 fMeterR = mpfMeterSignal[1];

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfL[l];
			pfOut1[l] = pfR[l];

			tfloat32 fOutL = fabsf(pfOut0[l]);
			tfloat32 fOutR = fabsf(pfOut1[l]);

		}
		return;
	}
*/

	tfloat* pfBuffer1 = mpfBuffer1;
	tfloat* pfBuffer2 = mpfBuffer2;
	tint32 iPos = miPos;

	for (tuint32 uiSample = 0; uiSample < lC; uiSample++) {
		// Move writing position forward
		iPos++;
		// Wrap (if neccasary)
		iPos &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Apply filters
		// LP
		tfloat fInL = mfLPHistoryL = mfLPHistoryL + (pfL[uiSample] - mfLPHistoryL) * mfLPCoeff;
		tfloat fInR = mfLPHistoryR = mfLPHistoryR + (pfR[uiSample] - mfLPHistoryR) * mfLPCoeff;
		// HP (as LP)
		tfloat fHPL = mfHPHistoryL = mfHPHistoryL + (fInL - mfHPHistoryL) * mfHPCoeff;
		tfloat fHPR = mfHPHistoryR = mfHPHistoryR + (fInR - mfHPHistoryR) * mfHPCoeff;
		// Subtract LP from original to get HP
		fInL = fInL - fHPL;
		fInR = fInR - fHPR;

		// Write input to circular buffer
		pfBuffer1[iPos] = fInL;
		pfBuffer2[iPos] = fInR;
				
		// Find reading tap
		tint32 iTap = iPos - miDelayTime;
		iTap &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Modulate tap
		// Left
		mfIndexCur += mfOneOverRate;
		if (mfIndexCur >= 1) {
			mfIndexCur -= 1;
		}

		// Interpolate
		tfloat fIndex = mfIndexCur * 256;
		tint iIndex = (tint)fIndex;
		tfloat fDelta = fIndex - iIndex;

		// Look up in table
		tfloat fDepth =	gpfSinTable[iIndex] * (1.0f - fDelta);
		fDepth += gpfSinTable[iIndex + 1] * fDelta;

		// Convert to bipolar
//		fDepth = (fDepth - 0.5f) * 2;

		// Apply depth
		fDepth *= mfDepth;
		fDepth *= miDelayTime;

		// Interpolate
		fIndex = iTap + fDepth;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		iTap = iIndex;
		iTap &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);
		tint iTap2 = iIndex + 1;
		iTap2 &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Read output from reading tap
		tfloat fOutL = mpfBuffer1[iTap] * (1.0f - fDelta);
		fOutL += mpfBuffer1[iTap2] * fDelta;

		// Left2
		iTap = iPos - miDelayTime;
		fIndex = mfIndexCur + 0.25f;
		if (fIndex > 1) {
			fIndex -= 1;
		}
		fIndex *= 256;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		// Look up in table
		fDepth = gpfSinTable[iIndex] * (1.0f - fDelta);
		fDepth += gpfSinTable[iIndex + 1] * fDelta;

		// Convert to bipolar
//		fDepth = (fDepth - 0.5f) * 2;

		// Apply depth
		fDepth *= mfDepth;
		fDepth *= miDelayTime;

		// Interpolate
		fIndex = iTap + fDepth;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		iTap = iIndex;
		iTap &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);
		iTap2 = iIndex + 1;
		iTap2 &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Read output from reading tap
		fOutL += mpfBuffer1[iTap] * (1.0f - fDelta);
		fOutL += mpfBuffer1[iTap2] * fDelta;

		// Right
		iTap = iPos - miDelayTime;
		fIndex = mfIndexCur + mfWidth * 0.5f;
		if (fIndex >= 1) {
			fIndex -= 1;
		}
		fIndex *= 256;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		// Look up in table
		fDepth = gpfSinTable[iIndex] * (1.0f - fDelta);
		fDepth += gpfSinTable[iIndex + 1] * fDelta;

		// Convert to bipolar
//		fDepth = (fDepth - 0.5f) * 2;

		// Apply depth
		fDepth *= mfDepth;
		fDepth *= miDelayTime;

		// Interpolate
		fIndex = iTap + fDepth;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		iTap = iIndex;
		iTap &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);
		iTap2 = iIndex + 1;
		iTap2 &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Read output from reading tap
		tfloat fOutR = mpfBuffer2[iTap] * (1.0f - fDelta);
		fOutR += mpfBuffer2[iTap2] * fDelta;

		// Right2
		iTap = iPos - miDelayTime;
		fIndex = mfIndexCur + mfWidth * 0.5f + 0.25f;
		if (fIndex > 1) {
			fIndex -= 1;
		}
		fIndex *= 256;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		// Look up in table
		fDepth =	gpfSinTable[iIndex] * (1.0f - fDelta);
		fDepth += gpfSinTable[iIndex + 1] * fDelta;


		// Apply depth
		fDepth *= mfDepth;
		fDepth *= miDelayTime;

		// Interpolate
		fIndex = iTap + fDepth;
		iIndex = (tint)fIndex;
		fDelta = fIndex - iIndex;

		iTap = iIndex;
		iTap &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);
		iTap2 = iIndex + 1;
		iTap2 &= (EFFECT_CHORUSVINTAGE_BUFFER_SIZE - 1);

		// Read output from reading tap
		fOutR += mpfBuffer2[iTap] * (1.0f - fDelta);
		fOutR += mpfBuffer2[iTap2] * fDelta;

		// Width
		tfloat fSum = (fOutL + fOutR) * 0.5f;
		tfloat fDiff = (fOutL - fOutR) * 0.5f;

		fSum *= (1 - mfWidth);
		fDiff *= (1 + mfWidth);
	
		pfOut0[uiSample]	=  (fSum + fDiff) * mfMixWet;
		pfOut0[uiSample]	+=	pfL[uiSample] * mfMixDry;
		
		pfOut1[uiSample]	=	(fSum + fDiff) * mfMixWet;;
		pfOut1[uiSample]	+=	pfR[uiSample] * mfMixDry;
	}

	miPos = iPos;
	
}

void CVChorusDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}


/*
	CEffect::SetParameter(iIndex, iValue);

	switch(iIndex) {
		case Time:
			miDelayTime = (tint32)((iValue * 0.1f) * GetSampleRate() / 1000);
			break;
		case Depth:
			mfDepth = iValue * 0.01f * 0.99;
			break;
		case Rate:
			mfOneOverRate = (iValue * 0.01f) / GetSampleRate();
			break;
		case Width:
			mfWidth = iValue * 0.01f;
			break;
	}
*/



void CVChorusDSP::SetDelayTime(tint32 iDelayTime)
{
	miDelayTime = (tint32)((iDelayTime * 0.1f) * muiSampleRate / 1000);
}
	
void CVChorusDSP::SetDepth(tint32 iDepth)
{
	mfDepth = iDepth * 0.01f * 0.99;
}
	
void CVChorusDSP::SetRate(tint32 iRate)
{
	mfOneOverRate = (iRate * 0.01f) / muiSampleRate;
}
	
void CVChorusDSP::SetWith(tint32 iWith)
{	
	mfWidth = iWith * 0.01f;
}

void CVChorusDSP::SetMix(tint32 iMix)
{	
	mfMixWet	=	iMix/128.0f;
	mfMixDry	=	1.0f - mfMixWet;
}