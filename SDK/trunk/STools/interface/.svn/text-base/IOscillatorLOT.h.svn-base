
class IOscillatorLOT
{
public:
	static IOscillatorLOT* Create();
	virtual void Destroy() = 0;

	virtual void Reset() = 0;
	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	virtual void SetLOT(tfloat* pfLOT, tint iLOTSize) = 0;

	/*! Process "LOT oscillator"
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Frequency buffer !Notice the pitch is in HZ
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessLOT(tfloat32* pfOut, const tfloat32* pfPitch, tint lC) = 0;

	/*! Process "LOT oscillator"
	\	param pfOut [out]:	Pointer to output buffer
	\	param fPitch [in]: Frequency !Notice the pitch is in HZ
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessLOT(tfloat32* pfOut, tfloat fPitch, tint lC) = 0;

	/*! Process "LOT oscillator"
	\	param pfOut [out]:	Pointer to output buffer
	\	param fPitch [in]: Frequency !Notice the pitch is in HZ
	*/
	virtual void ProcessLOT32(tfloat32* pfOut, tfloat fPitch) = 0;
	
	/*! Process "LOT oscillator" with FM
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Frequency buffer !Notice the pitch is in HZ
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFMAmt [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessLOTFM(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC) = 0;

	/*! Process "LOT oscillator" with FM
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Frequency buffer !Notice the pitch is in HZ
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFMAmt [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessLOTFM(tfloat32* pfOut, const tfloat fPitch, const tfloat32* pfFM, const tfloat fFMAmt, tint lC) = 0;

};



