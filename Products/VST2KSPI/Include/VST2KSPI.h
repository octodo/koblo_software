



// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "BaseControlIDs.h"
#include "IBaseDezipperCallback.h"
#include "CBaseDezipper.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseTopPane.h"
#include "FontSizes.h"
#include "CBaseMIDICCEvent.h"

// Module
#include "CVST2KSPIModule.h"

// Parameter
#include "CParmGlobal.h"

// PlugIn
#include "CVST2KSPIPlugIn.h"

// GUI
#include "CVST2KSPIGUI.h"

// DSP
#include "CVST2KSPI.h"

// VST
#include "AEffect.h"
#include "AEffectX.h"

typedef AEffect* (*VSTMainFunc)(audioMasterCallback);


