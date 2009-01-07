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



// (mo) Hmm, we ought to find a better solution here, instead of a global...
extern st::IStreamManager* gpStreamManager;

class CDSP : 
public virtual CBaseDSPEngine,
public virtual CRegion_Model
{
public:
	//! Constructor
	CDSP();

	//! Destructor
	virtual ~CDSP();

	//! IDestructable override
	virtual void Destroy();

	// CBaseDSPEngine override
	virtual void Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	// CBaseDSPEngine override
	virtual void Start();
	// CBaseDSPEngine override
	virtual void Stop();
	// CBaseDSPEngine override
	virtual void SetSampleRate(tuint32 uiSampleRate);
	// CBaseDSPEngine override
	virtual void SetMaxBufferSize(tint32 uiMaxBufferSize);

	tuint32 GetSampleRate() const {return muiSampleRate;}

	void SetChannels(tuint32 uiChannels);
	
	void SetInputsForChannel(tuint32 uiChannel, tint32 iInCount, tint32 iInFirst);
	void SetModeForChannel(tuint32 uiChannel, tint32 iMode);
	void SetPanningLeftRightForChannel(tuint32 uiChannel, tfloat32 fLeftRight);
	void SetPanningFrontBackForChannel(tuint32 uiChannel, tfloat32 fFrontBack);
	
	void ProcessMono(float** ppfOut, const float** ppfIn, long lC);
	void ProcessStereo(float** ppfOut, const float** ppfIn, long lC);

//	void SetTrackInputFile(tint32 iTrack, const std::string& sTargetPathName);

	void SetSongPosition(tuint64 uiPosNew);
	void SetSongPosition_AndResetEffectsTails(tuint64 uiPosNew);

	tuint32 GetFinalSoundPos();

	void UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel);
	void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBus);
//	void UpdateAUXData(tint32 iID, tint32 iValue, tint32 iAUX);
	void UpdateAUX1Data(tint32 iID, tint32 iValue);
	void UpdateAUX2Data(tint32 iID, tint32 iValue);
	void UpdateMasterData(tint32 iID, tint32 iValue);

	//! Struct for all meter values in system
	struct SMeters_All {
		// Arrays for current peaks
		tfloat32 pfMeters_Master[8];
		tfloat32* ppfMeters_Tracks[giNumber_Of_Tracks];
		tfloat32* ppfMeters_Busses[giNumber_Of_Busses];
		tfloat32* ppfMeters_AUXes[giNumber_Of_AUXes];

		//! Factor for decaying previous peaks (0.0 for ignore previous peaks)
		tfloat32 fDecayFactor;

		// constructor
		SMeters_All()
		{
			// Max 8 meters for all tracks/busses/AUXes (7.1 surround)
			for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
				ppfMeters_Tracks[iTrack] = new tfloat32[8];
			}
			for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
				ppfMeters_Busses[iBus] = new tfloat32[8];
			}
			for (tint32 iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
				ppfMeters_AUXes[iAUX] = new tfloat32[8];
			}

			// Default: Make sure we ignore all previous values
			fDecayFactor = 0.0;
		};

		// destructor
		virtual ~SMeters_All()
		{
			for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
				delete[] ppfMeters_Tracks[iTrack];
			}
			for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
				delete[] ppfMeters_Busses[iBus];
			}
			for (tint32 iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
				delete[] ppfMeters_AUXes[iAUX];
			}
		};

		void Zero()
		{
			// Reset meter arrays for all possible channels (max 7.1 surround)

			// Master
			memset(
				(tchar*)(pfMeters_Master),
				'\0',
				8 * sizeof(tfloat32)
				);
			// Tracks
			for (tint32 iTrack = 0; iTrack < giNumber_Of_Tracks; iTrack++) {
				memset(
					(tchar*)(ppfMeters_Tracks[iTrack]),
					'\0',
					8 * sizeof(tfloat32)
					);
			}
			// Busses
			for (tint32 iBus = 0; iBus < giNumber_Of_Busses; iBus++) {
				memset(
					(tchar*)(ppfMeters_Busses[iBus]),
					'\0',
					8 * sizeof(tfloat32)
					);
			}
			// AUXes
			for (tint32 iAUX = 0; iAUX < giNumber_Of_AUXes; iAUX++) {
				memset(
					(tchar*)(ppfMeters_AUXes[iAUX]),
					'\0',
					8 * sizeof(tfloat32)
					);
			}
		}
	}; // struct SMeters_All

	//! Gets every meter value for system. Last GUI should reset by calling with bClear = true
	void GetAllMeters_MaybeClear(SMeters_All* psMeters_All, tbool bClear);

	//! Clears the arrays for meters values without reading them (memcpy)
	void ClearAllMeters();

	//! Get number of channels for input of channel/bus
	virtual tint32 GetNumberOfInputChannels(tint32 iChannelOrBus);
	//! Get number of channels for panner for channel/bus
	virtual tint32 GetNumberOfChannelsForPanner(tint32 iChannelOrBus);
	
	virtual tint32 GetDestinationForChannelOrBus(tint32 iChOrBus);
	virtual void SetDestinationForChannelOrBus(tint32 iChOrBus, tint32 iDestination, tint32 iDestNumberOfChannels);
	virtual tbool UpdateDestinationForChannelOrBus_ButOnlyIfItMatches(tint32 iChOrBus, tint32 iDestination, tint32 iDestNumberOfChannels);


	
	//! cut a region in two
	void CutRegion(tuint32 uiTrack, tuint32 uiRegionID, tuint64 uiCutPos);
	
	//! Trim region
	void TrimRegion(tuint32 uiTrack, tuint32 uiRegionID, tbool bStart, tint64 uiSamplePos);
	
	//! Duplicate a region
	void DuplicateRegion();
	
	//! Normalise a region
	void NormaliseRegion();
	
	//! Inverse a region
	void InverseRegion();
	
	//! Fade In
	tuint64 Fade_In(tuint32 uiRegionID, tuint64 uiFadeInLength);
	
	//! Fade Out
	tuint64 Fade_Out(tuint32 uiRegionID, tuint64 uiFadeOutLength);
	
	//! Region Volume
