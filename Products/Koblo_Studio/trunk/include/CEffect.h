// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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

class CEffect;

class CParmEffect : public virtual de::IParameter
{
public:
	CParmEffect(CEffect* pEffect, tint32 iID, tint32 iMinValue, tint32 iMaxValue, tint32 iValue);

	virtual ~CParmEffect();

	virtual void Destroy();

	// IParameter override
	virtual tint32 GetID() const;
	// IParameter override
	virtual tint32 GetDefaultValue() const;
	// IParameter override
	virtual tint32 GetValue() const;
	// IParameter override
	virtual void SetValue(tint32 iValueNew, tbool bCallOnChange = true);
	// IParameter override
	virtual tint32 GetMinValue() const;
	// IParameter override
	virtual tint32 GetMaxValue() const;
	// IParameter override
	virtual void OnChange();

protected:
	CEffect* mpEffect;
	tint32 miID;
	tint32 miDefaultValue;
	tint32 miValue;
	tint32 miMinValue;
	tint32 miMaxValue;
};

//! Number of parameters in an effect
const tuint guiEffectParameters = 5;


/*! \brief Base class for synth effect
*
*	This base class handles some of the details of a kspi effect
*/
class CEffect : public virtual kspi::IPlugIn, public virtual de::IParameterManagerCallback
{
public:
	//! Constructor
	CEffect();

	//! Destructor
	virtual ~CEffect();

	//! IPlugIn override
	virtual kspi::IGUI* KSPI_CALL CreateGUI(tint32 iIndex);
	//! IPlugIn override
	virtual void KSPI_CALL PreInitialize();
	//! IPlugIn override
	virtual void KSPI_CALL Initialize();
	virtual void KSPI_CALL Initialize(ge::IPane* pPane);
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
//	virtual tuint32 KSPI_CALL GetInputChannelCount();
	//! IPlugIn override
	virtual tuint32 KSPI_CALL GetOutputChannelCount();
	//! IPlugIn override
	virtual void KSPI_CALL SetChannelCount(tuint32 iChannelsIn, tuint32 iChannelsOut, tuint32 iSideChainChannels) {}
	//! IPlugIn override
	virtual void KSPI_CALL SetMaxBufferSize(tuint32 iBufferSize);
	//! IPlugIn override
	virtual tint32 KSPI_CALL SupportSampleRate(tuint32 iSampleRate);
	//! IPlugIn override
	virtual void KSPI_CALL SetSampleRate(tuint32 iSampleRate);
	//! IPlugIn override
	virtual void KSPI_CALL InputMIDI(const kspi::SMIDIEvent* pEvents, tuint32 iNrOfEvents);
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
	virtual tint32 KSPI_CALL GetProductID() {return 0;}

	//! Returns the set max buffer size
	/*!
		\return: The max. buffer size
	*/
	tint32 GetMaxBufferSize() const {return miMaxBufferSize;}

	//! Returns the set sample rate
	/*!
		\return: The sample rate
	*/
	tuint32 GetSampleRate() const {return muiSampleRate;}

	//! Set Host BPM
	virtual void SetBPM( tfloat mfBPM);

	//! Set the pane
//	virtual void SetPaneAndGUI(ge::IPane* pPane, CBaseGUI* pGUI);
	virtual void SetPane(CBasePane* pPane);

	virtual void EventValueChange(ge::IControl* pSender, tint32 iValueNew);

	virtual void UpdateData(tint32 iID, tint32 iValue) = 0;

	ge::IWindow* GetWindow() {return mpPane->GetWindow();}

protected:
	//! The max buffer size, or 0 if not set yet
	tint32 miMaxBufferSize;

	//! The sample rate, or 0 if not set yet
	tuint32 muiSampleRate;

	//! Array of current parameter values
	tint32 mpiParmValue[guiEffectParameters];

	tfloat mfBPM;

//	ge::IPane* mpPane;

//	CBaseGUI* mpGUI;
	CBasePane* mpPane;

	de::IParameterManager* mpParmMan;

	inline void AddEffectParm(tint iSection, tint32 iParmID, tint32 iParmMin, tint32 iParmMax, tint32 iParmDefault)
	{
		CParmEffect* pParmEffect = new CParmEffect(dynamic_cast<CEffect*>(this), iParmID, iParmMin, iParmMax, iParmDefault);
		mpParmMan->Add(dynamic_cast<de::IParameter*>(pParmEffect),
			de::IParameterManager::TypeGlobal, iSection);
	}

	static CAutoDelete<IUniqueID> gpControlIDs;

	//! Used to store controls IDs used by effect, so they can be released at destruction time
	std::list<tuint32> mControlIDs;

	//! Gets an unique id, and stores it in list of IDs used
	tuint32 GetUniqueID();

	void RegisterGlobalControl(tint iSection, tint32 iParameterID, ge::IControl* pControl);
	de::IDisplayConverter* RegisterGlobalControlWithDisplayTranslater(tint iSection, tint32 iParameterID, ge::IControl* pControl, tint iDecimals = 0, tint iMaxNrOfStringChars = 5, tint iMaxNrOfStringDecimals = 0, std::string sPostFix = std::string(""));
	void RegisterGlobalControlWithLinearTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl);
	void RegisterGlobalControlWithLogTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl, tint iStrength);
	void RegisterGlobalControlWithTrueLogTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl);
	void RegisterGlobalControlWithEnumTranslation(tint iSection, tint32 iParameterID, ge::IControl* pControl);
	void RegisterGlobalControlWithX3Translation(tint iSection, tint32 iControlID, tint32 iParameterID, ge::IControl* pControl, tuint uiParmDecimals);
};






