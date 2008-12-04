/*!	\file IDeviceCallback.h
	\author Michael Olsen
	\date 23/Feb/2005
	\date 23/Feb/2005
*/

/*! \class IDeviceCallback
 * \brief Callback for audio device
*/
class IDeviceCallback
{
public:
	//! Called when it's time to process audio
	/*!
		\param ppOut [in]: Pointer to array of buffers to put data into
		\param ppOut [in]: Pointer to array of buffers to read data from
		\param iSamples [in]: Nr of samples in buffer
	*/
	virtual void ProcessAudio(tfloat32** ppOut, const tfloat32** ppIn, tint iSamples) = 0;
};

