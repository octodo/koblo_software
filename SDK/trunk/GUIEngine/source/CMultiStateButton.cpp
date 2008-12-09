/*!	\file CMultiStateButton.cpp
	\author Michael Olsen
	\date 19/Jan/2005
	\date 19/Jan/2005
*/


#include "geInternalOS.h"


CMultiStateButton::CMultiStateButton()
	: mpStateInfos(NULL),
	mbEventMouseUp(false),
	mbButtonHit(false),
	mbInverted(false), 
	mbMouseStateFrames(false), 
	mbCycling(false), 
	mbMouseReleased(true),
	mpState(0)
{
	SetAutomaticResize(false);
}

CMultiStateButton::~CMultiStateButton()
{
	if (mpStateInfos) {
		for (tint iState = 0; iState < miNrOfStates; iState++) {
			if (mpStateInfos[iState].pText) {
				mpStateInfos[iState].pText->Destroy();
			}
		}

		delete[] mpStateInfos;
	}
}

IMultiStateButton* IMultiStateButton::Create()
{
	return dynamic_cast<IMultiStateButton*>(new CMultiStateButton());
}

void CMultiStateButton::Destroy()
{
	delete dynamic_cast<CMultiStateButton*>(this);
}

void CMultiStateButton::Init(tint32 iID, IBitmapResource* pBitmap, tint iNrOfStates)
{
	CControl::Init(iID, pBitmap);

	pBitmap->GetRealSize(mSize);

	miNrOfStates = iNrOfStates;

	mpStateInfos = new SStateInfo[miNrOfStates];
	tint iState;
	for (iState = 0; iState < miNrOfStates; iState++) {
		mpStateInfos[iState].pText = NULL;
	}
}

void CMultiStateButton::EnableMouseStateFrames()
{
	mbMouseStateFrames = true;
}


void CMultiStateButton::Cycle(tbool bCycle /*= true*/, tint32 iCycleIntervalMs /*= 250*/)
{
	if (
		(bCycle!=mbCycling)
		||
		( (bCycle) && (iCycleIntervalMs!=miCycleIntervalMs) )
	)
	{
		miCycleIntervalMs = iCycleIntervalMs;
		miCycleLastIx0Ms = ITime::GetTimeMS();
		mbCycling = bCycle;
		if (mbCycling) {
			miCyclingFrameCurrent = GetValue();
		}
	}
} // CMultiStateButton::Cycle


tbool CMultiStateButton::IsCycling()
{
	return mbCycling;
} // CMultiStateButton::IsCycling()


void CMultiStateButton::AddText(tint iStateValue, const tchar* pszText, IFontResource* pFont)
{
	IText* pText = IText::Create();
	pText->Init(IControl::giNoID, pFont);

	pText->SetPos(SPos(0, 1));
	pText->SetSize(GetSize());

	pText->SetParentPane(dynamic_cast<IPane*>(this));

	pText->SetHorzAlignment(IText::HorzAlignCenter);
	pText->SetVertAlignment(IText::VertAlignCenter);

	mpStateInfos[iStateValue].pText = pText;

	pText->SetText(pszText);
}

void CMultiStateButton::SetEventMouseUp()
{
	mbEventMouseUp = true;
}

void CMultiStateButton::Invert()
{
	mbInverted = true;
}

void CMultiStateButton::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	tint iFrame = GetValue();
	if (mbCycling) {
		iFrame = miCyclingFrameCurrent;
	}

	if (mbInverted) {
		iFrame = miNrOfStates - iFrame - 1;
	}

	tbool bGhosted = false;
	if (mbCanGhost) {
		// Control can ghost...
		if (mbEnabled == false) {
			// ... and is currently disabled
			bGhosted = true;
		}
	}

	if (mbMouseStateFrames) {
		// Amount of frames is doubled
		iFrame *= 2;

		if (mbMouseReleased == false) {
			// Mouse is still down
			iFrame--;
			if (iFrame == -1) {
				iFrame = miNrOfStates * 2 - 1;
			}
		}

		DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), iFrame, miNrOfStates * 2, bGhosted);
	}
	else {
		DrawBitmapFrame(GetBitmapResource(), rUpdate, SRect(GetPos(), SSize(mSize.iCX, mSize.iCY)), iFrame, miNrOfStates, bGhosted);

		if (mpStateInfos[iFrame].pText) {
			mpStateInfos[iFrame].pText->OnDraw(rUpdate);
		}
	}
}

tbool CMultiStateButton::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	if (MouseMsg == LeftButtonDown || MouseMsg == LeftButtonDblClk) {
		if (GetRect().Inside(Pos)) {
			mbButtonHit = true;
			//Get Value
			mpState = GetValue();
			// Calculate new value
			mpState++;
			if (mpState == miNrOfStates) {
				mpState = 0;
			}
			/*
			tint iValue = GetValue();
			iValue++;
			if (iValue == miNrOfStates) {
				iValue = 0;
			}
			*/
			if (mbEventMouseUp == false) {
				SetValue(mpState);
			}
			//SetValue(iValue);

			CControl::Redraw();

			if (mbMouseStateFrames) {
				GetParentWindow()->GetMouseFocus(dynamic_cast<IControl*>(this));

				mbMouseReleased = false;
			}

			return true;
		}
		else {
			return false;
		}
	}

	if (MouseMsg == LeftButtonUp) {
		if (GetRect().Inside(Pos)) {
			if (mbButtonHit) {
				if (mbEventMouseUp) {
					SetValue(mpState);
				}
				mbButtonHit = false;
			}
		}
		CControl::Redraw();

		if (mbMouseReleased == false) {
			ASSERT(mbMouseStateFrames);

			GetParentWindow()->ReleaseMouseFocus();

			mbMouseReleased = true;
		}
	}

	return false;
}

tbool CMultiStateButton::OnKeyDown(EKey /*Key*/)
{
	return false;
}

void CMultiStateButton::OnTimer()
{
	CControl::OnTimer();

	if( !IsVisible() )
		return;

	if (mbCycling) {
		tint32 iNowMs = ITime::GetTimeMS();
		tint32 iElapsedMs = iNowMs - miCycleLastIx0Ms;
		tint32 iFrameAbsolut = (iElapsedMs / miCycleIntervalMs) + 1;
		int iCycleNew = iFrameAbsolut % miNrOfStates;
		if (iCycleNew != miCyclingFrameCurrent) {
			if (iCycleNew >= miNrOfStates) {
				iCycleNew -= miNrOfStates;
				miCycleLastIx0Ms = iNowMs;
			}
			miCyclingFrameCurrent = iCycleNew;
			CControl::Redraw();
		}
	}
}

