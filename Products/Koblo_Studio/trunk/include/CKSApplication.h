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


// Forward definitions
class CKSGUI;
class CEffect;
class CDSP;

// The max buffer size of the audio processing. Larger buffers will be split into smaller ones
// This should be set to the buffer size over which performance will start to decrease.
// Note: The dezipper is using this buffer size, that is, it is only called each giAudioMaxBufferSize samples.
const tint32 giAudioMaxBufferSize = 32;

extern CDSP* gpDSPEngine;
extern CKSApplication* gpApplication;

class CMIDIEvent
{
public:
	tuchar cStatus;
	tuchar cData0;
	tuchar cData1;
};

class CMIDITrack
{
public:
	std::list<CMIDIEvent> mEvents;
};

class CMIDISong
{
public:
	std::list<CMIDITrack> mTracks;
};

class CPreviewCallback
{
public:
	virtual void Accumulate(tfloat32** ppfBuffer, tint32 iSamples) = 0;
};


class CExportClipTask; // Forward declaration
class CImportAudioTask; // Forward declaration
class CProgressTask; // Forward declaration
class CProgressTaskList; // Forward declaration


class CKSApplication : 
public virtual CBasePlugIn, 
public virtual IBaseDezipperCallback, 
public virtual ITimerCallback_CanStop, 
public virtual CKSInternet_Features,
public virtual CKSXML_Create_Sample,
public virtual CGUI_Controller,
public virtual CKSFile_Controller,
public virtual CImport_Auido_Controller

