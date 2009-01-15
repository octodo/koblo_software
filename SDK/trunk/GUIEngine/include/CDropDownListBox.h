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


/*! \class CDropDownListBox
 * \brief Implementation of IDropDownListBox
*/
class CDropDownListBox : public virtual IDropDownListBox, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CDropDownListBox();
	//! Destructor
	virtual ~CDropDownListBox();

	//! IDestructable override
	virtual void Destroy();
 
	//! IDropDownListBox override
	virtual void Init(tint32 iID);
	//! IDropDownListBox override
	virtual void DontSetText();
	//! IDropDownListBox override
	virtual IPopupMenu* GetPopup();


	//! CPane override
	void AddControl(IControl* pToAdd, const SPos& Pos);
	//! CPane override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! CPane override
	virtual void ControlUpdateAll();

	//! CControl override
	void SetValue(tint32 iNew, tbool bNotify);
	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);
	//! CEventReceiver override
	void EventGeneric(IControl* pSender, void* pEventData);

protected:
	//! Indicates whether the popup menu currently is active (visible)
	tbool mbPopUpActive;

	//! Positions and shows popup
	void ShowPopUp();

	//! If false, button will not be updated with text
	tbool mbSetText;

};

