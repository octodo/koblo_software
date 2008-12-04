

#include "VST2KSPIOS.h"


// -313dB, approx. 52 bits
#define UNDERFLOW_POSITIVE 2.2387211385683396119549508524657e-16
//#define UNDERFLOW_POSITIVE 2.2387211385683396119549508524657e-1
#define UNDERFLOW_NEGATIVE (0 - UNDERFLOW_POSITIVE)
// -138dB, approx. 23 bits
//#define UNDERFLOW_POSITIVE_FLOAT 1.2589254117941672104239541063958e-7
// -120dB, approx. 20 bits
#define UNDERFLOW_POSITIVE_FLOAT 0.000001
//#define UNDERFLOW_POSITIVE_FLOAT 1.2589254117941672104239541063958e-1
#define UNDERFLOW_NEGATIVE_FLOAT (0 - UNDERFLOW_POSITIVE_FLOAT)

inline void UnderflowFix(double& d) {
	if ((d < UNDERFLOW_POSITIVE) &&
		(d > UNDERFLOW_NEGATIVE)) {
		d = 0.0;
	}
}
inline void UnderflowFix(float& f) {
	if ((f < UNDERFLOW_POSITIVE_FLOAT) &&
		(f > UNDERFLOW_NEGATIVE_FLOAT)) {
		f = 0.0f;
	}
}

CEcho::CEcho()
	: muiChannels(0)
{
}

CEcho::~CEcho()
{
}

void CEcho::Destroy()
{
	delete dynamic_cast<CEcho*>(this);
}

void CEcho::Start()
{
	mEnvelope1 = 0.0;
	mEnvelope2 = 0.0;

	mdHPHistoryL = mdHPHistoryR = 0.0;
}

void CEcho::Stop()
{
}

void CEcho::SetSampleRate(tuint32 uiSampleRate)
{
	CBaseDSPEngine::SetSampleRate(uiSampleRate);
}

void CEcho::SetChannels(tuint32 uiChannels)
{
	muiChannels = uiChannels;
}

void CEcho::SetThreshold(float fThreshold)
{
	mfThresholddB = fThreshold;

	CookCurve();
}

void CEcho::SetGain(float fGain)
{
	if (fGain == 0) {
		mfGain = 1;
	}
	else {
		mfGain = pow(10, fGain / 20);
	}
}

void CEcho::SetAttack(float fAttack)
{
}

void CEcho::SetRelease(float fRelease)
{
}

void CEcho::SetRatio(float fRatio)
{
	mfRatio = fRatio;

	CookCurve();
}

void CEcho::SetKnee(int iKnee)
{
	miKnee = iKnee;

	CookCurve();
}

void CEcho::CookCurve()
{
	if (mfRatio == 0.0f) {
		return;
	}
}

void CEcho::ProcessMono(float** ppfOut, const float** ppfIn, long lC)
{
}

void CEcho::ProcessStereo(float** ppfOut, const float** ppfIn, long lC)
{
/*	if (bBypass) {
		float fVU1 = 0.0;
		for (long l = 0; l < lC; l++) {
			double dIn1 = fabs(pf1[l]);
			double dIn2 = fabs(pf2[l]);
			double dIn = max(dIn1, dIn2);
			fVU1 = max(fVU1, (float)dIn);
		}
		fVU = fVU1;
		fVUGR = 1.0f;
		return;
	}*/

	float* pfOutL = ppfOut[0];
	float* pfOutR = ppfOut[1];
	const float* pfInL = ppfIn[0];
	const float* pfInR = ppfIn[1];

	// Input meter
	for (long l = 0; l < lC; l++) {
//		float fInL = fabsf(pfInL[l]);
//		float fInR = fabsf(pfInR[l]);
//		float fIn = max(fInL, fInR);
		float fIn = fabsf((pfInL[l] + pfInR[l]) / 2);

/*		double d = VUInFilter.b[0] * fIn +
			VUInFilter.b[1] * VUInFilter.x[0] +
			VUInFilter.b[2] * VUInFilter.x[1] -
			VUInFilter.a[0] * VUInFilter.y[0] -
			VUInFilter.a[1] * VUInFilter.y[1];

		VUInFilter.x[1] = VUInFilter.x[0];
		VUInFilter.x[0] = fIn;
		VUInFilter.y[1] = VUInFilter.y[0];
		VUInFilter.y[0] = d;*/
	}
//	mpfMeterSignal[0] = VUInFilter.y[0];

	lC++;
	while (--lC != 0) {
	}
}

void CEcho::Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples)
{
	if (muiChannels == 1) {
		ProcessMono(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
	else {
		ProcessStereo(ppfSamplesOut, ppfSamplesIn, iNrOfSamples);
	}
}

