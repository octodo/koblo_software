

#ifdef WIN32

// Base Utilities
#include "bu.h"

// Graphical Engine
#include "ge.h"

// KSPI
#include "kspi.h"
#include "kspiC.h"

// Base Classes
#include "FontSizes.h"

#include "BaseControlIDs.h"
#include "IBaseDezipperCallback.h"
#include "CBaseDezipper.h"
#include "CBaseDSPEngine.h"
#include "CBaseModule.h"
#include "CBasePlugIn.h"
#include "CBasePane.h"
#include "CBasePaneStd.h"
#include "CBaseGUI.h"
#include "CBaseMIDICCEvent.h"

#endif	// WIN32

CAutoDelete<IUniqueID> CBasePane::gpPaneIDs(IUniqueID::Create(0));

CBasePane::CBasePane(CBasePane* pPaneParent, CBaseGUI* pGUI)
	: mpParent(pPaneParent), mpGUI(pGUI), mpPlugIn(pGUI->GetPlugIn()), mpParmMan(pGUI->GetParmMan())
{
	miPaneID = gpPaneIDs->GetID();

	mpPane = NULL;

	mpResMan = mpGUI->GetResMan();
	
	// std::cerr << sPathName.c_str();

	if (mpParent) {
		mpParent->RegisterChild(this);
	}

	PaneAdded(miPaneID, dynamic_cast<CBasePane*>(this));
}

CBasePane::~CBasePane()
{
//	if (mpParent) {
//		mpParent->RemoveChild(this);
//	}

	
	std::list<CBasePane*>::iterator it = mChilds.begin();
	for (; it != mChilds.end(); it++) {
		delete *it;
	}

	gpPaneIDs->ReleaseID(miPaneID);
}

ge::IWindow* CBasePane::GetWindow()
{
	return GetGUI()->GetWindow();
}

CBasePlugIn* CBasePane::GetPlugIn()
{
//	GetGUI()->GetPlugIn();
	return mpPlugIn;
}

void CBasePane::RegisterChild(CBasePane* pChild)
{
	mChilds.push_back(pChild);
}

void CBasePane::RemoveChild(CBasePane* pChild)
{
	std::list<CBasePane*>::const_iterator it = mChilds.begin();
	for (; it != mChilds.end(); it++) {
		if(*it == pChild){
			mChilds.remove(*it) ;
			break;
		}
	}
}

void CBasePane::SendMsg(CBasePane::SMsg* pMsg)
{
	std::list<CBasePane*>::const_iterator it = mChilds.begin();
	for (; it != mChilds.end(); it++) {
		(*it)->SendMsg(pMsg);
		(*it)->HandleMsg(pMsg);
	}
}

void CBasePane::SendMsgFromTop(CBasePane::SMsg* pMsg)
{
	if (mpParent) {
		mpParent->SendMsgFromTop(pMsg);
	}
	else {
		SendMsg(pMsg);
	}
}

CBasePane* CBasePane::FindPane(ge::IPane* pPane)
{
	if (dynamic_cast<ge::IPane*>(mpPane) == dynamic_cast<ge::IPane*>(pPane)) {
		// It's us
		return dynamic_cast<CBasePane*>(this);
	}
	else {
		// It's not us, so search subpanes
		std::list<CBasePane*>::const_iterator it = mChilds.begin();
		for (; it != mChilds.end(); it++) {
			CBasePane* pRet = (*it)->FindPane(pPane);
			if (pRet != NULL) {
				return pRet;
			}
		}
		// It's not in one of the subpanes, so return NULL
		return NULL;
	}
}

void CBasePane::SetControlValue(tint32 iPaneID, tint32 iControlID, tint32 iValue)
{
/*	if (miPaneID == iPaneID) {
		// This is the pane, so set control value
		GetWindow()->SetControlValue(iControlID, iValue);
	}
	else {
		// This is not the pane, so search sub panes
		std::list<CBasePane*>::const_iterator it = mChilds.begin();
		for (; it != mChilds.end(); it++) {
			(*it)->SetControlValue(iPaneID, iControlID, iValue);
		}
	}*/

	if (mpParent) {
		// Not top window, so relay
		mpParent->SetControlValue(iPaneID, iControlID, iValue);
	}
	else {
		CBasePane* pPane = GetPane((tuint32)iPaneID);
		if (pPane) {
			// The pane is in this window, so set control value
			GetWindow()->SetControlValue(iControlID, iValue);
		}
	}
}

