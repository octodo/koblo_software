

class CRack_Reverb_DSP : public virtual CTrack_DSP
{
public:
	CRack_Reverb_DSP(CDSP* pDSP, tint32 iChannelNumber, tbool bIsBusOrMix, CTrack_DSP** ppAUXes = 0) : CTrack_DSP(pDSP, iChannelNumber, bIsBusOrMix, ppAUXes)
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
	virtual void Set_AUX2_Return(tfloat32 fValue);
	
private:

	revmodel KobloVerb; 
	tint32 miMix;
	tbool  mbReverb_On;
	tfloat32 mfAUX2_Return;
			
};







