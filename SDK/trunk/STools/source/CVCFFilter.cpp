

#include "SToolsInternalOS.h"


static const long glFakeFreq = 16000;


IVCFFilter* IVCFFilter::Create()
{
	return dynamic_cast<IVCFFilter*>(new CVCFFilter());
}

void CVCFFilter::Destroy()
{
	delete dynamic_cast<CVCFFilter*>(this);
}

CVCFFilter::CVCFFilter()
{
}

CVCFFilter::~CVCFFilter()
{
}

void CVCFFilter::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;
	mfOneDivSampleRate = 1 / mfSampleRate;
	mfOneDivSampleRateMulPI = (float)(mfOneDivSampleRate * M_PI);
}

void CVCFFilter::Reset()
{
	y1 = y2 = y3 = y4 = oldx = oldy1 = oldy2 = oldy3 = 0;
}

void CVCFFilter::Process(tfloat32* pf, tfloat fFreq, tfloat fQ, tint lC)
{
	double f = 2 * fFreq * mfOneDivSampleRate;
	// Empirical tunning
	double k = 3.6 * f - 1.6 * f * f - 1;
	double p = (k + 1) * 0.5;
//	double scale = pow(e, ((1 - p) * 1.386249));
	double scale = exp((1 - p) * 1.386249);
	// Scale Q according to frequency
	double dTmp = ((30000 - fFreq) / 30000);
	double r = (fQ * dTmp * dTmp * dTmp * dTmp) * scale;

	for (long l = 0; l < lC; l++) {
		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Four cascaded onepole filters (bilinear transform)
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

void CVCFFilter::Process(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;
//		double scale = pow(e, ((1 - p) * 1.386249));
		double scale = exp((1 - p) * 1.386249);
		// Scale Q according to frequency
		double dTmp = ((30000 - psFreq[l]) / 30000);
		double r = (psQ[l] * dTmp * dTmp * dTmp * dTmp) * scale;

		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Four cascaded onepole filters (bilinear transform)
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

void CVCFFilter::Process(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;
		double scale = exp((1 - p) * 1.386249);
		// Scale Q according to frequency
		double dTmp = ((30000 - psFreq[l]) / 30000);
		double r = (sQ * dTmp * dTmp * dTmp * dTmp) * scale;

		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Four cascaded onepole filters (bilinear transform)
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

void CVCFFilter::Process3(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;
		double scale = exp((1 - p) * 1.386249);
		// Scale Q according to frequency
		double dTmp = ((30000 - psFreq[l]) / 30000);
		double r = (sQ * dTmp * dTmp * dTmp * dTmp) * scale;

		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Three cascaded onepole filters (bilinear transform)
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y4 = y2 * p + oldy2 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
	}
}

void CVCFFilter::ProcessQ(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;
		double scale = exp((1 - p) * 1.386249);
		// Scale Q according to frequency
		double dTmp = ((30000 - psFreq[l]) / 30000);
		double r = (psQ[l] * dTmp * dTmp * dTmp * dTmp) * scale;

		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Four cascaded onepole filters (bilinear transform)
		// Recalc for "fake" freq
		f = 2 * glFakeFreq * mfOneDivSampleRate;
		k = 3.6 * f - 1.6 * f * f - 1;
		p = (k + 1) * 0.5;
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

void CVCFFilter::ProcessQ(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;
		double scale = exp((1 - p) * 1.386249);
		// Scale Q according to frequency
		double dTmp = ((30000 - psFreq[l]) / 30000);
		double r = (sQ * dTmp * dTmp * dTmp * dTmp) * scale;

		// Inverted feed back for corner peaking
		double x = pf[l] - r * y4;

		// Four cascaded onepole filters (bilinear transform)
		// Recalc for "fake" freq
		f = 2 * glFakeFreq * mfOneDivSampleRate;
		k = 3.6 * f - 1.6 * f * f - 1;
		p = (k + 1) * 0.5;
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

void CVCFFilter::Process(tfloat32* pf, const tfloat32* psFreq, tint lC)
{
	for (long l = 0; l < lC; l++) {
		double f = 2 * psFreq[l] * mfOneDivSampleRate;
		// Empirical tunning
		double k = 3.6 * f - 1.6 * f * f - 1;
		double p = (k + 1) * 0.5;

		double x = pf[l];

		// Four cascaded onepole filters (bilinear transform)
		y1 = x * p + oldx * p - k * y1;
		y2 = y1 * p + oldy1 * p - k * y2;
		y3 = y2 * p + oldy2 * p - k * y3;
		y4 = y3 * p + oldy3 * p - k * y4;

		// Clipper band limited sigmoid
		y4 = y4 - (y4 * y4 * y4 / 6);
		pf[l] = (tfloat32)y4;

		oldx = x;
		oldy1 = y1;
		oldy2 = y2;
		oldy3 = y3;
	}
}