void CBasePane::Resize(ge::SSize SizeNew)
{
	mpPane->SetSize(SizeNew, true);
}

void CBasePane::PaneAdded(tuint32 uiPaneID, CBasePane* pPane)
{
	if (mpParent) {
		// Relay to parent pane
		mpParent->PaneAdded(uiPaneID, pPane);
	}
	else {
		// No parent, i.e. we're the top pane
		mPaneMap.insert(std::pair<tuint32, CBasePane*>(uiPaneID, pPane));
	}
}

CBasePane* CBasePane::GetPane(tuint32 uiPaneID)
{
	std::map<tuint32, CBasePane*>::const_iterator it = mPaneMap.find(uiPaneID);

	if (it != mPaneMap.end()) {
		// It's there, return it
		CBasePane* pPane = it->second;
		return pPane;
	}

	// Pane wasn't in map.
	return NULL;
}

ge::IKnob* CBasePane::CreateKnob(tint32 iControlID, tint32 iResourceID, ge::SPos Pos, tint iFrames, tint32 iRows)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, iFrames, iRows);
	ge::IKnob* pKnob = ge::IKnob::Create();
	pKnob->Init(iControlID, pBmp);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pKnob), Pos);

	pKnob->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pKnob;
}

ge::IKnob2* CBasePane::CreateKnob2(tint32 iControlID, ge::SPos Pos, ge::SSize Size, tint32 iAngleStart, tint32 iAngleEnd, tint32 iResourceID)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, 1, 1);
	ge::IKnob2* pKnob = ge::IKnob2::Create();
	pKnob->Init(iControlID, pBmp, iAngleStart, iAngleEnd);
	pKnob->SetSize(Size);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pKnob), Pos);

	pKnob->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pKnob;
}

ge::ISlider* CBasePane::CreateSlider(tint32 iControlID, ge::SPos Pos, ge::SSize Size, tint32 iResourceID)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, 1, 1);
	ge::ISlider* pSlider = ge::ISlider::Create();
	pSlider->Init(iControlID, pBmp, NULL, ge::SPos(0, 0));
	pSlider->SetSize(Size);

	pSlider->SetResponsiveness(0.7f);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pSlider), Pos);

	pSlider->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pSlider;
}

ge::IRect* CBasePane::CreateRect(tint32 iControlID, ge::SRect Rect, ge::SRGB Colour)
{
	ge::IRect* pRect = ge::IRect::Create();
	pRect->Init(iControlID, Rect, Colour, GetWindow());

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pRect), ge::SPos(0, 0));

	return pRect;
}

ge::IInvert* CBasePane::CreateInvert(tint32 iControlID, ge::SRect Rect)
{
	ge::IInvert* pInvert = ge::IInvert::Create();
	pInvert->Init(iControlID, Rect, GetWindow());

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pInvert), ge::SPos(0, 0));

	return pInvert;
}

ge::ILine* CBasePane::CreateLine(tint32 iControlID, ge::SPos PosStart, ge::SPos PosEnd, ge::SRGB Colour)
{
	ge::ILine* pLine = ge::ILine::Create();
	pLine->Init(iControlID, PosStart, PosEnd, Colour, GetWindow());

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pLine), ge::SPos(0, 0));

	return pLine;
}

ge::IBitmap* CBasePane::CreateBitmap(tint32 iControlID, tint32 iResourceID, ge::SPos Pos)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, 1, 1);
	ge::SSize Size;
	pBmp->GetRealSize(Size);
	ge::IBitmap* pBitmap = ge::IBitmap::Create();
	pBitmap->Init(iControlID, pBmp);
	pBitmap->SetSize(Size);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pBitmap), Pos);

	return pBitmap;
}

ge::I2D* CBasePane::Create2D(tint32 iControlID, tint32 iResourceID, ge::SPos Pos, ge::SSize Size)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, 1, 1);
	ge::I2D* p2D = ge::I2D::Create();
	p2D->Init(iControlID, pBmp);
	p2D->SetSize(Size);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(p2D), Pos);

	return p2D;
}

