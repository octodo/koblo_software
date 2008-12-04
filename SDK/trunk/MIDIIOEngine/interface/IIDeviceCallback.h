/*!	\file IIDeviceCallback.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IIDeviceCallback
 * \brief Callback used by IIDevice
*/
class IIDeviceCallback
{
public:
	//! Called when MIDI has been received
	/*!
		\param ucStatus [in]: Status byte
		\param ucDB1 [in]: Data byte 1
		\param ucDB2 [in]: Data byte 2
	*/
	virtual void OnMIDI(tuchar ucStatus, tuchar ucDB1, tuchar ucDB2) = 0;

	//! Called when MIDI sys-ex has been received
	/*!
		\param p [in]: Pointer to sysex buffer
		\param iSize [in]: Size, in bytes, of sysex buffer
	*/
	virtual void OnMIDISysEx(const void* p, tint iSize) = 0;
};

