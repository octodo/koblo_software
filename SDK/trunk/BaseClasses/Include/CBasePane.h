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


class CBaseGUI;

class CBasePaneStd;

/*! \class CBasePane
*/
class CBasePane : public virtual ge::IEventReceiver
{
public:
	//! Constructor
	CBasePane(CBasePane* pPaneParent, CBaseGUI* pGUI);

	//! Destructor
	virtual ~CBasePane();

	ge::IPane* GetPane() {return mpPane;}

	CBaseGUI* GetGUI() {return mpGUI;}

	CBasePlugIn* GetPlugIn();

	//! Returns window we're in
	ge::IWindow* GetWindow();

	ge::IResourceManager* GetResMan() {return mpResMan;}

	de::IParameterManager* GetParmMan() {return mpParmMan;}

	virtual void Init() = 0;

	virtual void RegisterChild(CBasePane* pChild);
	
	virtual void RemoveChild(CBasePane* pChild);

	struct SMsg
	{
		SMsg() : iOpcode(-1), pDataIn(NULL), pDataOut(NULL) {}
		SMsg(tint32 iOpcode) : iOpcode(iOpcode), pDataIn(NULL), pDataOut(NULL) {}
		SMsg(tint32 iOpcode, void* pDataIn) : iOpcode(iOpcode), pDataIn(pDataIn), pDataOut(NULL) {}

		tint32 iOpcode;
		void* pDataIn;
		void* pDataOut;
	};

	virtual void SendMsg(SMsg* pMsg);

	virtual void SendMsgFromTop(SMsg* pMsg);

	virtual void HandleMsg(SMsg* pMsg) {}
	
//	ge::IControl* GetControl(tint32 iID) {return mpPane->GetControl(iID);}
	ge::IControl* GetControl(tint32 iID) {return GetWindow()->GetControl(iID);}

	//! Locates the basepane which is a placeholder for pPane. Works from current pane and "below".
	CBasePane* FindPane(ge::IPane* pPane);

	void SetControlValue(tint32 iPaneID, tint32 iControlID, tint32 iValue);

	tint32 GetPaneID() const {return miPaneID;}

	virtual void Resize(ge::SSize SizeNew);

	//! Called by a subpane when a pane is added
	/*!
		\param uiPaneID [in]: ID of added pane
		\param pPane [in]: Pointer to added pane
	*/
	virtual void PaneAdded(tuint32 uiPaneID, CBasePane* pPane);

	CBasePane* GetPane(tuint32 uiPaneID);
	
	//! Redraw the Pane rest
	virtual void Redraw_Pane_Rect();

protected:
	static CAutoDelete<IUniqueID> gpPaneIDs;

	tint32 miPaneID;

	CBasePane* mpParent;

	std::list<CBasePane*> mChilds;

	CBaseGUI* mpGUI;

	ge::IPane* mpPane;

	CBasePlugIn* mpPlugIn;

	de::IParameterManager* mpParmMan;
	
	ge::IResourceManager* mpResMan;

	//! Map used to store control IDs. As we learn about them, we add them to the map. Note: This will currently not work if controls are deleted.
	std::map<tuint32, CBasePane*> mPaneMap;

