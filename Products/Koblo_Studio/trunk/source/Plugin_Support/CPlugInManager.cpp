
#include "KSOS.h"

#include "kspiCHost.h"

#ifdef WIN32
#include <ShlObj.h>
#endif

const CPlugInManager::PlugInHandle CPlugInManager::mInvalidHandleValue = -1;

CPlugInManager::CPlugInManager()
{
}

CPlugInManager::~CPlugInManager()
{
}

void CPlugInManager::Init()
{
	CAutoLock Lock(gpApplication->GetMeterMutex());

#ifdef _Mac
	// Get the path to the plug-in folder
	tchar pszPlugInPath[1024];
	IFile::GetSystemDirectory(IFile::SystemDirApplicationSupport, pszPlugInPath);
	std::string sPlugInPath(pszPlugInPath);
	sPlugInPath += std::string("Koblo:KSPI:Plug-Ins:");

	// Scan the system for plug-ins
	CAutoDelete<IFileSearch> pFileSearch(IFileSearch::Create());
	// Lasse, modified 2008-04-15 - don't crash if folder missing
	//pFileSearch->Init(sPlugInPath.c_str());
	pFileSearch->Init2(sPlugInPath.c_str());
	// .. Lasse

	tchar pszName[256];
	tbool bDirectory;
	while (pFileSearch->GetNext(pszName, bDirectory)) {
		std::string sName(pszName);
		std::string sPathName = sPlugInPath + sName;

		// Load the plug-in to get info
		{
			CFBundleRef BundleRef;
			kspi::IModule* pModule;
			kspi::IPlugIn* pPlugIn;

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

			kspi::kspiMainFunc mainFunc = (kspi::kspiMainFunc)(void*)CFBundleGetFunctionPointerForName( 
						BundleRef, CFSTR("kspiMain") );
			if (mainFunc == NULL) {
				continue;
			}

			tuint32 uiKSPIVersionNumber;
			void* pDispatcher = NULL;
			void* hModule = mainFunc(&kspi::kspiCCallbackDispatcher, dynamic_cast<kspi::IHost*>(this), &uiKSPIVersionNumber, &pDispatcher);
			if (hModule == NULL) {
				continue;
			}

			kspi::kspiCDispatcherFunc CFunc = (kspi::kspiCDispatcherFunc)pDispatcher;
			if (CFunc == NULL) {
				continue;
			}
			pModule = kspi::kspiModule::Create(CFunc, hModule, &uiKSPIVersionNumber, false);

			// Get the number of plug-ins in the module
			tint32 iPlugIns = pModule->GetNrOfPlugIns();

			tint32 iPlugIn;
			for (iPlugIn = 0; iPlugIn < iPlugIns; iPlugIn++) {
				SPlugInInfo* pInfo = new SPlugInInfo();

				pInfo->PlugInType = SPlugInInfo::PlugInTypeEffect;

				pInfo->sPathName = sPathName;
				pInfo->iPlugInIndex = iPlugIn;

				pInfo->puiInputs = new tuint32[1024];
				pInfo->puiOutputs = new tuint32[1024];
				pInfo->puiSideChains = new tuint32[1024];

				pPlugIn = pModule->CreateProcess(iPlugIn);
				if (pPlugIn == NULL) {
					// No plugin with given index
					continue;
				}

				// Get the channel configurations
				pPlugIn->GetChannelConfigurations(&(pInfo->uiChannelConfigurations), pInfo->puiInputs, pInfo->puiOutputs, pInfo->puiSideChains);

				// Get the ids and names
				tchar pszTmp[256];
				pInfo->uiCompanyID = pModule->GetCompanyID();
				pModule->GetCompanyName(pszTmp);
				pInfo->sCompanyName = std::string(pszTmp);

				pInfo->uiProductID = pPlugIn->GetProductID();
				pPlugIn->GetProductName(pszTmp);
				pInfo->sProductName = std::string(pszTmp);

				mPlugIns.push_back(pInfo);

				pPlugIn->Destroy();
			}

			pModule->Destroy();
		}
	}
#endif	// _Mac

#ifdef WIN32
	// Get the path to the plug-in folder
	tchar pszPlugInPath[1024];
	::CoInitialize(NULL);
	// This only works with NT-based Windows versions
	::SHGetSpecialFolderPath(NULL, (LPSTR)pszPlugInPath, CSIDL_PROGRAM_FILES_COMMON, FALSE);
	::CoUninitialize();
	std::string sPlugInPath(pszPlugInPath);
	sPlugInPath += std::string("\\Koblo\\KSPI\\Plug-Ins\\");

	// Scan the system for plug-ins
	CAutoDelete<IFileSearch> pFileSearch(IFileSearch::Create());
	// Lasse, modified 2008-04-15 - don't crash if folder missing
	//pFileSearch->Init(sPlugInPath.c_str());
	pFileSearch->Init2(sPlugInPath.c_str());
	// .. Lasse

	tchar pszName[256];
	tbool bDirectory;
	while (pFileSearch->GetNext(pszName, bDirectory)) {
		std::string sName(pszName);
		std::string sPathName = sPlugInPath + sName;

		// Load the plug-in to get info
		{
			HMODULE hDLL;
			kspi::IModule* pModule;
			kspi::IPlugIn* pPlugIn;

			hDLL = LoadLibrary(sPathName.c_str());
			if (hDLL == NULL) {
				continue;
			}

			kspi::kspiMainFunc mainFunc = (kspi::kspiMainFunc)(void*)GetProcAddress(hDLL, "kspiMain"); 
			if (mainFunc == NULL) {
				continue;
			}

			tuint32 uiKSPIVersionNumber;
			void* pDispatcher = NULL;
			void* hModule = mainFunc(&kspi::kspiCCallbackDispatcher, dynamic_cast<kspi::IHost*>(this), &uiKSPIVersionNumber, &pDispatcher);
			if (hModule == NULL) {
				continue;
			}

			kspi::kspiCDispatcherFunc CFunc = (kspi::kspiCDispatcherFunc)pDispatcher;
			if (CFunc == NULL) {
				continue;
			}
			pModule = kspi::kspiModule::Create(CFunc, hModule, &uiKSPIVersionNumber, false);

			// Get the number of plug-ins in the module
			tint32 iPlugIns = pModule->GetNrOfPlugIns();

			tint32 iPlugIn;
			for (iPlugIn = 0; iPlugIn < iPlugIns; iPlugIn++) {
				SPlugInInfo* pInfo = new SPlugInInfo();

				pInfo->PlugInType = SPlugInInfo::PlugInTypeEffect;

				pInfo->sPathName = sPathName;
				pInfo->iPlugInIndex = iPlugIn;

				pInfo->puiInputs = new tuint32[1024];
				pInfo->puiOutputs = new tuint32[1024];
				pInfo->puiSideChains = new tuint32[1024];

				pPlugIn = pModule->CreateProcess(iPlugIn);
				if (pPlugIn == NULL) {
					// No plugin with given index
					continue;
				}

				// Get the channel configurations
				pPlugIn->GetChannelConfigurations(&(pInfo->uiChannelConfigurations), pInfo->puiInputs, pInfo->puiOutputs, pInfo->puiSideChains);

				// Get the ids and names
				tchar pszTmp[256];
				pInfo->uiCompanyID = pModule->GetCompanyID();
				pModule->GetCompanyName(pszTmp);
				pInfo->sCompanyName = std::string(pszTmp);

				pInfo->uiProductID = pPlugIn->GetProductID();
				pPlugIn->GetProductName(pszTmp);
				pInfo->sProductName = std::string(pszTmp);

				mPlugIns.push_back(pInfo);

				pPlugIn->Destroy();
			}

			pModule->Destroy();
		}
	}
#endif	// WIN32
}

