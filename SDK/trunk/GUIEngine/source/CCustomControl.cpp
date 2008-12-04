/*!	\file CCustomControl.cpp
	\author Michael Olsen
	\date 18/Mar/2005
	\date 18/Mar/2005
*/


#include "geInternalOS.h"


CCustomControl::CCustomControl()
	: mpCallback(NULL)
{
}

CCustomControl::~CCustomControl()
{
}

ICustomControl* ICustomControl::Create()
{
	return dynamic_cast<ICustomControl*>(new CCustomControl());
}

void CCustomControl::Destroy()
{
	delete dynamic_cast<CCustomControl*>(this);
}

void CCustomControl::Init(tint32 iID, ICustomControlCallback* pCallback, IBitmapResource* pBitmap)
{
	CControl::Init(iID, pBitmap);

	mpCallback = pCallback;
}

void CCustomControl::SetValue(tint32 iNew, tbool bNotify)
{
	CControl::SetValue(iNew, bNotify);

	mpCallback->OnValueChange();
}

void CCustomControl::OnDraw(const SRect &rUpdate)
{
	if (!IsVisible()) {
		return;
	}

	mpCallback->OnDraw(rUpdate);
}

tbool CCustomControl::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (!IsVisible()) {
		return false;
	}

	return mpCallback->OnMouse(MouseMsg, Pos);
}

tbool CCustomControl::OnKeyDown(EKey Key)
{
	if (!IsVisible()) {
		return false;
	}

	return mpCallback->OnKeyDown(Key);
}

void CCustomControl::OnTimer()
{
	CControl::OnTimer();

	if (!IsVisible()) {
		return;
	}

	mpCallback->OnTimer();
}

void CCustomControl::SetSize(const SSize& Size)
{
	CControl::SetSize(Size);

	mpCallback->SetSize(Size);
}

void CCustomControl::SetPos(const SPos& Pos)
{
	CControl::SetPos(Pos);

	mpCallback->SetPos(Pos);
}


