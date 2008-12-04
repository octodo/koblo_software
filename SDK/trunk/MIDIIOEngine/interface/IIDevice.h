/*!	\file IIDevice.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IIDevice
 * \brief A device with 16 input channels
*/
class IIDevice : public virtual IDestructable
{
public:
	//! Create a new IIDevice. Should only be called by MIDI Manager.
	/*!
		\param p [in]: System specific. Windows: Pointer to tint, holding index of device to create
		\return IIDevice*: Pointer to created device, or NULL if error
	*/
	static IIDevice* Create(void* p);

	//! Sets the callback. This must be done immediately after creating the device.
	/*!
		\param pCallback [in]: Callback to call on MIDI in
	*/
	virtual void SetCallback(IIDeviceCallback* pCallback) = 0;
};