CPlugInManager::PlugInHandle CPlugInManager::LoadPlugIn(tint32 iCompanyID, tint32 iProductID, tint32 iChannel, tint32 iInsertIndex)
{
	//!!! TO DO KS dont load old plug-in's here
	CAutoLock Lock(gpApplication->GetMeterMutex());

	tint32 iIndex = 0;
	std::list<SPlugInInfo*>::const_iterator it = mPlugIns.begin();
	for (; it != mPlugIns.end(); it++, iIndex++) {
		SPlugInInfo* pInfo = *it;

		if (pInfo->uiCompanyID == iCompanyID &&
			pInfo->uiProductID == iProductID) {
				return LoadPlugIn(iIndex, iChannel, iInsertIndex);
			}
	}

	return mInvalidHandleValue;
}

kspi::IPlugIn* CPlugInManager::GetPlugInFromHandle(CPlugInManager::PlugInHandle Handle)
{
	if (Handle == mInvalidHandleValue) {
		throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, "Invalid Handle");
	}

	std::list<kspi::IPlugIn*>::const_iterator it = mLoadedPlugIns.begin();
	for (; it != mLoadedPlugIns.end(); it++) {
		if (Handle == (PlugInHandle)(*it)) {
			return *it;
		}
	}

	return NULL;
}

