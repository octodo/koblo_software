
class CDigitalEchoGUI;

// The max buffer size of the audio processing. Larger buffers will be split into smaller ones
// This should be set to the buffer size over which performance will start to decrease.
// Note: The dezipper is using this buffer size, that is, it is only called each giAudioMaxBufferSize samples.
const tint32 giAudioMaxBufferSize = 32;

class CDigitalEchoPlugIn : public virtual CBasePlugIn, public virtual IBaseDezipperCallback
{
public:
	CDigitalEchoPlugIn(CDigitalEchoModule* pModule, tuint32 uiProcessIndex);

	virtual ~CDigitalEchoPlugIn();

	//! IPlugIn override
	virtual void KSPI_CALL Destroy();
	//! IPlugIn override
	virtual kspi::IGUI* KSPI_CALL CreateGUI(tint32 iIndex);
	//! IPlugIn override
//	virtual tuint32 KSPI_CALL GetInputChannelCount();
	//! IPlugIn override
	virtual void KSPI_CALL Initialize();
	//! IPlugIn override
	virtual void KSPI_CALL GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels);

	//! CBasePlugIn override
	tint32 KSPI_CALL GetProductID();
	//! CBasePlugIn override
	void KSPI_CALL GetProductName(tchar* pszName);

	//! IBaseDezipper override
	virtual void SetValue(tint32 iParmID, tint32 iValue);

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

protected:
	//! If true bypass is enabled
	tbool mbBypass;

	//! Flag used to determine if we should update GUI settings
	tbool mbUpdateGUISettings;

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
		CParmGlobal* pParmGlobal = new CParmGlobal(dynamic_cast<CDigitalEchoPlugIn*>(this), iParmID, iParmMin, iParmMax, iParmDefault);
		mpParmMan->Add(dynamic_cast<de::IParameter*>(pParmGlobal),
			de::IParameterManager::TypeGlobal, iSection);
	}
};

