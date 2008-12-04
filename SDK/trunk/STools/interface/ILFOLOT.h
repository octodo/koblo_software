
/*

LFO generator

 Works only with 2048 samplepoint long tables

*/
class ILFOLOT
{
public:
	static ILFOLOT* Create();
	virtual void Destroy() = 0;

	virtual void Reset() = 0;
	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	virtual void SetAttack(tfloat fMs) = 0;
	virtual void SetLOT(tfloat* pfLOT, tint iLOTSize) = 0;
	// Pitch buffer is in Hz
	virtual float Process(const tfloat32 pfRate, tint lC) = 0;
	
};



