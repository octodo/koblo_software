/*!	\file CKnob2.cpp
	\author Michael Olsen
	\date 12/Oct/2004
	\date 12/Oct/2004
*/


#include "geInternalOS.h"


CKnob2::CKnob2()
{
}

CKnob2::~CKnob2()
{
}

IKnob2* IKnob2::Create()
{
	return dynamic_cast<IKnob2*>(new CKnob2());
}

void CKnob2::Destroy()
{
	delete dynamic_cast<CKnob2*>(this);
}

void CKnob2::Init(tint32 iID, IBitmapResource* pBitmap, tint32 iAngleStart, tint32 iAngleEnd)
{
	CKnob::Init(iID, pBitmap);

	pBitmap->GetRealSize(mSizeBitmap);

	miAngleStart = iAngleStart;
	miAngleEnd = iAngleEnd;
}

void CKnob2::OnDraw(const SRect &rUpdate)
{
	if (IsVisible() == false) {
		return;
	}

	tint32 iNrOfXPositions = GetSize().iCX - mSizeBitmap.iCX;
	tint32 iNrOfYPositions = GetSize().iCY - mSizeBitmap.iCY;

	// Calculate angle
	tfloat64 dRange = (tint64)gControlMax - (tint64)gControlMin + 1;
	tfloat64 dIndex = (miValue - gControlMin) / dRange;
//	tfloat64 dPerCircle = dIndex * 360;
	tint32 iAngleRange = miAngleEnd - miAngleStart + 1;
//	double dAngle = miAngleStart + (iAngleRange * dPerCircle / 360);
	double dAngle = iAngleRange * dIndex + miAngleStart;
	dAngle += 90;
	while (dAngle >= 360) {
		dAngle -= 360;
	}
	while (dAngle < 0) {
		dAngle += 360;
	}
	int iAngle = (int)dAngle;

	// Calculate position
	double dRadian = iAngle * (2 * 3.14) / 360;
	double dX = cos(dRadian);
	dX = (dX + 1) / 2;
//	dRadian -= 3.14;
//	if (dRadian < 0) {
//		dRadian += (2 * 3.14);
//	}
	double dY = sin(dRadian);
	dY = (dY + 1) / 2;
	SPos Pos;
	Pos.iX = (int)(dX * iNrOfXPositions + 0.5);
	Pos.iY = (int)(dY * iNrOfYPositions + 0.5);

	DrawBitmap(GetBitmapResource(), rUpdate, SRect(GetPos() + Pos, mSizeBitmap));
}



