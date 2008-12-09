

#include "SToolsInternalOS.h"

// -160 dB
#define MIN_VOL 1e-8f

// -120 dB
#define MIN_SUSTAIN 1e-6f

// -100 dB
#define MIN_RESETVOL 1e-5f



IEnvelope* IEnvelope::Create()
{
	return dynamic_cast<IEnvelope*>(new CEnvelope());
}

void CEnvelope::Destroy()
{
	delete dynamic_cast<CEnvelope*>(this);
}

CEnvelope::CEnvelope()
{
	mfSampleRate = 44100;

	mfAttackVal = 0;
	mfHoldVal = 0;
	mfDecayVal = 0;
	mfSustainVal = 1;
	mfReleaseVal = 0;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();
}

CEnvelope::~CEnvelope()
{
}

void CEnvelope::SetSampleRate(float fSampleRate)
{
	mfSampleRate = fSampleRate;

	CalcAttack();
	CalcHold();
	CalcDecay();
	CalcSustain();
	CalcRelease();

	// Calc reset
	// ms.
	float fResetVal = 20.0f;
	float fSamples = mfSampleRate / 1000 * fResetVal;
	mfReset = (float)exp(log(0.0000000001) / fSamples);
}

void CEnvelope::Reset()
{
	meState = eStateAttack;
	mfCurMul = 0;
//	mlHold = 0;
}

void CEnvelope::ResetSoft()
{
	meState = eStateReset;
}

void CEnvelope::SetAttack(float fAttackMS)
{
	mfAttackVal = fAttackMS;
	CalcAttack();
}

void CEnvelope::SetHold(float fHoldMS)
{
	mfHoldVal = fHoldMS;
	CalcHold();
}

void CEnvelope::SetDecay(float fDecayMS)
{
	mfDecayVal = fDecayMS;
	CalcDecay();
}

void CEnvelope::SetSustain(float fSustain)
{
	mfSustainVal = fSustain;
	CalcSustain();
}

void CEnvelope::SetRelease(float fReleaseMS)
{
	mfReleaseVal = fReleaseMS;
	CalcRelease();
}

void CEnvelope::EnterRelease()
{
	meState = eStateRelease;
}

tbool CEnvelope::Process(tfloat32* pfOut, tint lC)
{
	tfloat32* pfEnd = pfOut + lC;
	while (pfOut != pfEnd) {
		switch(meState) {
			case eStateAttack:
				while (pfOut != pfEnd) {
					if (mbLinearAttackMode) {
						mfCurMul += mfAttack;
					}
					else {
						if (mfCurMul < 0.0000000001f) {
							mfCurMul = 0.0000000001f;
						}
						mfCurMul *= mfAttack;
					}
					if (mfCurMul >= 1) {
						mfCurMul = 1;
						*pfOut++ = mfCurMul;
						mlHoldIndex = 0;
						meState = eStateHold;
						break;
					}
					else {
						*pfOut++ = mfCurMul;
					}
				}
				break;
			case eStateHold:
				while (pfOut != pfEnd) {
					if (mlHoldIndex++ == mlHold) {
						meState = eStateDecay;
						break;
					}
					else {
						*pfOut++ = 1;
					}
				}
				break;
			case eStateDecay:
				while (pfOut != pfEnd) {
					mfCurMul *= mfDecay;
					if (mfCurMul <= mfSustain) {
						mfCurMul = mfSustain;
						*pfOut++ = mfCurMul;
						meState = eStateSustain;
						break;
					}
					else {
						*pfOut++ = mfCurMul;
					}
				}
				break;
			case eStateSustain:
				while (pfOut != pfEnd) {
					*pfOut++ = mfSustain;
				}
				break;
			case eStateRelease:
				{
					bool bDone = false;
					while (pfOut != pfEnd) {
						mfCurMul *= mfRelease;
						*pfOut++ = mfCurMul;
						if (mfCurMul < MIN_VOL) {
							bDone = true;
						}
					}
					if (bDone) {
						return false;
					}
				}
				break;
			case eStateReset:
				{
					bool bDone = false;
					while ((pfOut != pfEnd) && (bDone == false)) {
						mfCurMul *= mfReset;
						*pfOut++ = mfCurMul;
						if (mfCurMul < MIN_RESETVOL) {
							bDone = true;
						}
					}
					if (bDone) {
						meState = eStateAttack;
					}
				}
				break;
		}
	}

	return true;
}

