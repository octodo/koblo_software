// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



class CModEnvelope : public virtual IModEnvelope
{
public:
	CModEnvelope();
	virtual ~CModEnvelope();

	// IEnvelope overrides
	virtual void Destroy();
	virtual void SetSampleRate(tfloat fSampleRate);
	virtual void Reset();
	virtual void ResetSoft();
	virtual float Process(tint lC);
	virtual float ProcessMul(tint lC);
	virtual void EnterRelease();
	virtual void SetAttack(tfloat fAttackMS);
	virtual void SetHold(tfloat fHoldMS);
	virtual void SetDecay(tfloat fDecayMS);
	virtual void SetSustain(tfloat fSustain);
	virtual void SetRelease(tfloat fReleaseMS);

protected:
	float mfSampleRate;
	enum EState {
		eStateAttack = 0,
		eStateHold,
		eStateDecay,
		eStateSustain,
		eStateRelease,
		eStateReset
	};
	EState meState;

	#define gfOneDivThityTwo 1.0f/32.0f

	float mfAttackVal;
	float mfHoldVal;
	float mfDecayVal;
	float mfSustainVal;
	float mfReleaseVal;

	// Cooked values
	float mfCurMul;
	float mfAttack;
	long mlHoldIndex;
	long mlHold;
	float mfDecay;
	float mfSustain;
	float mfRelease;
	float mfReset;

	virtual void CalcAttack();
	virtual void CalcHold();
	virtual void CalcDecay();
	virtual void CalcSustain();
	virtual void CalcRelease();

	bool mbLinearAttackMode;
};

