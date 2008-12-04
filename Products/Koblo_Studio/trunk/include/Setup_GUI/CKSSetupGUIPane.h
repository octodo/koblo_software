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



class CKSDSP;
class CKS_Import_File_Browser;
class CKS_Import_Files;

class CKSSetupGUIPane : public virtual CBasePane
{
public:
	CKSSetupGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSSetupGUIPane();

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
	
	//! Get auctual audio device settings fro DSP engine and put the values into the settings popups
	virtual void InitAudioDevicePopupValues();

	virtual void InitAudioDevicePopupValuesInput();

protected:
	tbool mbMenuInited;

	// Popup used to navigate the os file system
	ge::IPopupMenu::SMenuItemList mpFile_Browser_Popup;
	virtual void Prepare_Popups();
	
//	ge::IPopupMenu::SMenuItemList mpCompression_Setup;
	
	ge::IBitmap* mpDimm_Bmp;
	
//	ge::IPopupMenu::SMenuItemList mpCompression_Setup;
	
	virtual ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane);

	//! For listing / selecting audio device
	aioe::IDeviceManager* mpAudioManager;

	ge::IDropDownListBox* mpPopupMenuAudioIn;
	ge::IDropDownListBox* mpPopupMenuAudioDevices;
	ge::IDropDownListBox* mpPopupMenuAudioRates;
	ge::IDropDownListBox* mpPopupMenuAudioBuffs;
	
	ge::IButton* mpASIO_Button;
	volatile tint32 miBlockSetAudioDevice_volatile;
	
	void SetupDevice();
	void SetupDeviceAsio();
	void SetupIO();
	

	ge::IBitmap*  mpDevice_Asio_BMP;
	ge::IBitmap*  mpDevice_BMP;

	void ThatDidntWork();

	
};

