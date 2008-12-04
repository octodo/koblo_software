/*!	\file CDeviceManagerCoreAudio.h
	\author Michael Olsen
	\date 10/Feb/2006
	\date 10/Feb/2006
*/

/*! \class CDeviceManagerCoreAudio
 * \brief CoreAudio implementation of IDeviceManager
*/
class CDeviceManagerCoreAudio : public virtual IDeviceManager
{
public:
	//! Constructor
	CDeviceManagerCoreAudio();

	//! Destructor
	virtual ~CDeviceManagerCoreAudio();

	//! IDestructable override
	virtual void Destroy();

	//! IDeviceManager override
	virtual tint GetNrOfDevices();
	//! IDeviceManager override
	virtual tbool GetDeviceName(tint iIndex, tchar* pszName);
	//! IDeviceManager override
	virtual tbool GetDeviceName_InputsOnly(tint iIndex, tchar* pszName);
	//! IDeviceManager override
	virtual tbool GetDeviceName_OutputsOnly(tint iIndex, tchar* pszName);
	//! IDeviceManager override
	virtual IDevice* CreateDevice(tint iIndex, tbool bEnableInput = true, tbool bEnableOutput = true);
	//! IDeviceManager override
	virtual tbool DeviceMayHaveOutput(tint iIndex);
	//! IDeviceManager override
	virtual tbool DeviceMayHaveInput(tint iIndex);
	//! IDeviceManager override
	virtual tbool DevicesMayBeDuplex() { return true; };

protected:
	virtual void TestDeviceName_InputOutput(tint iIndex, tbool* pbNameContainsInput, tbool* pbNameContainsOutput);
};

