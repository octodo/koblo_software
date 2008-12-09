

class ILimiter
{
public:
	static ILimiter* ST_CALL Create();
	virtual void ST_CALL Destroy() = 0;

	virtual void ST_CALL SetSampleRate(tfloat fSampleRate) = 0;

	virtual void ST_CALL Reset() = 0;

	// Stereo
	virtual void ST_CALL Process(tfloat32* pfLeft, tfloat32* pfRight, tint iSamples) = 0;

};



