

#include "KSOS.h"

CKSModule::CKSModule(kspi::IHost* pHost)
	: CBaseModule(pHost)
{
}

CKSModule::~CKSModule()
{
}

void CKSModule::Destroy()
{
	delete dynamic_cast<CKSModule*>(this);
}

kspi::IPlugIn* CKSModule::CreateProcess(tuint32 iIndex)
{
	return new CKSPlugIn(this, iIndex);
}

#ifdef WIN32
extern "C" __declspec(dllexport) kspi::IModule* kspiMain(kspi::IHost* pHost, tuint32* piVersionNumber)
#endif	// WIN32
#ifdef _Mac
extern "C" kspi::IModule* kspiMain(kspi::IHost* pHost, k2s::tuint32* piVersionNumber)
#endif	// _Mac
{
/*	*piVersionNumber = kspi::giCurrentRevision;

	return new CKSModule(pHost);*/
	return NULL;
}




