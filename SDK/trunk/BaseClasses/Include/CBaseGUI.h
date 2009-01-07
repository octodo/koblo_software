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


// Forwards
class CBaseTopPane;
class CBaseMIDICCEvent;


/*! \class CBaseGUI
 * \brief Implementation of kspi::IGUI
*/
class CBaseGUI : public virtual kspi::IGUI, public virtual ge::IContextCallback
{
	friend class CBaseTopPane;

public:
	//! Constructor
	CBaseGUI(CBasePlugIn* pPlugIn, de::IParameterManager* pParmMan);

	//! Destructor
	virtual ~CBaseGUI();

	// IGUI override
	virtual void KSPI_CALL MakeWindow(void* p);
	// IGUI override
	virtual void KSPI_CALL PrepareBuffer();
	// IGUI override
	virtual void KSPI_CALL Paint();
	// IGUI override
	virtual void KSPI_CALL OnMouseDown(tuint32 iPosX, tuint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnKeyDown(tuint32 iKeyCode);
	// IGUI override
	virtual void KSPI_CALL OnTimer();
	// IGUI override
	virtual void KSPI_CALL OnDraw(tuchar* puchBuffer, tuint32 uiInterleave, tint32 iUpdateX, tint32 iUpdateY, tint32 iUpdateCX, tint32 iUpdateCY);
	// IGUI override
	virtual void KSPI_CALL OnMouseLeftButtonDown(tint32 iPosX, tint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnMouseLeftButtonUp(tint32 iPosX, tint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnMouseMove(tint32 iPosX, tint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnMouseRightButtonDown(tint32 iPosX, tint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnMouseRightButtonUp(tint32 iPosX, tint32 iPosY);
	// IGUI override
	virtual void KSPI_CALL OnMouseLeftButtonDoubleClick(tint32 iPosX, tint32 iPosY);

	//! IContext callback
	virtual tbool OnKeyDown(ge::EKey Key) {return false;}

	virtual void SetPane(CBasePane* pPane);

	virtual CBasePane* GetPane() {return mpPane;}

	//! Is called when a control needs updating
	virtual void SetControlValue(tint32 iPaneID, tint32 iControlID, tint32 iValue);

	//! Returns the GUI engine windows
	virtual ge::IWindow* GetWindow() {return mpWindow;}

	//! Overload
	virtual const ge::IWindow* GetWindow() const {return mpWindow;}

	//! Returns the resource manager
	virtual ge::IResourceManager* GetResMan() {return mpResMan;}

	//! Overload
	virtual const ge::IResourceManager* GetResMan() const {return mpResMan;}

	//! Returns the plug-in
	CBasePlugIn* GetPlugIn() {return mpPlugIn;}

	//! Overload
	const CBasePlugIn* GetPlugIn() const {return mpPlugIn;}

	//! Returns the folder which the application is inside
	/*!
		\return string: Folder which holds the application
	*/
	virtual std::string GetApplicationFolder();

	//! Returns the folder one step "up" from the plug-in folder (GetPlugInFolder())
	/*!
		\return string: Folder which is one step "up" compared to the plug-in folder
	*/
	virtual std::string GetCompanyFolder();

	//! Returns the folder which holds folders like presets folder. Is a subfolder of the plug-in folder. (GetPlugInFolder())
	/*!
		\return string: Folder to place preset folder.
	*/
	virtual std::string GetFilesFolder();

	//! Returns the preset folder. Is a subfolder of the files folder (GetFilesFolder())
	/*!
		\return string: Folder to place presets in.
	*/
	virtual std::string GetPresetFolder();

	//! Returns the folder which holds misc resources and authorizer application. Is a subfolder of the plug-in folder. (GetPlugInFolder()).
	/*!
		\return string: 
	*/
	virtual std::string GetResourceFolder();

	//! Returns the folder to put user preferences in (on a per-user basis).
	/*!
		\return string: Folder to place user preferences in.
	*/
	virtual std::string GetPreferenceFolder();

	//! Returns the folder to put company preferences in (on a per-user basis). Is one step "up" from preference folder (GetPreferencesFolder())
	/*!
		\return string: Folder to put company preferences in
	*/
	virtual std::string GetCompanyPreferenceFolder();

	//! Called to open a preset. Can be overriden for non-standard presets
	/*!
		\param pFile [in]: File to save to
		\param iProductID [in]: ID of product, or -1 if product ID shouldn't be checked in file
	*/
	virtual void OnLoadPreset(IChunkFile* pFile, tint32 iProductID = -1);

	//! Called to save a preset. Can be overriden for non-standard presets
	/*!
		\param pFile [in]: File to save to
		\param iProductID [in]: ID of product, or -1 if product ID shouldn't be saved to file
	*/
	virtual void OnSavePreset(IChunkFile* pFile, tint32 iProductID = -1);

	//! Called when the bypass button on the GUI has changed
	virtual void SetBypass(tbool bBypass);

	//! MIDI CC Learn
	virtual void MIDICCLearn(tint32 iID);

	ge::IEventReceiver* GetMIDICCReceiver();

	virtual void SetMIDILearn(tbool bNew) {mbMIDILearn = bNew;}

	virtual tbool GetMIDILearn() const {return mbMIDILearn;}

	virtual void SetMIDIUnlearn(tbool bNew) {mbMIDIUnLearn = bNew;}

	virtual tbool GetMIDIUnlearn() const {return mbMIDIUnLearn;}

	//! Disables learn and unlearn turning off after a control has been touched
	virtual void DisableMIDIAutomaticTurnOff();

	de::IParameterManager* GetParmMan() {return mpParmMan;}

	virtual void Resize(tint32 iSizeX, tint32 iSizeY) {ge::SSize SizeNew(iSizeX, iSizeY);
		mpWindow->SetSize(SizeNew);
		mpPane->Resize(SizeNew);}

//protected:
	//! The graphical context
	ge::IContext* mpContext;
	//! The resource manager
	ge::IResourceManager* mpResMan;
	//! The plug-in
	CBasePlugIn* mpPlugIn;
	//! The window
	ge::IWindow* mpWindow;
	//! The pane
	CBasePane* mpPane;

	tbool mbBufferedDrawing;

	//! Set if MIDI learn is active
	tbool mbMIDILearn;

	//! Set if MIDI unlearn is active
	tbool mbMIDIUnLearn;

	tbool mbMIDIAutomaticTurnOff;

	//! Parameter manager
	de::IParameterManager* mpParmMan;

	//! MIDI CC callback
	CBaseMIDICCEvent* mpMIDICCCallback;

#ifdef WIN32
	//! Parent window
	HWND mhWndParent;
#endif	// WIN32
#ifdef _Mac
	//! Not actually the parent window, but rather the window
	void* mhWndParent;
#endif	// _Mac

	//! Switch preset
	virtual void ABPreset();
	//! Copy preset
	virtual void CopyPreset();

	//! Background preset A (i.e. current set not used)
	IChunkFileMemory* mpPresetBackground;

	//! Called once to make the GUI. Good place to make controls. Inheriting classes must implement this.
	virtual void Initialize() = 0;

	//! Call to create resource manager and initialize it. Called from PreInitialize().
	virtual void CreateResourceManager();

	//! Called immediately before window is created
	virtual void PreCreate();

	//! Parameter/controls unlinking
	virtual void DisconnectControls();
	
};


