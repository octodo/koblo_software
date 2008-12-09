/*!	\file CDeviceManagerASIO.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class CDeviceManagerASIO
 * \brief ASIO implementation of IDeviceManager
*/
class CDeviceManagerASIO : public virtual IDeviceManager
{
public:
	//! Constructor
	CDeviceManagerASIO();

	//! Destructor
	virtual ~CDeviceManagerASIO();

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
	AsioDrivers mDriverList;

	virtual void TestDeviceName_InputOutput(tint iIndex, tbool* pbNameContainsInput, tbool* pbNameContainsOutput);
};

