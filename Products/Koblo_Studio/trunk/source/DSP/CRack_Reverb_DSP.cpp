#include "KSOS.h"


void CRack_Reverb_DSP::Process(tint32 iSamples)
{

	 mpBuffer->SetChannels(2);
	 
	 tfloat32* pfLeft = mpBuffer->GetData(0);
	 tfloat32* pfRight = mpBuffer->GetData(1);
	 
	 // Reverb code goes here. pfLeft and pfRight buffers can be modified. Buffer length is in iSamples
	 float* pfStereo[2];
	 pfStereo[0] = pfLeft;
	 pfStereo[1] = pfRight;
	 
	 KobloVerb.processReplace(pfStereo, pfStereo, iSamples);
	 
	 // Maybe do down or up mix for destination
	 mpBuffer->SetChannels(miDestinationNumberOfChannels);
	 muiSongPos += iSamples;
	
}

void CRack_Reverb_DSP::Initialize()
{
}

void CRack_Reverb_DSP::DeInitialize()
{
}

void CRack_Reverb_DSP::ResetAllEffectsTails()
{
	Stop();
	Start();
}

void CRack_Reverb_DSP::Start()
{
}

void CRack_Reverb_DSP::Stop()
{
	KobloVerb.mute();
	KobloVerb.flush();
}

void CRack_Reverb_DSP:: Set_Reverb_On(tint32 iData)
{
	KobloVerb.setReverbOn(iData ? true: false);
}

void CRack_Reverb_DSP:: Set_Reverb_Size(tint32 iData)
{
	KobloVerb.setRoomSize(iData / 128.0f);
}

void CRack_Reverb_DSP:: Set_Reverb_PDelay(tint32 iData)
{
//	miPDelay = iData;
	KobloVerb.setPredelay(iData);
}

void CRack_Reverb_DSP:: Set_Reverb_Damp(tint32 iData)
{
//	miDamp = iData;
	KobloVerb.setDamp(iData / 128.0f);
}

void CRack_Reverb_DSP:: Set_Reverb_Panorama(tint32 iData)
{
//	miPanorama = iData;
	// notImpl
}

void CRack_Reverb_DSP:: Set_Reverb_Color1(tint32 iData)
{
	//	miColor1 = iData;
	KobloVerb.setHP(iData / 600.0f);
}

void CRack_Reverb_DSP:: Set_Reverb_Color2(tint32 iData)
{
	
	KobloVerb.setLP(iData / 600.0f);
}

void CRack_Reverb_DSP:: Set_Reverb_Mix(tint32 iData)
{
	miMix = iData;
	
	
}


