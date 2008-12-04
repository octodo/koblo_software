// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.

/*! \class CButton
 * \brief Implementation of IButton
*/
class CButton : public virtual IButton, public virtual CControl, public virtual CPane
{
public:
	//! Constructor
	CButton();
	//! Destructor
	virtual ~CButton();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();

	//! IButton override
	virtual void Init(tint iID, IBitmapResource* pBitmap);
	//! IButton override
	virtual void AddText(const tchar* pszText, IFontResource* pFont, const SSize& Size, const SPos* pPos = NULL, IText::EHorizontalAlignment HorzAlign = IText::HorzAlignCenter);
	//! IButton override
	virtual void ChangeText(const tchar* pszText);
	//! IButton override
	virtual void SetEventMouseUp();
	//! IButton override
	virtual void SetEventMouseUpDown();
	//! IButton override
	virtual void SetLight(tbool bLight);
	//! IButton override
	virtual void ActivateHandCursor();
	//! IMultiStateButton override
	virtual void EnableMouseStateFrames();

protected:
	//! NULL if no text
	IText* mpText;

	//! If true events are send on mouse up, otherwise on mouse down
	tbool mbEventMouseUp;

	//! If true events are send on both mouse up and down
	tbool mbEventMouseUpDown;

	//! Keeps track of whether button was hit on last mouse down
	tbool mbButtonHit;

	//! If true the button lights even if not pressed, if false it has default behaviour (only lights when pressed)
	tbool mbLightAlways;

	//! If true, the cursor is changed into a pointing hand when the mouse is over the control
	tbool mbShowHand;

	//! True if mouse-state frames are enabled. See IButton::EnableMouseStateFrames
	tbool mbMouseStateFrames;

	//! Keeps track of whether mouse has been released after pressed (used by mouse state frames)
	tbool mbMouseReleased;
};
