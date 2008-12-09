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



/*! \class CDialog
 * \brief Implementation of IDialog
*/
class CDialog : public virtual IDialog, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CDialog();
	//! Destructor
	virtual ~CDialog();

	//! IDestructable override
	virtual void Destroy();
 
	//! IDialog override
	virtual void Init(tint32 iID);
	//! IDialog override
	virtual void SetCallback(IDialogCallback* pCallback);
	//! IDialog override
	virtual void AddControl(IControl* pToAdd, const SPos& Pos, EButtonType ButtonType = ButtonTypeNormal);
	//! IDialog override
	virtual void Show();
	//! IDialog override
	virtual void Hide(tint32 iID);
	//! IDialog override
	virtual IDialogCallback* GetCallback();
	//! IDialog override
	virtual const IDialogCallback* GetCallback() const;

	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

protected:
	//! Callback to call
	IDialogCallback* mpCallback;

	//! List of IDs that can close the dialog
	std::list<tint32> miCloseIDs;
};