//	void Region_Volume(tuint32 uiRegionID, tfloat32 fRegion_Volume);
	
	//! Loop Selectin
	void LoopSelection();

	
	//! Move a region
	void MoveRegion(tuint32 uiID, tint32 iChannel, tuint64 uiTrackPosStart, tuint64 uiSamplePosStart, tuint64 uiSamplePosEnd);
	
	//! Copy a region
//	void CopyRegion(tuint32 uiID, tint32 iChannel, tuint64 uiTrackPosStart, tuint64 uiSamplePosStart, tuint64 uiSamplePosEnd);
	//-------------------------------------

	//! Refresh the region GUI
	void Refresh_Region_GUI(tint32 iRegion);
	void Refresh_Region_GUI(tint32 iRegion, tuint32 iTrack);
	
	// Delete all selected regions
//	void DeleteSelectedRegions(tuint32 uiTrack);
//	//! Delete one region
	void DeleteRegion(tuint32 uiTrack, tuint32 uiRegionID);
	//! Delete one region
	void DeleteRegionOnGUI(tuint32 uiTrack, tuint32 uiRegionID);
	
	
	//! Get the region size in samplepoints
	tuint64 GetRegionSize(const std::string& sSoundPathName, tuint64 uiSamplePosStart, tint64 uiSamplePosEnd);
	//! Get the region size in samplepoints
	tuint64 GetRegionSize(tuint32 uiID);
	//! Get Drawing info for a region
	SRegion_Drawing_Info Get_Region_Drawing_Info(tuint uiRegion);
	//! Get Loop Drawing info 
	SLoop_Drawing_Info Get_Loop_Drawing_Info(){ return mLoop_Drawing_Info;};



	// iSize == 0: small (1 / 1024 sample). iSize == 1: large (1 / 64 sample)
	void GetRegionPeakFile(tuint32 uiID, IFile** ppFile, tint32 iChannel, tint32 iSize);

	//! Delete selection
	void Delete_Selection();
	
	void DeleteTrack(tuint32 uiTrack);

	void DeleteAllRegionsForTrack(tint32 iTrack);

	void MoveRegion(tuint32 uiID, tint32 iChannelNew, tuint64 uiTrackPosStartNew);

	tbool SaveTrackRegionDataToChunk(tint32 iTrack, IChunk* pChunk);
	tbool CreateRegionFromChunkData(tint32 iTrack, IChunk* pChunk);
	
	CTrack_DSP* GetTrack(tuint32 uiIndex) {return mppTracks[uiIndex];}

	const CTrack_DSP* GetTrack(tuint32 uiIndex) const {return mppTracks[uiIndex];}

	CTrack_DSP* GetBus(tuint32 uiIndex) {return mppBusses[uiIndex];}

	const CTrack_DSP* GetBus(tuint32 uiIndex) const {return mppBusses[uiIndex];}

	const CTrack_DSP* GetMaster() const {return mpMaster;}

