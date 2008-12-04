/*!	\file CDeviceManagerWin.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class CDeviceManager
 * \brief Windows implementation of IDeviceManager
*/
class CDeviceManagerWin : public virtual IDeviceManager
{
public:
	//! Constructor
	CDeviceManagerWin();

	//! Destructor
	virtual ~CDeviceManagerWin();

	//! IDestructable override
	virtual void Destroy();

	//! IDeviceManager override
	virtual tint GetNrOfInputDevices();
	//! IDeviceManager override
	virtual void GetInputDeviceName(tint iIndex, tchar* pszName);
	//! IDeviceManager override
	virtual IIDevice* CreateInputDevice(tint iIndex);
	//! IDeviceManager override
	virtual tint GetNrOfOutputDevices();
	//! IDeviceManager override
	virtual void GetOutputDeviceName(tint iIndex, tchar* pszName);
	//! IDeviceManager override
	virtual IODevice* CreateOutputDevice(tint iIndex);
};

