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


class CKSUsername_And_Password_Pane : public virtual CBasePane
{
public:
	CKSUsername_And_Password_Pane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSUsername_And_Password_Pane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}
	
	void OnTimer(tint32 iTimerID);

	virtual void UpdateGUIData(tint32 iID, tint32 iValue);

	virtual void ConnectControls();

	//! CBasePane override
//	virtual void Resize(ge::SSize SizeNew);

	//! CBasePane override
	virtual void SendMsgFromTop(SMsg* pMsg);
	

protected:
	CKSPlugIn* mpKSPlugIn;
	
	ge::IMultiStateButton* mpRemember_Me_Button;
	
	ge::IText* mpUser_Name_Txt;
	
	ge::IText* mpPassword_Txt;



	
};

