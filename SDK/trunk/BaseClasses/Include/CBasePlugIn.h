// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


// Forward declarations
class CBaseGUI;


/*! \class CBasePlugIn
 * \brief Implementation of kspi::IPlugIn
*/
class CBasePlugIn : public virtual kspi::IPlugIn, public virtual de::IParameterManagerCallback
{
public:
	//! Constructor
	CBasePlugIn(CBaseModule* pModule, tuint32 iMaxSubBufferSize, CBaseDSPEngine* pDSPEngine, tuint32 uiProcessIndex, std::string sCompanyName, std::string sProductName);

	//! Destructor
	~CBasePlugIn();

	//! IPlugIn override
	virtual void KSPI_CALL PreInitialize();
	//! IPlugIn override
	virtual void KSPI_CALL Initialize();
	//! IPlugIn override
	virtual void KSPI_CALL DeInitialize();
	//! IPlugIn override
	virtual void KSPI_CALL Start();
	//! IPlugIn override
	virtual void KSPI_CALL Stop();
	//! IPlugIn override
	virtual tint32 KSPI_CALL IsSignalGenerator();
	//! IPlugIn override
	virtual tint32 KSPI_CALL CanProcessInPlace();
	//! IPlugIn override
	virtual tint32 KSPI_CALL WantsMIDI();
	//! IPlugIn override
	virtual tint32 KSPI_CALL IsSynth();
	//! IPlugIn override
	virtual tint32 KSPI_CALL GetLatency();
	//! IPlugIn override
	virtual void KSPI_CALL SetMaxBufferSize(tuint32 iBufferSize);
	//! IPlugIn override
	virtual tint32 KSPI_CALL SupportSampleRate(tuint32 iSampleRate);
	//! IPlugIn override
	virtual void KSPI_CALL SetSampleRate(tuint32 iSampleRate);
	//! IPlugIn override
	virtual void KSPI_CALL InputMIDI(const kspi::SMIDIEvent* pEvents, tuint32 iNrOfEvents);
	//! IPlugIn override
	virtual void KSPI_CALL ProcessInPlace(tfloat** ppfSamples, tuint32 iNrOfSamples);
	//! IPlugIn override
	virtual void KSPI_CALL ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	//! IPlugIn override
	virtual tuint32 KSPI_CALL GetNrOfParameters();
	//! IPlugIn override
	virtual void KSPI_CALL GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo);
	//! IPlugIn override
	virtual tint32 KSPI_CALL GetParameter(tuint32 iIndex);
	//! IPlugIn override
	virtual void KSPI_CALL SetParameter(tuint32 iIndex, tint32 iValue);
	//! IPlugIn override
	virtual tuint32 KSPI_CALL GetChunkSize();
	//! IPlugIn override
	virtual void KSPI_CALL GetChunk(void* p);
	//! IPlugIn override
	virtual void KSPI_CALL SetChunk(const void* p, tuint32 iChunkSize);
	//! IPlugIn override
	virtual void KSPI_CALL Timer();
	//! IPlugIn override
	virtual void KSPI_CALL SetChannelConfiguration(tuint32 uiChannelsIn, tuint32 uiChannelsOut, tuint32 uiSideChainChannels);
	//! IPlugIn override
	virtual tuint32 KSPI_CALL GetChannelGroups();
	//! IPlugIn override
	virtual void KSPI_CALL SetChannelGroups(tuint32 uiGroups);
	//! IPlugIn override
//	virtual void KSPI_CALL SetChannelCount(tuint32 iChannelsIn, tuint32 iChannelsOut, tuint32 iSideChainChannels);
	//! IPlugIn override
	virtual tint32 KSPI_CALL GetProductID();
	//! IPlugIn override
	virtual void KSPI_CALL GetProductName(tchar* pszName);

	//! IParameterManagerCallback override
	virtual void GUINotify(tint32 iPaneID, tint32 iControlID, tint32 iValue);
	//! IParameterManagerCallback override
	virtual void AutomationNotify(de::IParameterManager::EParameterType Type, tint32 iExtra, tint32 iParameterID, tint32 iValue);

	//! Returns the module
	virtual CBaseModule* GetModule() {return mpModule;}

	//! Returns the GUI, or NULL if none
