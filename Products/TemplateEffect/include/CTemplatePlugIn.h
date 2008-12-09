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


class CTemplateGUI;

// The max buffer size of the audio processing. Larger buffers will be split into smaller ones
// This should be set to the buffer size over which performance will start to decrease.
// Note: The dezipper is using this buffer size, that is, it is only called each giAudioMaxBufferSize samples.
const tint32 giAudioMaxBufferSize = 32;

class CTemplatePlugIn : public virtual CBasePlugIn, public virtual IBaseDezipperCallback
{
public:
	CTemplatePlugIn(CTemplateModule* pModule, tuint32 uiProcessIndex);

	virtual ~CTemplatePlugIn();

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
		CParmGlobal* pParmGlobal = new CParmGlobal(dynamic_cast<CTemplatePlugIn*>(this), iParmID, iParmMin, iParmMax, iParmDefault);
		mpParmMan->Add(dynamic_cast<de::IParameter*>(pParmGlobal),
			de::IParameterManager::TypeGlobal, iSection);
	}
};

