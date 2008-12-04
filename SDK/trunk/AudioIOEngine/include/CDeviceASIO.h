/*!	\file CDeviceASIO.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#define ASIO_MAX_INPUT_CHANNELS 32
#define ASIO_MAX_OUTPUT_CHANNELS 64

/*! \class CDeviceASIO
 * \brief ASIO implementation of IDevice
*/
class CDeviceASIO : public virtual IDevice
{
public:
	//! Constructor
	CDeviceASIO();

	//! Destructor
	virtual ~CDeviceASIO();

	//! IDestructable override
	virtual void Destroy();

	//! IDevice override
	virtual tbool Initialize(tint32 iSampleRate, tint iHWBufferSize);
	//! IDevice override
	virtual tbool Start();
	//! IDevice override
	virtual void SetCallback(IDeviceCallback* pCallback);
	//! IDevice override
	virtual tuint32 GetTimeStamp();
	//! IDevice override
	virtual tuint32 GetPreferredBufferSize();
	//! IDevice override
	virtual tuint32 GetBufferSize();
	//! IDevice override
	virtual tuint32 GetMinBufferSize();
	//! IDevice override
	virtual tuint32 GetMaxBufferSize();
	//! IDevice override
	virtual tuint32 GetPreferredSampleRate();
	//! IDevice override
	virtual tuint32 GetSampleRate();
	//! IDevice override
	virtual tuint32 EnumeratePossibleSampleRates(tuint32 uiLast);
	//! IDevice override
	virtual void ShowSettings();
	//! IDevice override
	virtual tint32 GetInputChannels();
	//! IDevice override
	virtual tint32 GetOutputChannels();

	//! Called immediately after Create to load driver
	/*!
		\param pszName [in]: Name of driver to load
	*/
	virtual tbool LoadDriver(const tchar* pszName, tbool bEnableInput, tbool bEnableOutput);
	//virtual void LoadDriver(const tchar* pszName);


protected:
	//! Callback pointer
	IDeviceCallback* mpCallback;

	//! ASIO SDK class to access drivers
	AsioDrivers mDrivers;

	//! Info about driver loaded
	ASIODriverInfo mInfo;

	//! Modified version of ASIOInit from the ASIO SDK
	ASIOError ASIOInit();

	tbool mbEnableInput;
	tbool mbEnableOutput;

	//! Modified version of init_asio_static_data from the ASIO SDK
	void InitASIOStaticData(tint32 iSampleRate, tint iBufferSize);

	//! Modified version of create_asio_buffers from the ASIO SDK
	virtual void CreateASIOBuffers();

	//! ASIO buffers
	ASIOBufferInfo mBuffers[ASIO_MAX_INPUT_CHANNELS + ASIO_MAX_OUTPUT_CHANNELS];

	//! Buffer size we're using
	tint miHWBufferSize;

	//! Current sample rate
	tfloat64 mfSampleRate;

	//! Flag that tells whether we're using the ASIOOutputReady() optimization (reported by driver)
	tbool mbPostOutput;

	//! Input channels of driver
	tint miChannelsIn;

	//! Output channels of driver
	tint miChannelsOut;

	//! Minimum buffer size of driver
	tint miBufferSizeMin;

	//! Maximum buffer size of driver
	tint miBufferSizeMax;

	//! Preferred buffer size of driver
	tint miBufferSizePreferred;

	//! Buffer size granularity of driver (see ASIO docs for explanation)
	tint miBufferSizeGranularity;

	//! ASIO structure with callback info
	ASIOCallbacks mASIOCallbacks;

	//! Static buffer switch callback
	static ASIOTime* BufferSwitchTimeInfoWrap(ASIOTime* pTimeInfo, long iIndex, ASIOBool bProcessNow);

	//! Non-static buffer switch callback
	ASIOTime* BufferSwitchTimeInfo(ASIOTime* pTimeInfo, long iIndex, ASIOBool bProcessNow);

	//! Static buffer switch callback
	static void BufferSwitchWrap(long iIndex, ASIOBool bProcessNow);

	//! Static call for samplerate change
	static void SampleRateChangedWrap(ASIOSampleRate Rate);

	//! Static call for ASIO messages
	static long ASIOMessagesWrap(long iSelector, long iValue, void* pMessage, double* pOpt);

	//! Non-static call for ASIO messages
	long ASIOMessages(long iSelector, long iValue, void* pMessage, double* pOpt);

	//! CPU time at beginning of last processing call
	LARGE_INTEGER mTimeLast;

	//! Frequency of performance counter. Used to calculate CPU time into "real" time
	LARGE_INTEGER mPerfFreq;

	float** mppfInputs;
	float** mppfOutputs;
};

