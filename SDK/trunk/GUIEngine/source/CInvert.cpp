/*!	\file CInvert.cpp
	\author Michael Olsen
	\date 05/May/2008
	\date 05/May/2008
*/


#include "geInternalOS.h"


CInvert::CInvert()
	: mpPrimitives(NULL)
{
}

CInvert::~CInvert()
{
	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}
}

IInvert* IInvert::Create()
{
	return dynamic_cast<IInvert*>(new CInvert());
}

void CInvert::Destroy()
{
	delete dynamic_cast<CInvert*>(this);
}

void CInvert::Init(tint32 iID, const SRect& Rect, IWindow* pParentWindow)
{
	CControl::Init(iID, NULL);

	mRect = Rect;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pParentWindow);
}

void CInvert::SetRect(const SRect& Rect)
{
	mRect = Rect;
}

void CInvert::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	SRect Rect(mRect);
	Rect += GetPos();
	mpPrimitives->InvertRect(rUpdate, Rect);
}

tbool CInvert::OnMouse(EMouseMsg /*MouseMsg*/, const SPos& /*Pos*/)
{
	return false;
}

tbool CInvert::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CInvert::OnTimer()
{
	CControl::OnTimer();
}