ge::IURL* CBasePane::CreateURL(tint32 iControlID, ge::SPos Pos, ge::SSize Size, const std::string& sURL)
{
	ge::IURL* pURL = ge::IURL::Create();
	pURL->Init(iControlID, (const tchar*)(sURL.c_str()));
	pURL->SetSize(Size);
	mpPane->AddControl(dynamic_cast<ge::IControl*>(pURL), Pos);

	return pURL;
}

ge::IBitmap* CBasePane::CreateBitmapVU(tint32 iControlID, tint32 iResourceID, ge::SPos Pos, tint iNrOfFrames)
{
	ge::IBitmapResource* pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, iNrOfFrames, 1);
	ge::SSize Size;
	pBmp->GetRealSize(Size);
	ge::IBitmap* pBitmap = ge::IBitmap::Create();
	pBitmap->Init(iControlID, pBmp, iNrOfFrames);
	pBitmap->SetSize(Size);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pBitmap), Pos);

	return pBitmap;
}

ge::IButton* CBasePane::CreateButton(
					tint32 iControlID,
					tint32 iResourceID, 
					ge::SPos Pos, 
					tbool bMouseStateFrames,
					tint32 iRows)
{
	ge::IBitmapResource* pBmp;

	// Get bitmap
	pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, bMouseStateFrames ? 4 : 2, iRows);
	if (pBmp == NULL) {
		return NULL;
	}

	ge::IButton* pButton = ge::IButton::Create();
	pButton->Init(iControlID, pBmp);
	pButton->SetEventMouseUp();

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pButton), Pos);

	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	if (bMouseStateFrames) {
		pButton->EnableMouseStateFrames();
	}

	return pButton;
}

ge::IMultiStateButton* CBasePane::CreateMultiStateButton(tint32 iControlID, tint32 iResourceID,  ge::SPos Pos,  tint iNrOfStates, tint iNrOfFrames/* = 0*/, tint32 iRows /*= 1*/)
{
	ge::IBitmapResource* pBmp;

	if (iNrOfFrames==0) {
		iNrOfFrames = iNrOfStates;
	}

	// Get bitmap
	pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, iNrOfFrames, iRows);
	if (pBmp == NULL) {
		return NULL;
	}

	// Create button
	ge::IMultiStateButton* pButton = ge::IMultiStateButton::Create();
	pButton->Init(iControlID, pBmp, iNrOfStates);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pButton), Pos);

	pButton->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pButton;
}

ge::IMultiStateButton* CBasePane::Create2StateButton(
					tint32 iControlID, 
					tint32 iResourceID, 
					ge::SPos Pos,
					tbool bMouseStateFrames,
					tint32 iRows)
{
	ge::IMultiStateButton* pButton = CreateMultiStateButton(iControlID, iResourceID, Pos, 2, bMouseStateFrames ? 4 : 0, iRows);

	if (bMouseStateFrames) {
		pButton->EnableMouseStateFrames();
	}

	return pButton;
}

ge::IMultiStateButton* CBasePane::CreateRadioButton(tint32 iControlID, tint32 iResourceID, ge::SPos Pos, ge::IRadioButtons* pGroup, tint32 iRows)
{
	ge::IBitmapResource* pBmp;

	// Get bitmap
	pBmp = GetResMan()->GetBitmap(GetWindow(), iResourceID, 2, iRows);
	if (pBmp == NULL) {
		return NULL;
	}

	// Create button
	ge::IMultiStateButton* pButton = ge::IMultiStateButton::Create();
	pButton->Init(iControlID, pBmp, 2);

	pGroup->AddControl(dynamic_cast<ge::IControl*>(pButton), Pos);

	return pButton;
}

ge::IRadioButtons* CBasePane::CreateRadioButtonsGroup(tint32 iControlID, ge::SPos Pos)
{
	ge::IRadioButtons* pRadioButtons;
	pRadioButtons = ge::IRadioButtons::Create();
	pRadioButtons->Init(iControlID);
	pRadioButtons->DisableDrawingRectCheck();
	pRadioButtons->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pRadioButtons), Pos);

	return pRadioButtons;
}

ge::ISpinner* CBasePane::CreateSpinner(tint32 iControlID, ge::SPos Pos)
{
	ge::ISpinner* pSpinner;
	pSpinner = ge::ISpinner::Create();
	pSpinner->Init(iControlID);
	pSpinner->DisableDrawingRectCheck();
	pSpinner->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pSpinner), Pos);

	return pSpinner;
}

