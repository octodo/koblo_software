
#include "KSOS.h"

CEffectControl::CEffectControl(ge::ICustomControl* pControl, kspi::IGUI* pPlugInGUI)
	: mpControl(pControl), mpPlugInGUI(pPlugInGUI), mbMouseCaptured(false)
{
}

CEffectControl::~CEffectControl()
{
}

void CEffectControl::OnDraw(const ge::SRect &rUpdate)
{
	// Get the buffer to draw into
	void* pDrawBuffer;
	mpControl->GetParentWindow()->GetDrawPointer(&pDrawBuffer);

	// Calculate the interleave from the window width
	ge::SSize SizeWnd;
	mpControl->GetParentWindow()->GetSize(SizeWnd);
	tuint32 uiInterleave = SizeWnd.iCX * 4;

	// Get our position, so we can calculate offset
	ge::SPos PosThis;
	mpControl->GetPos(PosThis);
	tuint32* pDrawBuffer2 = (tuint32*)pDrawBuffer;
	pDrawBuffer2 += PosThis.iY * SizeWnd.iCX;
	pDrawBuffer2 += PosThis.iX;

	// Create an update rect for the plugin, by adjusting the update rect for our position
	ge::SRect UpdateRect = rUpdate;
	UpdateRect -= PosThis;
	// Now adjust the update rect for the proper size
	ge::SSize SizeThis;
	mpControl->GetSize(SizeThis);
	ge::SRect RectThisOffset0(ge::SPos(0, 0), SizeThis);
	// Check if we need to draw at all
	if (UpdateRect.Inside(RectThisOffset0)) {
		// Fit it inside our size
		UpdateRect.FitInside(RectThisOffset0);

		mpPlugInGUI->Paint();
		mpPlugInGUI->OnDraw((tuchar*)pDrawBuffer2, uiInterleave, UpdateRect.iX, UpdateRect.iY, UpdateRect.iCX, UpdateRect.iCY);
	}
}

tbool CEffectControl::OnMouse(ge::EMouseMsg MouseMsg, const ge::SPos& Pos)
{
	if (mpControl->IsVisible() == false) {
		return false;
	}

	ge::SRect rctThis;
	mpControl->GetRect(rctThis);
	tbool bInsideControl = false;
	if (rctThis.Inside(Pos)) {
		bInsideControl = true;
	}

	// Modify the position, so it matches the position the effect knows, i.e. top corner is 0,0
	ge::SPos PosRelative = Pos;
	PosRelative -= (ge::SPos)rctThis;

	switch(MouseMsg) {
		case ge::LeftButtonDown:
			if (bInsideControl) {
				// We react and capture mouse, if it's inside the control
				mpPlugInGUI->OnMouseLeftButtonDown(PosRelative.iX, PosRelative.iY);
				mpControl->GetParentWindow()->GetMouseFocus(mpControl);
				mbMouseCaptured = true;
				return true;
			}
			break;
		case ge::LeftButtonUp:
			if (mbMouseCaptured) {
				// We had the capture, so react and release it
				mpPlugInGUI->OnMouseLeftButtonUp(PosRelative.iX, PosRelative.iY);
				mpControl->GetParentWindow()->ReleaseMouseFocus();
				mbMouseCaptured = false;
				return true;
			}
			if (bInsideControl) {
				// We react if it's inside the control
				mpPlugInGUI->OnMouseLeftButtonUp(PosRelative.iX, PosRelative.iY);
				return true;
			}
			break;
		case ge::RightButtonDown:
			if (bInsideControl) {
				// We react and capture mouse, if it's inside the control
				mpPlugInGUI->OnMouseRightButtonDown(PosRelative.iX, PosRelative.iY);
				mpControl->GetParentWindow()->GetMouseFocus(mpControl);
				mbMouseCaptured = true;
				return true;
			}
			break;
		case ge::RightButtonUp:
			if (mbMouseCaptured) {
				// We had the capture, so react and release it
				mpPlugInGUI->OnMouseRightButtonUp(PosRelative.iX, PosRelative.iY);
				mpControl->GetParentWindow()->ReleaseMouseFocus();
				mbMouseCaptured = false;
				return true;
			}
			if (bInsideControl) {
				// We react if it's inside the control
				mpPlugInGUI->OnMouseRightButtonUp(PosRelative.iX, PosRelative.iY);
				return true;
			}
			break;
		case ge::LeftButtonDblClk:
			if (bInsideControl) {
				// We react if it's inside the control
				mpPlugInGUI->OnMouseLeftButtonDoubleClick(PosRelative.iX, PosRelative.iY);
				return true;
			}
			break;
		case ge::MouseMove:
			if (mbMouseCaptured) {
				// We have the capture, so react
				mpPlugInGUI->OnMouseMove(PosRelative.iX, PosRelative.iY);
				return true;
			}
			if (bInsideControl) {
				// We react if it's inside the control
				mpPlugInGUI->OnMouseMove(PosRelative.iX, PosRelative.iY);
				return true;
			}
			break;
	}

	return false;
}

tbool CEffectControl::OnKeyDown(ge::EKey Key)
{
	tint32 iKey = -1;

	if ((Key >= ge::Keya) && (Key <= ge::Keyz)) {
		iKey = (tint32)('a' + (Key - ge::Keya));
	}
	else if ((Key >= ge::KeyA) && (Key <= ge::KeyZ)) {
		iKey = (tint32)('A' + (Key - ge::KeyA));
	}
	else if ((Key >= ge::Key0) && (Key <= ge::Key9)) {
		iKey = (tint32)('0' + (Key - ge::Key0));
	}
	else if (Key == ge::KeySpace) {
		iKey = ' ';
	}
	else if (Key == ge::KeyDot) {
		iKey = '.';
	}
	else if (Key == ge::KeyComma) {
		iKey = ',';
	}
	else if (Key == ge::KeyMinus) {
		iKey = '-';
	}
	else if (Key == ge::KeyPlus) {
		iKey = '+';
	}
	else if (Key == ge::KeyDelete) {
		iKey = 46;
	}
	else if (Key == ge::KeyBackSpace) {
		iKey = 8;
	}
	else if (Key == ge::KeyLeft) {
		iKey = 37;
	}
	else if (Key == ge::KeyRight) {
		iKey = 39;
	}
	else if (Key == ge::KeyUp) {
		iKey = 38;
	}
	else if (Key == ge::KeyDown) {
		iKey = 40;
	}
	else if (Key == ge::KeyReturn) {
		iKey = 13;
	}
	else if (Key == ge::KeyEscape) {
		iKey = 7;
	}

	if (iKey != -1) {
		mpPlugInGUI->OnKeyDown(iKey);
	}

	return false;
}

void CEffectControl::OnTimer()
{
	mpPlugInGUI->OnTimer();
}

void CEffectControl::OnValueChange()
{
}

void CEffectControl::SetSize(const ge::SSize& Size)
{
}

void CEffectControl::SetPos(const ge::SPos& Pos)
{
}

void CEffectControl::Init()
{
	mpPlugInGUI->PrepareBuffer();
}

