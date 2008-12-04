

#include "KVerbOS.h"

CKVerbModule::CKVerbModule(kspi::IHost* pHost)
	: CBaseModule(pHost)
{
}

CKVerbModule::~CKVerbModule()
{
}

void CKVerbModule::Destroy()
{
	delete dynamic_cast<CKVerbModule*>(this);
}

kspi::IPlugIn* CKVerbModule::CreateProcess(tuint32 iIndex)
{
	return new CKVerbPlugIn(this, iIndex);
}

tint32 CKVerbModule::GetCompanyID()
{
	return 1;
}

void CKVerbModule::GetCompanyName(tchar* pszName)
{
	strcpy(pszName, "Koblo");
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

	return new CKVerbModule(pHost);
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

