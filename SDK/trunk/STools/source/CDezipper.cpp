

#include "SToolsInternalOS.h"

IDezipper* IDezipper::Create()
{
	return dynamic_cast<IDezipper*>(new CDezipper());
}

void CDezipper::Destroy()
{
	delete dynamic_cast<CDezipper*>(this);
}

CDezipper::CDezipper()
{
	mfSampleRate = 44100;
	mfFreq = 10.0f;

	CalcFilter();
	Reset();

	mdTarget = 0;
}

CDezipper::~CDezipper()
{
}

void CDezipper::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;

	CalcFilter();
}

void CDezipper::Reset()
{
	dHistory = 0;
}

void CDezipper::JumpToTarget(double dTarget)
{
	dHistory = mdTarget = dTarget;
}

void CDezipper::SetTarget(double dTarget)
{
	mdTarget = dTarget;
}

double CDezipper::Dezip(long lC)
{
	double dHistory = this->dHistory;
	double dX = this->dX;
	double mdTarget = this->mdTarget;

	for (long l = 0; l < lC; l++) {
		dHistory = dHistory + (mdTarget - dHistory) * dX;
	}

	UnderflowFix(dHistory);

	this->dHistory = dHistory;

	return this->dHistory;
}

void CDezipper::CalcFilter()
{
	dX = mfFreq / (mfSampleRate / 2);
}
