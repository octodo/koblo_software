
class IWaveOscStream
{
public:
	static IWaveOscStream* Create();

	virtual void Destroy() = 0;

	virtual void Reset() = 0;

	//! Sets the callback. Must be called immediately after Create.
	/*!
		\param pCallback [in]: Callback to set.
		\param iID [in]: ID of callback. Can be used by the callback to identify the caller.
	*/
	virtual void SetCallback(IWaveOscStreamCallback* pCallback, tint32 iID) = 0;

	//! Sets the buffer size. Must be set once, and once only, immediately after SetCallback
	/*!
		\param iBufferSize [in]: Buffer size in samples per buffer.
	*/
	virtual void SetBufferSize(tint iBufferSize) = 0;

	//! Sets the buffer count. Must be set once, and once only, immediately after SetBufferSize
	/*!
		\param iBufferCount [in]: Number of buffers. Must be 2 or higher.
	*/
	virtual void SetBufferCount(tint iBufferCount) = 0;

	//! Sets the max output buffer size (iC) that Process will be called with. Must be called once, and once only, immediately after SetBufferCount
	/*!
		\param iMaxOutputBufferSize [in]: Max output buffer size in samples
	*/
	virtual void SetMaxBufferSize(tint iMaxOutputBufferSize) = 0;

	//! Sets the max pitch (fPitch) that Process will be called with. Must be called once, and once only, immeidately after SetMaxBufferSize
	/*!
		\param fMaxPitch [in]: Max pitch (relative to sound source pitch)
	*/
	virtual void SetMaxPitch(tfloat fMaxPitch) = 0;

	//! Generate audio
	/*!
		\param pfOut [out]: Pointer to output buffer, audio is generated in
		\param fPitch [in]: Pitch (Relative to sound source pitch, i.e. 1.0f is original speed, 2.0f is double speed).
		\param iC [in]: Number of samples to generate
	*/
	virtual void Process(tfloat32* pfOut, tfloat32 fPitch, tint iC) = 0;

	//! Call this on regular intervals to prefill buffers
	virtual void Timer() = 0;
};



