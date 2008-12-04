

#include "SToolsInternalOS.h"

CDSPTools::CDSPTools()
{
}

CDSPTools::~CDSPTools()
{
}

void CDSPTools::Copy(tfloat32* pDest, const tfloat32* pSrc, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = pSrc[l];
	}
}

void CDSPTools::Copy32(tfloat32* pDest, const tfloat32* pSrc)
{
	Copy(pDest, pSrc, 32);
}

void CDSPTools::Set(tfloat32* pDest, tfloat32 Val, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = Val;
	}
}

void CDSPTools::Set32(tfloat32* pDest, tfloat32 Val)
{
	Set(pDest, Val, 32);
}

void CDSPTools::Clear(tfloat32* pDest, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = 0;
	}
}

void CDSPTools::Clear32(tfloat32* pDest)
{
	Clear(pDest, 32);
}

void CDSPTools::Mul(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = pSrc1[l] * pSrc2[l];
	}
}

void CDSPTools::Mul32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2)
{
	Mul(pDest, pSrc1, pSrc2, 32);
}

void CDSPTools::Mul(tfloat32* pDest, const tfloat32* pSrc, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] *= pSrc[l];
	}
}

void CDSPTools::Mul32(tfloat32* pDest, const tfloat32* pSrc)
{
	Mul(pDest, pSrc, 32);
}

void CDSPTools::Mul(tfloat32* pDest, tfloat32 Src, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] *= Src;
	}
}

void CDSPTools::Mul32(tfloat32* pDest, tfloat32 Src)
{
	Mul(pDest, Src, 32);
}

void CDSPTools::Add(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] = pSrc1[l] + pSrc2[l];
	}
}

void CDSPTools::Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2)
{
	Add(pDest, pSrc1, pSrc2, 32);
}

void CDSPTools::Add(tfloat32* pDest, const tfloat32* pSrc, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] += pSrc[l];
	}
}

void CDSPTools::Add32(tfloat32* pDest, const tfloat32* pSrc)
{
	Add(pDest, pSrc, 32);
}

void CDSPTools::Add(tfloat32* pDest, tfloat32 Src, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] += Src;
	}
}

void CDSPTools::Add32(tfloat32* pDest, tfloat32 Src)
{
	Add(pDest, Src, 32);
}

void CDSPTools::Add(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val, tint lC)
{
	for (long l = 0; l < lC; l++) {
		pDest[l] += pSrc[l] * Val;
	}
}

void CDSPTools::Add32(tfloat32* pDest, const tfloat32* pSrc, tfloat32 Val)
{
	Add(pDest, pSrc, Val, 32);
}




