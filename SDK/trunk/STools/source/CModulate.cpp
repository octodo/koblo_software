

#include "SToolsInternalOS.h"

IModulate* IModulate::Create()
{
	return dynamic_cast<IModulate*>(new CModulate());
}

void CModulate::Destroy()
{
	delete dynamic_cast<CModulate*>(this);
}

CModulate::CModulate()
{
}

CModulate::~CModulate()
{
}

void CModulate::SetMod(tfloat32* pDest, const tfloat32* pSrc, tfloat fModAmount, tfloat fFloor, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = pSrc[l]*fModAmount+fFloor;
	}
}

void CModulate::AddMod(tfloat32* pDest, const tfloat32* pSrc, tfloat fModAmount, tfloat fFloor, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] *= pSrc[l]*fModAmount+fFloor;
	}
}
