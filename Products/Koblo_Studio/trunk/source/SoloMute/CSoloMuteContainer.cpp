#include "KSOS.h"

CSoloMuteContainer::CSoloMuteContainer()
{

}


CSoloMuteContainer* CSoloMuteContainer::Create()
{
	return dynamic_cast<CSoloMuteContainer*>(new CSoloMuteContainer());
}


void CSoloMuteContainer::Destroy()
{

}


void CSoloMuteContainer::Init(CBasePane* pPane)
{
	mpPane			= pPane;
	miNumChannels	= 0;		
}

void CSoloMuteContainer::AddChannel(tint32 iMuteParamID, tint32 iSoloParemID, tint32 iChannel)
{

	mpSoloMuteState[miNumChannels] = CSoloMuteState::Create();
	mpSoloMuteState[miNumChannels]->Init(mpPane, this, iMuteParamID, iSoloParemID, iChannel);
	miNumChannels++;

}
/*
void CSoloMuteContainer::UpdateAuxData(tint32 iID, tint32 iValue)
{
	if( iID >= giParam_Aux1Solo && iID < giParam_Aux1Mute +giNumberOfAuxs) {
		tint32 iAUX;
		tint32 iParamID;
		// Find AUX nr
		iAUX	= iID - giParam_Aux1Solo;
		if(iAUX >= giNumberOfAuxs ) 
			iAUX -= giNumberOfAuxs;
		
		iParamID = iID - iAUX;

		SetAUXData(iParamID , iValue,  iAUX);
	}
}
*/

void CSoloMuteContainer::UpdateChannelData(tint32 iID, tint32 iValue, tint32 iChannel)
{	
	switch(iID) {
		default: break;
		case giParam_ChSolo: SoloIn(iChannel, iValue); break;
		case giParam_ChMute: MuteIn(iChannel, iValue); break;
	}
}

/*
void CSoloMuteContainer::SetAUXData(tint32 iID, tint32 iValue, tint32 iAUX)
{
	switch(iID) {
		default: break;
		case giParam_Aux1Solo: SoloIn(iAUX, iValue); break;
		case giParam_Aux1Mute: MuteIn(iAUX, iValue); break;
	}
}
*/

void CSoloMuteContainer::SoloIn(tint32 iCaller, tint32 iValue)
{
	if(mpSoloMuteState[iCaller]->IsLocked()) return;
	if(iValue) { 
		MuteAll(iCaller);
		UnMuteThis(iCaller);
		mpSoloMuteState[iCaller]->SetSoloFlag(true);
	}
	else  {
		ReMuteThis(iCaller);
		UnMuteAll(iCaller);
	} 
}

void CSoloMuteContainer::MuteIn(tint32 iCaller, tint32 iValue)
{
	if(mpSoloMuteState[iCaller]->IsLocked()) return;

	if(iValue) {
		UnSolo(iCaller);
		mpSoloMuteState[iCaller]->SetMuteFlag(true);
		}
		else {
			mpSoloMuteState[iCaller]->SetMuteFlag(false);
		}
}

void CSoloMuteContainer::MuteAll(tint32 iCaller)
{
	for(tint32 i=0; i<miNumChannels; i++){
		if(i!=iCaller) {
		
			if(!mpSoloMuteState[i]->IsMute() && !mpSoloMuteState[i]->IsSolo())
				mpSoloMuteState[i]->SoftMute();
		}
	}
}

void CSoloMuteContainer::ReMuteThis(tint32 iCaller)
{
	mpSoloMuteState[iCaller]->SetSoloFlag(false);
	
	if(mpSoloMuteState[iCaller]->WasMuted()) {
		mpSoloMuteState[iCaller]->SoftReMute();
		mpSoloMuteState[iCaller]->SetMuteFlag(true);

	}
	else if(!CheckForSolo()) {
		mpSoloMuteState[iCaller]->SetMuteFlag(false);
		mpSoloMuteState[iCaller]->SoftMute();
	}
	else {
		mpSoloMuteState[iCaller]->SetMuteFlag(false);
	}
}

void CSoloMuteContainer::UnMuteAll(tint32 iCaller)
{	
	if(CheckForSolo()) {
		for(tint32 i=0; i<miNumChannels; i++){
			if(iCaller != i){
			
				if(mpSoloMuteState[i]->IsSoftMute())
					mpSoloMuteState[i]->SoftUnMute();
			}
		}
	}
}

void CSoloMuteContainer::UnSolo(tint32 iCaller)
{
	mpSoloMuteState[iCaller]->Solo(false);
	UnMuteAll(iCaller);
}

void CSoloMuteContainer::UnMuteThis(tint32 iCaller)
{
	if( mpSoloMuteState[iCaller]->IsMute() )
		mpSoloMuteState[iCaller]->SetWasMutedFlag(true);
	else
		mpSoloMuteState[iCaller]->SetWasMutedFlag(false);
		
	mpSoloMuteState[iCaller]->SoftUnMute();
	

}

tbool CSoloMuteContainer::CheckForSolo()
{
	// Check for Solo
	for(tint32 i=0; i<miNumChannels; i++){
		if(mpSoloMuteState[i]->IsSolo() )
			return false;
	}
	return true;

}