	//! Registration of global control which requires no parameter value translation
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
	*/
	void RegisterGlobalControl(tint iSection, tint32 iControlID, tint32 iParameterID)
	{
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		GetWindow()->GetControl(iControlID)->SetDefaultValueConverted(mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection)->GetDefaultValue());
	}

	//! Registration of global control with display translater
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
		\param iDecimals [in]: Number of decimals in parameter
		\param iMaxNrOfStringChars [in]: Max. number of characters to write in string
		\param iMaxNrOfStringDecimals [in]: Max number of decimals to write in string
		\param sPostFix [in]: Post fix string (for instance "%" or "dB") to use, or "" if none
		\return de::IDisplayConverter: Display converter (for adding of fixed strings)
	*/
	de::IDisplayConverter* RegisterGlobalControlWithDisplayTranslater(tint iSection, tint32 iControlID, tint32 iParameterID, tint iDecimals = 0, tint iMaxNrOfStringChars = 5, tint iMaxNrOfStringDecimals = 0, std::string sPostFix = std::string(""))
	{
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);

		de::IDisplayConverter* pConverter = de::IDisplayConverter::Create(iDecimals, iMaxNrOfStringChars, iMaxNrOfStringDecimals);
		if (sPostFix.size() != 0) {
			pConverter->SetPostFix((const tchar*)(sPostFix.c_str()));
		}
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetDisplayConverter(pConverter);
		pControl->SetDefaultValueConverted(mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection)->GetDefaultValue());
		return pConverter;
	}

	//! Registration of global control with parameter translation
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
	*/
	void RegisterGlobalControlWithLinearTranslation(tint iSection, tint32 iControlID, tint32 iParameterID)
	{
		const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
		tint32 iMin = pParm->GetMinValue();
		tint32 iMax = pParm->GetMaxValue();
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetConverter(de::IConverter::CreateLinear(iMin, iMax));
		pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
	}

	//! Registration of global control with logarithmic parameter translation
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
		\param iStrength [in]: "Strength" of the logarithm. 1000 is "normal". < 1000 is between logarithmic and linear. > 1000 is "more logarithmic".
	*/
	void RegisterGlobalControlWithLogTranslation(tint iSection, tint32 iControlID, tint32 iParameterID, tint iStrength)
	{
		const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
		tint32 iMin = pParm->GetMinValue();
		tint32 iMax = pParm->GetMaxValue();
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetConverter(de::IConverter::CreateLog(iMin, iMax, iStrength));
		pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
	}

	//! Registration of global control with "true" logarithmic parameter translation
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
	*/
	void RegisterGlobalControlWithTrueLogTranslation(tint iSection, tint32 iControlID, tint32 iParameterID)
	{
		const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
		tint32 iMin = pParm->GetMinValue();
		tint32 iMax = pParm->GetMaxValue();
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetConverter(de::IConverter::CreateTrueLog(iMin, iMax));
		pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
	}

	//! Registration of global control with enum translation (continuous control works as a discrete control)
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
	*/
	void RegisterGlobalControlWithEnumTranslation(tint iSection, tint32 iControlID, tint32 iParameterID)
	{
		const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ASSERT(pParm->GetMinValue() == 0);
		tint32 iMax = pParm->GetMaxValue();
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetConverter(de::IConverter::CreateEnum(iMax));
		pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
	}

	//! Registration of global control with X3 parameter translation
	/*!
		\param iControlID [in]: ID of control to register
		\param iParameterID [in]: ID of parameter to register control with
		\param uiParmDecimals [in]: Number of decimals in parameter.
	*/
	void RegisterGlobalControlWithX3Translation(tint iSection, tint32 iControlID, tint32 iParameterID, tuint uiParmDecimals)
	{
		const de::IParameter* pParm = mpParmMan->Get(iParameterID, de::IParameterManager::TypeGlobal, iSection);
		tint32 iMin = pParm->GetMinValue();
		tint32 iMax = pParm->GetMaxValue();
		mpParmMan->RegisterControl(miPaneID, iControlID, iParameterID, de::IParameterManager::TypeGlobal, iSection);
		ge::IControl* pControl = GetWindow()->GetControl(iControlID);
		pControl->SetConverter(de::IConverter::CreateX3(iMin, iMax, uiParmDecimals));
		pControl->SetDefaultValueConverted(pParm->GetDefaultValue());
	}

