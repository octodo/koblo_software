

/*

Note: None of the process calls must be feeded frequencies above 30000

*/

class IVCFFilter
{
public:
	static IVCFFilter* ST_CALL Create();
	virtual void ST_CALL Destroy() = 0;

	virtual void ST_CALL SetSampleRate(tfloat fSampleRate) = 0;

	virtual void ST_CALL Reset() = 0;

	// 0 <= fQ <= 1
	virtual void ST_CALL Process(tfloat32* pf, tfloat fFreq, tfloat fQ, tint lC) = 0;
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC) = 0;
	// This version is 3-pole
	virtual void ST_CALL Process3(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC) = 0;
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC) = 0;
	// This version does not do low-pass, only resonance (does not seem to work)
	virtual void ST_CALL ProcessQ(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC) = 0;
	// This version does not do low-pass, only resonance (does not seem to work)
	virtual void ST_CALL ProcessQ(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC) = 0;
	// Q == 0 for this call
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, tint lC) = 0;
};



