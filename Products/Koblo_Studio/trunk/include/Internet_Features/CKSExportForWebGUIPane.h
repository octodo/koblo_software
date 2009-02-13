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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.



class CDSP;

class CKSExportForWebGUIPane : public virtual CBasePane
{
public:
	CKSExportForWebGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSExportForWebGUIPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}
	
	void OnTimer(tint32 iTimerID);

	virtual void UpdateGUIData(tint32 iID, tint32 iValue);

	virtual void ConnectControls();

	//! CBasePane override
	virtual void Resize(ge::SSize SizeNew);

	//! CBasePane override
	virtual void SendMsgFromTop(SMsg* pMsg);
	//! CBasePane override
	virtual void HandleMsg(SMsg* pMsg);

protected:

	
	tbool mbIsVisible;
	
//	ge::IPopupMenu::SMenuItemList mpCompression_Popup;
	
	virtual void Prepare_Popups();
	
	virtual ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane);

	virtual void ShowSorryDialog(std::string sViolation);

	ge::IBitmap*	mpLicense;
	

	ge::IURL* mpBY_URL;
	ge::IURL* mpBY_SA_URL;
	ge::IURL* mpBY_ND_URL;
	ge::IURL* mpBY_NC_URL;
	ge::IURL* mpBY_NC_SA_URL;
	ge::IURL* mpBY_NC_ND_URL;
	
	ge::IURL* mpCC_URL;

	ge::IRadioButtons* mpSetup_Radio_Button;
	tint32 miLicenseNb;
	ac::EQuality meQuality;
//	ge::IText* mpTextAuthor;
//	ge::IText* mpTextUrl;
//	ge::IText* mpTextStatus;

	tbool mbHasPreviousLicense;
	tbool mbMayBeCommercial;
	tbool mbMayChangeLicense;
	tbool mbMayExport;
};