public:
	/*
		Creation order:
		ControlID
		ResourceID
		Pos
		Size
		.
		.
		.
		Frames
		Rows
	*/

	//! Enum for font creation
	enum EFontType {
		//! .. Arial18Bold - Old style fonts with sizing predefined in static array.
		Arial10,
		Arial10Narrow,
		Arial12,
		Arial12Bold,
		Arial14,
		Arial20,
		Arial14Bold,
		Arial18Bold,
		//! - New style font with 128 ASCII chars ( - 32). Character delimiters must be defined in alpha channel
		Generic128,
		//! - New style font with 256 X-ASCII chars ( - 32). Character delimiters must be defined in alpha channel<br><b>Not fully implemented!</b>
		Generic256,
		//! - New style font with between 33 and 256 X-ASCII chars ( - 32). Character delimiters must be defined in alpha channel<br><b>Not fully implemented!</b>
		Generic,
	};

	//----------------------------------------------
	// Helpers
	//---------------------------------------------

	//! Helper call to create state-less button (for "actions")
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iResourceID [in]: ID of bitmap
		\param Pos [in]: Position of control (relative to pane its added to)
		\param bMouseStateFrames [in]: If true extra frames are shown (see ge::IButton for details)
		\param iRows [in]; Number of rows in bitmap (use 2 if you have a row of ghosted frames below the ordinary ones)
	*/
	ge::IButton* CreateButton(
					tint32 iControlID,
					tint32 iResourceID, 
					ge::SPos Pos, 
					tbool bMouseStateFrames = false,
					tint32 iRows = 1);

	//! Helper call to create 2-state button
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iResourceID [in]: ID of bitmap
		\param Pos [in]: Position of control (relative to pane its added to)
		\param bMouseStateFrames [in]: If true extra frames are shown (see ge::IButton for details)
		\param iRows [in]; Number of rows in bitmap (use 2 if you have a row of ghosted frames below the ordinary ones)
	*/
	ge::IMultiStateButton* Create2StateButton(
				tint32 iControlID,
				tint32 iResourceID, 
				ge::SPos Pos,
				tbool bMouseStateFrames = false,
				tint32 iRows = 1);

	//! Helper call to create multi state button
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iResourceID [in]: ID of bitmap
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iStates [in]: Number of states
		\param iNrOfFrames [in]: Number of frames in bitmap, or 0 means = number of states
		\param iRows [in]; Number of rows in bitmap (use 2 if you have a row of ghosted frames below the ordinary ones)
	*/
	ge::IMultiStateButton* CreateMultiStateButton(
				tint32 iControlID,
				tint32 iResourceID, 
				ge::SPos Pos, 
				tint iNrOfStates,
				tint iNrOfFrames = 0,
				tint32 iRows = 1);

	//! Helper call to create radio button
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iResourceID [in]: ID of bitmap
		\param Pos [in]: Position of control (relative to pane of radio butttons group(!) its added to)
		\param pGroup [in]: Group to add button to
		\param iRows [in]; Number of rows in bitmap (use 2 if you have a row of ghosted frames below the ordinary ones)
	*/
	ge::IMultiStateButton* CreateRadioButton(
				tint32 iControlID,
				tint32 iResourceID,
				ge::SPos Pos, 
				ge::IRadioButtons* pGroup,
				tint32 iRows = 1);

	//! Helper call to create radio buttons group
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
	*/
	ge::IRadioButtons* CreateRadioButtonsGroup(
				tint32 iControlID,
				ge::SPos Pos);

	//! Helper call to create a mouse-trap (invisible control that can listen for mouse messages)
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param pControlBelow [in]: Another control to place the mouse-trap on top of.<br>Leave NULL for no position, the mouse-trap can still catch "outside control" mouse messages
	*/
	ge::IMouseTrap* CreateMouseTrap(tint32 iControlID,
		ge::IControl* pControlBelow = NULL
	);

	//! Helper call to create spinner (1 up + 1 down button)
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
	*/
	ge::ISpinner* CreateSpinner(
				tint32 iControlID,
				ge::SPos Pos);

	//! Helper call to create a peek meter
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iSteps [in]: How many steps to divide top (last) bitmap into
		\param eDir [in]: Which direction peek meter grows in (default "Up")
	*/
	ge::IPeakMeter* CreatePeakMeter(
				tint32 iControlID,
				ge::SPos Pos,
				tint32 iSteps,
				ge::IPeakMeter::EDirection eDir = ge::IPeakMeter::Up);

	//! Helper call to create knob
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iResourceID [in]: ID of resource
		\param iFrames [in]: Number of frames in knob
	*/
	ge::IKnob* CreateKnob(tint32 iControlID,
		tint32 iResourceID,
		ge::SPos Pos,
		tint iFrames,
		tint32 iRows = 1);

	//! Helper call to create knob2 (1 bitmap drawn in an arc)
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane it's added to)
		\param Size [in]: Size of control (relative to pane it's added to)
		\param iAngleStart [in]: Start angle
		\param iAngleEnd [in]: End angle
		\param iResourceID [in]: ID of resource
	*/
	ge::IKnob2* CreateKnob2(tint32 iControlID,
		ge::SPos Pos,
		ge::SSize Size,
		tint32 iAngleStart,
		tint32 iAngleEnd,
		tint32 iResourceID);

	//! Helper call to create slider
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param iResourceID [in]: ID of resource
	*/
	ge::ISlider* CreateSlider(tint32 iControlID,
		ge::SPos Pos,
		ge::SSize Size,
		tint32 iResourceID);

	//! Helper call to create rect
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Rect [in]: Position and size of control (relative to pane its added to)
		\param Colour [in]: Colour of rect
	*/
	ge::IRect* CreateRect(tint32 iControlID,
		ge::SRect Rect,
		ge::SRGB Colour);

	//! Helper call to create inverter
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Rect [in]: Position and size of control (relative to pane its added to)
	*/
	ge::IInvert* CreateInvert(tint32 iControlID,
		ge::SRect Rect);

	//! Helper call to create line
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param PosStart [in]: Position of control (relative to pane its added to)
		\param PosEnd [in]: End position of control (relative to pane its added to)
		\param Colour [in]: Colour of rect
	*/
	ge::ILine* CreateLine(tint32 iControlID,
		ge::SPos PosStart,
		ge::SPos PosEnd,
		ge::SRGB Colour);

	//! Helper call to create bitmap
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iResourceID [in]: Resource ID
	*/
	ge::IBitmap* CreateBitmap(tint32 iControlID,
		tint32 iResourceID,
		ge::SPos Pos);

	//! Helper call to create 2D control
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iResourceID [in]: Resource ID
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size (area it can move in)
	*/
	ge::I2D* Create2D(tint32 iControlID,
		tint32 iResourceID,
		ge::SPos Pos,
		ge::SSize Size);

	//! Helper call to create URL (invisible control which calls URL)
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param sURL [in]: URL to call
	*/
	ge::IURL* CreateURL(tint32 iControlID,
		ge::SPos Pos,
		ge::SSize Size,
		const std::string& sURL);

	//! Helper call to create VU using bitmap frames to draw
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iResourceID [in]: Resource ID
		\param iNrOfFrames [in]; Number of frames in bitmap
	*/
	ge::IBitmap* CreateBitmapVU(tint32 iControlID,
		tint32 iResourceID,
		ge::SPos Pos,
		tint iNrOfFrames);

	//! Helper call to create drop-down list box button
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param iBitmapID [in]: ID of bitmap for top section
		\param iResourceIDs [in]: Resource IDs. See documentation for ge::IPopup
		\param FontType [in]: Type of font used
		\param iFontID [in]: ID of font used
		\param List [in]: List of items in popup
		\param Pos [in]: Position of control (relative to pane its added to)
		\param iLeftPaneID [in]: ID of left pane, which pane we're adding to belongs to
		\param iTopPaneIndex [in]: Index (0-based) of top pane, which pane we're adding to belongs to
		\param iSubPaneIndex [in]: Index (0-based) of sub pane, which pane we're adding to belongs to. Normally this is 0, since most pane don't have sub panes
		\param Offset [in]: Offset for button. Defafults to no offset
		\return ge::IDropDownListBox*: Drop down list box created
	*/
	ge::IDropDownListBox* CreateDropDownListBox(
				tint32 iControlID,
				const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount],
				EFontType FontType,
				tint32 iFontID,
				tint32 iBitmapID, 
				const ge::IPopupMenu::SMenuItemList& List, 
				ge::SPos Pos,
				ge::SPos Offset		=	ge::SPos(0, 0),
				ge::SSize SizeText  =	ge::SSize(60, 18),
				ge::SRGB			=	ge::SRGB(10, 10, 10));

	//! Helper call to create button. Unlike other helper calls, it is not added to a pane
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param piParmIDs [in]: Resource IDs (see documentation for ge::IPopUp);
		\param FontType [in]: Type of font used
		\param iFontID [in]: ID of font used
		\param List [in]: List of items in popup
		\return ge::IPopUp*: PopUp menu created
	*/
	ge::IPopupMenu* CreatePopupMenu(tint32 iControlID, const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount], EFontType FontType, tint32 iFontID, const ge::IPopupMenu::SMenuItemList& List);

	//! Helper call to create a multi-state button based drop-down list
	/*!
		\param iCtrlIDForMultiButton [in]: Control ID for multi-state button
		\param iResIDForMultiButton [in]: Ressource ID of bitmap to use for multi-state button
		\param iNrOfStates [in]: The number of *used* frames in the bitmap
		\param iNrOfFrames [in]: The full number of frames in the bitmap
		\param posRelativeForMultiButton [in]: The position of multi-state button (the drop-down list itself will assume the size and position of the owning pane, for pop-up to display correctly)
		\param piResourceIDs [in]: List of resources for pop-up menu
		\param FontType [in]: Font for pop-up menu
		\param iFontID [in]: Resource ID for font for pop-up menu
		\param List [in]: List (of text) used to initiate pop-up
		\param bAutoCopyButtonPicturesToPopup [in]: True = the frames in the multi-state button will be displayed left of the text in the pop-up
		\param msgTrigger [in]: The mouse message that will trigger the pop-up
		\param bPopupAtClickedPosition [in]: True = pop-up will appear at the position that was clicked (probably covering part of the multi-state button), False = pop-up will appear next to the multi-state button (will not overlay it)
		\return ge::IMultiStateDropDownListBox*: multi-state drop-down list created
	*/
	virtual ge::IMultiStateDropDownListBox* CreateMultiButtonPopup(
				tint32 iCtrlIDForMultiButton,
				tint32 iResIDForMultiButton,
				tint iNrOfStates,
				tint iNrOfFrames,
				ge::SPos posRelativeForMultiButton,
				
				const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount],
				EFontType FontType, tint32 iFontID,
				ge::IPopupMenu::SMenuItemList& List,
				tbool bAutoCopyButtonPicturesToPopup,

				ge::EMouseMsg msgTrigger = ge::RightButtonUp,
				tbool bPopupAtClickedPosition = true
	);

	//! Helper call to create text
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param pFont [in]: Font to write text with
		\param sText [in]: Text to write
		\param iLeftPaneID [in]: ID of left pane, which pane we're adding to belongs to
		\param HorzAlignment [in]: Horizontal alignment of the control
		\param VertAlignment [in]: Vertical alignment of the control
		\return IText*: Pointer to the control created (may be ignored)
	*/
	ge::IText* CreateText(tint32 iControlID, 
				ge::SPos Pos, 
				ge::SSize Size, 
				ge::IFontResource* pFont, 
				std::string sText, 
				ge::IText::EHorizontalAlignment HorzAlignment = ge::IText::HorzAlignLeft, 
				ge::IText::EVerticalAlignment VertAlignment = ge::IText::VertAlignTop);

	//! Helper call to create text field
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param pFont [in]: Font to write text with
		\param sText [in]: Text to write
		\param iLeftPaneID [in]: ID of left pane, which pane we're adding to belongs to
		\param HorzAlignment [in]: Horizontal alignment of the control
		\param VertAlignment [in]: Vertical alignment of the control
		\return ITextField*: Pointer to the control created (may be ignored)
	*/
	ge::ITextField* CreateTextField(tint32 iControlID, 
				ge::SPos Pos, 
				ge::SSize Size, 
				ge::IFontResource* pFont, 
				std::string sText, 
				ge::ITextField::EHorizontalAlignment HorzAlignment = ge::ITextField::HorzAlignLeft, 
				ge::ITextField::EVerticalAlignment VertAlignment = ge::ITextField::VertAlignTop);

	//! Helper call to create text for display
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param pFont [in]: Font to write text with
		\param iLeftPaneID [in]: ID of left pane, which pane we're adding to belongs to
		\return IText*: Pointer to the control created (may be ignored)
	*/
	ge::IText* CreateDisplay(tint32 iControlID, 
				ge::SPos Pos, 
				ge::SSize Size, 
				ge::IFontResource* pFont,
				tbool bEdiable = false);

	//! Helper call to create a text list with validation for display
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param pFont [in]: Font to write text with
		\param iLeftPaneID [in]: ID of left pane, which pane we're adding to belongs to
		\param pszEnumList [in]: Zero-terminated string containing valid texts to show. The individual values must be delimited by a character.
		\param cEnumDelimiter [in]: The delimiter character used to split the string pszEnumList into values (use '@' for same string format as CBasePlugIn::InsertParameterEnum(...).)
		\param HorzAlignment [in]: Horizontal alignment of the control
		\param VertAlignment [in]: Vertical alignment of the control
		\return ITextList*: Pointer to the control created (may be ignored)
	*/
	ge::ITextList* CreateTextList(tint32 iControlID, 
				ge::SPos Pos, 
				ge::SSize Size, 
				ge::IFontResource* pFont, 
				const tchar* pszEnumList,
				tchar cEnumDelimiter,
				ge::IText::EHorizontalAlignment HorzAlignment = ge::IText::HorzAlignLeft, 
				ge::IText::EVerticalAlignment VertAlignment = ge::IText::VertAlignTop);

	//! Helper call to create a listbox with validation for display
	/*!
		\param iControlID [in]: ID of control (or ge::IControl::giNoID)
		\param Pos [in]: Position of control (relative to pane its added to)
		\param Size [in]: Size of control
		\param pFont [in]: Font to write text with
		\return IListBox*: Pointer to the control created (may be ignored)
	*/
	ge::IListBox* CreateListBox(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont);

	//! Helper call to create font
	/*!
		\param Type [in]: Font type<br>Use Generic128 for new fonts
		\param iFontResID [in]: Ressource ID of bitmap to use for font<br>See IFontResource::GetFont(...) for technical info
		\param Color [in]: Color of font
		\return IFontResource*: Pointer to the font created
	*/
	ge::IFontResource* CreateFont(EFontType Type, tint32 iFontResID, ge::SRGB Color = ge::SRGB(10, 10, 10));

	//! Helper call to create stereo or quad panner
	/*!
		\brief For a stereo panner you should leave one of the control IDs to ge::IControl::giNoID.<br>For quad panners both control IDs must be defined.
		\param iXCtrlID [in]: Control ID for horizontal position of panner (x dimension)
		\param iYCtrlID [in]: Control ID for vertical position of panner (y dimension)
		\param iBackResID [in]: Bitmap resource to use for background of implicit IPane. Set to NULL for transparent background
		\param iHndlResID [in]: Bitmap resource to use for dot / handle
		\param posBack [in]: Position to create panner (relative to parent-pane). Background bitmap will display here
		\param posOfsArea [in]: Offset of top-left corner of the area that the handle bitmap will move around inside 
		\param sizeArea [in]: The size of the area that the handle will move around inside. For stereo panners you must leave one of the dimensions to 0.
		\param pPane [in]: NULL => the panner is added directly to the base-pane, otherwise a pointer to the pane-switcher the panner should be added to
		\return IPanner*: Pointer to the created control (may be ignored)
	*/
	ge::IPanner* CreatePanner(tint32 iXCtrlID, tint32 iYCtrlID,
							  tint32 iBackResID, tint32 iHndlResID,
							  ge::SPos posBack, ge::SPos posOfsArea, 
							  ge::SSize sizeArea,
							  ge::IPaneSwitcher* pPane);

	//! Call to create pane
	/*!
		\param iControlID [in]: Control ID
		\param Pos [in]: Position in parent pane
		\param Size [in]: Size of scroll pane. This is the visible size.
		\return IPane*: Pointer to the created control (may be ignored)
	*/
	ge::IPane* CreatePane( ge::SPos Pos, ge::SSize Size );
	
	//! Call to create scroll pane
	/*!
		\param iControlID [in]: Control ID
		\param Pos [in]: Position in parent pane
		\param Size [in]: Size of scroll pane. This is the visible size.
		\return IScrollPane*: Pointer to the created control (may be ignored)
	*/
	ge::IScrollPane* CreateScrollPane(tint32 iControlID, ge::SPos Pos, ge::SSize Size, tint32 iTime);
	
	//! Call to create paneSwitcher pane
	/*!
		\param iControlID [in]: Control ID
		\param Pos [in]: Position in parent pane
		\return IPaneSwitcher*: Pointer to the created control (may be ignored)
	*/
	ge::IPaneSwitcher* CreatePaneSwitcher(tint32 iControlID, ge::SPos Pos );
	
	// Lasse, out-commented
	/*
	//! Call to create paneSwitcher pane
	/*!
		\param iControlID [in]: Control ID
		\param BitmapID [in]: Bitmap ID
		\param List [in]: Popup meny list of names
		\param Pos [in]: position
		\param Pane [in]: Bitmap ID
		\return IDropDownListBox*: Pointer to the created control
	* /
	ge::IDropDownListBox* CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos);
	*/
	// .. Lasse
};