ge::IPeakMeter* CBasePane::CreatePeakMeter(tint32 iControlID, ge::SPos Pos,
				tint32 iSteps, ge::IPeakMeter::EDirection eDir /*= ge::IPeakMeter::EDirection::Up*/)
{
	ge::IPeakMeter* pPeakMeter;
	pPeakMeter = ge::IPeakMeter::Create();
	pPeakMeter->Init(iControlID, iSteps, eDir);
	pPeakMeter->DisableDrawingRectCheck();
	pPeakMeter->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pPeakMeter), Pos);

	CBasePaneStd* pPane = new CBasePaneStd(this, GetGUI());
	pPane->SetPane(dynamic_cast<ge::IPane*>(pPeakMeter));

	return pPeakMeter;
}

ge::IMouseTrap* CBasePane::CreateMouseTrap(tint32 iControlID,
	ge::IControl* pControlBelow /*= NULL*/
)
{
	ge::IMouseTrap* pTrap = ge::IMouseTrap::Create();
	pTrap->Init(iControlID, NULL);

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pTrap), ge::SPos(0,0));
	pTrap->PlaceOnTopOf(pControlBelow);

	return pTrap;
}

ge::IDropDownListBox* CBasePane::CreateDropDownListBox(
									tint32 iControlID,
									const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount],
									EFontType FontType,
									tint32 iFontID,
									tint32 iBitmapID, 
									const ge::IPopupMenu::SMenuItemList& List, 
									ge::SPos Pos,
									ge::SPos Offset,
									ge::SSize SizeText,
									ge::SRGB color)
{
	ge::IBitmapResource* pBmp;
	ge::IFontResource* pFont;

//	ge::SSize SizeText(60, 18);

	// Create the font
	pFont = CreateFont(FontType, iFontID, color);

	// Get bitmap
	pBmp = GetResMan()->GetBitmap(GetWindow(), iBitmapID,
		2,	// 2 frames (off / on)
		1);
	if (pBmp == NULL) {
		return NULL;
	}

	// Create button
	ge::IButton* pButton = ge::IButton::Create();
	if (pButton == NULL) {
		return NULL;
	}
	pButton->Init(ge::IControl::giNoID, pBmp);

	// Add text to button
	ge::SPos PosText(4, 1);
	PosText = PosText - Offset;
	pButton->AddText((tchar*)"", pFont, SizeText, &PosText, ge::IText::HorzAlignLeft);

	// Popup
	ge::IPopupMenu* pPopup = CreatePopupMenu(ge::IControl::giNoID, piResourceIDs, FontType, iFontID, List);

	ge::IDropDownListBox* pListBox = ge::IDropDownListBox::Create();
	pListBox->Init(iControlID);

	pListBox->AddControl(dynamic_cast<ge::IControl*>(pButton), Offset);
	pListBox->AddControl(dynamic_cast<ge::IControl*>(pPopup),  ge::SPos(0, 0));

	mpPane->AddControl(dynamic_cast<ge::IControl*>(pListBox), Pos);

	pListBox->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	// Lasse: Make popup ignore button area for mouse clicks
	//ge::SRect rectButton;
	//pButton->GetRect(rectButton);
	//pPopup->AddBlindSpot(rectButton);

	return pListBox;
}

ge::IPopupMenu* CBasePane::CreatePopupMenu(tint32 iControlID, const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount], EFontType FontType, tint32 iFontID, const ge::IPopupMenu::SMenuItemList& List)
{
	// Create popup
	ge::IPopupMenu* pPopup;
	pPopup = ge::IPopupMenu::Create();
	ge::IFontResource* pFont = CreateFont(FontType, iFontID);
	ge::IFontResource* pFontInv = CreateFont(FontType, iFontID);
	pPopup->Init(iControlID, GetWindow(), GetResMan(), (tint32*)piResourceIDs, pFont, pFontInv, List);

	return pPopup;
}


