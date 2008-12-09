/*!	\file CURL.cpp
	\author Michael Olsen
	\date 19/Oct/2005
	\date 19/Oct/2005
*/


#include "geInternalOS.h"


CURL::CURL()
	: mbShowHand(false)
{
}

CURL::~CURL()
{
}

IURL* IURL::Create()
{
	return dynamic_cast<IURL*>(new CURL());
}

void CURL::Destroy()
{
	delete dynamic_cast<CURL*>(this);
}

void CURL::Init(tint32 iID, const tchar* pszURL)
{
	CControl::Init(iID, NULL);

	msURL = std::string((const char*)pszURL);
}

void CURL::ActivateHandCursor()
{
	mbShowHand = true;
}

void CURL::OnDraw(const SRect &rUpdate)
{
	// Control is invisible
}

tbool CURL::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (!IsVisible()) {
		return false;
	}

	if (mbShowHand) {
		if (GetRect().Inside(Pos)) {
			GetParentWindow()->SetMouseCursor(IWindow::CursorHand);
		}
	}

	if (MouseMsg == LeftButtonDown) {
		if (GetRect().Inside(Pos)) {
			OpenBrowser((const tchar*)(msURL.c_str()));

			return true;
		}
	}

	return false;
}

tbool CURL::OnKeyDown(EKey /*Key*/)
{
	return false;
}


