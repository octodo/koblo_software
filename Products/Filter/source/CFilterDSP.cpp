

#include "FilterOS.h"

#define EFFECT_ECHO_BUFFER_SIZE 131072*2

/*
	CFilterDSP
*/

CFilterDSP::CFilterDSP()
	: muiChannels(0)
{
	mpFilterLeft	=	st::IChamberlinFilter::Create(mpfFreqToCutoff, giFreqToCutoffSize);
	mpFilterRight	=	st::IChamberlinFilter::Create(mpfFreqToCutoff, giFreqToCutoffSize);
}

CFilterDSP::~CFilterDSP()
{
	
	
}

void CFilterDSP::Destroy()
{
	mpFilterLeft->Destroy();
	mpFilterRight->Destroy();
	
	delete dynamic_cast<CFilterDSP*>(this);
}

void CFilterDSP::Start()
{
	mpFilterLeft->Reset();
	mpFilterRight->Reset();
}

void CFilterDSP::Stop()
{
}

void CFilterDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CFilterDSP::SetSampleRate(tuint32 uiSampleRate)
{
	muiSampleRate	=	uiSampleRate;
	mpFilterLeft->SetSampleRate(uiSampleRate);
	mpFilterRight->SetSampleRate(uiSampleRate);
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CFilterDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CFilterDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
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

void CFilterDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];

	if (mbBypass) {

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];

			tfloat32 fOutL = fabsf(pfOut0[l]);
			tfloat32 fOutR = fabsf(pfOut1[l]);
		}
		return;
	}

	for (long i = 0; i < lC; i++) {
		
		pfOut0[i] = pfIn0[i];
		pfOut1[i] = pfIn1[i];
	}

	
	if(lC == 32){
		mpFilterLeft->Process32(pfOut0, muiCutoff, mfResonance, mfLP, mfBP, mfHP,lC);
		mpFilterRight->Process32(pfOut1, muiCutoff, mfResonance, mfLP, mfBP, mfHP,lC);
	}
	else{
		mpFilterLeft->Process(pfOut0, muiCutoff, mfResonance, mfLP, mfBP, mfHP,lC);
		mpFilterRight->Process(pfOut1, muiCutoff, mfResonance, mfLP, mfBP, mfHP,lC);
	}
	
	for (long i = 0; i < lC; i++) {
		
		pfOut0[i] = (pfIn0[i] * mfDry) + (pfOut0[i] * mfWet);
		pfOut1[i] = (pfIn1[i] * mfDry) + (pfOut1[i] * mfWet);
	}
	

}

void CFilterDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}

void CFilterDSP::Set_Cutoff(tuint32 uiCutoff)
{

	
	muiCutoff = uiCutoff;
	
}

void CFilterDSP:: Set_Resonance(tint32 iResonance)
{

	mfResonance	=	1.0f - ((tfloat32)iResonance / 1020.0);
}

void CFilterDSP::Set_Mix(tint32 iMix)
{
	mfWet = (tfloat32)iMix / 100.0f;
	mfDry	=	1.0f - mfWet;
	
	mfWet += 0.0000001f;
	mfDry += 0.0000001f;
}

void CFilterDSP::Set_LoPass(tint32 iLP)
{
	mfLP	=	(tfloat32)iLP / 100.0f;
	mfLP	*=	mfLP;
}

void CFilterDSP::Set_BandPass(tint32 iBP)
{
	mfBP	=	(tfloat32)iBP / 100.0f;
	mfBP	*=	mfBP;
}

void CFilterDSP::Set_HighPass(tint32 iHP)
{
	mfHP	=	(tfloat32)iHP / 100.0f;
	mfHP	*=	mfHP;
}