CPlugInManager::PlugInHandle CPlugInManager::LoadPlugIn(tint32 iIndex, tint32 iChannel, tint32 iInsertIndex)
{
	CAutoLock Lock(gpApplication->GetMeterMutex());

	if (iIndex >= (tint32)mPlugIns.size()) {
		throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, "Invalid Index / Unknown plug-in");
	}

	// Locate the plug-in
	std::list<SPlugInInfo*>::const_iterator it = mPlugIns.begin();
	for (; iIndex; it++, iIndex--) {
	}
	SPlugInInfo* pInfo = *it;

#ifdef _Mac
	CFBundleRef BundleRef;
	kspi::IModule* pModule;
	kspi::IPlugIn* pPlugIn;

	// Make a CFURLRef from the CFString representation of the bundle's path.
	tchar pszPathName[1024];
	strcpy(pszPathName, pInfo->sPathName.c_str());
	ToUnix(pszPathName);
	CFURLRef bundleURL = CFURLCreateWithFileSystemPath( 
		kCFAllocatorDefault, 
		__CFStringMakeConstantString(pszPathName),
		kCFURLPOSIXPathStyle,
		true );
	if ( !bundleURL ) {
	}

	// Make a bundle instance using the URLRef.
	BundleRef = CFBundleCreate( kCFAllocatorDefault, bundleURL );

	// We don't need the URL anymore
	CFRelease( bundleURL );	

	if (!BundleRef) {
	}

	if (!CFBundleLoadExecutable(BundleRef)) {
		CFRelease( BundleRef );
		BundleRef = NULL;
	}

	kspi::kspiMainFunc mainFunc = (kspi::kspiMainFunc)(void*)CFBundleGetFunctionPointerForName( 
				BundleRef, CFSTR("kspiMain") );
	if (mainFunc == NULL) {
	}

	tuint32 uiKSPIVersionNumber;
	void* pDispatcher = NULL;
	void* hModule = mainFunc(&kspi::kspiCCallbackDispatcher, dynamic_cast<kspi::IHost*>(this), &uiKSPIVersionNumber, &pDispatcher);
	if (hModule == NULL) {
	}

	kspi::kspiCDispatcherFunc CFunc = (kspi::kspiCDispatcherFunc)pDispatcher;
	if (CFunc == NULL) {
	}
	pModule = kspi::kspiModule::Create(CFunc, hModule, &uiKSPIVersionNumber, false);

	pPlugIn = pModule->CreateProcess(pInfo->iPlugInIndex);
	if (pPlugIn == NULL) {
		// No plugin with given index
	}

	tint32 iLoadedPlugs = mLoadedPlugIns.size();

	mLoadedPlugIns.push_back(pPlugIn);

	pPlugIn->PreInitialize();
	pPlugIn->SetSampleRate(44100); //!!! Get sample rate from KSPlugIn here 
	pPlugIn->SetMaxBufferSize(32);
	pPlugIn->Initialize();
	pPlugIn->Start();

	CBaseGUI* pBaseGUI = gpMainApplication->CreateExtraWindow(4, (void*)CFSTR("PlugInWnd"));
	CKSPlugInGUI* pPlugInGUI = dynamic_cast<CKSPlugInGUI*>(pBaseGUI);

	kspi::IGUI* pGUI = pPlugIn->CreateGUI(0);

	pPlugInGUI->SetInfo(iChannel, iInsertIndex);

	pPlugInGUI->SetGUI(pGUI);

	SLoadedPlugInInfo LoadedInfo;
	LoadedInfo.pPlugIn = pPlugIn;
	LoadedInfo.bGUILoaded = true;
	LoadedInfo.pBaseGUI = pBaseGUI;
	LoadedInfo.pPlugInGUI = pPlugInGUI;
	LoadedInfo.pGUI = pGUI;

	mPlugInMap.insert(std::pair<tint32, SLoadedPlugInInfo>(iChannel << 16 | iInsertIndex, LoadedInfo));