ge::IMultiStateDropDownListBox* CBasePane::CreateMultiButtonPopup(
			tint32 iCtrlIDForMultiButton,
			tint32 iResIDForMultiButton,
			tint iNrOfStates,
			tint iNrOfFrames,
			ge::SPos posRelativeForMultiButton,
			
			const tint32 piResourceIDs[ge::IPopupMenu::BitmapCount],
			EFontType FontType, tint32 iFontID,
			ge::IPopupMenu::SMenuItemList& List,
			tbool bAutoCopyButtonPicturesToPopup,

			ge::EMouseMsg msgTrigger,
			tbool bPopupAtClickedPosition
)
{
	// Create pane
	ge::IMultiStateDropDownListBox* pPopupPane = ge::IMultiStateDropDownListBox::Create();
	pPopupPane->Init(ge::IControl::giNoID, msgTrigger, bPopupAtClickedPosition);
	mpPane->AddControl(pPopupPane, ge::SPos(0,0));//posRelativeForMultiButton);
	ge::SSize sizeOfContainer;
	mpPane->GetSize(sizeOfContainer);
	pPopupPane->SetSize(sizeOfContainer);

	// Add multi-state button
	ge::IMultiStateButton* pBtn = CreateMultiStateButton(iCtrlIDForMultiButton, iResIDForMultiButton, posRelativeForMultiButton, iNrOfStates, iNrOfFrames);
	
	// Add mouse trap (to catch trigger)
	{
		// Lasse, mod 2008-04-19 - this would crash!
		/*
//		ge::IMouseTrap* pMouseTrap = CreateMouseTrap(ge::IControl::giNoID, pPopupPane, pBtn);
		ge::IMouseTrap* pMouseTrap;
		// Listen for mouse message
		pMouseTrap->EnableOneTrap(msgTrigger);
		*/
		ge::IMouseTrap* pMouseTrap = CreateMouseTrap(ge::IControl::giNoID, pBtn);
		// Listen for mouse message
		pMouseTrap->EnableOneTrap(msgTrigger);
		// .. Lasse
	}

	// Add popup menu
	{
		if (bAutoCopyButtonPicturesToPopup) {
			for (tint32 i = 0; i < List.iItemCount; i++) {
				if (i<iNrOfStates) {
					ge::IBitmapResource* pBitmap = GetResMan()->GetBitmap(GetWindow(), iResIDForMultiButton, iNrOfStates, 1);
					ge::IPopupMenu::SMenuItem* pItem = &List.pItems[i];
					pItem->pBitmap = pBitmap;
					pItem->iBitmapFrame = i;
					pItem->iNrOfBitmapFrames = iNrOfStates;
				}
			}
		}

		// Create pop-up control
		ge::IPopupMenu* pPopup = CreatePopupMenu(ge::IControl::giNoID, piResourceIDs, FontType, iFontID, List);
		// Add (position is irrelevant - will be repositioned dynamically)
		pPopupPane->AddControl(pPopup, ge::SPos(0,0));
		// Initially hidden
		pPopup->SetVisible(false);
	}

	return pPopupPane;
}

ge::IText* CBasePane::CreateText(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont, 
					std::string sText, 
					ge::IText::EHorizontalAlignment HorzAlignment, 
					ge::IText::EVerticalAlignment VertAlignment)
{
	ge::IText* pText = ge::IText::Create();
	pText->Init(iControlID, pFont);
	pText->SetSize(Size);

	mpPane->AddControl(pText, Pos);

	pText->SetHorzAlignment(HorzAlignment);
	pText->SetVertAlignment(VertAlignment);

	pText->EnableTextLimiting();

	pText->SetText((tchar*)sText.c_str());

	pText->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pText;
}

ge::ITextField* CBasePane::CreateTextField(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont, 
					std::string sText, 
					ge::ITextField::EHorizontalAlignment HorzAlignment, 
					ge::ITextField::EVerticalAlignment VertAlignment)
{
	ge::ITextField* pTextField = ge::ITextField::Create();
	pTextField->Init(iControlID, pFont);
	pTextField->SetSize(Size);

	mpPane->AddControl(pTextField, Pos);

	pTextField->SetHorzAlignment(HorzAlignment);
	pTextField->SetVertAlignment(VertAlignment);

	pTextField->EnableTextLimiting();

	pTextField->SetText((tchar*)sText.c_str());

	pTextField->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pTextField;
}

ge::IText* CBasePane::CreateDisplay(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont,  tbool bEdiable)
{
	ge::IText* pRet = CreateText(iControlID,
		Pos,
		Size,
		pFont,
		std::string(""),
		ge::IText::HorzAlignRight,
		ge::IText::VertAlignCenter);

	if(bEdiable)
		pRet->MakeEditable();

	return pRet;
}