//	CTrack_DSP* GetAUX(tuint32 uiIndex) {return mppAUXes[uiIndex];}

//	const CTrack_DSP* GetAUX(tuint32 uiIndex) const {return mppAUXes[uiIndex];}

	typedef void* RegionSearchHandle;

	const RegionSearchHandle mRegionSearchEnd;
	
	// Get First region look on all tracks
	RegionSearchHandle GetFirstRegion(SRegionInfo& rInfo);
	// Get next region on all tracks
	void GetNextRegion(RegionSearchHandle& Handle, SRegionInfo& rInfo);

	void FinishRegionSearch(RegionSearchHandle Handle);

	const tchar* GetClipNameOfSelectedRegion();

	// Lasse, added 2008-05-15
	//! Assign audio / midi callback
	/*!
		\param pAudioCallback [in]: Pointer to global audio handler call back
	*/
	void SetAudioCallback_Duplex(aioe::IDeviceCallback* pAudioCallback)
	{ mpAudioCallback_Duplex = pAudioCallback; };

	void SetAudioCallback_Input(aioe::IDeviceCallback* pAudioCallback)
	{ mpAudioCallback_Input = pAudioCallback; };

	void SetAudioCallback_Output(aioe::IDeviceCallback* pAudioCallback)
	{ mpAudioCallback_Output = pAudioCallback; };

	//! Tries to open an audio device - preferences are saved on success
	/*!
		\param iMgrIx [in]: Which audio manager to use (two exist for Windows only p.t. - for Mac it's always 0)
		\param iDevIx [in]: Which device to open, -1 for default (-1 is only valid for Windows 16 bit driver p.t.)
		\param pszDevName [in]: Name of the device to open. If this doesn't match the given index (due to resent plug-in or removal of USB or FireWire interface) some semi-intelligent guessing will be performed
		\param iFreqHz [in]: The wanted sample-rate
		\param iBuffSize [in]: The wanted frequency
		\param bInvokeStart [in]: True => the Start() method is called for opened audio device
		\return tbool: True on succes, False if device didn't open
	*/
	tbool OpenAudioDevice_Duplex(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, tint32 iFreqHz, tint32 iBuffSize, tbool bInvokeStart);

	tbool OpenAudioDevice_Output(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, tint32 iFreqHz, tint32 iBuffSize, tbool bInvokeStart);

	tbool OpenAudioDevice_Input(tint32 iMgrIx, tint32 iDevIx, tchar* pszDevName, /*tint32 iFreqHz,*/ tint32 iBuffSize, tbool bInvokeStart);

	//! Stops and closes audio device(s) (if it's running / opened)
	void CloseAudioDevices();

	void CloseAudioDevice_Duplex();
	void CloseAudioDevice_Input();
	void CloseAudioDevice_Output();

	//! True if input == output device and it is capable of duplex
	tbool IsAudioDeviceDuplex();
	tbool IsAudioDeviceDuplex(tint32 iInMgrIx, tint32 iInDevIx, tint32 iOutMgrIx, tint32 iOutDevIx);

	//! Is audio device instantiated?
	tbool IsAudioDeviceAllocated()
	{ return ((mpAudio_Duplex != NULL) || (mpAudio_Output != NULL)); };

	//! Reads names and indexes of all audio devices and fill into a menu struct
	/*!
		\param list [out]: Is filled with names and values pointing to all possible devices for system 
		\return tint32: Number of valid devices (size of list)
	*/
	tint32 GetAudioDeviceMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue);

	tint32 GetAudioDeviceMenuItemsInput(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue);

	//! Reads the possible buffer sizes for currently selected audio device and fill into a menu struct
	/*!
		\param list [out]: Is filled with names and values of all possible buffer sizes for currently selected audio device 
		\return tint32: Number of valid devices (size of list)
	*/
	tint32 GetAudioBuffsMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue);
	
	tint32 GetAudioBuffsMenuItemsInput(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue);

	//! Reads the possible sample rates for currently selected audio device and fill into a menu struct
	/*!
		\param list [out]: Is filled with names and values of all possible sample rates for currently selected audio device (the values are /100 to not violate the 32k param max)
		\return tint32: Number of valid devices (size of list)
	*/
	tint32 GetAudioFreqsMenuItems(ge::IPopupMenu::SMenuItemList& rlist, tint32& riSelectedValue);

	//! Tries to open an audio device - preferences are saved on success
	/*!
		\param iListValue [in]: The value (not device index!) from the menu item list created by GetAudioDeviceMenuItems(...)
		\return tbool: True on success, False if device didn't open
	*/
	tbool OpenAudioDevice_FromMenu_Output(tint32 iListValue, tint32 iFreqHz, tint32 iBuffSize);

	tbool OpenAudioDevice_FromMenu_Input(tint32 iListValue, /*tint32 iFreqHz,*/ tint32 iBuffSize);

	//! Tries to read the audio-device preferences file. Then tries to open the device - preferences are saved back on success
	/*!
		\param rbAudioDevice_LastOpenCrashed [in]: True => Audio Device not opened because of a previous application crash while opening it
		\param rbMidiDevice_LastOpenCrashed [in]: True => Midi Device not opened because of a previous application crash while opening it
		\return tbool: True if an audio device was successfully opened. False on error, which means user must confirm the settings from the system-setup dialog
	*/
	tbool OpenAudioDevice_FromPrefs_Output(tbool* pbAudioDevice_LastOpenCrashed, tbool* pbMidiDevice_LastOpenCrashed);

	tbool OpenAudioDevice_FromPrefs_Input(tbool* pbAudioDevice_LastOpenCrashed);

	//! Sets a new frequency for opened audio device(s)
	/*!
		\param iFreqHz [in]: The new frequency to set
		\return tbool: True on success, False if device doesn't support this rate
	*/
	tbool SetAudioFrequency(tint32 iFreqHz);

	//! Call method Start() for opened audio device(s)
	void StartAudioDevices();

	void StartAudioDevice_Duplex();	
	void StartAudioDevice_Input();
	void StartAudioDevice_Output();

