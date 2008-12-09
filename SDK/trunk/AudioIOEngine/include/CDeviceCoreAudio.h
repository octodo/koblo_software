/*!	\file CDeviceCoreAudio.h
	\author Michael Olsen
	\date 10/Feb/2006
	\date 10/Feb/2006
*/

#define COREAUDIO_MAX_INPUT_CHANNELS 32
#define COREAUDIO_MAX_OUTPUT_CHANNELS 64

/*! \class CDeviceCoreAudio
 * \brief CoreAudio implementation of IDevice
*/
class CDeviceCoreAudio : public virtual IDevice
{
public:
	//! Constructor
	CDeviceCoreAudio();

	//! Destructor
	virtual ~CDeviceCoreAudio();

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

	//! Called immediately after Create()
	virtual void SetAudioDeviceID(AudioDeviceID ID);

	//! Called immediately after Create()
	virtual void SetBehavior(tbool bEnableInput, tbool bEnableOutput);
	
	//! Called from driver callback
	void Process(tfloat32* pfOut, const tfloat32* pfIn, tint32 iBufferSize, tint32 iBuffersIn);

	//! (lasse) --this is before I changed things; didn't delete just yet... Called from driver callback - not
	void Process_Old(tfloat32* pfOut, const tfloat32* pfIn, tint32 iBufferSize, tint32 iBuffersIn);

	//! Called from driver callback
	void ProcessNonInterleaved(tfloat32* pfOut0, tfloat32* pfOut1, const tfloat32* pfIn0, const tfloat32* pfIn1, tint32 iBufferSize);

protected:
	tbool mbEnableInput;
	tbool mbEnableOutput;

	//! ID of the device we're using
	AudioDeviceID mAudioDeviceID;

	//! Callback pointer
	IDeviceCallback* mpCallback;

	//! Buffer size we're using
	tint miHWBufferSize;

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

	float** mppfInputs;
	float* mppfInputTmp[COREAUDIO_MAX_INPUT_CHANNELS];
	float** mppfOutputs;
};

