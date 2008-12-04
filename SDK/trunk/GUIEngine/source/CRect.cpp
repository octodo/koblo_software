/*!	\file CRect.cpp
	\author Michael Olsen
	\date 14/Jun/2005
	\date 14/Jun/2005
*/


#include "geInternalOS.h"


CRect::CRect()
	: mpPrimitives(NULL)
{
}

CRect::~CRect()
{
	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}
}

IRect* IRect::Create()
{
	return dynamic_cast<IRect*>(new CRect());
}

void CRect::Destroy()
{
	delete dynamic_cast<CRect*>(this);
}

void CRect::Init(tint32 iID, const SRect& Rect, const SRGB& Colour, IWindow* pParentWindow)
{
	CControl::Init(iID, NULL);

	mRect = Rect;
	mColour = Colour;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pParentWindow);
}

void CRect::SetRect(const SRect& Rect)
{
	mRect = Rect;
}

void CRect::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	SRect Rect(mRect);
	Rect += GetPos();
	mpPrimitives->DrawRect(rUpdate, Rect, mColour);
}

tbool CRect::OnMouse(EMouseMsg /*MouseMsg*/, const SPos& /*Pos*/)
{
	return false;
}

tbool CRect::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CRect::OnTimer()
{
	CControl::OnTimer();
}

