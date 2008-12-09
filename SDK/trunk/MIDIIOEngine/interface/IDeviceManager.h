/*!	\file IDeviceManager.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IDeviceManager
 * \brief A manager to keep track of devices. Use this to create devices and to enumerate them.
*/
class IDeviceManager : public virtual IDestructable
{
public:
	static IDeviceManager* Create();

	//! Gets the number of input devices
	/*!
		\return tint: Number of input devices
	*/
	virtual tint GetNrOfInputDevices() = 0;

	//! Gets the name of a input device with a given ID (0 to number of input devices - 1)
	/*!
		\param iIndex [in]: Index of device to get name for (0 to number of input devices - 1)
		\param pszName [out]: Pointer to array to receive name. Must be 129 chars or more long
	*/
	virtual void GetInputDeviceName(tint iIndex, tchar* pszName) = 0;

	//! Creates an input device from a given ID (0 to number of input devices - 1)
	/*!
		\param iIndex [in]: Index of device to create (0 to number of input devices - 1)
		\return IIDevice*: Created device
	*/
	virtual IIDevice* CreateInputDevice(tint iIndex) = 0;

	//! Gets the number of output devices
	/*!
		\return tint: Number of output devices
	*/
	virtual tint GetNrOfOutputDevices() = 0;

	//! Gets the name of a output device with a given ID (0 to number of output devices - 1)
	/*!
		\param iIndex [in]: Index of device to get name for (0 to number of output devices - 1)
		\param pszName [out]: Pointer to array to receive name. Must be 129 chars or more long
	*/
	virtual void GetOutputDeviceName(tint iIndex, tchar* pszName) = 0;

	//! Creates an output device from a given ID (0 to number of output devices - 1)
	/*!
		\param iIndex [in]: Index of device to create (0 to number of output devices - 1)
		\return IODevice*: Created device
	*/
	virtual IODevice* CreateOutputDevice(tint iIndex) = 0;
};