//	void SetSelected_Region(tint32 iRegion){ miSelected_Region = iRegion;};
	//! Obsolete
//	void SetSelection(tint32 iSelection, tint32 iRegion, tint64 iStart, tint64 iDuration);
	//! Set Selection on a track
	/*!
		\param iTrack [in]: track to apply selection to
		\param iStart_Sample [in]: Start Sample of selection
		\param iDuration [in]: Duration in samples
		\param iCmd [in]: Command to apply to track defined in KSGlobals.h ESelectionType
	*/
	void SetTrackSelection(tint32 iTrack, tint64 iStart_Sample, tint64 iDuration, tint32 iCmd, tint32 iRegionID = -1);
	//! Get Selection on a track
	STrackSelectionInfo GetTrackSelection(tint32 iTrack){ return mpTrackSelectionInfo[iTrack];};
	//! Select region
	STrackSelectionInfo SelectRegion(tint32 iRegionID);
	//! remove  selection on all tracks
	virtual void Deselect_All_Tracks();
	//! remove  selection on one track
	virtual void Deselect_Track(tuint32 uiTrack);
	
	virtual void Loop_On(tbool bLoop);
	
	virtual void Set_Loop_Start(tuint64 uiStart);
	
	virtual void Set_Loop_End(tuint64 uiEnd);
	
	virtual void Set_Loop_To_Selection();
	
	tint64 Get_Session_End_Sample(){ return muiSession_End_Sample;};
	
	void Reset_Session_End_Sample(){ muiSession_End_Sample = 0;};

	void Sanitize_Session_End_Sample();

	//! Runs through regions on a track and finds very first and very last sample (not including effect tail)
	/*!
		\param iTrack [in]: Which track to run through
		\param piFirstSample [out]: Receives first sample position (or 0 if no regions)
		\param piFirstSample [out]: Receives last sample position (or -1 if no regions)
		\return tbool: True if track is valid (even if it has no regions), False if track is invalid (not visible)<br/><b>Note!</b> Always returns true just now...
	*/
	tbool CalcTrackDuration(tint32 iTrack, tint64* piFirstSample, tint64* piFinalSample);

	void LimitDataFromStreams(tbool bEnableLimiting, tint64 iFirstSampleIx, tint64 iFinalSampleIx);
	
//	SSelectionInfo Get
	
