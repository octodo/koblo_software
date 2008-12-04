

/*

AHDSR Envelope generator

To get a ADSR envelope, simply set hold to 0
This envelope is optimiced so it only produces one samplepoint for each 32 samples

*/

class IModEnvelope
{
public:
	static IModEnvelope* Create();
	virtual void Destroy() = 0;

	virtual void SetSampleRate(tfloat fSampleRate) = 0;

	// Full reset
	virtual void Reset() = 0;
	// Fluent reset
	virtual void ResetSoft() = 0;
	// Returns false if release ended
	virtual float Process(tint lC) = 0;
	// This version has no linar segments
	virtual float ProcessMul(tint lC) = 0;
	virtual void EnterRelease() = 0;

	virtual void SetAttack(tfloat fAttackMS) = 0;
	virtual void SetHold(tfloat fHoldMS) = 0;
	virtual void SetDecay(tfloat fDecayMS) = 0;
	// Normalised volume
	virtual void SetSustain(tfloat fSustain) = 0;
	virtual void SetRelease(tfloat fReleaseMS) = 0;
};



