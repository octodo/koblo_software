

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "FontSizes.h"

#include "BaseControlIDs.h"
#include "IBaseDezipperCallback.h"
#include "CBaseDezipper.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseMIDICCEvent.h"

#endif	// WIN32


CBasePaneStd::CBasePaneStd(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: CBasePane(pPaneParent, pGUI)
{
}

CBasePaneStd::~CBasePaneStd()
{
}

void CBasePaneStd::Init()
{
	mpPane = ge::IPane::Create();
}

void CBasePaneStd::SetPane(ge::IPane* pPane)
{
	mpPane = pPane;
}

void CBasePaneStd::EventValueChange(ge::IControl* pControl, tint32 iValueNew)
{
	mpParent->EventValueChange(pControl, iValueNew);
}

void CBasePaneStd::EventGeneric(ge::IControl* pControl, void *pEventData)
{
	mpParent->EventGeneric(pControl, pEventData);
}



