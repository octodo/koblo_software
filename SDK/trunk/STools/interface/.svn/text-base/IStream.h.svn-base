/*!	\file IStream.h
	\author Michael Olsen
	\date 19/Jun/2007
*/

/*! A hard disk stream, used for sample players. Currently only works with 16-bit samples.
*
*	Streams are created and managed by the IStreamManager, and should not be created directly by the application / plug-in.
*/
class IStream
{
public:
	//! Initializes and resets the stream. Can be called multiple times with different parameters
	/*!
		\param pFile [in]: File to read stream from
		\param iOffset [in]: Offset into file (in bytes) to start reading from
		\param iLength [in]: Length of sound in file (in samples)
		\param bLoop [in]: If true looping should occur, i.e. after reading iLength samples, reading is restarted from beginning
		\param pInitialData [in]: OBSOLETE! Set to NULL.<br/>Pointer to buffer with 32kb of samples which are identical to the first 32kb of sound.
		\param iBitWidth [in]: Bits per sample. Valid values or 16 or 24, default is 24.
	*/
	virtual void Reset(IFile* pFile, tint32 iOffset, tint32 iLength, tbool bLoop, tint16* piInitialData, tint32 iBitWidth = 24, tint32 iChannels = 1) = 0;

	//! Call this to get the next samples in the stream. The position in the stream will be moved after this call.
	/*!
		\param pfBuffer [out]: Buffer to be filled with data (converter to floating point)
		\param iCount [in]: Number of samples to put in buffer
	*/
	virtual void GetSamples(tfloat32* pfBuffer, tint32 iCount) = 0;

	//! Call this to see if sound playback has stuttered resently (flag will be reset after)
	virtual tbool GetStutter() = 0;

	//! Call this to change the current position
	/*!
		\param tuint32 uiPos: New position, in samples
	*/
	virtual void SetPosition(tuint64 uiPos) = 0;
};