//	CBaseGUI* GetGUI() {return mpGUI;}
	const std::list<CBaseGUI*>& GetGUIs() {return mGUIs;}

	//! Called by GUI immediately before it destroys itself
//	virtual void GUIDestroyed();
	virtual void GUIDestroyed(CBaseGUI* pGUI);

	//! Returns true if its ok to update DSP engine
	virtual tbool UpdateEngineSettings() const {return mbUpdateEngineSettings;}

	//! Returns global parameter
	virtual tint32 GetGlobalParm(tint32 iIndex, tint32 iSection);

	//! Sets global parameter
	virtual void SetGlobalParm(tint32 iIndex, tint32 iValue, tint iSection = 0);

	//! Win32: Returns the folder which the application is inside
	/*!
		\return string: Folder which holds the application
	*/
	virtual std::string GetApplicationFolder();

	//! Returns the folder one step "up" from the application folder (GetApplicationFolder())
	/*!
		\return string: Folder which is one step "up" compared to the plug-in folder
	*/
	virtual std::string GetCompanyFolder();

	//! Returns the folder which holds folders like presets folder. Is a subfolder of the application folder. (GetApplicationFolder())
	/*!
		\return string: Folder to place preset folder.
	*/
	virtual std::string GetFilesFolder();

	//! Returns the preset folder. Is a subfolder of the files folder (GetFilesFolder())
	/*!
		\return string: Folder to place presets in.
	*/
	virtual std::string GetPresetFolder();

	//! Returns the folder which holds misc resources and authorizer application. Is a subfolder of the application folder. (GetApplicationFolder()).
	/*!
		\return string: 
	*/
	virtual std::string GetResourceFolder();

	//! Returns the folder to put user preferences in (on a per-user basis).
	/*!
		\return string: Folder to place user preferences in.
	*/
	virtual std::string GetPreferenceFolder();

	//! Returns the folder to put company preferences in (on a per-user basis). Is one step "up" from preference folder (GetPreferencesFolder())
	/*!
		\return string: Folder to put company preferences in
	*/
	virtual std::string GetCompanyPreferenceFolder();

	//! Returns the DSP engine
	CBaseDSPEngine* GetDSPEngine() {return mpDSPEngine;}

	//! Overload
	const CBaseDSPEngine* GetDSPEngine() const {return mpDSPEngine;}

	//! Gets signal meter (using mutex)
	virtual tfloat GetMeterSignal(tint iIndex);

	//! Gets GR meter (using mutex)
	virtual tfloat GetMeterGR(tint iIndex);

	//! Gets and clears signal meter (using mutex)
	virtual tfloat GetMeterSignalAndClear(tint iIndex);

	//! Gets and clears GR meter (using mutex)
	virtual tfloat GetMeterGRAndClear(tint iIndex);

	//! Returns the number of input channels, or -1 if unknown
	/*
		\return tint32: Number of input channels, or -1 if unknown
	*/
	virtual tint32 GetChannelsIn() const {return miChannelsIn;}

	//! Returns the number of output channels, or -1 if unknown
	/*
		\return tint32: Number of output channels, or -1 if unknown
	*/
	virtual tint32 GetChannelsOut() const {return miChannelsOut;}

	//! Returns the number of side-chain input channels, or -1 if unknown
	/*
		\return tint32: Number of side-chain input channels, or -1 if unknown
	*/
	virtual tint32 GetChannelsSideChainIn() const {return miChannelsSideChainIn;}

	//! Returns the current samplerate
	/*!
		\return tuint32: Current sample frequency
	*/
	virtual tuint32 GetSampleRate() const {return miSampleRate;}

	//! Returns the process index
	virtual tuint32 GetProcessIndex() const {return muiProcessIndex;}

	//! Resets all parameters to default values
	virtual void Reset();

	//! Set MIDI Controller map
	virtual void MIDICCMapLearn(tint32 iParamId, tint32 iMinVal, tint32 iMaxVal, tint32 iStrength, kspi::SParameterInfo::ECurveType CurveType);

	//! UnLearn MIDI Controller 
	virtual void MIDICCMapUnlearn( tint32 iParamId);

	//! Loads a MIDI CC map.
	/*!
		\param sPathName [in]: Pathname to MIDI map
	*/
	virtual void LoadMIDICCMap(const std::string& sPathName);

	//! Saves a MIDI CC map.
	/*!
		\param sPathName [in]: Pathname to MIDI map
	*/
	virtual void SaveMIDICCMap(const std::string& sPathName);

	void MIDICCReset();

	//! Returns the company name
	const std::string GetCompanyName() const {return msCompanyName;}

	//! Returns the product name
	const std::string GetProductName() const {return msProductName;}

	//! Called to open a preset. Can be overriden for non-standard presets
	/*!
		\param pFile [in]: File to save to
		\param iProductID [in]: ID of product, or -1 if product ID shouldn't be checked in file
	*/
	virtual void OnLoadPreset(IChunkFile* pFile, tint32 iProductID = -1);

	//! Called to save a preset. Can be overriden for non-standard presets
	/*!
		\param pFile [in]: File to save to
		\param iProductID [in]: ID of product, or -1 if product ID shouldn't be saved to file
	*/
	virtual void OnSavePreset(IChunkFile* pFile, tint32 iProductID = -1);

	//! Sets the MIDI channel to listen to. If this is not called default is listening to all.
	/*!
		\param iChannel [in]: MIDI channel to listen to, or -1 for all.
	*/
	virtual void SetMIDIChannel(tint32 iChannel);

	//! Copy background preset set to foreground
	void CopyBackgroundPresetToForeground();

	//! Copy foreground preset set to background
	void CopyForegroundPresetToBackground();

	//! Called when the A or B button has been pressed
	/*!
		\param iIndex [in]: If 0 the A button has been pressed, if 1 the B button has been pressed
	*/
	void ActionAB(tint iIndex);

	//! Called when the A->B button has been pressed
	void ActionAToB();

	//! Called when the B->A button has been pressed
	void ActionBToA();

	//! Call this once after parametersr has been setup to initialize a/b preset sets. Calling this also means that the B set will be saved into the "host chunk"
	void SetupAB();

	//! Returns the current preset set (0 for A, 1 for B)
	tint32 GetPresetSetCurrent() const {return miPresetSetCurrent;}
	
	virtual void OnGUITimer();

