

#include "VChorusOS.h"

CVChorusModule::CVChorusModule(kspi::IHost* pHost)
	: CBaseModule(pHost)
{
}

CVChorusModule::~CVChorusModule()
{
}

void CVChorusModule::Destroy()
{
	delete dynamic_cast<CVChorusModule*>(this);
}

kspi::IPlugIn* CVChorusModule::CreateProcess(tuint32 iIndex)
{
	return new CVChorusPlugIn(this, iIndex);
}

tint32 CVChorusModule::GetCompanyID()
{
	return 1;
}

void CVChorusModule::GetCompanyName(tchar* pszName)
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

	return new CVChorusModule(pHost);
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