//	return (PlugInHandle)iLoadedPlugs;
	return (PlugInHandle)pPlugIn;
#endif	// _Mac

#ifdef WIN32
	HMODULE hDLL;
	kspi::IModule* pModule;
	kspi::IPlugIn* pPlugIn;

	hDLL = LoadLibrary(pInfo->sPathName.c_str());
	if (hDLL == NULL) {
	}

	kspi::kspiMainFunc mainFunc = (kspi::kspiMainFunc)(void*)GetProcAddress(hDLL, "kspiMain"); 
	if (mainFunc == NULL) {
	}

	tuint32 uiKSPIVersionNumber;
	void* pDispatcher = NULL;
	void* hModule = mainFunc(&kspi::kspiCCallbackDispatcher, dynamic_cast<kspi::IHost*>(this), &uiKSPIVersionNumber, &pDispatcher);
	if (hModule == NULL) {
	}

	kspi::kspiCDispatcherFunc CFunc = (kspi::kspiCDispatcherFunc)pDispatcher;
	if (CFunc == NULL) {
	}
	pModule = kspi::kspiModule::Create(CFunc, hModule, &uiKSPIVersionNumber, false);

	pPlugIn = pModule->CreateProcess(pInfo->iPlugInIndex);
	if (pPlugIn == NULL) {
		// No plugin with given index
	}

	tint32 iLoadedPlugs = mLoadedPlugIns.size();

	mLoadedPlugIns.push_back(pPlugIn);

	pPlugIn->PreInitialize();
	pPlugIn->SetSampleRate(44100);
	pPlugIn->SetMaxBufferSize(32);
	pPlugIn->Initialize();
	pPlugIn->Start();

	CBaseGUI* pBaseGUI = gpMainApplication->CreateExtraWindow(4, (void*)"PlugInWnd", true);
	CKSPlugInGUI* pPlugInGUI = dynamic_cast<CKSPlugInGUI*>(pBaseGUI);

	kspi::IGUI* pGUI = pPlugIn->CreateGUI(0);

	pPlugInGUI->SetInfo(iChannel, iInsertIndex);

	pPlugInGUI->SetGUI(pGUI);

	SLoadedPlugInInfo LoadedInfo;
	LoadedInfo.pPlugIn = pPlugIn;
	LoadedInfo.bGUILoaded = true;
	LoadedInfo.pBaseGUI = pBaseGUI;
	LoadedInfo.pPlugInGUI = pPlugInGUI;
	LoadedInfo.pGUI = pGUI;

	mPlugInMap.insert(std::pair<tint32, SLoadedPlugInInfo>(iChannel << 16 | iInsertIndex, LoadedInfo));

//	return (PlugInHandle)iLoadedPlugs;
	return (PlugInHandle)pPlugIn;
#endif	// WIN32
}

kspi::IGUI* CPlugInManager::CreateGUI(PlugInHandle Handle, tint32 iIndex)
{	///!!! This is newer called
	CAutoLock Lock(gpApplication->GetMeterMutex());

	if (Handle == mInvalidHandleValue) {
		throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, "Invalid Handle");
	}
	// Lasse, what's this? Why no return?
}