protected:
	//! The module we reside in
	CBaseModule* mpModule;

	//! The GUI
//	CBaseGUI* mpGUI;
	std::list<CBaseGUI*> mGUIs;

	//! Parameter manager
	de::IParameterManager* mpParmMan;

	//! Name of company
	std::string msCompanyName;

	//! Name of product
	std::string msProductName;

	//! Dezipper
	CBaseDezipper* mpDezipper;

	//! Current stream time in samples (from last Start()). -1 if we're stopped (Stop()) or havn't started yet (Start()).
	tint64 miStreamTime;

	//! Mutex used to lock audio/midi/parameter access
	CMutex mMutex;

	//! Current max buffer size
	tuint32 miMaxBufferSize;

	//! Max subbuffer size
	const tuint32 miMaxSubBufferSize;

	//! Current sample rate
	tuint32 miSampleRate;

	//! Number of input channels (or -1 if unknown)
	tint32 miChannelsIn;

	//! Number of output channels (or -1 if unknown)
	tint32 miChannelsOut;

	//! Number of in side-chain channels (or -1 if unknown)
	tint32 miChannelsSideChainIn;

	//! Used to store input for subbuffer processing
	tfloat* mppfIn[32];

	//! Used to store output for subbuffer processing
	tfloat* mppfOut[2];

	//! The DSP engine
	CBaseDSPEngine* mpDSPEngine;

	//! Flag used to determine if we should update engine settings
	tbool mbUpdateEngineSettings;

	// Used to hold information about 1 automateable parameter
	struct SAutomationInfo {
		// Standard kspi info
		kspi::SParameterInfo Info;

		// Used to locate the parameter in the data manager
		de::IParameterManager::EParameterType Type;

		// This is the parameter index used to locate the parameter in the data manager
		tint32 iIndex;

		// Used to locate the parameter in the data manager. Only used if layer or part parameter (For global it's giSectionPerf)
		tint32 iExtra;
	};

	// Array of automateable parameters
	std::vector<SAutomationInfo> mAutomationInfo;

	// Map of global parameters with automation
	std::multimap<tint32, tint32> mAutomationMapGlobal;

	// Map of layer parameters with automation
	std::multimap<tint32, tint32> mAutomationMapLayer;

	// Map of part parameters with automation
	std::multimap<tint32, tint32> mAutomationMapPart;

	//! The actual processing method.
	virtual void DoProcess(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);

	//! Called to initialize parameters. Inheriting classes must implement this.
	virtual void AddParameters() = 0;

	//! Called to create automation map (to locate automateable parameters). Inheriting classes must implement this.
	virtual void MakeAutomationMap() = 0;

	//! Creates a "chunk" (in the plug-in sense of the word). The caller must destroy the object returned
	/*!
		return IChunkFileMemory*: Create chunk file. The caller must destroy after use
	*/
	virtual IChunkFileMemory* CreateChunkFile();

	//! Current preset set being used (0 == A, 1 == B)
	tint miPresetSetCurrent;

	//! Background preset A (i.e. current set not used)
	IChunkFileMemory* mpPresetBackground;

	//! Index of process (i.e. type)
	tuint32 muiProcessIndex;