ge::ITextList* CBasePane::CreateTextList(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont, 
					const tchar* pszEnumList,
					tchar cEnumDelimiter,
					ge::IText::EHorizontalAlignment HorzAlignment /*= ge::IText::HorzAlignLeft*/, 
					ge::IText::EVerticalAlignment VertAlignment /*= ge::IText::VertAlignTop*/)
{
	ge::ITextList* pTextList = ge::ITextList::Create();
	pTextList->Init(iControlID, pFont, pszEnumList, cEnumDelimiter);
	pTextList->SetSize(Size);

	mpPane->AddControl(pTextList, Pos);

	pTextList->SetHorzAlignment(HorzAlignment);
	pTextList->SetVertAlignment(VertAlignment);

	pTextList->EnableTextLimiting();

	//pTextList->SetText((tchar*)sText.c_str());

	pTextList->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pTextList;
}

ge::IListBox* CBasePane::CreateListBox(tint32 iControlID, 
					ge::SPos Pos, 
					ge::SSize Size, 
					ge::IFontResource* pFont)
{
	ge::IListBox* pListBox = ge::IListBox::Create();
	pListBox->Init(iControlID, pFont);
	pListBox->SetSize(Size);

	mpPane->AddControl(pListBox, Pos);

	pListBox->AddListener(dynamic_cast<ge::IEventReceiver*>(this));

	return pListBox;
}

ge::IFontResource* CBasePane::CreateFont(EFontType Type, tint32 iFontResID, ge::SRGB Color)
{
	switch(Type) {
		case Arial10:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial10Regular) / sizeof(piArial10Regular[0]), (tint*)piArial10Regular, Color);
		case Arial10Narrow:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial10Narrow) / sizeof(piArial10Narrow[0]), (tint*)piArial10Narrow, Color);
		case Arial12:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial12Regular) / sizeof(piArial12Regular[0]), (tint*)piArial12Regular, Color);
		case Arial12Bold:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial12Bold) / sizeof(piArial12Bold[0]), (tint*)piArial12Bold, Color);
		case Arial14:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial14Regular) / sizeof(piArial14Regular[0]), (tint*)piArial14Regular, Color);
		case Arial20:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial20Regular) / sizeof(piArial20Regular[0]), (tint*)piArial20Regular, Color);
		case Arial14Bold:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial14Bold) / sizeof(piArial14Bold[0]), (tint*)piArial14Bold, Color);
		case Arial18Bold:
			return GetResMan()->GetFont(GetWindow(), iFontResID, sizeof(piArial18Bold) / sizeof(piArial18Bold[0]), (tint*)piArial18Bold, Color);
		case Generic128:
			// 128 chars in font (actual count +32, since the first 32 are invisible control chars)
			return GetResMan()->GetFont(GetWindow(), iFontResID, 128, NULL, Color);
		case Generic256:
			// 256 chars in font (actual count +32, since the first 32 are invisible control chars)
			return GetResMan()->GetFont(GetWindow(), iFontResID, 256, NULL, Color);
		case Generic:
			// Between 33 and 256 chars in font (actual count +32, since the first 32 are invisible control chars)
			return GetResMan()->GetFont(GetWindow(), iFontResID, -1, NULL, Color);

		default:
			// Keep compiler happy
			return NULL;
	}
}

