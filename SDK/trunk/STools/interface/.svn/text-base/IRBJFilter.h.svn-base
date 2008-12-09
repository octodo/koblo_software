

/*

Robert Bristow-Johnson based IIR filters.
2-pole

*/

class IRBJFilter
{
public:
	static IRBJFilter* Create();
	virtual void Destroy() = 0;

	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	virtual void Reset() = 0;

	virtual void CreateLowPass(tfloat fFreq, tfloat fQ = 0.5) = 0;

	virtual void Process(tfloat32* pf, tint lC) = 0;

	// Call 1 initial CreateLowPass(...) before using this
	virtual void ProcessLP(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC) = 0;
};



