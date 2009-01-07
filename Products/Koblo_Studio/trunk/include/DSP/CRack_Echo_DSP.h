

//! RAM allocated for echo effect (per channel). Must be a power of 2.
#define EFFECT_ECHO_BUFFER_SIZE 131072*2


class CRack_Echo_DSP : public virtual CTrack_DSP
{
public:
	CRack_Echo_DSP(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes = 0) : CTrack_DSP(pDSP, iChannelNumber, bIsBusOrMix, ppAUXes)
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