ge::IPanner* CBasePane::CreatePanner(tint32 iXCtrlID, tint32 iYCtrlID, tint32 iBackResID, tint32 iHndlResID, ge::SPos posBack, ge::SPos posOfsArea, ge::SSize sizeArea, ge::IPaneSwitcher* pPane)
{
	ge::IBitmapResource* pBmpResBack = GetResMan()->GetBitmap(GetWindow(), iBackResID, 1, 1);
	ge::IBitmapResource* pBmpResHndl = GetResMan()->GetBitmap(GetWindow(), iHndlResID, 1, 1);
	ge::IPanner* pPanner = ge::IPanner::Create();
	pPanner->Init(iXCtrlID, iYCtrlID, pBmpResBack, pBmpResHndl, posOfsArea, sizeArea, true);
	// Lasse, mod 2008-04-30 - allow for no pane-switcher
	//pPane->AddControl(pPanner, posBack);
	if (pPane) {
		// Add panner to pane-switcher
		pPane->AddControl(pPanner, posBack);
	}
	else {
		// Add panner to base-pane (like default control behaviour)
		mpPane->AddControl(pPanner, posBack);
	}
	// .. Lasse
	pPanner->AddListener(dynamic_cast<ge::IEventReceiver*>(this));
	return pPanner;
}
/*
ge::IPanner* CBasePane::CreatePanner(tint32 iXCtrlID, tint32 iYCtrlID, tint32 iBackResID, tint32 iHndlResID, ge::SPos posBack, ge::SPos posOfsArea, ge::SSize sizeArea)
{
	ge::IBitmapResource* pBmpResBack = GetResMan()->GetBitmap(GetWindow(), iBackResID, 1, 1);
	ge::IBitmapResource* pBmpResHndl = GetResMan()->GetBitmap(GetWindow(), iHndlResID, 1, 1);
	ge::IPanner* pPanner = ge::IPanner::Create();
	pPanner->Init(iXCtrlID, iYCtrlID, pBmpResBack, pBmpResHndl, posOfsArea, sizeArea, true);
	mpPane->AddControl(pPanner, posBack);
	pPanner->AddListener(dynamic_cast<ge::IEventReceiver*>(this));
	return pPanner;
}
*/
ge::IPane* CBasePane::CreatePane( ge::SPos Pos, ge::SSize Size )
{
	ge::IPane* pNewPane = ge::IPane::Create();
//	pNewPane->Init();
	pNewPane->SetSize(Size);

	mpPane->AddControl(pNewPane, Pos);

	CBasePaneStd* pPane = new CBasePaneStd(this, GetGUI());
	pPane->SetPane(pNewPane);

	return pNewPane;
}

ge::IScrollPane* CBasePane::CreateScrollPane(tint32 iControlID, ge::SPos Pos, ge::SSize Size, tint32 iTime )
{
	ge::IScrollPane* pScrollPane = ge::IScrollPane::Create();
	pScrollPane->Init(iControlID, iTime);
	pScrollPane->SetSize(Size);

	mpPane->AddControl(pScrollPane, Pos);

	CBasePaneStd* pPane = new CBasePaneStd(this, GetGUI());
	pPane->SetPane(pScrollPane);

	return pScrollPane;
}

ge::IPaneSwitcher* CBasePane::CreatePaneSwitcher(tint32 iControlID, ge::SPos Pos )
{
	ge::IPaneSwitcher* pPaneSwitcher = ge::IPaneSwitcher::Create();
	pPaneSwitcher->Init(iControlID);
	mpPane->AddControl(pPaneSwitcher, Pos);

	CBasePaneStd* pPane = new CBasePaneStd(this, GetGUI());
	pPane->SetPane(pPaneSwitcher);

	return pPaneSwitcher;
}


// Lasse, out-commented
/*
ge::IDropDownListBox* CBasePane::CreatePop( tint32 iControlID, tint32 iBitmapID,  const ge::IPopupMenu::SMenuItemList& List, ge::SPos Pos)
{

	// Resources
	tint32 piIDs[ge::IPopupMenu::BitmapCount];
	piIDs[ge::IPopupMenu::BitmapFolderArrow]	= IDB_Button_Popup;
	piIDs[ge::IPopupMenu::BitmapTop]			= IDB_PopupBorder;
	piIDs[ge::IPopupMenu::BitmapCenter]			= IDB_POPUP_BODY;
	piIDs[ge::IPopupMenu::BitmapCenterInv]		= IDB_POPUP_BODY_INVERTED;
	piIDs[ge::IPopupMenu::BitmapBottom]			= IDB_PopupBorder;
	ge::IDropDownListBox* pListBox = CreateDropDownListBox( iControlID, piIDs, Generic128, IDF_Fuxley_712, iBitmapID, List, Pos);
	pListBox->DontSetText();
	return pListBox;
}
*/
// .. Lasse

void CBasePane::Redraw_Pane_Rect()
{
	ge::SPos Pos;
	mpPane->GetPos(Pos);
	ge::SSize Size;
	mpPane->GetSize(Size);
	ge::SRect Rct(Pos, Size);
	mpPane->Redraw(Rct);

}

void CBasePane::EventEditBegin()
{
}

void CBasePane::EventEditEnd()
{
}