void CPlugInManager::UnloadPlugIn(PlugInHandle Handle, tint32 iChannel, tint32 iInsertIndex)
{
	CAutoLock Lock(gpApplication->GetMeterMutex());
	
	if (Handle == mInvalidHandleValue) {
		throw IException::Create(IException::TypeCode, IException::ReasonCodeInvalidArgument, EXCEPTION_INFO, "Invalid Handle");
	}
	
	std::list<kspi::IPlugIn*>::iterator it = mLoadedPlugIns.begin();
	for (; it != mLoadedPlugIns.end(); it++) {
		if (Handle == (PlugInHandle)(*it)) {
			break;
		}
	}
	mLoadedPlugIns.erase(it);
	
	std::map<tint32, SLoadedPlugInInfo>::iterator it2 = mPlugInMap.find(iChannel << 16 | iInsertIndex);
	
	ASSERT(it2 != mPlugInMap.end());
	
	SLoadedPlugInInfo* pInfo = &(it2->second);
	
	if (pInfo->bGUILoaded) {
		pInfo->pPlugInGUI->CloseWindow();
	}
	
	pInfo->pPlugIn->Stop();
	pInfo->pPlugIn->DeInitialize();
	pInfo->pPlugIn->Destroy();
	
	mPlugInMap.erase(it2);
}

void CPlugInManager::CloseGUI(tint32 iChannel, tint32 iInsertIndex)
{


}
	
void CPlugInManager::OpenGUI(tint32 iChannel, tint32 iInsertIndex)
{
	gpApplication->SetGUIsReady(false);
	
	std::map<tint32, SLoadedPlugInInfo>::iterator it = mPlugInMap.find(iChannel << 16 | iInsertIndex);

//	ASSERT(it != mPlugInMap.end());
	if (it == mPlugInMap.end()) {
		return;
	}

	SLoadedPlugInInfo* pInfo = &(it->second);
	if (pInfo->bGUILoaded) {
		// GUI already exists
	}
	else {
		// GUI doesn't exist
#ifdef _Mac
		CBaseGUI* pBaseGUI = gpMainApplication->CreateExtraWindow(4, (void*)CFSTR("PlugInWnd"));
#else
		CBaseGUI* pBaseGUI = gpMainApplication->CreateExtraWindow(4, (void*)"PlugInWnd", true);
#endif
		CKSPlugInGUI* pPlugInGUI = dynamic_cast<CKSPlugInGUI*>(pBaseGUI);

		kspi::IGUI* pGUI = pInfo->pPlugIn->CreateGUI(0);

		pPlugInGUI->SetInfo(iChannel, iInsertIndex);

		pPlugInGUI->SetGUI(pGUI);

		pInfo->bGUILoaded = true;
		pInfo->pBaseGUI = pBaseGUI;
		pInfo->pPlugInGUI = pPlugInGUI;
		pInfo->pGUI = pGUI;
	}
	
	gpApplication->SetGUIsReady(true);
}

void CPlugInManager::WindowClosed(tint32 iChannel, tint32 iInsertIndex)
{
	std::map<tint32, SLoadedPlugInInfo>::iterator it = mPlugInMap.find(iChannel << 16 | iInsertIndex);

	ASSERT(it != mPlugInMap.end());

//	mPlugInMap.erase(it);
	SLoadedPlugInInfo* pInfo = &(it->second);
	pInfo->bGUILoaded = false;
}

std::string CPlugInManager::GetPlugInVendor(tuint32 iId)
{
	
	tuint32 uiCompanyID = iId >> 8;
	tuint32 uiProductID = iId & 0xff;

	
	std::list<SPlugInInfo*>::const_iterator it = mPlugIns.begin();
	
	for (; it != mPlugIns.end(); it++) {
		
		SPlugInInfo* pInfo = *it;

		if (pInfo->uiCompanyID == uiCompanyID && pInfo->uiProductID == uiProductID) {
			return pInfo->sProductName;
		}
	}
	std::string  s = "NA";
	return s;
}
std::string CPlugInManager::GetPlugInName(tuint32 iId)
{
	
	tuint32 uiCompanyID = iId >> 8;
	tuint32 uiProductID = iId & 0xff ;
	
	
	std::list<SPlugInInfo*>::const_iterator it = mPlugIns.begin();
	
	for (; it != mPlugIns.end(); it++) {
		
		SPlugInInfo* pInfo = *it;
		tint32 iTest = pInfo->uiProductID;
		tint32 iTest2 = pInfo->uiCompanyID;
		if (iTest2 == uiCompanyID && iTest == uiProductID) {
			return pInfo->sCompanyName;
		}
	}
	std::string  s = "NA";
	return s;
}