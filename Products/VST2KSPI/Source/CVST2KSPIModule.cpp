

#include "VST2KSPIOS.h"

CVST2KSPIModule::CVST2KSPIModule(kspi::IHost* pHost)
	: CBaseModule(pHost)
{
	Init();
}

CVST2KSPIModule::~CVST2KSPIModule()
{
}

void CVST2KSPIModule::Destroy()
{
	delete dynamic_cast<CVST2KSPIModule*>(this);
}

kspi::IPlugIn* CVST2KSPIModule::CreateProcess(tuint32 iIndex)
{
	// Find the right effect
	std::list<std::string>::const_iterator it = msEffectPathNames.begin();
	while (iIndex--) {
		it++;
	}
	std::string sPathName = *it;

	return new CVST2KSPIPlugIn(this, sPathName);
}

tint32 CVST2KSPIModule::GetCompanyID()
{
	return 2;
}

void CVST2KSPIModule::GetCompanyName(tchar* pszName)
{
	strcpy(pszName, "Koblo (VST)");
}

// AEffect* main_macho (audioMasterCallback audioMaster);

VstIntPtr VSTCALLBACK audioMasterCallbackModule(AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	switch(opcode) {
		case audioMasterAutomate:
			return 1;

		case audioMasterVersion:
			return 2400;

		case audioMasterCurrentId:
			return 0;

		case audioMasterIdle:
			return 0;

		case audioMasterGetVendorString:
			strcpy((char*)ptr, "Koblo");
			return 1;

		case audioMasterGetProductString:
			strcpy((char*)ptr, "Studio");
			return 1;

		case audioMasterCanDo:
		{
			char* psz = (char*)ptr;
			if (strcmp(psz, "supplyIdle") == 0) {
				return 0;
			}
			if (strcmp(psz, "sendVstMidiEvent") == 0) {
				return 1;
			}
			if (strcmp(psz, "sendVstTimeInfo") == 0) {
				return 1;
			}
		}
			return 0;
	}

	return 0;
}

void ModifyPathName(std::string& rsPathName);

