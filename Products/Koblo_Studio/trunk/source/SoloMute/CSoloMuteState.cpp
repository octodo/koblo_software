#include "KSOS.h"

CSoloMuteState::CSoloMuteState()
{

}


CSoloMuteState* CSoloMuteState::Create()
{
	return dynamic_cast<CSoloMuteState*>(new CSoloMuteState());
}


void CSoloMuteState::Destroy()
{

}

void CSoloMuteState::Init(CBasePane* pPane, CSoloMuteContainer* pParrent, tint32 iMuteParamID, tint32 iSoloParemID, tint32 iSection)
{
	mpPane			= pPane;
	mpParrent		= pParrent;
	miMuteParamID	= iMuteParamID;
	miSoloParamID	= iSoloParemID;
	miSection		= iSection;
	mbMute			= false;
	mbSolo			= false;
	mbSoftMute		= false;
	mbSoftUnMute	= false;
	mbWasMuted		= false;
	mbWasSolo		= false;
	mbLock			= false;

	
}

void CSoloMuteState::Solo(tbool bSolo)
{	
	// Prevent feed back
	mbLock = true;

	mbSolo = bSolo;
	mpPane->GetPlugIn()->SetGlobalParm(miSoloParamID, bSolo, miSection);

	mbLock = false;
	
}

void CSoloMuteState::SoftMute()
{	
	// Prevent feed back
	mbLock = true;
	
	mbSoftMute		= true;
	mpPane->GetPlugIn()->SetGlobalParm(miMuteParamID, true, miSection);
	
	mbLock = false;
}

void CSoloMuteState::SoftUnMute()
{	
	// Prevent feed back
	mbLock = true;

	mbSoftMute		= false;
	mpPane->GetPlugIn()->SetGlobalParm(miMuteParamID, false, miSection);	
	
	mbLock = false;
	
}

void CSoloMuteState::SoftReMute()
{	
	// Prevent feed back
	mbLock = true;
	
	mbWasMuted		= false;
	mbMute			= true;
	mpPane->GetPlugIn()->SetGlobalParm(miMuteParamID, true, miSection);
	
	mbLock = false;
	
}
















