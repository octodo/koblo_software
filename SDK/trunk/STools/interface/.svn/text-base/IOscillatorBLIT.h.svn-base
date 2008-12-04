/*!	\file IInterpolate.h
	\author Michael Olsen and Max Grønlund
	\date 01/Feb/2006 
	\date 09/Feb/2006
*/

/*! \class CPart
	\brief class using interpolation to finde a value from look up table.
*/


class IOscillatorBLIT
{
public:
	static IOscillatorBLIT* Create();
	virtual void Destroy() = 0;
	//! Reset oscillator phase
	virtual void Reset() = 0;
	//! Set system sample rate
	virtual void SetSampleRate(tfloat fSampleRate) = 0;
	
	/*! Process "Square" wave in output buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, tint lC) = 0;
	
	/*! Process "Square" wave with FM in output buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSquareFM(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC) = 0;
	
	/*! Process "Square" wave with FM in output buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	FM amount
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSquareFM(tfloat32* pfOut, const tfloat pfPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat pfFMAmt, tint lC) = 0;

	/*! Process "Square" wave with FM in buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat pfPitch, const tfloat pfPWM, tint lC) = 0;

	/*! Process "Square" wave with FM in buffer. Buffers is 32 samples long.
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	Pointer to FM amount buffer
	*/
	virtual void ProcessSquare32(tfloat32* pfOut, const tfloat pfPitch, const tfloat pfPWM) = 0;

	/*! Process "Square" wave with FM in output buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tint lC) = 0;
	// Special version to emulate SH-101 pulse subosc
	virtual void ProcessSquareSH(tfloat32* pfOut, const tfloat32* pfPitch, tint lC) = 0;
	virtual void ProcessSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tint lC) = 0;
	
	/*! Process "Square" wave with FM in output buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSaw(tfloat32* pfOut, const tfloat32* pfPitch, tint lC) = 0;
	
	/*! Process "Square" wave
	\	param pfOut [out]:	Pointer to output buffer
	\	param fPitch [in]: frequency !Notice the pitch is in HZ
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSaw(tfloat32* pfOut, tfloat fPitch, tint lC) = 0;

	/*! Process "Square" wave
	\	param pfOut [out]:	Pointer to output buffer
	\	param fPitch [in]: frequency !Notice the pitch is in HZ
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSaw32(tfloat32* pfOut, tfloat fPitch) = 0;
	
	/*! Process "Saw" wave with FM in buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param pfPitch [in]: Pointer to frequency buffer !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param pfFM [in]:	Pointer to FM amount buffer
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSawFM(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC) = 0;
	
	/*! Process "Saw" wave with FM in buffer
	\	param pfOut [out]:	Pointer to output buffer
	\	param fPitch [in]: frequency !Notice the pitch is in HZ
	\	param fPWM [in]:	Pulse with in the range 0.0f-1.0f
	\	param pfFM [in]:	Pointer to buffer with the modulation audio signal
	\	param fFMAmt [in]:	FM amount
	\	param lC [in]:		Number of samples to process
	*/
	virtual void ProcessSawFM(tfloat32* pfOut, const tfloat fPitch, const tfloat32* pfFM, const tfloat fFMAmt, tint lC) = 0;

	// Need to be able to set inital phase for master (for cases where sync is switched on while playing a note)
	virtual void ProcessSawSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, tint lC) = 0;
	virtual void ProcessSawAndSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint) = 0;
	virtual void ProcessSawAndSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint lC) = 0;
};



