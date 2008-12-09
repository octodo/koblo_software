/*!	\file IDeviceManager.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IDeviceManager
 * \brief A device manager exists for each device type, i.e. ASIO, DirectX, etc. supported. Use the device manager to enumerate devices, and to create them.
*/
class IDeviceManager : public virtual IDestructable
{
public:
	//! Creates an ASIO device manager (Win32 only)
	static IDeviceManager* CreateASIO();
	//! Creates a WAVE device (Win32 mono/stereo only)
	static IDeviceManager* CreateWaveIO();
	//! Creates a CoreAudio device manager (MacOSX only)
	static IDeviceManager* CreateCoreAudio();

	//! Gets the number of devices. The range of devices go from -1 (meaning default device) through 0 (first device) up to number-of-devices - 1 (last device)
	/*!
		\return tint: Number of devices - this counts both inputs and outputs
	*/
	virtual tint GetNrOfDevices() = 0;

	//! Gets the name of an audio device with a given ID (-1 to number of devices - 1)
	/*!
		\param iIndex [in]: Index of device to get name for (-1 to number of devices - 1)
		\param pszName [out]: Pointer to array to receive name. Must be 129 chars or more long
		\return tbool: The index is valid and a name has been found for it
	*/
	virtual tbool GetDeviceName(tint iIndex, tchar* pszName) = 0;

	//! Gets the name of an audio device with a given ID (-1 to number of devices - 1). The device is probably an input enabled device
	/*!
		\param iIndex [in]: Index of device to get name for (-1 to number of devices - 1)
		\param pszName [out]: Pointer to array to receive name. Must be 129 chars or more long
		\return tbool: The index is indeed probably an input device and a name has been found
	*/
	virtual tbool GetDeviceName_InputsOnly(tint iIndex, tchar* pszName) = 0;

	//! Gets the name of an audio device with a given ID (-1 to number of devices - 1). The device is probably an output enabled device
	/*!
		\param iIndex [in]: Index of device to get name for (-1 to number of devices - 1)
		\param pszName [out]: Pointer to array to receive name. Must be 129 chars or more long
		\return tbool: The index is indeed probably an output device and a name has been found
	*/
	virtual tbool GetDeviceName_OutputsOnly(tint iIndex, tchar* pszName) = 0;

	//! Creates an device from a given ID (-1 to number of devices - 1)
	/*!
		\param iIndex [in]: Index of device to create (-1 to number of devices - 1)
		\param bEnableInput [in]: True if the device should run it's input buffers / code / what-ever
		\param bEnableInput [in]: True if the device should run it's output buffers / code / what-ever
		\return IIDevice*: Created device - is NULL upon error (driver load failed)
	*/
	virtual IDevice* CreateDevice(tint iIndex, tbool bEnableInput = true, tbool bEnableOutput = true) = 0;

	//! Check if a given device may have output (no certainty)
	/*!
		\param iIndex [in]: Index of device to look at (-1 to number of devices - 1)
		\return tbool: True: This device will probably have output
	*/
	virtual tbool DeviceMayHaveOutput(tint iIndex) = 0;

	//! Check if a given device may have input (no certainty)
	/*!
		\param iIndex [in]: Index of device to look at (-1 to number of devices - 1)
		\return tbool: True: This device will probably have input
	*/
	virtual tbool DeviceMayHaveInput(tint iIndex) = 0;

	//! Returns true if the devices created by this manager can be running as both input and output at the same time - wave devices cannot, ASIO can, Core audio can. Note: Individual devices may be only input or output even though their device type as a whole supports duplex
	virtual tbool DevicesMayBeDuplex() = 0;

};

