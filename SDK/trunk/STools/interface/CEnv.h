



class CEnv : public virtual IEnv
{
public:
	CEnv();
	virtual ~CEnv();

	// IEnv overrides
	virtual void Destroy();
	virtual void SetSampleRate(float fSampleRate);
	virtual void Reset();
	virtual void ResetSoft();
	virtual bool Process(sample* pfOut, long lC);
	virtual bool ProcessMul(sample* pfOut, long lC);
	virtual void EnterRelease();
	virtual void SetAttack(float fAttackMS);
	virtual void SetHold(float fHoldMS);
	virtual void SetDecay(float fDecayMS);
	virtual void SetSustain(float fSustain);
	virtual void SetRelease(float fReleaseMS);

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