{

public:
	CKSApplication(CKSModule* pModule, tuint32 uiProcessIndex);

	virtual ~CKSApplication();

	//! IPlugIn override
	virtual void KSPI_CALL Destroy();
	//! IPlugIn override
	virtual kspi::IGUI* KSPI_CALL CreateGUI(tint32 iIndex);
	//! IPlugIn override
	virtual void KSPI_CALL GetChannelConfigurations(tuint32* puiNrOfConfigurations, tuint32* puiChannelsIn, tuint32* puiChannelsOut, tuint32* puiSideChainChannels);
	//! IPlugIn override
//	virtual tuint32 KSPI_CALL GetInputChannelCount();
	//! IPlugIn override
//	virtual tuint32 KSPI_CALL GetOutputChannelCount();
	//! IPlugIn override
	virtual void KSPI_CALL Initialize();

	//! IBaseDezipper override
	virtual void SetValue(tint32 iParmID, tint32 iValue);

	// Call this to determine if we should update GUI when GUISettingsChanged is called
	void SetUpdateGUISettings(tbool bNew) {mbUpdateGUISettings = bNew;}

	void KSPI_CALL ProcessNonInPlace(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);

	void ProcessNonInPlace_InputOnly(const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	void ProcessNonInPlace_OutputOnly(tfloat** ppfSamplesOut, tuint32 iNrOfSamples);

	void ProcessNonInPlace_NoLock(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);

	void AudioInput_IntermediateBuffer_Push(const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	void AudioInput_IntermediateBuffer_Pop(tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	void AudioInput_IntermediateBuffer_Kill();
	tint64 AudioInput_IntermediateBuffer_CalcExtraLatency();

	std::list<tfloat**> mlistAudioInput_IntermediateBuffer;
	std::list<tint32> mlistAudioInput_IntermediateBuffer_Samples;
	#define kiAudioInput_IntermediateBuffer_DefaultFullChainCount 3
	tint32 miAudioInput_IntermediateBuffer_FullChainCount;
	tbool mbAudioInput_IntermediateBuffer_EverFull;
	tint32 miAudioInput_IntermediateBuffer_FirstLinkIx;
	tint32 miAudioInput_IntermediateBuffer_FirstLinkSamples;

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
	void UpdateGUIData(tint32 iID, tint32 iValue);
	void UpdateCommandData(tint32 iID, tint32 iValue);
	void UpdateTrackData(tint32 iID, tint32 iValue, tint32 iTrack = 0);
	void UpdateBussData(tint32 iID, tint32 iValue, tint32 iBuss);
	void UpdateMasterData(tint32 iID, tint32 iValue);
	void UpdateAUX1Data(tint32 iID, tint32 iValue);
	void UpdateAUX2Data(tint32 iID, tint32 iValue);

	void DoProcess(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);

	// Calls UpdateAll on the GUI if it exists
	virtual void UpdateAllGUI();
	
	virtual void GetDefaultProjectFolder(tchar* pszPathName);
	virtual void GetTempFolder(tchar* pszPathName);
	virtual void DeleteFileThatOccupiesFolderName(std::string sFolderName);
	virtual tbool VerifyFolderCreated(std::string sFolderName);

	virtual void OnLoad(const std::string& sPathName);

	void LoadSaveErrDlg(std::string sMsg);
	static void ShowMessageBox(const tchar* pszMessage, const tchar* pszHeader);
	virtual void ShowMessageBox_NonModal(const tchar* pszMessage, const tchar* pszHeader);

	virtual void OnMenuEvent(const tchar* pszString);
	
	virtual void Export(ac::EAudioCodec eCodec, tint32 iQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);
	
	virtual void Export_For_Web(ac::EQuality eQuality, tint32 iLicenseNb, std::string sUserName, std::string sUserUrl, std::string sUserStatus);
	tchar* mapszLicenseCodes[6];
	tchar* mapszLicenseTexts[6];
	tchar* mapszLicenseUrls[6];
	tint32 miPreviousLicenseNb;
	struct SExportForWeb {
		ac::EQuality eQuality;
		tint32 iLicenseNb;
		std::string sUserName;
		std::string sUserUrl;
		std::string sUserStatus;
	} msExportForWeb;


	virtual tbool MenuFileCreateNewProject();
	virtual tbool MenuFileSaveProjectAs(const tchar* pszDefaultName = "", tbool bCompress = false);
	virtual tbool MenuFileLoadProject();
	
	
	
	virtual tbool Open_Project();
	
	
	
	
	virtual tbool MenuFileLoadProject_QueueClips(IChunkFile* pFile, std::list<CImportAudioTask*>* plistpImportTasks);
	virtual tbool MenuFileSaveProject(tbool bOverwriteIcons = false);
	virtual void MenuFileDistributeMix(ac::EAudioCodec eCodec, tint32 iQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);
	virtual void MenuSetupAudio();

	

	virtual void VerifyCreatePeakFiles(const tchar* pszWavePathL, const tchar* pszWavePathR, tbool bForceRewrite);

	virtual tbool GenerateMix(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);

	//! Get's extra error info for last method that failed 
	virtual std::string GetExtendedError();

	virtual void CleanProject(tint32 iCreateEmptyTracks);

	virtual tbool OnAudioFileImport();

//	virtual tbool DoAudioFileImport(const std::string& sPathName, tbool bDoCopy = true, tbool bAlwaysStereo = false);
	virtual tbool QueueAudioFileImport(const tchar* pszPathName, tbool bAlwaysKeepStereo, tint32 iTrackID =-1, tint64 iTrackPos = -1);

	virtual tbool CanWaveFilePlay(const std::string& sWaveFilePath, tbool bAllowErrorDialog, tbool bIsMissingFileAnError);

	//! Song playback states
	enum EPlaybackState
	{
		geStateStopped,
		geStatePlaying,
		geStateRecording,

		geStateInProgressTask,
		geStateExportingOutMix,
		geStateExportingTrack,
	};

	EPlaybackState GetPlaybackState() const {return mePlaybackState;}
	tint32 GetTrackToExport() const	{ return miTrackToExportNow; };

	tbool IsInProgressTaskState()
	{ return (mePlaybackState >= geStateInProgressTask); };

	tbool IsPlayingOrRecording()
	{ return ((mePlaybackState == geStatePlaying) || (mePlaybackState == geStateRecording)); };

	void PlaybackStart();
	void PlaybackStop();
	void PlaybackStop_NoLock();
	void PlaybackFF();
	void PlaybackRewind();
	void PlaybackGoToStart(tbool bIgnorePlayState = false);
	void PlaybackGoToEnd();
	void PlaybackGoToPos(tint64 iSample);
	
	void PrepareMovePlayhead();
	void MovePlayhead(tuint64 uiSamplePos);
	void EndMovePlayhead();

	void Playback_ExportTrack(tint32 iTrack, tint64 iFirstSample, tint64 iFinalSample);
	void Playback_ExportOutMix(tint64 iPosToStart);
	void Playback_InProgressTask(tint64 iNewPos = -1);


	// ITimerCallback implementation
	virtual void OnTimer(tint32 iID);
	virtual tbool OnTimer_CanStop(tint32 iID);
	ITimer* mpTimer_ProgressTasks;
	CProgressTaskList* mpProgressTasks;
	virtual tbool DoProgressTasks();
	std::string msProgress;
	tuint64 muiProgressIx;
	tuint64 muiProgressAll;

	tbool StartProgressTasks(EPlaybackState eState = geStateInProgressTask, tint64 iPos = -1, tint32 iTrack = -1);
	tbool StartProgressTasks_NoLock(EPlaybackState eState = geStateInProgressTask, tint64 iPos = -1, tint32 iTrack = -1);
	tbool StopProgressTasks();
	tbool StopProgressTasks_NoLock();
	void AbortProgressTasks(tbool bNoDialogBoxOnAbort = false);
	void AbortProgressTasks_NoLock(tbool bNoDialogBoxOnAbort = false);
	CMutex mMutex_Progress;

	void Zoom(tint32 iCmd, tint32 iVal);


	tuint64 GetSongPos() const {return miSongPos;}

	void IncSongPos(tuint32 uiOffset) {miSongPos += uiOffset;}

	const std::string& GetChannelName(tint32 iChannel) {return mpsChannelName[iChannel];}

	void SetChannelName(tint32 iChannel, const std::string& sName);

//	kspi::IPlugIn* GetEffect(tint32 iChannel, tint32 iInsert);

	virtual tbool OnKeyDown(ge::EKey Key);

	CPlugInManager* GetPlugInManager() {return mpPlugInManager;}
	
	tint32  AddTrack();
	virtual void DeleteTrack();
	virtual void SelectTrack(tint32 iID);
//	virtual const tint32 GetSelectedTrack() { return miSelected_Track; };
	

	virtual void Set_Track_Visible(tuint iID, tbool bVisible);
	virtual void Set_Track_To_Default(tuint iID); 
	
	virtual void GUI_Created(){mbGUI_Created	= true;};
	virtual void All_Plugins_Created(tbool bDone){mbAll_Plugins_Created	= bDone;};
	virtual void SetGUIsReady(tbool bGUIsReady) { mbAreGUIsReady = bGUIsReady; };
	virtual const tbool AreGUIsReady() { return mbAreGUIsReady; };
	
	virtual tint32 Get_Selected_Track(){return miSelected_Track;};
	virtual void Set_Selected_Track(tint32 iID){ miSelected_Track = iID;};


	std::list<CSample_Data*> Get_Sample_Data_List(){return mSample_Data_List;}
	std::list<CSample_Data*>::const_iterator Get_Sample_Data_List() const {return mSample_Data_List.begin();}
	std::list<CSample_Data*>::const_iterator Get_Sample_Data_List_End() const {return mSample_Data_List.end();}
	
	
	
	
	
	
	
	
	
	
	
	//--------------------------------
	// Obsolete stuff has to remain until old projects are converted
	
	tbool IsClipNameInUse(const tchar* pszClipName, const tchar* pszWaveNameL, const tchar* pszWaveNameR, std::string* psDescription);
	void AddClipToList(CImportAudioTask* pImportInfo);
	

	std::string GetProjDir() const {return msProjectFolder;}
	std::string GetProjDir_Contents() const {return GetProjDir() + "Contents:";}
	std::string GetProjDir_Icons() const {return GetProjDir_Contents() + "Resources:";}

	std::string GetProjDir_Audio() const;
	std::string GetProjDir_Clips() const;
	std::string GetProjDir_ClipsDecomp() const;

	std::string GetFromWaveName_ClipWave(const tchar* pszWaveName) const;
	std::string GetFromWaveName_ClipWaveOld(const tchar* pszWaveName) const;
	std::string GetFromWaveName_ClipDefaultOgg(const tchar* pszWaveName) const;
	std::string GetFromWaveName_ClipWaveDecomp(const tchar* pszWaveName) const;
	std::string GetFromWaveName_ClipWave_Safe(const tchar* pszWaveName);
	std::string GetFromWaveName_ClipComp_Safe(const tchar* pszWaveName) const;
	tint32 GetFromListName_ClipWavePathNames(const tchar* pszListName, std::string& rsWavePathNameL, std::string& rsWavePathNameR, tbool* pbIsDecompressed = NULL) const;
	
	
	

	

	
	virtual void Stack_Tracks();
	virtual void Update_Zoom();
	virtual void DeleteAllRegionsForTrack(tint32 iTrack);
	
	virtual void Set_All_Meters(void* pvoidMeters_All);

	tint32 Get_Number_Of_Tracks();
	
	tint32 Find_Track_Pos(tint32 iTrack);


	//! Get every meter value for system. Last GUI should reset by calling with bClear = true
	void GetAllMeters_MaybeClear(void* pvoid_sMeters_All, tuint32 muiTimeMS_Elapsed, tbool bClear);

	//! Clears the arrays for meters values without reading them (saves a memcpy)
	void ClearAllMeters();

	CMutex& GetMeterMutex() {return mMutexMeter;}

	void WindowClosed(void* pWnd);

	void CloseWindow(void* pWnd);

	tfloat64 GetPixelPrSample(){ return mfPixelPrSample;};
	tfloat64 GetSamplesPrPixel(){ return mfSamplesPrPixel;};

	de::IParameterManager* GetParmMan() {return mpParmMan;}
	
	tuint64 SnapToGridStart(tuint64 uiSamplePos);
	
	tuint64 SnapToGrid(tuint64 uiSamplePos);
	
	tuint64 SnapToGridEnd(tuint64 uiSamplePos);
	
	void SetGridLinesInSamples(tfloat64 fGrid_In_Samples){ mfGrid_In_Samples = fGrid_In_Samples;};
	tfloat64 GetGridLinesInSamples(){ return mfGrid_In_Samples;};
	
	tfloat64 GetSamples_Pr_32();
	
	//! Show or hide a progress-indicator at the top bar
	/*!
		\param bOn [in]: True = show, false = hide
		\param fProgress [in]: How much of progress indicator to display; 0.0 = nothing, 1.0 = everything
		\param pszText [in]: Explaning text to display above progress indicator. Leave NULL for default text
	*/
	virtual void Set_Progress(tbool bOn, tfloat32 fProgress, const tchar* pszText = NULL);

	virtual void ShowProgress(tuint64 uiIx, tuint64 uiMax, const tchar* pszText, ge::IContext* pContext, tbool* pbAbort);
	
//	virtual void SetMousePos(tuint32 uiMousePos){ muiMousePos = uiMousePos;};
//	tuint32 GetMousePos(){ return muiMousePos;};

	void SetPreviewCallback(CPreviewCallback* pCallback) {mpPreviewCallback = pCallback;}

	tint32 GetNrOfInputChannels();

	void UpdateAudioGUI();

	void SetRecord(tbool bNew) {mbRecord = bNew;}

	tbool IsRecording() const {return mbRecord;}
	
	tuint32 Get_Track_Id(tuint32 uiTrack){ return msStack.iTrack[uiTrack];}
	
	virtual void Send_Msg_To_All_Panes(CBasePane::SMsg* pMsg);
	
	void Start_Timer(){ mbTimer = true;};
	void Stop_Timer(){ mbTimer = false;};
	tbool Timer_Is_Running() { return mbTimer;};
	
	virtual void Zoom();
	
	tbool Project_Is_Imported(){return mbKSProject_Imported;};
	
	
	
	

protected:
	tbool mbRecord;

	CPreviewCallback* mpPreviewCallback;

	//! If true bypass is enabled
	tbool mbBypass;
	
	// Selected track
	tint32 miSelected_Track;
	
	// GUI is created
	tbool mbGUI_Created;
	
	// GUI is created
	tbool mbAll_Plugins_Created;
	
	//! All GUI's are ready. Is set true after OnInit(..) has been run for all GUI's
	tbool mbAreGUIsReady;
	
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
		CParmGlobal* pParmGlobal = new CParmGlobal( iParmID, iParmMin, iParmMax, iParmDefault, iSection);
		mpParmMan->Add(dynamic_cast<de::IParameter*>(pParmGlobal),
			de::IParameterManager::TypeGlobal, iSection);
	}
	
	CMIDISong mSong;
	

	
	

	
	
	
	
	
	//! Extended info about last error that happened in a failing method
	std::string msExtendedError;

	//! Song position in samples
	tuint64 miSongPos;

	//! Current playback state
	volatile EPlaybackState mePlaybackState;
	tint32 miTrackToExportNow;
	
	std::string mpsChannelName[giNumber_Of_Tracks];

	std::string mpsStreamNames[giNumber_Of_Tracks]; //!!! Max asks is this enough?

//	void DoWaveFileLoad(tint32 iTrack, const std::string& sPathName);

	CPlugInManager* mpPlugInManager;

	tbool ExportProjectForWeb_Compress(
		std::string sWavePathL, std::string sWavePathR,
		CSample_Data* pSample_Info, ac::EQuality ePreviousQuality);

	std::list<CSample_Data*> mSample_Data_List;

	virtual void UpdateGUIFileList();
public:
	void SanitizeProjectFolderPath(std::string& rsProjectFolder, const tchar* pszCorrectExt);
protected:	
	tint32 piTrack_Setup[giNumber_Of_Tracks];
	
	
	virtual tint32 Get_Free_Track();

	tbool ExportAllTracks(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);
	tbool ExportSelectedTrack(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);

	void ExportSelection(tbool bIncludeEffects, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);
	void ExportAllClips(ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels);
	tbool ExportRegionClip(const tchar* pszClipName, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels);
	tbool ExportTracksSelection_Raw(std::list<tint32> listiTrack, tint64 iStartIx, tint64 iDuration, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels);
	tbool ExportTracksSelection_Raw_AddOne(tint32 iTrack, tint64 iStartIx, tint64 iDuration, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, std::list<CExportClipTask*>* plistpInfo);

	tbool ExportClipsList(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels);
	tbool ExportClipsList_VerifyOne(CExportClipTask* pInfo);
	tbool ExportClipsList_SelectDestination(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, tchar* pszDestinationFolder);
	tbool ExportClipsList_WarnQuality(
		std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels,
		tbool* pbGoAhead);
	tbool ExportClipsList_AndAction(std::list<CExportClipTask*>* plistpInfo, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels);

	tbool ExportTracksOrMix(EPlaybackState eExportState, std::list<tint32>* plistiTracks, tbool bSelectionOnly, ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS, tbool bNormalize);
	
	
	// Struct to pass on to the GUI
	STrack_Stack msStack;
	
	
	
	virtual void Maintain_Number_Of_Tracks(tint32 iNr_Tracks);

	CMutex mMutexMeter;
	
	tfloat64 mfPixelPrSample;
	tfloat64 mfSamplesPrPixel;
	
	tfloat mfGrid_In_Samples;
	tfloat32 mfTempo;
	

	ITimer* mpTimer_NonModalDialog;
	struct SNonModalDialogInfo {
		std::string sHeader;
		std::string sLongerText;
	};
	std::list<SNonModalDialogInfo*> mlistpNonModalDialogInfo;
	void NonModalDialog_Push(const tchar* pszHeader, const tchar* pszLongerText);
	tbool NonModalDialog_Pop(SNonModalDialogInfo** ppInfo);
	void NonModalDialogs_ZapList();
	CMutex mMutex_NonModalDialog;

	
	tbool mbWasPlayingWhenMovePlayheadWasStarted;
	tuint64 muiMoveToPos;
	
	
	void Set_Project_License();
	
	tuint32 miStoredZoom;
	tbool mbZoomFlipFlop;
	
	tbool mbTimer;
	
	tbool mbKSProject_Imported;


	
	
};

