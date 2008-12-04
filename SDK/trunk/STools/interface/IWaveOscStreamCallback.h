


class IWaveOscStreamCallback
{
public:
	//! Called when a buffer of samples needs to be filled
	/*!
		\param iID [in]: ID of callback as when SetCallback was called.
		\param pfOut [out]: Buffer to be filled
		\param iCount [in]: Number of samples to fill in buffer
	*/
	virtual void FillBuffer(tint iID, tfloat32* pfOut, tint iCount) = 0;

	//! Called when a buffer of samples needs to be filled. Call should return immediately, after initiating async data transfer
	/*!
		\param iID [in]: ID of callback as when SetCallback was called.
		\param pfOut [out]: Buffer to be filled
		\param iCount [in]: Number of samples to fill in buffer
	*/
//	virtual void FillBufferAsync(int iID, sample* pfOut, int iCount) = 0;

	//! Called to complete an async transfer. Should not return until the transfer has completed.
	/*!
		\param iID [in]: ID of callback as when SetCallback was called.
	*/
//	virtual void CompleteAsyncTransfer(int iID) = 0;

	//! Called when the osc is reset, i.e. sound is played back from beginning.
	/*!
		\param iID [in]: ID of callback as when SetCallback was called.
	*/
	virtual void Reset(tint32 iID) = 0;
};


