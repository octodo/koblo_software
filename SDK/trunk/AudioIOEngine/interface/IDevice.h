/*!	\file IDevice.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IDevice
 * \brief A device can have inputs, outputs or both. The functionality supported by various devices varies, so be sure to ask for support of advanced functionality.
*/
class IDevice : public virtual IDestructable
{
public:
	//! Creates an ASIO device (Win32 only)
	static IDevice* CreateASIO();
	//! Creates a WAVE device (Win32 mono/stereo only)
	static IDevice* CreateWaveIO();
	//! Creates a CoreAudio device (MacOSX only)
	static IDevice* CreateCoreAudio();

	//! Initialize the device
	/*!
		\param iSampleRate [in]: Sample rate to set
		\param iHWBufferSize [in]: Buffersize to use in hardware
		\return tbool: True if settings were ok, False if they weren't acceptable
	*/
	virtual tbool Initialize(tint32 iSampleRate, tint iHWBufferSize) = 0;

	//! Start the device. It will run until destroyed
	/*!
		\return tbool: True if device started
	*/
	virtual tbool Start() = 0;

	//! Sets the callback which will be called for processing
	/*!
		\param pCallback [in]: Callback to set
	*/
	virtual void SetCallback(IDeviceCallback* pCallback) = 0;

	//! Gets the time stamp, measured in samples since last processing call initiated
	/*!
		\return tuint32: Time stamp since last processing call initiated
	*/
	virtual tuint32 GetTimeStamp() = 0;

	//! Gets the devices preferred size (may be only size supported)
	/*!
		\return tuint32: Buffer size
	*/
	virtual tuint32 GetPreferredBufferSize() = 0;

	//! Gets the buffersize used by the hardware (and thus used in the callbacks)
	/*!
		\return tuint32: Buffer size
	*/
	virtual tuint32 GetBufferSize() = 0;
	
	//! Gets the min buffersize used by the hardware
	/*!
		\return tuint32: Min. buffer size
	*/
	virtual tuint32 GetMinBufferSize() = 0;

	//! Gets the max buffersize used by the hardware
	/*!
		\return tuint32: Max. buffer size
	*/
	virtual tuint32 GetMaxBufferSize() = 0;

	virtual tuint32 GetPreferredSampleRate() = 0;
	virtual tuint32 GetSampleRate() = 0;
	virtual tuint32 EnumeratePossibleSampleRates(tuint32 uiLast) = 0;

	virtual tint32 GetInputChannels() = 0;

	virtual tint32 GetOutputChannels() = 0;

	//! Opens a setting dialog (only supported by ASIO)
	virtual void ShowSettings() = 0;
};

