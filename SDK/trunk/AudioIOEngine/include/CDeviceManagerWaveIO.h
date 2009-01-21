/*!	\file CDeviceManagerWaveIO.h
	\author Lasse Steen Bohnstedt
	\date 2008-05-02
*/

/*! \class CDeviceManagerWaveIO
 * \brief WaveIO implementation of IDeviceManager
*/
class CDeviceManagerWaveIO : public virtual IDeviceManager
{
public:
	//! Constructor
	CDeviceManagerWaveIO();

	//! Destructor
	virtual ~CDeviceManagerWaveIO();

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
	virtual tbool DevicesMayBeDuplex() { return false; };

protected:
	class SMyWaveOutCaps {
	public:
		tbool b44100;
		tbool b48000;
		tbool b88200;
		tbool b96000;
		WAVEOUTCAPS wocaps;

		SMyWaveOutCaps() {
			b44100 = FALSE;
			b48000 = FALSE;
			b88200 = FALSE;
			b96000 = FALSE;
			wocaps.dwFormats = 0;
			wocaps.wChannels = 0;
			wocaps.szPname[0] = '\0';
		};
	};

	class SWaveInCaps {
	public:
		WAVEINCAPS wicaps;

		SWaveInCaps() {
		}
	};

	tuint32 muiNbOfDrivers_Outputs;
	tuint32 muiNbOfDrivers_Total;
	std::list<SMyWaveOutCaps*> mlistpMyWaveOutCaps;
	std::list<SWaveInCaps*> mWaveInCaps;
};

