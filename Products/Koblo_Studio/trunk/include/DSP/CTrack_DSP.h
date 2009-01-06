

class CTrack_DSP
{
public:
	//! Constructor
	CTrack_DSP(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes);

	//! Destructor
	virtual ~CTrack_DSP();

	//! Initializw
	virtual void Initialize();
	//! deinitialize
	virtual void DeInitialize();
	//! process track
	virtual void Process(tint32 iSamples);

	//! Set volume 0.0-0.1f
	virtual void SetVolume(tfloat32 fVolume);
	//! set aux send
	virtual void SetAUXVolume(tint32 iAUX, tfloat32 fVolume);
	//! clear all effect tails
	virtual void ResetAllEffectsTails();
	//! set left right panning
	virtual void SetPanningLeftRight(tfloat32 fLeftRight);
	//! set front back panning !not used
	virtual void SetPanningFrontBack(tfloat32 fFrontBack);
	//! Set input channel count
	virtual void SetInputChannelCount(tuint32 iChannels);
	//! set first input
	virtual void SetFirstInput(tuint32 uiFirstInput);
	//! set track mode (curently always stereo)
	virtual void SetTrackMode(tuint32 iChannels);
	//! out put destination
	virtual void SetOutputDestination(tuint32 uiDest, tint32 iDestNumberOfChannels);
	//! get track mode
	tuint32 GetChannelMode() const {return miModeChannelCount;};

	//! IMixerChannel override
	virtual tuint32 GetOutputDestination() const {return miDestination;};

	virtual tint32 GetNumberOfChannelsForPanner() { return miNumberOfChannelsForPanner; };
	virtual tint32 GetNumberOfInputChannels() { return miInputChannelCount; };

	CBuffer* GetBuffer() {return mpBuffer;}

	CRegion_DSP* CreateRegion(tint32 iUniqueID, 
								const std::string& sSoundListItemName, 
								tuint64 uiTrackPosStart, 
								tuint64 uiSamplePosStart, 
								tuint64& ruiSamplePosEnd,
								tuint64 uiFadeInLength = 0,
								tuint64 uiFadeOutLength = 0,
								tfloat32 fRegionVolume = 1.0f);
								
	//! Get size of region in Samples
	tint64 GetRegionSize(tint32 iUniqueID, const std::string& sSoundListItemName, tuint64 uiSoundPosStart, tint64 uiSoundPosEnd);
	
	void Update_Regions_For_Playback();
	
	void DeleteRegion(tint32 iID);

	CRegion_DSP* GetRegion_DSP(tuint32 uiID);
	
	//! Get region track pos
	tuint64 GetRegionPosOnTrack(tuint32 uiID);
	//! Update the size of a region;
	void TrimRegion(tuint32 uiID, tuint64 iTrackStartPos, tuint64 iSoundStartPos, tint64 iSoundEndPos);

	void SetSongPosition(tuint64 uiPosNew);
	
	tbool HasRegions() const {return mRegionInfoList.size() != 0;}

	void IncSongPos(tint32 iSamples) {muiSongPos += iSamples;}

	struct SChannelRegionInfo {
		CRegion_DSP* pSoundObject;
		tuint64 uiTrackPosStart;
		tuint32 uiRegionID;
	};

	void Insert_Region_Info( SChannelRegionInfo* pInfo);
	
	const std::list<SChannelRegionInfo*>& GetRegionList() {return mRegionInfoList;}

	void AddInsert(tint32 iInsert, tuint32 uiCompanyID, tuint32 uiProductID, tuint32 uiProductID2);

	void SetInsertBypass(tint32 iINsert, tbool bBypass);

	void LimitDataFromStream(tbool bEnableLimiting, tint64 iFirstSampleIx, tint64 iFinalSampleIx)
	{ mbLimitDataFromStream = bEnableLimiting; miFirstStreamIx_Limited = iFirstSampleIx; miFinalStreamIx_Limited = iFinalSampleIx; };


	void GetChannelRegionInfo(tuint32 iRegion, SChannelRegionInfo info);
	
//	void Edit_Selection_On_Track(tuint64 uiSelection_Pos, tuint64  uiSelection_Duration);
	// Fade selection in
	void Edit_Selection(tint32 iCmd, tuint64 uiSelection_Pos, tuint64  uiSelection_Duration);

	
	//! Render fade region and insert it on track
	tint32 Create_Fade_Region(	tint32 iCmd, 
								const std::string& sSoundClipName, 
								tuint64 uiTrack_Pos, 
								tuint64 uiSample_Start, 
								tuint64 uiSamplePosEnd,
								tuint64 uiSelection_Duration);

	kspi::IPlugIn* GetInsert(tint32 iIndex) {return mppInsert[iIndex];}

	void SetArmed(tbool bNew) {mbArmed = bNew;}

	tbool IsArmed() const {return mbArmed;}

	void SetInputChannel(tint32 iNew) {miInputChannel = iNew;}

	void Start();

	void Stop();

protected:
	tbool mbArmed;

	tint32 miInputChannel;

	std::list<SChannelRegionInfo*> mRegionInfoList;

	std::list<SChannelRegionInfo*>::const_iterator mitRegionsInfo;

	tint32 miRecordingChannels;

	tint64 miRecordingSongPos;

	CBuffer* mpBuffer;
	// Parrent pointer, not needed there is a global one
	CDSP* mpDSP;

	CKSPlugIn* mpPlugIn;

	st::IDSPTools* mpDSPTools;

	CTrack_DSP** mppAUXes;