#ifdef WIN32
#pragma pack(push, 1)
#endif
	//! Struct used to hold information about one MIDI automateable parameter
	struct SMIDICCInfo {
		//! Parameter ID used to locate the parameter in the data manager
		tint32 iParamID;

		//! Curve type
		kspi::SParameterInfo::ECurveType CurveType;

		//! Minimum value used by converter
		tint32 iMinVal;

		//! Maximum value used by converter
		tint32 iMaxVal;

		//! Curve strength. Only used for log curves
		tint32 iStrength;

	} PACKED;
#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32

	//! Holds information about parameter while learn is active
	SMIDICCInfo mMIDICCParmInfo;

	//! Listen for MIDI controller flag
	tbool mbListenForCC;

	//! Midi automization map
	SMIDICCInfo mpMIDICC[128];

	//! MIDI channel we're listening to, or -1 for all. Default is -1.
	tint32 miMIDIChannel;

	// Handle MIDI CC
	virtual void MIDICC(const kspi::SMIDIEvent* pEvent);

	//! Inserts a linear parameter into the automation map
	virtual void InsertParameterLinear(tint32 iParmID, const std::string& sName, tint32 iDecimals, const std::string sUnit = "");

	//! Inserts an enum parameter into the automation map
	virtual void InsertParameterEnum(tint32 iParmID, const std::string& sName, const std::string& sEnumName, const std::string sUnit = "");

	//! Inserts a "truelog" parameter into the automation map
	virtual void InsertParameterTrueLog(tint32 iParmID, const std::string& sName, tint32 iDecimals, const std::string sUnit = "");

	//! Inserts a "log" parameter into the automation map
	virtual void InsertParameterLog(tint32 iParmID, tint32* piStrength, const std::string& sName, tint32 iDecimals, const std::string sUnit = "");
};