//	void TrimSelection();

	//! Audio preferences - mainly about output
	SKSPrefs_1 mPrefs_1;
	//! Audio preferences - mainly about input
	CKSPrefs_2 mPrefs_2;

	tuint64 GetLoopEnd(){return muiLoopEnd;};

	tbool IsTrackMuted(tint32 iTrack)
	{ return mbMute_Track_Flag[iTrack]; };
	tint32 GetNrOfInputChannels();

	void GetInput(CBuffer* mpBuffer, tint32 iChannels, tint32 iInputChannel, tint32 iNrOfSamples);

//	void Start();

//	void Stop();

	tint32 GetStutterCounter();

	void ResetStutterCounter(){ miStutterCounter = 0; };
	
	//! finds a region info for a region
	/*!
	 \param RegionInfo [out]: region info to write in
	 \param uiID [in]: unique region id
	*/
	void GetRegionInfo(SRegionInfo& RegionInfo,tuint32 uiID);


protected:

//	IRegionCallback* mpRegionCallback;

	tfloat** mppfInputCur;

	tuint32 muiChannels;

	st::IStreamManager* mpStreamManager;

	tuint muiFinalSongPosition;

	CTrack_DSP** mppTracks;

	CTrack_DSP** mppBusses;

	CTrack_DSP* mpMaster;

	CTrack_DSP** mppAUXes;

//	CBuffer* mpBufferMix;

	//! Max 8 meters for all tracks/busses/AUXes (7.1 surround) (note: mix meter array part of struct unused because CDSP handles already)
	SMeters_All* mpsMeters_All;
	void SetMeters_Master(tfloat32* pfAbsValues);
	void SetMeters_Track(tint32 iTrack, tfloat32* pfAbsValues);
	void SetMeters_Bus(tint32 iBus, tfloat32* pfAbsValues);
	void SetMeters_AUX(tint32 iAUX, tfloat32* pfAbsValues);


	//! Stored volume for Mute recover
	tint32 miStored_Track_Volume[giNumber_Of_Tracks];
	tint32 miStored_Buss_Volume[ giNumber_Of_Busses];
	tint32 miStored_AUX_Volume[ giNumber_Of_AUXes];

	//! Mute flag
	tbool mbMute_Track_Flag[giNumber_Of_Tracks];
	
	//! Mute flag
	tbool mbMute_Buss_Flag[giNumber_Of_Busses];

	tbool mbMute_AUX_Flag[giNumber_Of_AUXes];

	//! Find number of channels for a destination (bus or master mix)
	virtual tint32 FindDestinationNumberOfChannels(tint32 iDestination);

	struct SRegionSearchInfo {
		tuint32 uiTrackCur;

		std::list<CTrack_DSP::SChannelRegionInfo*>::const_iterator itCur;
	};
	
//	void GetRegionInfo(SRegionInfo& RegionInfo,tuint32 uiID);

	// Lasse, 2008-0515
#ifdef WIN32
	#define NUMBER_OF_AUDIO_MANAGERS 2
#endif // WIN32
#ifdef _Mac
	#define NUMBER_OF_AUDIO_MANAGERS 1
#endif // _Mac
	//! For listing / selecting audio devices
	aioe::IDeviceManager* mapAudioManager[NUMBER_OF_AUDIO_MANAGERS];
	aioe::IDevice* mpAudio_Duplex;
	aioe::IDevice* mpAudio_Input;
	aioe::IDevice* mpAudio_Output;
	aioe::IDeviceCallback* mpAudioCallback_Duplex;
	aioe::IDeviceCallback* mpAudioCallback_Input;
	aioe::IDeviceCallback* mpAudioCallback_Output;

	virtual void KillAudioManagers();

	void LoadPrefs();
	void SavePrefs();
	
	STrackSelectionInfo mpTrackSelectionInfo[giNumber_Of_Tracks];
	tint32 miSelectedTrack;
	
	void Update_Loop();
	
	tuint64 muiSongPos;
	tuint64 muiLoopStart;
	tuint64 muiLoopEnd;
	tuint64 muiStored_LoopEnd;
	tbool mbLoop_On;
	
	SLoop_Drawing_Info mLoop_Drawing_Info;
	
	tuint64 muiSession_End_Sample;
	virtual void Set_Session_Size(tuint64 uiRegionEndSample);

	// .. Lasse

	tint32 miStutterCounter;

}; // class CDSP



