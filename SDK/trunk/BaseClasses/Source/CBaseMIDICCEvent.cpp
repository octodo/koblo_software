
#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "FontSizes.h"
#include "IBaseDezipperCallback.h"
#include "CBaseMIDICCEvent.h"
#include "CBaseDezipper.h"
#include "CBaseDezipper2.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"

#endif	// WIN32


CBaseMIDICCEvent::CBaseMIDICCEvent(CBaseGUI* pGUI)
	: mpGUI(pGUI)
{
}

CBaseMIDICCEvent::~CBaseMIDICCEvent()
{

}

void CBaseMIDICCEvent::EventValueChange(ge::IControl* pSender, tint32 iValueNew)
{
//!!! Max version 2. Why does i end here	mpGUI->MIDICCLearn(pSender->GetID());
}