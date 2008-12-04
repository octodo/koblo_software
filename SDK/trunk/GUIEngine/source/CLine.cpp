/*!	\file CLine.cpp
	\author Michael Olsen
	\date 21/Mar/2005
	\date 21/Mar/2005
*/


#include "geInternalOS.h"


CLine::CLine()
	: mpPrimitives(NULL)
{
}

CLine::~CLine()
{
	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}
}

ILine* ILine::Create()
{
	return dynamic_cast<ILine*>(new CLine());
}

void CLine::Destroy()
{
	delete dynamic_cast<CLine*>(this);
}

void CLine::Init(tint32 iID, const SPos& PosStart, const SPos& PosEnd, const SRGB& Colour, IWindow* pParentWindow)
{
	CControl::Init(iID, NULL);

	mPosStart = PosStart;
	mPosEnd = PosEnd;
	mColour = Colour;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pParentWindow);
}

void CLine::SetLinePos(const SPos& PosStart, const SPos& PosEnd)
{
	mPosStart = PosStart;
	mPosEnd = PosEnd;
}

void CLine::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	mpPrimitives->DrawLine(rUpdate, GetPos() + mPosStart, GetPos() + mPosEnd, mColour);
}

tbool CLine::OnMouse(EMouseMsg /*MouseMsg*/, const SPos& /*Pos*/)
{
	return false;
}

tbool CLine::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CLine::OnTimer()
{
	CControl::OnTimer();
}

void CLine::SetColour(const SRGB& Colour)
{
	mColour = Colour;
}

