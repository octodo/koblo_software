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



/*! \class CRadioButtons
 * \brief Implementation of IRadioButtons
*/
class CRadioButtons : public virtual IRadioButtons, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CRadioButtons();
	//! Destructor
	virtual ~CRadioButtons();

	//! IDestructable override
	virtual void Destroy();
 
	//! IRadioButtons override
	virtual void Init(tint32 iID);

	//! CPane override
	void AddControl(IControl* pToAdd, const SPos& Pos);
	//! CPane override
	virtual void ControlUpdateAll();
	//! CPane override
	virtual void SetParentPane(IPane* pPane);

	//! CControl override
	void SetValue(tint32 iNew, tbool bNotify);
	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

protected:
};

