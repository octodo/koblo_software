/*!	\file CBitmap.cpp
	\author Michael Olsen
	\date 30/Dec/2004
	\date 30/Dec/2004
*/


#include "geInternalOS.h"


CBitmap::CBitmap()
	: miFrameCur(0), miFrames(0)
{
}

CBitmap::~CBitmap()
{
}

IBitmap* IBitmap::Create()
{
	return dynamic_cast<IBitmap*>(new CBitmap());
}

void CBitmap::Destroy()
{
	delete dynamic_cast<CBitmap*>(this);
}

void CBitmap::Init(tint32 iID, IBitmapResource* pBitmap, tint iFrames)
{
	CControl::Init(iID, pBitmap);

	pBitmap->GetRealSize(mSize);
	mSizeBitmap = mSize;

	miFrames = iFrames;
}

void CBitmap::SetFrame(tint iFrame)
{
	miFrameCur = iFrame;

	Redraw();
}

void CBitmap::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	if (mSize == mSizeBitmap) {
		// Size of control equals size of bitmap
		DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), mSizeBitmap),
			miFrameCur, miFrames);
	}
	else {
		if (mSize.iCX < mSizeBitmap.iCX) {
			DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), mSize),
				miFrameCur, miFrames);
		}
		else if (mSize.iCY < mSizeBitmap.iCY) {
			DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), mSize),
				miFrameCur, miFrames);
		}
		else {
			SPos PosThis;
			GetPos(PosThis);
			// Size of control doesn't equal size of bitmap, so tiling is needed
			tint32 iYPos = rUpdate.iY - PosThis.iY - mSizeBitmap.iCY;
			if (iYPos < 0) {
				iYPos = 0;
			}
			while ((iYPos < mSize.iCY) &&
				(PosThis.iY + iYPos < rUpdate.iY + rUpdate.iCY)) {
				tint32 iXPos = rUpdate.iX - PosThis.iX - mSizeBitmap.iCX;
				if (iXPos < 0) {
					iXPos = 0;
				}
				while ((iXPos < mSize.iCX) &&
					(PosThis.iX + iXPos < rUpdate.iX + rUpdate.iCX)) {
					DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(PosThis + SPos(iXPos, iYPos), mSizeBitmap),
						miFrameCur, miFrames);

						iXPos += mSizeBitmap.iCX;
				}

				iYPos += mSizeBitmap.iCY;
			}
		}
	}
}

tbool CBitmap::OnMouse(EMouseMsg /*MouseMsg*/, const SPos& /*Pos*/)
{
	return false;
}

tbool CBitmap::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CBitmap::OnTimer()
{
	CControl::OnTimer();
}

