/*!	\file CDeviceWaveIO.h
	\author Lasse Steen Bohnstedt
	\date 2008-05-02
*/

// Windows Wave devices only support mono and stereo...
#define WAVE_IO_MAX_INPUT_CHANNELS 2
#define WAVE_IO_MAX_OUTPUT_CHANNELS 2

/*! \class CDeviceWaveIO
 * \brief WaveIO implementation of IDevice
*/
class CDeviceWaveIO : public virtual IDevice
{
public:
	//! Constructor
	CDeviceWaveIO();

	//! Destructor
	virtual ~CDeviceWaveIO();

	//! IDestructable override
	virtual void Destroy();

	//! IDevice override
	virtual tbool Initialize(tint32 iSampleRate, tint iHWBufferSamples);
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

	//! Call immediately after Create to load driver
	/*!
		\param iIndex [in]: Index of driver to load
	*/
	virtual tbool LoadDriver(tint iIndex, tbool bEnableInput, tbool bEnableOutput);

	//! Halt output immediately, release buffers and close driver
	virtual void AbortCloseAndRelease();

	//! Call back member (for output)
	virtual void WaveOutProc_Callback(HWAVEOUT hwo, UINT uMsg, DWORD dwParam1);

	//! Call back member (for input)
	virtual void WaveInProc_Callback(HWAVEIN hwi, UINT uMsg, DWORD dwParam1);

	// Test frequency capabilities of a device
	static tbool CDeviceWaveIO::TestWaveOutCaps(tuint32* puiDevID, tint32 iHz);



protected:
	//! Callback pointer
	IDeviceCallback* mpCallback;

	//! Buffer size we're using
	tint miHWBufferSize;

	//! Samples in buffer
	tint miSamplesInPCM;

	//! Current sample rate
	tfloat64 mfSampleRate;

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


	tfloat32** mppfInputs;
	//float* mppfInputTmp[WAVE_IO_MAX_INPUT_CHANNELS];
	tfloat32** mppfOutputs;

#define MIN_WAVE_PCM_SAMPLES 32
#define PRF_WAVE_PCM_SAMPLES 128
#define MAX_WAVE_PCM_SAMPLES 256
#define MIN_RUNNING_WAVE_STRUCTS 8
#define PRF_RUNNING_WAVE_STRUCTS 32
#define MAX_RUNNING_WAVE_STRUCTS 32
	tint32 miRunningStructs;

	tchar* mapcInputPCM[MAX_RUNNING_WAVE_STRUCTS];
	tchar* mapcOutputPCM[MAX_RUNNING_WAVE_STRUCTS];
	tint32 miPCMLen;

	tuint32 muiDevIndex;
	HWAVEOUT mhandle_out;
	HWAVEIN mhandle_in;
	tbool mbDriverPseudoLoaded, mbStarted;
	volatile tbool mbBlocked_volatile;
	volatile tbool mbInCallback_volatile;
	WAVEHDR mawavehdrOutput[MAX_RUNNING_WAVE_STRUCTS];
	WAVEHDR mawavehdrInput[MAX_RUNNING_WAVE_STRUCTS];

	//! Take portion of a stereo stream and convert to wave-out struct data (used by WaveOutProc_Callback)
	static void XlatPPFloat2PCM(tfloat32** ppf, WAVEHDR* pwavehdr, tint32 iSamples);

	//! Take wave-in struct and convert it to a portion of stereo stream (used by WaveInProc_Callback)
	static void XlatPCM2PPFloat(WAVEHDR* pwavehdr, tfloat32** ppf, tint32* piSamples);

	//! Setup wave out format header for frequency
	static void InitFormat(WAVEFORMATEX& format, tint32 iHz);

	//! Starts up wave-header struct for output
	virtual MMRESULT StartBuffers();
	//! Stops the wave-header struct for output
	virtual MMRESULT StopBuffers();

	tint32 GetMinBufferSize_InBytes()
		{ return MIN_RUNNING_WAVE_STRUCTS * MIN_WAVE_PCM_SAMPLES * sizeof(tfloat32); };
	tint32 GetMaxBufferSize_InBytes() 
		{ return MAX_RUNNING_WAVE_STRUCTS * MAX_WAVE_PCM_SAMPLES * sizeof(tfloat32); };
	tint32 GetPrfBufferSize_InBytes()
		{ return PRF_RUNNING_WAVE_STRUCTS * PRF_WAVE_PCM_SAMPLES * sizeof(tfloat32); };


	virtual tbool LoadDriverInput(tint iIndex);
	tbool mbInput;
	tbool mbOutput;

	tbool mbUseAsInput;
	tbool mbUseAsOutput;
};

