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


/*! \class CBaseTopPane
 * \brief ...
*/
class CBaseTopPane : public virtual CBasePane
{
public:
	//! Constructor
	/*!
		\param pGUI [in]: The GUI
		\param pPane [in]: The pane it resides in
	*/
	CBaseTopPane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	//! Destructor
	virtual ~CBaseTopPane();

	virtual  void Init();

	//! Adds a load button to the bar
	virtual ge::IButton* InsertLoadButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a save button to the bar
	virtual ge::IButton* InsertSaveButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a save as button to the bar
	virtual ge::IButton* InsertSaveAsButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a/b buttons to the bar
	virtual ge::IRadioButtons* InsertABButtons(tuint32 uiResourceIDA, const ge::SPos& PosA, tuint32 uiResourceIDB, const ge::SPos& PosB, tbool bMouseStateFrames = false);

	//! Adds a a->b button to the bar
	virtual ge::IButton* InsertAToBButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a b->a button to the bar
	virtual ge::IButton* InsertBToAButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a midi learn button to the bar
	virtual ge::IMultiStateButton* InsertMIDILearnButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a midi unlearn button to the bar
	virtual ge::IMultiStateButton* InsertMIDIUnlearnButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a midi reset button to the bar
	virtual ge::IButton* InsertMIDIResetButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! Adds a bypass button to the bar
	virtual ge::IMultiStateButton* InsertBypassButton(tuint32 uiResourceID, const ge::SPos& Pos, tbool bMouseStateFrames = false);

	//! IEventReceiver override
	virtual void EventValueChange(ge::IControl *pSender, tint32 iValueNew);
	//! IEventReceiver override
	virtual void EventGeneric(ge::IControl *pSender, void *pEventData);

	//! Register controls
	virtual void RegisterControls();

	virtual void MIDICCLearn(tint32 iCtrlID);

protected:
	//! If true, the pane has a bypass button, otherwise not
	tbool mbHasBypass;

	//! Path name of current preset, or "" if none
	std::string msPresetPathName;

	//! Current preset set being used (0 == A, 1 == B)
	tint miPresetSetCurrent;

	//! Background preset A (i.e. current set not used)
	IChunkFileMemory* mpPresetBackground;

	//! Copy background preset set to foreground
	void CopyBackgroundPresetToForeground();

	//! Copy foreground preset set to background
	void CopyForegroundPresetToBackground();

	//! Called when the load button has been pressed
	void ActionLoad();

	//! Called when the save button has been pressed
	void ActionSave();

	//! Called when the save as button has been pressed
	void ActionSaveAs();

	//! Called when the A or B button has been pressed
	/*!
		\param iIndex [in]: If 0 the A button has been pressed, if 1 the B button has been pressed
	*/
	void ActionAB(tint iIndex);

	//! Called when the A->B button has been pressed
	void ActionAToB();

	//! Called when the B->A button has been pressed
	void ActionBToA();

	//! Called when the bypass button has been pressed
	/*!
		\param bBypass [in]: If true bypass has been enabled, if false disabled.
	*/
	void ActionBypass(tbool bBypass);

	void MIDILearnOrUnlearnAbort();

	void MIDICyclingOnOff();

	virtual void MIDIReset();
};