void CVST2KSPIModule::DoScanDirectory(const std::string& sPathName)
{
	// Scan the system for plug-ins
	CAutoDelete<IFileSearch> pFileSearch(IFileSearch::Create());
	pFileSearch->Init(sPathName.c_str());
	
	tchar pszName[256];
	tbool bDirectory;
	while (pFileSearch->GetNext(pszName, bDirectory)) {
		std::string sPlugInPath = sPathName;

		std::string sName(pszName);

#ifdef WIN32
		if (bDirectory) {
#else	// WIN32
		std::string sEnding = sName.substr(sName.size() - 4);
		if (sEnding != ".vst" && sEnding != ".VST" && sName.size() > 4) {
#endif	
			sPlugInPath += sName;
			sPlugInPath += ":";
			DoScanDirectory(sPlugInPath);
			continue;
		}

		std::string sPathName = sPlugInPath + sName;

#ifdef WIN32
		// Load the plug-in to get info
		{
			HMODULE hDLL;

			std::string sEnding = sName.substr(sName.size() - 4);
			if (sEnding != ".dll" && sEnding != ".DLL" && sEnding != ".Dll" && sName.size() > 4) {
				continue;
			}

			// Load plug-in
			std::string s2 = sPathName;
			ModifyPathName(s2);
			hDLL = LoadLibrary(s2.c_str());
			if (hDLL == NULL) {
				continue;
			}
			
			VSTMainFunc mainFunc = (VSTMainFunc)(void*)GetProcAddress(hDLL, "VSTPluginMain");
			if (mainFunc == NULL) {
				mainFunc = (VSTMainFunc)(void*)GetProcAddress(hDLL, "main");
			}
			if (mainFunc == NULL) {
				continue;
			}
			
			AEffect* pEffect = NULL;
			try {
				pEffect = mainFunc(audioMasterCallbackModule);
				// Note: Call setSampleRate and setBlockSize also (part of initialization sequence).
				pEffect->dispatcher(pEffect, effOpen, 0, 0, 0, 0.0);
				
				// We don't really need the name now, but we want to just try something to see if it crashes. Would be better to get some info about VST version and required host VST version.
				char pszProductName[1042];
				pEffect->dispatcher(pEffect, effGetEffectName, 0, 0, pszProductName, 0.0);
			}
			catch(...) {
				// Plug-in crashed
				//				CFBundleUnloadExecutable(BundleRef);
				//				CFRelease(BundleRef);
				continue;
			}
			
			// Effect appears ok, put it in list
			msEffectPathNames.push_back(sPathName);
			
			// Unload it
			pEffect->dispatcher(pEffect, effClose, 0, 0, 0, 0.0);
			//			CFBundleUnloadExecutable(BundleRef);
			//			CFRelease(BundleRef);
		}
#else	// WIN32
		// Load the plug-in to get info
		{
			CFBundleRef BundleRef;
			
			// Make a CFURLRef from the CFString representation of the bundle's path.
			tchar pszPathName[1024];
			strcpy(pszPathName, sPathName.c_str());
			ToUnix(pszPathName);
			CFURLRef bundleURL = CFURLCreateWithFileSystemPath( 
															   kCFAllocatorDefault, 
															   __CFStringMakeConstantString(pszPathName),
															   kCFURLPOSIXPathStyle,
															   true );
			if ( !bundleURL ) continue;
			
			// Make a bundle instance using the URLRef.
			BundleRef = CFBundleCreate( kCFAllocatorDefault, bundleURL );
			
			// We don't need the URL anymore
			CFRelease( bundleURL );	
			
			if ( !BundleRef ) {
				continue;
			}
			
			if ( !CFBundleLoadExecutable( BundleRef ) )
			{
				CFRelease( BundleRef );
				BundleRef = NULL;
				continue;
			}
			
			VSTMainFunc mainFunc = (VSTMainFunc)CFBundleGetFunctionPointerForName(BundleRef, CFSTR("VSTPluginMain") );
			//			VSTMainFunc mainFunc = (VSTMainFunc)CFBundleGetFunctionPointerForName(BundleRef, CFSTR("main") );
			if (mainFunc == NULL) {
				continue;
			}
			
			AEffect* pEffect = NULL;
			try {
				pEffect = mainFunc(audioMasterCallbackModule);
				// Note: Call setSampleRate and setBlockSize also (part of initialization sequence).
				pEffect->dispatcher(pEffect, effOpen, 0, 0, 0, 0.0);
//				int iVersion = pEffect->dispatcher(pEffect, effGetVstVersion, 0, 0, 0, 0.0);
				
				// We don't really need the name now, but we want to just try something to see if it crashes. Would be better to get some info about VST version and required host VST version.
				char pszProductName[1042];
				pEffect->dispatcher(pEffect, effGetEffectName, 0, 0, pszProductName, 0.0);
			}
			catch(...) {
				// Plug-in crashed
				//				CFBundleUnloadExecutable(BundleRef);
				//				CFRelease(BundleRef);
				continue;
			}
			
			if (pEffect->numInputs == 2 && pEffect->numOutputs == 2) {
				// Effect appears ok, put it in list
				msEffectPathNames.push_back(sPathName);
			}

			// Unload it
			pEffect->dispatcher(pEffect, effClose, 0, 0, 0, 0.0);
			//			CFBundleUnloadExecutable(BundleRef);
			//			CFRelease(BundleRef);
		}
#endif	// WIN32 / else
	}
}

#ifdef WIN32
extern "C" LONG SHRegGetValue(          HKEY hkey,
    LPCTSTR pszSubKey,
    LPCTSTR pszValue,
    INT srrfFlags,
    LPDWORD pdwType,
    LPVOID pvData,
    LPDWORD pcbData
);
#endif

void CVST2KSPIModule::Init()
{
/*	short sVolRef;
	long lDirID;
	tchar pszPlugInPath[1024];
	::FindFolder(kOnSystemDisk, kVolumeRootFolderType, kDontCreateFolder, &sVolRef, &lDirID);
	PathNameFromDirID(lDirID, sVolRef, (char*)pszPlugInPath);
	std::string sPlugInPath(pszPlugInPath);
	sPlugInPath += std::string("Library:Audio:Plug-Ins:VST:");*/
	// Note: This is hardcoded, and needs to change
#ifdef WIN32
	tchar psz[1024];
	memset(psz, 0, 1024);
	DWORD dwSize = 1024;

	HKEY hKey;
	::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\VST",
		0,
		KEY_QUERY_VALUE,
		&hKey);

	LONG lRet = ::RegQueryValueEx(hKey,
		"VSTPluginsPath",
		NULL,
		NULL,
		(LPBYTE)psz,
		&dwSize);

	::RegCloseKey(hKey);

	std::string sPlugInPath;
	if (lRet == ERROR_SUCCESS) {
		sPlugInPath = std::string(psz);
		sPlugInPath[1] = sPlugInPath[0];
		sPlugInPath[0] = ':';
		sPlugInPath[2] = ':';
		sPlugInPath += ":";
	}
	else {
		sPlugInPath = std::string(":C:VSTPlugIns:");
	}
	DoScanDirectory(sPlugInPath);
#else	// WIN32
//	std::string sPlugInPath(":Macintosh HD:Library:Audio:Plug-Ins:VST:");
	tchar pszPath[1024];
	IFile::GetSystemDirectory(IFile::SystemDirLibrary, pszPath);
	strcat(pszPath, "Audio:Plug-Ins:VST:");
	std::string sPlugInPath(pszPath);
	DoScanDirectory(sPlugInPath);

	IFile::GetSystemDirectory(IFile::SystemDirUser, pszPath);
	strcat(pszPath, "Library:Audio:Plug-Ins:VST:");
	sPlugInPath = std::string(pszPath);
	DoScanDirectory(sPlugInPath);
#endif	// WIN32 / else
}

