
class IResampler
{
public:
	static IResampler* Create();

	virtual void Destroy() = 0;

	virtual void Reset() = 0;

	virtual tint GetNrOfInputSamplesNeeded(tfloat fResampleFactor, tuint uiOutputBufferSize) = 0;

	virtual void Resample(tuint uiOutputBufferSize, tuint uiInputBufferSize, tfloat fResampleFactor, tfloat *ptOut, tfloat *ptIn) = 0;

	virtual void ResampleDual(tuint uiOutputBufferSize, tuint uiInputBufferSize, tfloat fResampleFactor, tfloat *ptOut1, tfloat *ptOut2, tfloat *ptIn1, tfloat *ptIn2) = 0;
};
