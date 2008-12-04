

#include "SToolsInternalOS.h"

CDSPToolsOSX::CDSPToolsOSX()
{
}

CDSPToolsOSX::~CDSPToolsOSX()
{
}

IDSPTools* IDSPTools::Create()
{
	return dynamic_cast<IDSPTools*>(new CDSPToolsOSX());
}

void CDSPToolsOSX::Destroy()
{
	delete dynamic_cast<CDSPToolsOSX*>(this);
}

void CDSPToolsOSX::Copy32(tfloat32* pDest, const tfloat32* pSrc)
{
#ifdef _Mac_PowerPC
	vector int Value = vec_ldl(0, (int*)pSrc);
	vec_st(Value, 0, (int*)pDest);
	Value = vec_ldl(4 * 4, (int*)pSrc);
	vec_st(Value, 4 * 4, (int*)pDest);
	Value = vec_ldl(8 * 4, (int*)pSrc);
	vec_st(Value, 8 * 4, (int*)pDest);
	Value = vec_ldl(12 * 4, (int*)pSrc);
	vec_st(Value, 12 * 4, (int*)pDest);
	Value = vec_ldl(16 * 4, (int*)pSrc);
	vec_st(Value, 16 * 4, (int*)pDest);
	Value = vec_ldl(20 * 4, (int*)pSrc);
	vec_st(Value, 20 * 4, (int*)pDest);
	Value = vec_ldl(24 * 4, (int*)pSrc);
	vec_st(Value, 24 * 4, (int*)pDest);
	Value = vec_ldl(28 * 4, (int*)pSrc);
	vec_st(Value, 28 * 4, (int*)pDest);
#else	// _Mac_PowerPC
	CDSPTools::Copy32(pDest, pSrc);
#endif	// _Mac_PowerPC
}

void CDSPToolsOSX::Set32(tfloat32* pDest, tfloat32 Val)
{
	Set(pDest, Val, 32);
}

void CDSPToolsOSX::Clear32(tfloat32* pDest)
{
	Clear(pDest, 32);
}

void CDSPToolsOSX::Mul32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2)
{
	Mul(pDest, pSrc1, pSrc2, 32);
}

void CDSPToolsOSX::Mul32(tfloat32* pDest, const tfloat32* pSrc)
{
	Mul(pDest, pSrc, 32);
}

void CDSPToolsOSX::Mul32(tfloat32* pDest, tfloat32 Src)
{
#ifdef _Mac_PowerPC
	vector float vSrc = (vector float)(Src, Src, Src, Src);

	vector float vDest = vec_ldl(0, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 0, pDest);

	vDest = vec_ldl(4 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 4 * 4, pDest);

	vDest = vec_ldl(8 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 8 * 4, pDest);

	vDest = vec_ldl(12 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 12 * 4, pDest);

	vDest = vec_ldl(16 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 16 * 4, pDest);

	vDest = vec_ldl(20 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 20 * 4, pDest);

	vDest = vec_ldl(24 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 24 * 4, pDest);

	vDest = vec_ldl(28 * 4, pDest);
	vDest = vec_madd(vDest, vSrc, (vector float)(0, 0, 0, 0));
	vec_st(vDest, 28 * 4, pDest);
#else	// _Mac_PowerPC
	CDSPTools::Mul32(pDest, Src);
#endif	// _Mac_PowerPC
}

void CDSPToolsOSX::Add32(tfloat32* pDest, const tfloat32* pSrc1, const tfloat32* pSrc2)
{
	Add(pDest, pSrc1, pSrc2, 32);
}

void CDSPToolsOSX::Add32(tfloat32* pDest, const tfloat32* pSrc)
{
#ifdef _Mac_PowerPC
	vector float vDest = vec_ldl(0, pDest);
	vector float vSrc = vec_ldl(0, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 0, pDest);

	vDest = vec_ldl(4 * 4, pDest);
	vSrc = vec_ldl(4 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 4 * 4, pDest);

	vDest = vec_ldl(8 * 4, pDest);
	vSrc = vec_ldl(8 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 8 * 4, pDest);

	vDest = vec_ldl(12 * 4, pDest);
	vSrc = vec_ldl(12 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 12 * 4, pDest);

	vDest = vec_ldl(16 * 4, pDest);
	vSrc = vec_ldl(16 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 16 * 4, pDest);

	vDest = vec_ldl(20 * 4, pDest);
	vSrc = vec_ldl(20 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 20 * 4, pDest);

	vDest = vec_ldl(24 * 4, pDest);
	vSrc = vec_ldl(24 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 24 * 4, pDest);

	vDest = vec_ldl(28 * 4, pDest);
	vSrc = vec_ldl(28 * 4, pSrc);
	vDest = vec_add(vDest, vSrc);
	vec_st(vDest, 28 * 4, pDest);
#else	// _Mac_PowerPC
	CDSPTools::Add32(pDest, pSrc);
#endif	// _Mac_PowerPC
}

void CDSPToolsOSX::Add32(tfloat32* pDest, tfloat32 Src)
{
	Add(pDest, Src, 32);
}