tuint32 CVST2KSPIModule::GetNrOfPlugIns()
{
	return msEffectPathNames.size();
}

void DoMsgBox(const tchar* psz);

// Lasse, added 2007-08-16 - new c style interface
void kspiDispatcherWrap(void* hObject, tint32 iCode, void** ppParms, void* pReturn);

#ifdef WIN32
extern "C" __declspec(dllexport) void Dispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn);
extern "C" __declspec(dllexport) kspi::IModule* kspiMain(void* pHostCallback, void* hHostObject, tuint32* piVersionNumber, void** ppDispatcher)
#endif	// WIN32
#ifdef _Mac
extern "C" void Dispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn);
extern "C" kspi::IModule* kspiMain(void* pHostCallback, void* hHostObject, k2s::tuint32* piVersionNumber, void** ppDispatcher)
#endif	// _Mac
{
	*piVersionNumber = kspi::giCurrentRevision;

	kspi::IHost* pHost = kspi::IHost::Create((kspi::kspiCCallbackDispatcherFunc)pHostCallback, hHostObject);

	*ppDispatcher = (void*)&Dispatcher;

	return new CVST2KSPIModule(pHost);
}

#ifdef WIN32
extern "C" __declspec(dllexport) void Dispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif // WIN32
#ifdef _Mac
extern "C" void Dispatcher(void* pObject, tint32 iCode, void** ppParms, void* pReturn)
#endif // _Mac
{
	try {
		kspiDispatcherWrap(pObject, iCode, ppParms, pReturn);
	}
	catch(IException* pException) {
		tchar psz[512];
		sprintf(psz, "Software error:\n%s", pException->GetFullDescription());

		DoMsgBox(psz);

		pException->Destroy();
	}
	catch(...) {
		DoMsgBox("Unknkown Exception");
	}
}


