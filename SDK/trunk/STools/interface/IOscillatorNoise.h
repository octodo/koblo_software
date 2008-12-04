
class IOscillatorNoise
{
public:
	static IOscillatorNoise* Create(tfloat* pfFreqToCutoff, tint iSize);
	virtual void Destroy() = 0;

	virtual void Reset() = 0;
	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	virtual void SetNoise(tfloat* pfNoise, tint iNoiseSize) = 0;
	// Pitch buffer is in Hz
	virtual void ProcessNoise(tfloat32* pfOut, const tfloat32* pfPitch, tint lC) = 0;
	virtual void ProcessNoise(tfloat32* pfOut, const tfloat pfPitch, tint lC) = 0;
	virtual void ProcessNoiseFM(tfloat32* pfOut, const tfloat32* pfPitch,const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC) = 0;

};



