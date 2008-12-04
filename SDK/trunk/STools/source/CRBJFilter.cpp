

#include "SToolsInternalOS.h"


IRBJFilter* IRBJFilter::Create()
{
	return dynamic_cast<IRBJFilter*>(new CRBJFilter());
}

void CRBJFilter::Destroy()
{
	delete dynamic_cast<CRBJFilter*>(this);
}

CRBJFilter::CRBJFilter()
{
	mfSampleRate = 44100;
	mfOneDivSampleRate = 1 / mfSampleRate;

	meFilterType = FilterTypeNone;
	CalcFilter();
}

CRBJFilter::~CRBJFilter()
{
}

void CRBJFilter::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;
	mfOneDivSampleRate = 1 / mfSampleRate;

	CalcFilter();
}

void CRBJFilter::Reset()
{
	x0 = x1 = y0 = y1 = 0;
}

void CRBJFilter::CreateLowPass(float fFreq, float fQ)
{
	mfFreq = fFreq;
	mfQ = fQ;
	meFilterType = FilterTypeLowPass;
	CalcFilter();
}

void CRBJFilter::Process(tfloat32* pf, tint lC)
{
	double a1 = this->a1;
	double a2 = this->a2;
	double b0 = this->b0;
	double b1 = this->b1;
	double b2 = this->b2;

	double x0 = this->x0;
	double x1 = this->x1;
	double y0 = this->y0;
	double y1 = this->y1;

	for (long l = 0; l < lC; l++) {
		double dTmp = b0 * pf[l] + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1;

		x1 = x0;
		x0 = pf[l];
		y1 = y0;
		y0 = dTmp;

		pf[l] = (tfloat32)dTmp;
	}

	UnderflowFix(x0);
	UnderflowFix(x1);
	UnderflowFix(y0);
	UnderflowFix(y1);

	this->x0 = x0;
	this->x1 = x1;
	this->y0 = y0;
	this->y1 = y1;
}

void CRBJFilter::ProcessLP(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC)
{
	double dOneDivQ = 1 / sQ;

	double a1 = this->a1;
	double a2 = this->a2;
	double b0 = this->b0;
	double b1 = this->b1;
	double b2 = this->b2;

	double x0 = this->x0;
	double x1 = this->x1;
	double y0 = this->y0;
	double y1 = this->y1;

	long lRecalc = 4;
	for (long l = 0; l < lC; l++) {
		double dTmp = b0 * pf[l] + b1 * x0 + b2 * x1 - a1 * y0 - a2 * y1;

		x1 = x0;
		x0 = pf[l];
		y1 = y0;
		y0 = dTmp;

		pf[l] = (tfloat32)dTmp;

		if (--lRecalc == 0) {
			double dOmega = 2 * M_PI * psFreq[l] * mfOneDivSampleRate;
			double dSinOmega = sin(dOmega);
			double dCosOmega = cos(dOmega);
			double dGamma = dSinOmega * 0.5 * dOneDivQ;

			double onediva0 = 1 / (1 + dGamma);
			b0 = ((1 - dCosOmega) * 0.5) * onediva0;
			b1 = (1 - dCosOmega) * onediva0;
			b2 = b0;
			a1 = (-2 * dCosOmega) * onediva0;
			a2 = (1 - dGamma) * onediva0;

			lRecalc = 4;
		}
	}

	// Recalc for last frequency (in case buffer was so small we didn't trigger any recalc)
	double dOmega = 2 * M_PI * psFreq[lC - 1] * mfOneDivSampleRate;
	double dSinOmega = sin(dOmega);
	double dCosOmega = cos(dOmega);
	double dGamma = dSinOmega * 0.5 * dOneDivQ;

	double onediva0 = 1 / (1 + dGamma);
	b0 = ((1 - dCosOmega) * 0.5) * onediva0;
	b1 = (1 - dCosOmega) * onediva0;
	b2 = b0;
	a1 = (-2 * dCosOmega) * onediva0;
	a2 = (1 - dGamma) * onediva0;

	UnderflowFix(x0);
	UnderflowFix(x1);
	UnderflowFix(y0);
	UnderflowFix(y1);

	this->x0 = x0;
	this->x1 = x1;
	this->y0 = y0;
	this->y1 = y1;

	this->a1 = a1;
	this->a2 = a2;
	this->b0 = b0;
	this->b1 = b1;
	this->b2 = b2;
}

void CRBJFilter::CalcFilter()
{
	switch (meFilterType) {
		case FilterTypeNone:
			CalcEmptyFilter();
			break;
		case FilterTypeLowPass:
			CalcLowPass();
			break;
	}
}

void CRBJFilter::CalcEmptyFilter()
{
	a1 = a2 = b1 = b2 = 0;
	b0 = 1;
}

void CRBJFilter::CalcLowPass()
{
	double dOmega = 2 * M_PI * mfFreq / mfSampleRate;
	double dSinOmega = sin(dOmega);
	double dCosOmega = cos(dOmega);
	double dGamma = dSinOmega / (2 * mfQ);

	double a0 = 1 + dGamma;
	b0 = ((1 - dCosOmega) / 2) / a0;
	b1 = (1 - dCosOmega) / a0;
	b2 = b0;
	a1 = (-2 * dCosOmega) / a0;
	a2 = (1 - dGamma) / a0;
}

