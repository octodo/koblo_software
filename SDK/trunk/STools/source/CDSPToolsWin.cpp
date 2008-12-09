

#include "SToolsInternalOS.h"

/*
	CDSPToolsWin
*/

CDSPToolsWin::CDSPToolsWin()
{
}

CDSPToolsWin::~CDSPToolsWin()
{
}

IDSPTools* IDSPTools::Create()
{
	CAutoDelete<ISystem> pSystem(ISystem::Create());

	tint32 iFeatures = pSystem->GetCPUFeatures();

	if (iFeatures & CPUFEATURES_SSE) {
		return dynamic_cast<IDSPTools*>(new CDSPToolsWinSSE());
	}
	else {
		return dynamic_cast<IDSPTools*>(new CDSPToolsWin());
	}
}

void CDSPToolsWin::Destroy()
{
	delete dynamic_cast<CDSPToolsWin*>(this);
}

/*
	CDSPToolsWinSSE
*/

CDSPToolsWinSSE::CDSPToolsWinSSE()
{
}

CDSPToolsWinSSE::~CDSPToolsWinSSE()
{
}

void CDSPToolsWinSSE::Destroy()
{
	delete dynamic_cast<CDSPToolsWinSSE*>(this);
}

void CDSPToolsWinSSE::Add32(tfloat32* pDest, const tfloat32* pSrc)
{
	CDSPTools::Add32(pDest, pSrc);
/*	__asm {
		mov esi, dword ptr[pDest];
		mov xmm0, ptr[esi];
	}*/
}

void CDSPToolsWinSSE::Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2)
{
	CDSPTools::Add32(pDest, pSrc1, pSrc2);
}

void CDSPToolsWinSSE::Mul32(tfloat32* pDest, const tfloat32* pSrc)
{
	CDSPTools::Mul32(pDest, pSrc);
}

void CDSPToolsWinSSE::Clear32(tfloat32* pDest)
{
	CDSPTools::Clear32(pDest);
}

void CDSPToolsWinSSE::Copy32(tfloat32* pDest, const tfloat32* pSrc)
{
	CDSPTools::Copy32(pDest, pSrc);
}






