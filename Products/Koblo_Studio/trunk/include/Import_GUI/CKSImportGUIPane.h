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

const tint32 kiNbOfBuffsInChain = 64;

class CKSImportGUIPane : public virtual CBasePane, public virtual CPreviewCallback, public virtual ITimerCallback
{
public:
	CKSImportGUIPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	virtual ~CKSImportGUIPane();

	virtual void Init();

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl* pControl, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl* pControl, void *pEventData) {}

	//! CPreviewCallback override
	void Accumulate(tfloat32** ppfBuffer, tint32 iSamples);

	virtual void OnTimer(tint32 iTimerID);

	virtual void UpdateGUIData(tint32 iID, tint32 iValue);

	virtual void ConnectControls();

	//! CBasePane override
//	virtual void Resize(ge::SSize SizeNew);

	//! CBasePane override
	virtual void SendMsgFromTop(SMsg* pMsg);

	virtual void SetInfo(ac::EQuality eQuality, tint32 iSampleRate, tint32 iChannels);

	virtual void SetType(tint32 iType);

	virtual void BrowseToDir(const std::string& sPath);

	void PreviewStop();

	virtual tbool AddFile(std::string sPathName);

protected:
	ge::IDropDownListBox* mpPop;

	// Popup used to navigate the os file system
	ge::IPopupMenu::SMenuItemList mpFile_Browser_Popup;
	virtual void Prepare_Popups();
	virtual ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos, ge::IPane* pPane);
	

	// Scroll pane showing files and folders
	CKS_Import_File_Browser* mpImport_File_Browser;
	ge::IScrollBar* mpScrollBarFileBrowser;
	virtual void Create_File_Browser_ScrollBar();
	
	
	// Scroll pane showing audio files to add to project
	CKS_Import_Files* mpImport_Files;
	ge::IScrollBar* mpScrollBarFiles;
	virtual void Create_File_ScrollBar();

	std::string msPathNameCur;

	volatile tbool mbPreviewing;
	tint32 miPreview_BuffSampleIx;
	tint32 miPreview_FileSampleIx;

	//IFileMemory* mpFilePreview;

	IFile* mpfPreview_Input;
	ac::IDecoder* mpdecPreview;
	IFileMemory* mapfPreview_OutputL[kiNbOfBuffsInChain];
	IFileMemory* mapfPreview_OutputR[kiNbOfBuffsInChain];

	volatile tint32 miChainHead;
	volatile tint32 miChainTail;
	tint32 GetPlayingChainIx()
	{ return (miChainHead == miChainTail) ? -1 : miChainTail; };
	tint32 GetFillingChainIx()
	{
		tint32 iNext = (miChainHead + 1) % kiNbOfBuffsInChain;
		return (iNext == miChainTail) ? -1 : iNext;
	};
	void FillChainPart();
	ITimer* mpTimer;
	volatile tint32 miInFillChain_Level;

	tbool PreviewStart();
	void PreviewStop_NoLock();

	tfloat mfPreviewLastSampleL;
	tfloat mfPreviewLastSampleR;

	CMutex mMutex;

};

