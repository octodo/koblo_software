

/*

AHDSR Envelope generator

To get a ADSR envelope, simply set hold to 0

*/

class IEnv
{
public:
	static IEnv* Create();
	virtual void Destroy() = 0;

	virtual void SetSampleRate(tfloat fSampleRate) = 0;

	// Full reset
	virtual void Reset() = 0;
	// Fluent reset
	virtual void ResetSoft() = 0;
	// Returns false if release ended
	virtual tbool Process(tfloat32* pfOut, tint lC) = 0;
	virtual tbool Process32(tfloat32* pfOut) = 0;
	virtual void EnterRelease() = 0;

	virtual void SetAttack(tfloat fAttackMS) = 0;
	virtual void SetHold(tfloat fHoldMS) = 0;
	virtual void SetDecay(tfloat fDecayMS) = 0;
	// Normalised volume
	virtual void SetSustain(tfloat fSustain) = 0;
	virtual void SetRelease(tfloat fReleaseMS) = 0;
};