	tfloat32 mfVolumeSlider;
	tfloat32 mfPanningLeft;//, 
	tfloat32 mfPanningFrontBack;
	tfloat32 mfPanningRight;
	tfloat32 mpfAUXVolume[2];

	CBaseDezipper2* mapDezipperOutAmp[8];
	CBaseDezipper2* mpDezipperPreAmp;
	
	tbool mbMeterBeforeEffects;
	tint32 miNumberOfChannelsForMeter;

	tint32 miChannelNumber;

	tfloat32 mpfBufferTmp1[giAudioMaxBufferSize];
	tfloat32 mpfBufferTmp2[giAudioMaxBufferSize];

	CPlugInManager::PlugInHandle mhInserts[4];
	kspi::IPlugIn* mppInsert[4];
	tbool mpbInsertBypass[4];

	tuint32 miInputChannelCount;
	tuint32 muiFirstInput;
	tuint32 miModeChannelCount;
	tuint32 miDestination;
	tint32 miDestinationNumberOfChannels;
	tint32 miNumberOfChannelsForPanner;
	tbool mbIsBusOrMix;
	tbool mbKillDecay;


	//! True if we only play back a certain range of the stream data
	tbool mbLimitDataFromStream;
	tint64 miFirstStreamIx_Limited;
	tint64 miFinalStreamIx_Limited;

	//! Internal use only (doesn't lock mutex) - calculates and sets how many channels should exist for panners and peak meters
	virtual void _CalcSetNumberOfChannelsForPanner();
	
	//! Internal use only (doesn't lock mutex) - calculates and sets the volume dezippers for the channels
	virtual void _CalcSetVolumeDezippers();
	
	//! Internal use only (doesn't lock mutex) - avoid hanging peak volumes when sliding volume and panners
	virtual void _KillPeakVolume();

	tuint64 muiSongPos;

	// fade volume
	tfloat32 mfFade_Vol;
	// Fade position
	tint64 miFade_Pos;

	IFile* mpFileRecording;
	tfloat32 mfPeak;
	tfloat32 mfPeakMono;

	std::string msRecordingName;

	std::string msRecordingNameDest;
};

class CAUXReverb : public virtual CTrack_DSP
{
public:
	CAUXReverb(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes = 0) : CTrack_DSP(pDSP, iChannelNumber, bIsBusOrMix, ppAUXes)
		{miDestinationNumberOfChannels = 2;}

	//! CTrack_DSP override
	virtual void Initialize();
	//! CTrack_DSP override
	virtual void DeInitialize();

	//! IMixerChannel override
	virtual void ResetAllEffectsTails();

	virtual void Start();
	virtual void Stop();

	virtual void Process(tint32 iSamples);
	
	virtual void Set_Reverb_On(tint32 iValue);
	virtual void Set_Reverb_Size(tint32 iValue);
	virtual void Set_Reverb_PDelay(tint32 iValue);
	virtual void Set_Reverb_Damp(tint32 iValue);
	virtual void Set_Reverb_Panorama(tint32 iValue);
	virtual void Set_Reverb_Color1(tint32 iValue);
	virtual void Set_Reverb_Color2(tint32 iValue);
	virtual void Set_Reverb_Mix(tint32 iValue);
	
private:

	revmodel KobloVerb; 
	tint32 miMix;
//	tint32 miSize;
//	tint32 miPDelay;
//	tint32 miDamp;
//	tint32 miPanorama;
//	tint32 miColor1;
//	tint32 miColor2;
	tbool  mbReverb_On;
			
};

//! RAM allocated for echo effect (per channel). Must be a power of 2.
#define EFFECT_ECHO_BUFFER_SIZE 131072*2

class CAUXEcho : public virtual CTrack_DSP
{
public:
	CAUXEcho(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes = 0) : CTrack_DSP(pDSP, iChannelNumber, bIsBusOrMix, ppAUXes)
		{miDestinationNumberOfChannels = 2;}

	//! CTrack_DSP override
	virtual void Initialize();
	//! CTrack_DSP override
	virtual void DeInitialize();

	//! IMixerChannel override
	virtual void ResetAllEffectsTails();

	virtual void Start();
	virtual void Stop();

	virtual void Process(tint32 iSamples);
	
	virtual void Set_Delay_Power(tbool bPower);
	virtual void Set_Delay_Time(tint32 iMSec);
	virtual void Set_Feedback(tint32 iFeedback);
	virtual void Set_LF_Damping(tint32 iLF_Damping);
	virtual void Set_HP_Damping(tint32 iHP_Damping);
	virtual void Set_Sync_To_Tempo(tbool bSync);
	
	//tint32 GetSampleRate(){ return mfSampleRate;};
	
	
private:
	
	//! Feedback
	tfloat mfFeedback;
	//! Damping
	tfloat mfDamping;
	//! High freq Damping
	tfloat mfHPDamping;
	//! Buffer (left channel)
	tfloat* mpfBuffer1;
	//! Buffer (right channel)
	tfloat* mpfBuffer2;
	//! Current writing position into buffer
	tint32 miPos;
	//! Current delay time in samples
	tint32 miInterFaceDelayTime;
	//! Current delay time in samples
	tint32 miDelayTime;
	//! Current Host delay time in samples
	tint32 miHostDelayTime;
	//! Synck to host time
	tbool  mbSyncToHost;
	//! Low pass history
	tfloat mpfLPHistory[2];
	//! High pass history
	tfloat mpfHPHistory[2];
	// Power on off
	tint32 miPower;
	// Delay time in mSec
	tint32 miMSec;
	

	virtual void Set_Delay_in_Samples();
	
	
	

};







