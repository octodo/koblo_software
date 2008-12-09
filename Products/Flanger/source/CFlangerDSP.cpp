

#include "FlangerOS.h"

/*
	CFlangerDSP
*/

CFlangerDSP::CFlangerDSP()
	: muiChannels(0)
{
	mpfBuffer1 = new tfloat[EFFECT_FLANGER_BUFFER_SIZE];
	mpfBuffer2 = new tfloat[EFFECT_FLANGER_BUFFER_SIZE];
}

CFlangerDSP::~CFlangerDSP()
{
	delete[] mpfBuffer1;
	mpfBuffer1 = NULL;

	delete[] mpfBuffer2;
	mpfBuffer2 = NULL;
}

void CFlangerDSP::Destroy()
{
	delete dynamic_cast<CFlangerDSP*>(this);
}

void CFlangerDSP::Start()
{
	memset(mpfBuffer1, 0, EFFECT_FLANGER_BUFFER_SIZE * sizeof(tfloat));
	memset(mpfBuffer2, 0, EFFECT_FLANGER_BUFFER_SIZE * sizeof(tfloat));

	miPos = 0;

	mfIndexCur = 0;

	// Empirical tuning
	miFeedback = (tint)(muiSampleRate / 177.777f);
}

void CFlangerDSP::Stop()
{
}

void CFlangerDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CFlangerDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CFlangerDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CFlangerDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
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

void CFlangerDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];
/*
	if (mbBypass) {
		// Output meter

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];
		}
		return;
	}
*/

	tfloat* pfBuffer1 = mpfBuffer1;
	tfloat* pfBuffer2 = mpfBuffer2;
	tint32 iPos = miPos;
	
	for (long uiSample = 0; uiSample < lC; uiSample++) {
		//pfOut0[uiSample] = pfIn0[uiSample];
		//pfOut1[uiSample] = pfIn1[uiSample];
		
		// Move writing position forward
		iPos++;
		// Wrap (if neccasary)
		iPos &= (EFFECT_FLANGER_BUFFER_SIZE - 1);

		// Find feedback reading tap
		tint32 iTapFB = iPos - miFeedback;
		iTapFB &= (EFFECT_FLANGER_BUFFER_SIZE - 1);
		tfloat fFeedback = pfBuffer1[iTapFB] * -0.7071f * mfFeedback;
		fFeedback += gfAnti_denormal;

		// Write feedback'ed input to circular buffer
		tfloat fTmp = pfIn0[uiSample] - fFeedback;
		pfBuffer1[iPos] = fTmp;

		// Save "blend" value for later
//		tfloat fBlend = fTmp * 0.7071f;
		tfloat fBlend = -fFeedback * 0.7071f;

		// Find feedforward reading tap
		tint iTap = iPos - miDelayTime;

		// Modulate tap
		// Left
		mfIndexCur += mfOneOverRate;
		if (mfIndexCur >= 1) {
			mfIndexCur -= 1;
		}
		// Triangle
		tfloat fIndex = mfIndexCur;
		if (fIndex <= 0.5f) {
			fIndex *= 2;
		}
		else {
			fIndex = (1 - fIndex) * 2;
		}
		tfloat fDepth =	fIndex;
		// Convert to bipolar
		fDepth = (fDepth - 0.5f) * 2;
		// Apply depth
		fDepth *= mfDepth;
		fDepth *= miDelayTime;

		// Interpolate
		fIndex = iTap + fDepth;
		tint iIndex = (tint)fIndex;
		tfloat fDelta = fIndex - iIndex;
		iTap = iIndex;
		iTap &= (EFFECT_FLANGER_BUFFER_SIZE - 1);
		tint iTap2 = iIndex + 1;
		iTap2 &= (EFFECT_FLANGER_BUFFER_SIZE - 1);

		// Read output from reading tap
		tfloat fOutL = mpfBuffer1[iTap] * (1.0f - fDelta);
		fOutL += mpfBuffer1[iTap2] * fDelta;
		fOutL *= 0.7071f;
		fOutL += fBlend;
		pfOut0[uiSample] =	fOutL* mfWet;
		pfOut0[uiSample] += pfIn0[uiSample] * mfDry;

		// Right
		fFeedback = pfBuffer2[iTapFB] * -0.7071f * mfFeedback;
		fFeedback += gfAnti_denormal;

		// Write feedback'ed input to circular buffer
		fTmp = pfIn1[uiSample] - fFeedback;
		pfBuffer2[iPos] = fTmp;

		// Save "blend" value for later
//		fBlend = fTmp * 0.7071f;
		fBlend = -fFeedback * 0.7071f;

		tfloat fOutR = pfBuffer2[iTap] * (1.0f - fDelta);
		fOutR += pfBuffer2[iTap2] * fDelta;
		fOutR *= 0.7071f;
		fOutR += fBlend;
		pfOut1[uiSample] = fOutR * mfWet;
		pfOut1[uiSample] += pfIn1[uiSample] * mfDry;
	
	}
	
	miPos = iPos;

}

void CFlangerDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}


void CFlangerDSP::Set_Time(tint32 iValue)
{
	miDelayTime = (tint32)((iValue * 0.01f) * muiSampleRate / 1000);

}
void CFlangerDSP::Set_Depth(tint32 iValue)
{
	mfDepth = iValue * 0.01f;

}
void CFlangerDSP::Set_Rate(tint32 iValue)
{
	mfOneOverRate = (iValue * 0.01f) / muiSampleRate;

}
void CFlangerDSP::Set_Feedback(tint32 iValue)
{
	mfFeedback = iValue * 0.01f * 0.99;

}
void CFlangerDSP::Set_Mix(tint32 iValue)
{

	mfWet		=	(tfloat32)iValue/128.0f;
	mfDry		=	1.0f - mfWet;
	

}