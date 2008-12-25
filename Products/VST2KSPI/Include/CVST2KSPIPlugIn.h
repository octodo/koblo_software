
class CVST2KSPIGUI;

// The max buffer size of the audio processing. Larger buffers will be split into smaller ones
// This should be set to the buffer size over which performance will start to decrease.
// Note: The dezipper is using this buffer size, that is, it is only called each giAudioMaxBufferSize samples.
const tint32 giAudioMaxBufferSize = 64;

class AEffect;

class CVST2KSPIPlugIn : public virtual CBasePlugIn, public virtual IBaseDezipperCallback
{
public:
	CVST2KSPIPlugIn(CVST2KSPIModule* pModule, const std::string& sPathName);

	virtual ~CVST2KSPIPlugIn();

	//! IPlugIn override
	virtual void KSPI_CALL Destroy();
	//! IPlugIn override
	virtual kspi::IGUI* KSPI_CALL CreateGUI(tint32 iIndex);
	//! IPlugIn override
//	virtual tuint32 KSPI_CALL GetInputChannelCount();
	//! IPlugIn override
//	virtual tuint32 KSPI_CALL GetOutputChannelCount();
	//! IPlugIn override
	virtual void KSPI_CALL GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels);
	//! IPlugIn override
	virtual void KSPI_CALL Initialize();

	//! IBaseDezipper override
	virtual void SetValue(tint32 iParmID, tint32 iValue);

	//! CBasePlugIn override
	virtual tint32 KSPI_CALL GetProductID();
	//! CBasePlugIn override
	virtual void KSPI_CALL GetProductName(tchar* pszName);
	//! CBasePlugIn override
	virtual void KSPI_CALL SetMaxBufferSize(tuint32 iBufferSize);
	//! CBasePlugIn override
	virtual tint32 KSPI_CALL SupportSampleRate(tuint32 iSampleRate);
	//! CBasePlugIn override
	virtual void KSPI_CALL SetSampleRate(tuint32 iSampleRate);

	// Called when a GUI parameter has changed, and needs updating
	/*!
		\param iID [in]: ID of parameter
		\param iValue [in]: Value of parameter
	*/
	virtual void GUISettingsChanged(tint32 iID, tint32 iValue);

	// Call this to determine if we should update GUI when GUISettingsChanged is called
	void SetUpdateGUISettings(tbool bNew) {mbUpdateGUISettings = bNew;}

	// Returns mutex used to control audio/midi/parameter access
	/*!
		return mMutex: Mutex
	*/
	CMutex& GetMutex() {return mMutex;}

	// Called to update the engine
	/*!
		\param iID [in]: Parameter ID
		\param iValue [in]: Parameter value
	*/
	void UpdateEngineData(tint32 iID, tint32 iValue);

	// Calls UpdateAll on the GUI if it exists
	virtual void UpdateAllGUI();

	//! Returns true if registered, or false if not registered (or not checked yet). Check is made in Initialize().
	/*!
		\return tbool: Returns true if registered, or false if not registered (or not checked yet).
	*/
	tbool IsRegistered() const {return mbRegistered;}

	//! CBasePlugIn override
	virtual void KSPI_CALL ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);

	tbool ShowDemoExpiredDialog() const {return mbShowDemoExpiredDialog;}

	void SetDemoExpiredDialog(tbool bNew) {mbShowDemoExpiredDialog = bNew;}

	AEffect* GetEffect() {return mpVSTEffect;}

protected:
	std::string msEffectPathName;

	AEffect* mpVSTEffect;

	//! If true bypass is enabled
	tbool mbBypass;

	//! If true the plug-in is registered, if false non-registered (or check not made yet). Check is made in Initialize.
	tbool mbRegistered;

	//! If false, processing is disabled (copy protection)
	tbool mbCanProcess;

	//! Flag used to determine if we should update GUI settings
	tbool mbUpdateGUISettings;

	//! Time when plugin was started (used for demo time)
	tuint32 muiTimeWhenStarted;

	//! If true, the demo expired dialog should be shown on next timer
	tbool mbShowDemoExpiredDialog;

	// 0: no noise, 1: noise
	tint miNoiseMode;
	tint miNoiseCounter;
	// Counter max for no noise
	tint miNoiseCounterMax0;
	// Counter max for noise
	tint miNoiseCounterMax1;
	tint32 miRandom;

	//! CBasePlugIn override
	virtual void AddParameters();
	//! CBasePlugIn override
	virtual void MakeAutomationMap();

	// Adds global parameter
	/*!
		\param iParmID [in]: ID of parameter to add
		\param iParmMin [in]: Minimum value of parameter to add
		\param iParmMax [in]: Maximum value of parameter to add
		\param iParmDefault [in]: Default value of parameter to add
	*/
	inline void AddGlobalParm(tint iSection, tint32 iParmID, tint32 iParmMin, tint32 iParmMax, tint32 iParmDefault)
	{
		CParmGlobal* pParmGlobal = new CParmGlobal(dynamic_cast<CVST2KSPIPlugIn*>(this), iParmID, iParmMin, iParmMax, iParmDefault);
		mpParmMan->Add(dynamic_cast<de::IParameter*>(pParmGlobal),
			de::IParameterManager::TypeGlobal, iSection);
	}
};

