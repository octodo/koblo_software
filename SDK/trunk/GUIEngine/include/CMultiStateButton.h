// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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


/*! \class CMultiStateButton
 * \brief Implementation of IMultiStateButton
*/
class CMultiStateButton : public virtual IMultiStateButton, public virtual CControl, public virtual CPane
{
public:
	//! Constructor
	CMultiStateButton();
	//! Destructor
	virtual ~CMultiStateButton();

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

	//! IMultiStateButton override
	virtual void Init(tint iID, IBitmapResource* pBitmap, tint iNrOfStates);
	//! IMultiStateButton override
	virtual void AddText(tint iStateValue, const tchar* pszText, IFontResource* pFont);
	//! IMultiStateButton override
	virtual void SetEventMouseUp();
	//! IMultiStateButton override
	virtual void EnableMouseStateFrames();
	//! IMultiStateButton implementation
	virtual void Cycle(tbool bCycle = true, tint32 iCycleIntervalMs = 250);
	//! IMultiStateButton implementation
	virtual tbool IsCycling();

	//! IMultiStateButton override
	virtual void Invert();

protected:
	tint miNrOfStates;

	struct SStateInfo {
		// NULL if no text
		IText* pText;
	};
	SStateInfo* mpStateInfos;

	//! If true events are send on mouse up, otherwise on mouse down
	tbool mbEventMouseUp;

	//! Keeps track of whether button was hit on last mouse down
	tbool mbButtonHit;

	tbool mbInverted;

	//! True if mouse-state frames are enabled. See IMultiStateButton::EnableMouseStateFrames
	tbool mbMouseStateFrames;

	//! True if button is cycling (button face frame changing automatically). See IMultiStateButton::Cycle
	tbool mbCycling;
	//! Currently visible button face (when cycling)
	tint32 miCyclingFrameCurrent;
	//! The amount of time (in milli-seconds) that each frame will show. See IMultiStateButton::Cycle
	tint32 miCycleIntervalMs;
	//! Cycling, time of last cycling index 0
	tint32 miCycleLastIx0Ms;

	//! Keeps track of whether mouse has been released after pressed (used by mouse state frames)
	tbool mbMouseReleased;

	//! State
	tint mpState;
};
