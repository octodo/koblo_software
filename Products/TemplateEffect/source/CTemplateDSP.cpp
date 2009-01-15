

#include "TemplateOS.h"

/*
	CTemplateDSP
*/

CTemplateDSP::CTemplateDSP()
	: muiChannels(0)
{
}

CTemplateDSP::~CTemplateDSP()
{
}

void CTemplateDSP::Destroy()
{
	delete dynamic_cast<CTemplateDSP*>(this);
}

void CTemplateDSP::Start()
{
}

void CTemplateDSP::Stop()
{
}

void CTemplateDSP::SetMaxBufferSize(tint32 uiMaxBufferSize)
{
	CBaseDSPEngine::SetMaxBufferSize(uiMaxBufferSize);
}

void CTemplateDSP::SetSampleRate(tuint32 uiSampleRate)
{
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CTemplateDSP::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CTemplateDSP::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
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

void CTemplateDSP::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
	const float* pfIn0 = ppfIn[0];
	const float* pfIn1 = ppfIn[1];
	float* pfOut0 = ppfOut[0];
	float* pfOut1 = ppfOut[1];

	if (mbBypass) {
		// Output meter
		tfloat32 fMeterL = mpfMeterSignal[0];
		tfloat32 fMeterR = mpfMeterSignal[1];

		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];

			tfloat32 fOutL = fabsf(pfOut0[l]);
			tfloat32 fOutR = fabsf(pfOut1[l]);

			if (fOutL > fMeterL) {
				fMeterL = fOutL;
			}
			if (fOutR > fMeterR) {
				fMeterR = fOutR;
			}
		}

		mpfMeterSignal[0] = fMeterL;
		mpfMeterSignal[1] = fMeterR;

		return;
	}


	// TODO: Replace with something meaningful
	{
		for (long l = 0; l < lC; l++) {
			pfOut0[l] = pfIn0[l];
			pfOut1[l] = pfIn1[l];
		}
	}


	// Output meter
	tfloat32 fMeterL = mpfMeterSignal[0];
	tfloat32 fMeterR = mpfMeterSignal[1];

	for (long l = 0; l < lC; l++) {
		tfloat32 fOutL = fabsf(pfOut0[l]);
		tfloat32 fOutR = fabsf(pfOut1[l]);

		if (fOutL > fMeterL) {
			fMeterL = fOutL;
		}
		if (fOutR > fMeterR) {
			fMeterR = fOutR;
		}
	}

	mpfMeterSignal[0] = fMeterL;
	mpfMeterSignal[1] = fMeterR;
}

void CTemplateDSP::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}


