

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


#ifdef WIN32
//! Instance handle. Do not use (go through CBaseModule instead)
HINSTANCE ghInstance = (HINSTANCE)INVALID_HANDLE_VALUE;
#endif	// WIN32

CBaseModule::CBaseModule(kspi::IHost* pHost)
	: mpHost(pHost)
#ifdef WIN32
, mhInstance(ghInstance)
#endif	// WIN32
{
}

CBaseModule::~CBaseModule()
{
//	dynamic_cast<kspi::CHost*>(mpHost)->Destroy();
}

tuint32 CBaseModule::GetNrOfPlugIns()
{
	return 1;
}

tint32 CBaseModule::GetCompanyID()
{
	return -1;
}

void CBaseModule::GetCompanyName(tchar* pszName)
{
	strcpy(pszName, "n/a");
}

#ifdef WIN32
extern "C" __declspec(dllexport) kspi::IModule* kspiMain(kspi::IHost* pHost, tuint32* piVersionNumber);
#endif	// WIN32
#ifdef _Mac
extern "C" kspi::IModule* kspiMain(kspi::IHost* pHost, k2s::tuint32* piVersionNumber);
#endif	// _Mac

kspi::IModule* MainWrap(kspi::IHost* pHost, tuint32* piVersionNumber)
{
	return kspiMain(pHost, piVersionNumber);
}

#ifdef WIN32

#ifdef _DEBUG
#include <crtdbg.h>
#endif	// _DEBUG

//! Main entry point into module
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /*lpvReserved*/)
{
	ghInstance = hinstDLL;

	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
#ifdef _DEBUG
			// Report memory leaks
//			ReportMemoryLeaks();
//			_CrtDumpMemoryLeaks();
#endif
			break;
	}

	return TRUE;
}

#endif	// WIN32


// The following files need to be present on OSX, otherwise we get linking errors (mo)

extern "C" int WriteCharsToConsole()
{
	return 0;
}

extern "C" int WriteCharsToErrorConsole()
{
	return 0;
}

extern "C" int ReadCharsFromConsole()
{
	return 0;
}

extern "C" int RemoveConsole()
{
	return 0;
}

extern "C" int InstallConsole()
{
	return 0;
}