tbool CEnvelope::ProcessMul(tfloat32* pfOut, tint lC)
{
	tfloat32* pfEnd = pfOut + lC;
	while (pfOut != pfEnd) {
		switch(meState) {
			case eStateAttack:
				while (pfOut != pfEnd) {
					if (mbLinearAttackMode) {
						mfCurMul += mfAttack;
					}
					else {
						if (mfCurMul < 0.0000000001f) {
							mfCurMul = 0.0000000001f;
						}
						mfCurMul *= mfAttack;
					}
					if (mfCurMul >= 1) {
						mfCurMul = 1;
						pfOut++;
						mlHoldIndex = 0;
						meState = eStateHold;
						break;
					}
					else {
						*pfOut++ *= mfCurMul;
					}
				}
				break;
			case eStateHold:
				while (pfOut != pfEnd) {
					if (mlHoldIndex++ == mlHold) {
						meState = eStateDecay;
						break;
					}
					else {
						pfOut++;
					}
				}
				break;
			case eStateDecay:
				while (pfOut != pfEnd) {
					mfCurMul *= mfDecay;
					if (mfCurMul <= mfSustain) {
						mfCurMul = mfSustain;
						*pfOut++ *= mfCurMul;
						meState = eStateSustain;
						break;
					}
					else {
						*pfOut++ *= mfCurMul;
					}
				}
				break;
			case eStateSustain:
				while (pfOut != pfEnd) {
					*pfOut++ *= mfSustain;
				}
				break;
			case eStateRelease:
				{
					bool bDone = false;
					while (pfOut != pfEnd) {
						mfCurMul *= mfRelease;
						*pfOut++ *= mfCurMul;
						if (mfCurMul < MIN_VOL) {
							bDone = true;
						}
					}
					if (bDone) {
						return false;
					}
				}
				break;
			case eStateReset:
				{
					bool bDone = false;
					while ((pfOut != pfEnd) && (bDone == false)) {
						mfCurMul *= mfReset;
						*pfOut++ *= mfCurMul;
						if (mfCurMul < MIN_RESETVOL) {
							bDone = true;
						}
					}
					if (bDone) {
						meState = eStateAttack;
					}
				}
				break;
		}
	}

	return true;
}

void CEnvelope::CalcAttack()
{
	if (mfAttackVal == 0) {
		mfAttack = 1;
		mbLinearAttackMode = true;
	}
	else {
		// High value, i.e. non-linear mode disabled
		if (mfAttackVal < 20000000.0f) {
			mfAttack = 1 / (mfSampleRate / 1000 * mfAttackVal);
			mbLinearAttackMode = true;
		}
		else {
			if (mfAttackVal == 0) {
				mfAttack = 0;
			}
			else {
				float fSamples = mfSampleRate / 1000 * mfAttackVal;
				mfAttack = (float)(1 / exp(log(0.0000000001) / fSamples));
			}

			mbLinearAttackMode = false;
		}
	}
}

void CEnvelope::CalcHold()
{
	if (mfHoldVal == 0) {
		mlHold = 0;
	}
	else {
		mlHold = (long)(mfSampleRate / 1000 * mfHoldVal);
	}
}

void CEnvelope::CalcDecay()
{
	if (mfDecayVal == 0) {
		mfDecay = 0;
	}
	else {
		float fSamples = mfSampleRate / 1000 * mfDecayVal;
		float fRange = 1 - mfSustain;
		if (fRange == 0) {
			mfDecay = 0;
		}
		else {
			if (fRange == 1) {
				mfDecay = 0;
			}
			else {
				mfDecay = exp(log(1 - fRange) / fSamples);
			}
		}
	}
}

void CEnvelope::CalcSustain()
{
	mfSustain = mfSustainVal;
	if (mfSustain < MIN_SUSTAIN) {
		mfSustain = MIN_SUSTAIN;
	}

	// Decay has to be re-calced
	CalcDecay();
}

void CEnvelope::CalcRelease()
{
	if (mfReleaseVal == 0) {
		mfRelease = 0;
	}
	else {
		float fSamples = mfSampleRate / 1000 * mfReleaseVal;
		mfRelease = (float)exp(log(0.0000000001) / fSamples);
	}
}

