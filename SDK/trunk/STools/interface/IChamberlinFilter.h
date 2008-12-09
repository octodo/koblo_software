

/*
 Chamberlin filter. 12 dB
*/

class IChamberlinFilter
{
public:
	static IChamberlinFilter* Create(tfloat* pfFreqToCutoff, tint iSize);
	virtual void Destroy() = 0;

	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	virtual void Reset() = 0;

	//! Lo pass 
	virtual void ProcessLP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC) = 0;
	//! 32-sample variant
	virtual void ProcessLP32(tfloat32* psAudio, tfloat fFreq, tfloat fRes) = 0;
	//! Band pass 
	virtual void ProcessBP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC) = 0;
	//! Hi pass 
	virtual void ProcessHP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC) = 0;
	//! Hi pass 
	virtual void Process(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tfloat32 fLP, tfloat32 fBP, tfloat32 fHP, tint lC) = 0;
	//! Hi pass 
	virtual void Process32(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tfloat32 fLP, tfloat32 fBP, tfloat32 fHP, tint lC) = 0;
};



