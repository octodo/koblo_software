/*!	\file CLED.cpp
	\author Michael Olsen
	\date 21/Mar/2005
	\date 21/Mar/2005
*/


#include "geInternalOS.h"


CLED::CLED()
	: mpPrimitives(NULL)
{
}

CLED::~CLED()
{
	if (mpPrimitives) {
		mpPrimitives->Destroy();
	}
}

ILED* ILED::Create()
{
	return dynamic_cast<ILED*>(new CLED());
}

void CLED::Destroy()
{
	delete dynamic_cast<CLED*>(this);
}

void CLED::Init(tint32 iID, tint32 iMin, tint32 iMax, SSize Size,IWindow* pParentWindow, SRGB RGB)
{
	CControl::Init(iID, NULL);

	miMin		=	iMin;
	miMax		=	iMax;
	miCurrent	=	iMin;
	mColor		=	RGB;

	mpPrimitives = IDrawPrimitives::Create();
	mpPrimitives->Init(pParentWindow);
}

void CLED::Set(tint32 iValue)
{
	if(miCurrent !=	iValue) {

		miCurrent	=	iValue;
		Redraw();
	}
}

void CLED::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;


	tfloat fDrawSize = (miCurrent - miMin) / (float)(miMax - miMin);
	
	int iPixelsToDraw = (int)(GetSize().iCY * fDrawSize);

	// Possion to draw from
	SPos Pos(GetPos() + SPos(0, GetSize().iCY - iPixelsToDraw));

	// Size to draw
	SSize Size(GetSize().iCX, iPixelsToDraw);

	// Set rect to draw
	SRect Rect(Pos, Size);

	// Draw
	mpPrimitives->DrawRect(rUpdate, Rect, mColor);
}


void CLED::OnTimer()
{
	CControl::OnTimer();
}

