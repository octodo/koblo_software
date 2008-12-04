/*!	\file IPlugIn.h
	\author Michael Olsen
	\date 14/Jan/2005
	\date 24/Jan/2005
*/

class IGUI;

/*! \brief Interface to describe 1 plug-in
*
*	Processing, parameters and MIDI is handled through the interface.
*/
class /*KSPI_EXPORT*/ IPlugIn
{
public:
	//! Destroys plug-in. Use this instead of delete, since it will make sure deallocation is done the right place
	virtual void KSPI_CALL Destroy() = 0;

	//! Create and attach a GUI interface. Initialize must have been called at least once first.
	virtual IGUI* KSPI_CALL CreateGUI(tint32 iIndex) = 0;

	//! Earliest initialization. Buffer size and sample rate has not yet been set. This is a good place to setup hardware communication. There's no matching call for this, i.e. cleaning up is done in the destructor
	virtual void KSPI_CALL PreInitialize() = 0;

	//! Startup initialization. Buffer size and sample rate has been set. This is a good place to initialize parameter structurs.
	virtual void KSPI_CALL Initialize() = 0;
	//! Must match with Initialize()
	virtual void KSPI_CALL DeInitialize() = 0;

	//! This is a good place to reset internal state (history). Must match with Stop()
	virtual void KSPI_CALL Start() = 0;
	//! Must match with Start()
	virtual void KSPI_CALL Stop() = 0;

	//! If plug-in is a signal generator (no input) returns 1, otherwise 0
	virtual tint32 KSPI_CALL IsSignalGenerator() = 0;
	//! If plug-in can process in-place (input and output buffers are the same) returns 1, otherwise 0. Note that it should always be able to process non-in-place (input and output buffers are different)
	virtual tint32 KSPI_CALL CanProcessInPlace() = 0;
	//! If plug-in wants incoming MIDI returns 1, otherwise 0
	virtual tint32 KSPI_CALL WantsMIDI() = 0;
	//! If plug-in is a synthesizer returns 1, otherwise 0. Do not confuse this with IsSignalGenerator(), which is stricly a question about buffers
	virtual tint32 KSPI_CALL IsSynth() = 0;
	//! Returns the plug-ins latency, measured in samples. Cam be 0 for no latency. Is always called after the samplerate has been set.
	virtual tint32 KSPI_CALL GetLatency() = 0;

	//! Returns the supported channel configurations
	virtual void KSPI_CALL GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels) = 0;

	//! Sets the configuration to one of the configurations reported as supported in GetChannelConfigurations().
	virtual void KSPI_CALL SetChannelConfiguration(tuint32 uiChannelsIn, tuint32 uiChannelsOut, tuint32 uiSideChainChannels) = 0;

	//! Returns the max. number of channel groups supported (1..n). A number >1 means that the plug-in has AUX outputs, that is, outputs not considered part of the main output. All output groups has the same channel configuration as the main group (SetChannelConfiguration()).
	virtual tuint32 KSPI_CALL GetChannelGroups() = 0;

	//! Sets the number of groups to a number reported as supported in GetChannelGroups().
	virtual void KSPI_CALL SetChannelGroups(tuint32 uiGroups) = 0;

	//! Returns number of input channels
//	virtual tuint32 KSPI_CALL GetInputChannelCount() = 0;
	//! Returns number of output channels. Some plug-ins may support a variable output channel count in certain hosts
//	virtual tuint32 KSPI_CALL GetOutputChannelCount() = 0;

	//! Called immediately before initialize with the current channel count. Side chain buffers are delivered as input (after the regular input buffers)
//	virtual void KSPI_CALL SetChannelCount(tuint32 iChannelsIn, tuint32 iChannelsOut, tuint32 iSideChainChannels) = 0;

	//! Set the maximum size of buffer used for processing.
	virtual void KSPI_CALL SetMaxBufferSize(tuint32 iBufferSize) = 0;

	//! Asks plug-in if it supports a given sample rate
	virtual tint32 KSPI_CALL SupportSampleRate(tuint32 iSampleRate) = 0;
	//! Set the sample frequency
	virtual void KSPI_CALL SetSampleRate(tuint32 iSampleRate) = 0;

	//! Sends MIDI. May be called numerous times between processing calls.
	virtual void KSPI_CALL InputMIDI(const SMIDIEvent* pEvents, tuint32 iNrOfEvents) = 0;

	//! Processing in-place, which means that the input and output buffer(s) is the same
	virtual void KSPI_CALL ProcessInPlace(tfloat32** ppfSamples, tuint32 iNrOfSamples) = 0;
	//! Processing non-in-place, which means that the input and output buffers are not the same
	virtual void KSPI_CALL ProcessNonInPlace(tfloat32** ppfSamplesOut, const tfloat32** ppfSamplesIn, tuint32 iNrOfSamples) = 0;

	//! Returns the number of automateable parameters
	virtual tuint32 KSPI_CALL GetNrOfParameters() = 0;
	//! Returns info about 1 automateable parameter
	virtual void KSPI_CALL GetParameterInfo(tuint32 iIndex, SParameterInfo* pInfo) = 0;
	//! Returns an automateable parameters current value
	virtual tint32 KSPI_CALL GetParameter(tuint32 iIndex) = 0;
	//! Set an automateable parameters value
	virtual void KSPI_CALL SetParameter(tuint32 iIndex, tint32 iValue) = 0;

	//! Returns size of parameter chunk (parameter block) or 0 is plug-in has no parameter chunk. An immediately following GetParameterSet call will pass a buffer of minimum this size.
	virtual tuint32 KSPI_CALL GetChunkSize() = 0;
	//! Gets the current chunk data. The buffer passed should be minimum the size returned by GetChunkSize()
	virtual void KSPI_CALL GetChunk(void* p) = 0;
	//! Sets chunk data
	virtual void KSPI_CALL SetChunk(const void* p, tuint32 iChunkSize) = 0;

	//! Use the timer call to process "idle" events (including GUI)
	virtual void KSPI_CALL Timer() = 0;

	//! Returns the product ID for the plug-in. Together with the IModule::GetCompanyID() value this is a unique identifier for the plug-in
	virtual tint32 KSPI_CALL GetProductID() = 0;

	//! Returns the product name. pszName is preallocated by the host with at least 257 characters (256 for the name, 1 for the trailing zero). The name can be delivered in different "versions", each seperated by a '@', starting with the longest and ending with the shortest. Example: "My 4-band eq@My4eq@Myeq". Note that the product name should not include the company name
	virtual void KSPI_CALL GetProductName(tchar* pszName) = 0;
};



