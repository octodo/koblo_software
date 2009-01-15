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


//#ifndef CMultiStateDropDownListBox_H
//#define CMultiStateDropDownListBox_H


/*! \class CMultiStateDropDownListBox
 * \brief Implementation of IMultiStateDropDownListBox
*/
class CMultiStateDropDownListBox : public virtual IMultiStateDropDownListBox, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CMultiStateDropDownListBox();
	//! Destructor
	virtual ~CMultiStateDropDownListBox();

	//! IDestructable override
	virtual void Destroy();
 
	//! IMultiStateDropDownListBox override
	virtual void Init(tint32 iID, EMouseMsg msgTrigger = RightButtonUp, tbool bPopupAtClickPos = true);

	//! IMultiStateDropDownListBox override
	virtual void HidePopup();

	//! CPane override
	void AddControl(IControl* pToAdd, const SPos& Pos);
	//! CPane override
	virtual void ControlUpdateAll();
	//! CPane override
	virtual void SetParentPane(IPane* pPane);

	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);
	//! CEventReceiver override
	virtual void EventGeneric(IControl *pSender, void *pEventData);



protected:

	//! IMultiStateDropDownListBox override
	virtual void ShowPopup(SPos& posAbsClicked);

	//! True = pop-up is showing, False = invisible.
	tbool mbPopUpActive;

	//! The mouse event that will trigger a "show popup" operation
	EMouseMsg meMouseMsgTrigger;

	//! True = the pop-up will show at the position of the click, False = it won't overlap the multi-state button.
	tbool mbPopupAtClickPos;
};


//#endif // CMultiStateDropDownListBox_H