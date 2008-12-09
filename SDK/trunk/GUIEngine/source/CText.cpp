/*!	\file CText.cpp
	\author Michael Olsen
	\date 01/Jan/2004
	\date 01/Jan/2004
*/

#include "geInternalOS.h"

CText::CText()
	: mAlignmentHorz(HorzAlignLeft), mAlignmentVert(VertAlignTop), mbLimiting(false),
	mbEditable(false), mbEditWhenDoubleClick(false), mbStopEditWhenDoubleClick(false), mbEditing(false),
	mbMouseDown(false), mbMouseVisible(true),
	mbCursorOn(false), miCursorCounter(0),
	mbClicksOutsideIsSuccess(false), mbPassword(false), mbNumericOnly(false),
	mbKeyScrollingEnabled(false), miKeyScrollUp(1), miKeyScrollDown(1), miKeyScrollBigUp(10), miKeyScrollBigDown(10),
	mbMouseDraggingEnabled(false), miMouseDragUp(1), miMouseDragDown(1), miMouseDragBigUp(10), miMouseDragBigDown(10)
{
	muiTimerLast = ITime::GetTimeMS();
	miCursorBlinkRateMs = 500;

	meBufferedSizeOptimization = BUFFERED_SIZE_DISABLED;
	mpFontRes = NULL;
}

CText::~CText()
{
}

IText* IText::Create()
{
	return dynamic_cast<IText*>(new CText());
}

void CText::Destroy()
{
	delete dynamic_cast<CText*>(this);
}

void CText::Init(tint32 iID, IFontResource* pFont)
{
	CControl::Init(iID, dynamic_cast<IBitmapResource*>(pFont));

	pFont->GetRealSize(mSize);
	mpFontRes = pFont;
}

void CText::ChangeFont(IFontResource* pFont)
{
	// Make sure we have a font already
	ASSERT(mpResource);

	mpResource->DecRefCount();

	mpResource = dynamic_cast<IBitmapResource*>(pFont);
	mpFontRes = pFont;
}

void CText::SetText(const tchar* pszText)
{
	if ((pszText) && (*pszText)) {
		// Set text
		if (mbLimiting) {
			std::string sTextNew = std::string(pszText);
			DoLimiting(sTextNew);

			if (msTextCur.compare(sTextNew) != 0) {
				msTextCur = sTextNew;
				Redraw();
			}
		}
		else {
			if (strcmp(pszText, msTextCur.c_str()) != 0) {
				msTextCur = pszText;
				Redraw();
			}
		}
	}
	else {
		// Clear text
		if (msTextCur.length() != 0) {
			msTextCur = "";
			Redraw();
		}
	}
}

void CText::GetText(tchar* pszText)
{
	strcpy((char*)pszText, msTextCur.c_str());
}

void CText::SetHorzAlignment(EHorizontalAlignment Alignment)
{
	mAlignmentHorz = Alignment;
}

void CText::SetVertAlignment(EVerticalAlignment Alignment)
{
	mAlignmentVert = Alignment;
}

void CText::EnableTextLimiting(tbool bEnable /*= true*/)
{
	mbLimiting = bEnable;
}

void CText::MakeEditable(tbool bEditable /*= true*/)
{
	mbEditable = bEditable;
}

void CText::EditWhenDoubleClick()
{
	mbEditWhenDoubleClick = true;
}

void CText::StopEditWhenDoubleClick()
{
	mbStopEditWhenDoubleClick = true;
}

void CText::SetPassword()
{
	mbPassword = true;
}

void CText::SetNumericOnly()
{
	mbNumericOnly = true;
}

void CText::GetDrawRect(SRect& rRect, const std::string& sText)
{
	// Default position to draw (alignment: left / top)
	SPos DrawPos(GetPos());

	if (mAlignmentHorz == HorzAlignRight) {
		//tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextWidth((const tchar*)(sText.c_str()));
		tint iPixels = mpFontRes->GetTextWidth(sText.c_str());
		tint iOffset = GetSize().iCX - iPixels;
		DrawPos.iX += iOffset;
	}
	if (mAlignmentHorz == HorzAlignCenter) {
		//tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextWidth((const tchar*)(sText.c_str()));
		tint iPixels = mpFontRes->GetTextWidth(sText.c_str());
		tint iOffset = (GetSize().iCX - iPixels) / 2;
		DrawPos.iX += iOffset;
	}

	if (mAlignmentVert == VertAlignBottom) {
		//tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextHeight();
		tint iPixels = mpFontRes->GetTextHeight();
		tint iOffset = GetSize().iCY - iPixels;
		// Lasse - modified 2007-06-01
		//DrawPos.iY += iOffset;
		if (iOffset > 0) {
			DrawPos.iY += iOffset;
		}
		// .. Lasse
	}
	if (mAlignmentVert == VertAlignCenter) {
		// Lasse - modified 2007-06-01
		//tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextHeight();
		//tint iOffset = (GetSize().iCY - iPixels) / 2;
		//DrawPos.iY += iOffset;
		//IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());
		tint iTopLine = mpFontRes->GetFontTopLine();
		tint iBaseLine = mpFontRes->GetFontBaseLine();
		tint iFontHeight = iBaseLine - iTopLine + 1;
		//tint iFontMid = iTopLine + (iFontHeight / 2);
		tint iOffset = ((GetSize().iCY - iFontHeight) / 2) - iTopLine;
		if (iOffset > 0) {
			DrawPos.iY += iOffset;
		}
		// .. Lasse
	}

	// Build rect with compensated size (so we don't exceed our bounds)
	rRect.iX = DrawPos.iX;
	rRect.iY = DrawPos.iY;
	rRect.iCX = GetSize().iCX - (DrawPos.iX - GetPos().iX);
	rRect.iCY = GetSize().iCY - (DrawPos.iY - GetPos().iY);
}



void CText::SpeedOptimize_RedrawUsingFixedSize(SSize sizeFixed, tbool bUseFixedPosToo /*= true*/)
{
	if ((sizeFixed.iCX <= 0) || (sizeFixed.iCY <= 0)) {
		// Can't make a valid rect - disable speed optimization
		meBufferedSizeOptimization = BUFFERED_SIZE_DISABLED;
	}
	else {
		// This size is OK - enable speed optimization
		SPos posAbs;
		GetAbsPos(posAbs);
		mRectBuffered.iX = posAbs.iX;
		mRectBuffered.iY = posAbs.iY;
		mRectBuffered.iCX = sizeFixed.iCX;
		mRectBuffered.iCY = sizeFixed.iCY;

		meBufferedSizeOptimization = (bUseFixedPosToo) ? BUFFERED_SIZE_AND_POS_ENABLED : BUFFERED_SIZE_ENABLED;
	}
}

void CText::SpeedOptimize_RedrawUsingFirstNonZeroSize(tbool bEnable /*= true*/, tbool bUseFixedPosToo /*= true*/)
{
	if (!bEnable) {
		// Disable speed optimization
		meBufferedSizeOptimization = BUFFERED_SIZE_DISABLED;
	}
	else {
		// Prepare for speed optimization - will take effect after next OnDraw(..) event where string value differs from ""
		meBufferedSizeOptimization = (bUseFixedPosToo) ? BUFFERED_SIZE_AND_POS_CALC : BUFFERED_SIZE_CALC;
	}
}

void CText::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	SRect Rect;
	std::string sTextEditing;
	if (mbEditing) {
		sTextEditing = msTextCurEditing;
		if (mbEditing_RecalcRect) {
			mbEditing_RecalcRect = false;

//			DoLimiting(sTextEditing);
//			GetDrawRect(Rect, sTextEditing);
			ge::SSize Size;
			GetSize(Size);
			GetDrawRect(Rect, sTextEditing);

			// While too big: Cut off first char - that should be rethought!
			while (Rect.iCX > Size.iCX) {
				sTextEditing = sTextEditing.substr(1);
				GetDrawRect(Rect, sTextEditing);
			}

			mRectBuffered.iX = Rect.iX;
			mRectBuffered.iY = Rect.iY;
			if (Rect.iCX > mRectBuffered.iCX)
				mRectBuffered.iCX = Rect.iCX;
			if (Rect.iCY > mRectBuffered.iCY)
				mRectBuffered.iCY = Rect.iCY;

			// Remember that calculation was done
			if (meBufferedSizeOptimization == BUFFERED_SIZE_CALC)
				meBufferedSizeOptimization = BUFFERED_SIZE_ENABLED;
			if (meBufferedSizeOptimization == BUFFERED_SIZE_AND_POS_CALC)
				meBufferedSizeOptimization = BUFFERED_SIZE_AND_POS_ENABLED;
		}
		else {
			// Text didn't change - no need to recalculate length
			Rect = mRectBuffered;
		}
	}
	else {
		// Not editing - just display text

		switch (meBufferedSizeOptimization) {

			// Calculate size (and maybe pos) of text - hopefully for the last time
			case BUFFERED_SIZE_CALC:
			case BUFFERED_SIZE_AND_POS_CALC:
				{
					GetDrawRect(Rect, msTextCur);
					// Buffer this size and maybe pos
					if (msTextCur.length() > 0) {
						// Should we buffer absolute pos as well?
						tbool bBufferPos = (meBufferedSizeOptimization == BUFFERED_SIZE_AND_POS_CALC);
						// Set buffered size (and maybe pos)
						SpeedOptimize_RedrawUsingFixedSize(
							SSize(Rect.iCX, Rect.iCY),
							bBufferPos
						);
					}
				}
				break;

			// Use buffered size - get actual absolute pos
			case BUFFERED_SIZE_ENABLED:
				{
					SPos posAbs;
					GetAbsPos(posAbs);
					Rect.iX = posAbs.iX;
					Rect.iY = posAbs.iY;
					Rect.iCX = mRectBuffered.iCX;
					Rect.iCY = mRectBuffered.iCY;
				}
				break;

			// Use buffered absolute pos and size
			case BUFFERED_SIZE_AND_POS_ENABLED:
				{
					Rect = mRectBuffered;
				}
				break;

			default:
			/*case BUFFERED_SIZE_DISABLED:*/
				{
					// Original behavior: Always Recalculate size of text
					GetDrawRect(Rect, msTextCur);
				}
				break;

		}
	}

	tchar* pszDisplayText;
	tint32 iDisplayLen;
	if (mbEditing) {
		pszDisplayText = (tchar*)sTextEditing.c_str();
		iDisplayLen = sTextEditing.length();
	}
	else {
		pszDisplayText = (tchar*)msTextCur.c_str();
		iDisplayLen = msTextCur.length();
	}
	if (mbPassword) {
		pszDisplayText = new tchar[iDisplayLen + 1];
		memset(pszDisplayText, '*', iDisplayLen);
		pszDisplayText[iDisplayLen] = '\0';
	}

	//DrawFont(dynamic_cast<IFontResource*>(GetBitmapResource()), rUpdate, Rect, pszDisplayText);
	DrawFont(mpFontRes, rUpdate, Rect, pszDisplayText);

	if (mbEditing) {
		// We're currently editing

		switch (mSelectionType) {
			case SelectionNone:
				{
					// No selection, so draw blinking cursor
					if (mbCursorOn) {
						if (mbEditing_RecalcCursorPos) {
							mbEditing_RecalcCursorPos = false;

							// String of text which is before cursor
							std::string s = pszDisplayText;
							tint32 iToErase = iDisplayLen - (miCursorPos + 1);
							if (iToErase > 0)
								s.erase(miCursorPos + 1, iToErase);

							//IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());
							// Width of text which is before cursor
							tint iTextWidth = mpFontRes->GetTextWidth((const tchar*)(s.c_str()));
							// The position of the cursor is the text drawing position + the width of text before cursor - 1
							mposCursor = (SPos)Rect + SPos(iTextWidth, 0);
							if (miCursorPos > -1)
								mposCursor.iX -= 1;
						}

						// Cursor is 2 bits wide and same height as font
						SSize CursorSize(2, mpFontRes->GetTextHeight());

						// Draw the cursor
						IDrawPrimitives* pPrimitive = IDrawPrimitives::Create();
						pPrimitive->Init(GetParentWindow());
						SRect CursorRect(mposCursor, CursorSize);
						// Get color
						CFontResource* pFont = dynamic_cast<CFontResource*>(mpResource);
						if (pFont) {
							pPrimitive->DrawRect(rUpdate, CursorRect, pFont->GetColour());
						}
						else {
							pPrimitive->DrawRect(rUpdate, CursorRect, SRGB(10, 10, 10));
						}
					}
				}
				break;

			case SelectionAll:
				{
					// All is selected, so inverse rectangle
					IDrawPrimitives* pPrimitive = IDrawPrimitives::Create();
					pPrimitive->Init(GetParentWindow());

					SRect Rect;
					GetRect(Rect);
					pPrimitive->InvertRect(rUpdate, Rect);
				}
				break;
		}
	}

	if (mbPassword) {
		delete[] pszDisplayText;
	}
}


tbool CText::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	SRect Rect;
	GetRect(Rect);

	if (mbEditable == false) {
		if (Rect.Inside(Pos)) {
			// Click was inside our rect
			if (MouseMsg == LeftButtonDown) {
				// Send a message, so the listeners will know the control was clicked
				NotifyValueChange();
			}
		}

		// Text is not editable, so return
		return false;
	}

	if (mbEditing) {
		// We're currently editing, so a mouse click outside the control will stop editing
		if ((MouseMsg == LeftButtonDown)||(MouseMsg == RightButtonDown)) {
			if (Rect.Inside(Pos)) {
				// Click was inside control
				mSelectionType = SelectionNone;

				// Calculate the cursor pos
				// The drawing rectangle of the text in window coordinates
				SRect Rect;
				GetDrawRect(Rect, msTextCurEditing);
				// The offset of the mouse position, relative to text start
				SPos PosTextOffset;
				PosTextOffset = Pos - (SPos)Rect;
				tint32 iNewIx = GetCharacterIndex(msTextCurEditing, PosTextOffset.iX);
				if (iNewIx != miCursorPos) {
					miCursorPos = iNewIx;
					mbEditing_RecalcCursorPos = true;
				}

				// Prepare for dragging
				StartMouseDragging(Pos);

				SyncCursorOnNow();
				Redraw();
			}
			else {
				// Click was not inside control
				StopEditing(mbClicksOutsideIsSuccess);

				// Tell the window to resend mouse event (to avoid the user having to click twice)
				GetParentWindow()->ResendMouseEvent();
			}
		}

		else if ((MouseMsg == LeftButtonUp)||(MouseMsg == RightButtonUp)) {
			// End dragging operation (if any)
			StopMouseDragging();
		}

		else if (MouseMsg == MouseMove) {
			// Maybe perform dragging operation
			if (mDragState == MouseDragDragging) {
				de::IDisplayConverter* pDispConv = GetDisplayConverter();
				if (pDispConv) {
					if (mbMouseVisible) {
						// Hide mouse while dragging
						GetParentWindow()->HideMouse();
						mbMouseVisible = false;
					}

					tbool bFineAdjust = GetParentWindow()->FineAdjustPressed();
					if (bFineAdjust != mbFineAdjustLast) {
						miValueDragStart = GetValue();
						mPosDragStart = Pos;
						mMouseMoved = SSize(0, 0);
						miMouseMoveCounter = 0;
						mbFineAdjustLast = bFineAdjust;
					}

					// Compensate mouse position with the amount we've moved the mouse
					mMousePos = Pos - SPos(mMouseMoved.iCX, mMouseMoved.iCY);

					tint32 iDelta = (mPosDragStart.iY - mMousePos.iY);
					// Adjust sensitivity
					if (iDelta>0) {
						// Dragging upwards
						if (bFineAdjust) {
							if (miMouseDragUp>0) {
								iDelta *= miMouseDragUp;
							}
						}
						else {
							if (miMouseDragBigUp>0) {
								iDelta *= miMouseDragBigUp;
							}
						}
					}
					else {
						// Dragging downwards
						if (bFineAdjust) {
							if (miMouseDragDown>0) {
								iDelta *= miMouseDragDown;
							}
						}
						else {
							if (miMouseDragBigDown>0) {
								iDelta *= miMouseDragBigDown;
							}
						}
					}
					tint32 iGUIValue = miValueDragStart + iDelta;

					// Set value without thinking of whether it's in range
					SetValue(iGUIValue);
					// Read back value (it is now in range)
					iGUIValue = GetValue();
					// 
					tint32 iParmValue = (mpDataConverter) ? mpDataConverter->GUI2Parm(iGUIValue) : iGUIValue;

					// Make value show
					tchar psz[128];
					pDispConv->Value2String(iParmValue, (tchar*)psz);
					msTextCur = std::string((char*)psz);
					msTextCurEditing = msTextCur;
					SetText((tchar*)(msTextCurEditing.c_str()));
					mSelectionType = SelectionAll;
					miCursorPos = 0;

#ifdef WIN32
					// Move the mouse position, to avoid leaving the screen area
					if (miMouseMoveCounter++ == 10) {
						GetParentWindow()->SetMousePos(mPosDragStart);
						mMouseMoved.iCX += mPosDragStart.iX - Pos.iX;
						mMouseMoved.iCY += mPosDragStart.iY - Pos.iY;
						miMouseMoveCounter = 0;
					}
#endif
				}

				return true;
			}
		}
		else if (MouseMsg == LeftButtonDblClk) {
			if (Rect.Inside(Pos) == true) {
				SetToDefaultValue();
				SetConvertedValue(GetValue(), true);
				msTextCurEditing = msTextCur;
				miCursorPos = 0;
				mSelectionType = SelectionAll;
				Redraw();

				if (mbStopEditWhenDoubleClick) {
					StopEditing(true);
				}

				return true;
			}
		}


		return true;
	}

	// We only get here if we're not currently editing

	if (Rect.Inside(Pos)) {
		// Click was inside our rect
		if ((MouseMsg == LeftButtonDown && mbEditWhenDoubleClick == false) ||
			(MouseMsg == LeftButtonDblClk && mbEditWhenDoubleClick == true)) {
			StartEditing();

			// Send a message, so the listeners will know the control was clicked
			NotifyValueChange();

			// Prepare for dragging
			StartMouseDragging(Pos);

			return true;
		}
	}
	
	return false;
} // CText::OnMouse


void CText::StartMouseDragging(const SPos& rPos)
{
	if (mbMouseDraggingEnabled) {
		mbMouseDown = true;
		XlatCurEditingText(miValueDragStart);
		mPosDragStart = rPos;
		mDragState = MouseDragDragging;
		mMouseMoved = SSize(0, 0);
		miMouseMoveCounter = 0;
		mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
	}
} // CText::StartMouseDragging


void CText::StopMouseDragging()
{
	mbMouseDown = false;
	if (mDragState == MouseDragDragging) {
		mDragState = MouseDragNormal;
		if (!mbMouseVisible) {
			GetParentWindow()->SetMousePos(mPosDragStart);
			GetParentWindow()->ShowMouse();
			mbMouseVisible = true;
		}
	}
} // CText::StopMouseDragging


tbool CText::OnKeyDown(EKey Key)
{
	if (IsVisible() == false) {
		return false;
	}

	if (mbEditing) {
		if ((Key >= Keya && Key <= Keyz) ||
			(Key >= KeyA && Key <= KeyZ) ||
			(Key >= Key0 && Key <= Key9) ||
			(Key == KeySpace) ||
			// Lasse: Added 2006-09-20:
			((Key >= KeyPlus) && (Key <= KeyDot)) ||
			((Key >= KeyExclamation) && (Key <= KeyTilde))
			// .. Lasse
		)
		{
			// Character

			if (mbNumericOnly) {
				// Only numbers and '-' '+' '.' ',' allowed
				if ((Key >= Key0 && Key <= Key9) ||
					(Key == KeyDot) ||
					(Key == KeyComma) ||
					(Key == KeyMinus)) {
				}
				else {
					// Not numeric, so return
					return false;
				}
			}

			char psz[2];
			psz[1] = 0;

			
			// Lasse: Added 2006-09-18:
			switch (Key) {
				case KeySpace:					psz[0] = ' '; break;
				case KeyExclamation:			psz[0] = '!'; break;
				case KeyQuotation:				psz[0] = '"'; break;
				case KeyPound:					psz[0] = '#'; break;
				case KeyDollar:					psz[0] = '$'; break;
				case KeyPercent:				psz[0] = '%'; break;
				case KeyAnd:					psz[0] = '&'; break;
				case KeyBracketLeft:			psz[0] = '('; break;
				case KeyBracketRight:			psz[0] = ')'; break;
				case KeyMul:					psz[0] = '*'; break;
				case KeyPlus:					psz[0] = '+'; break;
				case KeyComma:					psz[0] = ','; break;
				case KeyMinus:					psz[0] = '-'; break;
				case KeyDot:					psz[0] = '.'; break;
				case KeySlash:					psz[0] = '/'; break;
				case KeyColon:					psz[0] = ':'; break;
				case KeySemiColon:				psz[0] = ';'; break;
				case KeySmaller:				psz[0] = '<'; break;
				case KeyEqual:					psz[0] = '='; break;
				case KeyLarger:					psz[0] = '>'; break;
				case KeyQuestion:				psz[0] = '?'; break;
				case KeyAdd:					psz[0] = '@'; break;
				case KeySquaredBrackedLeft:		psz[0] = '['; break;
				case KeyBackSlash:				psz[0] = '\\'; break;
				case KeySquaredBrackedRight:	psz[0] = ']'; break;
				case KeyHat:					psz[0] = '^'; break;
				case KeyUnderLine:				psz[0] = '_'; break;
				case KeyNote:					psz[0] = '\''; break;
				case KeyNote2:					psz[0] = '`'; break;
				case KeyCurlyBrackedLeft:		psz[0] = '{'; break;
				case KeyLine:					psz[0] = '|'; break;
				case KeyCurlyBrackedRight:		psz[0] = '}'; break;
				case KeyTilde:					psz[0] = '~'; break;

				default:
				{
					if (Key >= Keya && Key <= Keyz) {
						psz[0] = 'a' + (Key - Keya);
					}
					else if (Key >= KeyA && Key <= KeyZ) {
						psz[0] = 'A' + (Key - KeyA);
					}
					else if (Key >= Key0 && Key <= Key9) {
						psz[0] = '0' + (Key - Key0);
					}
					else {
						// We should never come here! Exit gracefully
						psz[0] = 0;
						return false;
					}
				}
				break;
			}
			// .. Lasse

			if (mSelectionType == SelectionAll) {
				// All selected, so delete string, and add character
				msTextCurEditing = std::string(psz);

				mSelectionType = SelectionNone;
				miCursorPos = 0;
			}
			else {
				// No selection, so insert character at cursor position
				// If size == 0 we have to deal with it seperately (because substr on an empty string will crash on Windows)
				if (msTextCurEditing.size() == 0) {
					msTextCurEditing = std::string(psz);
					// Lasse - added 2007-05-10
					miCursorPos = 0;
					// .. Lasse
				}
				else {
					if (miCursorPos == msTextCurEditing.size() - 1) {
						// Cursor is at end of line
						msTextCurEditing = msTextCurEditing + std::string(psz);
					}
					else {
						msTextCurEditing = msTextCurEditing.substr(0, miCursorPos + 1) + std::string(psz) + msTextCurEditing.substr(miCursorPos + 1);
					}

					miCursorPos++;
				}
			}

			mbEditing_RecalcRect = true;
			mbEditing_RecalcCursorPos = true;
		}

		else {
			switch (Key)
			{
				case KeyDelete: // Delete
				{
					if (mSelectionType == SelectionAll) {
						msTextCurEditing = std::string("");
						mSelectionType = SelectionNone;
						miCursorPos = 0;
						mbEditing_RecalcCursorPos = true;
					}
					else {
						if (miCursorPos < (tint)(msTextCurEditing.size() - 1)) {
							// If we're not at the end of the text
							msTextCurEditing = msTextCurEditing.substr(0, miCursorPos + 1) + msTextCurEditing.substr(miCursorPos + 2);
						}
					}
				}
				break;

				case KeyBackSpace: // Backspace
				{
					if (mSelectionType == SelectionAll) {
						msTextCurEditing = std::string("");
						mSelectionType = SelectionNone;
						miCursorPos = 0;
						mbEditing_RecalcCursorPos = true;
					}
					else {
						// Lasse - modified 2007-05-10:
						//if (msTextCurEditing.size() > 0) {
						if ((msTextCurEditing.size() > 0) && (miCursorPos>=0)) {
							msTextCurEditing = msTextCurEditing.substr(0, miCursorPos) + msTextCurEditing.substr(miCursorPos + 1);

							// Lasse - modified 2007-05-10
							//if (miCursorPos > 0) {
							if (miCursorPos >= 0) {
							// .. Lasse
								miCursorPos--;
								mbEditing_RecalcCursorPos = true;
							}
						}
					}
				}
				break;

				case KeyLeft: // Left
				{
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = -1;//0;
						mbEditing_RecalcCursorPos = true;
					}
					else {
						// Lasse - modified 2007-05-10
						//if (miCursorPos > 0) {
						if (miCursorPos >= 0) {
						// .. Lasse
							miCursorPos--;
							mbEditing_RecalcCursorPos = true;
						}
					}
				}
				break;

				case KeyRight: // Right
				{
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = msTextCurEditing.size() - 1;
						mbEditing_RecalcCursorPos = true;
					}
					else {
						if (miCursorPos < (tint)(msTextCurEditing.size() - 1)) {
							miCursorPos++;
							mbEditing_RecalcCursorPos = true;
						}
					}
				}
				break;

				// Lasse - added 2007-05-22
				case KeyBigHome:
				case KeyHome: // Home
				{
					if ((mSelectionType != SelectionNone) || (miCursorPos > 0))
						mbEditing_RecalcCursorPos = true;
					mSelectionType = SelectionNone;
					miCursorPos = -1;
				}
				break;
				// .. Lasse

				// Lasse - added 2007-05-22
				case KeyBigEnd:
				case KeyEnd: // End
				{
					tint32 iNewIx = ((tint)msTextCurEditing.size()) - 1;
					if ((mSelectionType != SelectionNone) || (miCursorPos != iNewIx))
						mbEditing_RecalcCursorPos = true;
					mSelectionType = SelectionNone;
					miCursorPos = iNewIx;
				}
				break;
				// .. Lasse

				// Lasse: scrolling
				case KeyUp:
				case KeyDown:
				case KeyBigUp:
				case KeyBigDown:
				{
					if (mbKeyScrollingEnabled) {
						de::IDisplayConverter* pDispConv = GetDisplayConverter();
						if (pDispConv!=NULL) {
							tint32 iID = GetID();
							if (iID!=ge::IControl::giNoID) {
								tint32 iGUIValue;
								XlatCurEditingText(iGUIValue);
								switch (Key) {
									case KeyUp:			iGUIValue += miKeyScrollUp;			break;
									case KeyDown:		iGUIValue -= miKeyScrollDown;		break;
									case KeyBigUp:		iGUIValue += miKeyScrollBigUp;		break;
									case KeyBigDown:	iGUIValue -= miKeyScrollBigDown;	break;
								}
								// Set value without thinking of whether it's in range
								SetValue(iGUIValue);
								// Read back value; it is now in range (if a data-converter is attached)
								iGUIValue = GetValue();
								// 
								tint32 iParmValue = (mpDataConverter) ? mpDataConverter->GUI2Parm(iGUIValue) : iGUIValue;

								// Make value show
								tchar psz[128];
								pDispConv->Value2String(iParmValue, (tchar*)psz);
								msTextCur = std::string((char*)psz);
								msTextCurEditing = msTextCur;
								SetText((tchar*)(msTextCurEditing.c_str()));
								mSelectionType = SelectionAll;
								miCursorPos = 0;
								mbEditing_RecalcRect = true;
							}
						}
					}
				}
				break;
				// .. Lasse

				case KeyReturn: // Return
				{
					StopEditing(true);
				}
				break;

				case KeyEscape: // Cancel
				{
					StopEditing(false);
				}
				break;
			}
		}

		if (!mbCursorOn) {
			SyncCursorOnNow();
			mbEditing_RecalcCursorPos = true;
		}

		// We should only redraw if necessary
		if ((mbEditing_RecalcRect) || (mbEditing_RecalcCursorPos))
			Redraw();

		return true;
	}

	return false;
} // CText::OnKeyDown


void CText::SyncCursorOnNow()
{
	muiTimerLast = ITime::GetTimeMS();
	mbCursorOn = true;
}


void CText::OnTimer()
{
	CControl::OnTimer();

	if (IsVisible() == false) {
		return;
	}

	tuint32 uiTimeNow = ITime::GetTimeMS();
	tuint32 uiTimeDelta = uiTimeNow - muiTimerLast;
	muiTimerLast = uiTimeNow;

	miCursorCounter -= uiTimeDelta;
	if (miCursorCounter <= 0) {
		mbCursorOn = mbCursorOn ? false : true;

		if (mbEditing) {
			Redraw();
		}

		// 1 blink (1 on and 1 off) each second
		// Lasse - modified 2007-05-10
		//miCursorCounter = 500;
		miCursorCounter = miCursorBlinkRateMs;
		// .. Lasse
	}
}


void CText::SetValue(tint32 iNew, tbool bNotify)
{
	if (GetConverter())
		CControl::SetValue(iNew, bNotify);
	else {
		// A CText that isn't connected to a data-converter can have *any* GUI value.
		// Therefore we mustn't let CControl::SetValue(...) validate it.
		miValue = iNew;

		if (bNotify) {
			NotifyValueChange();
		}

		Redraw();
	}
} // CText::SetValue


void CText::SetConvertedValue(tint32 iNew, tbool bNotify)
{
	// CText has a slightly different behaviour when setting values, therefore we override this call

	if (GetDisplayConverter()) {
		tchar psz[128];
		GetDisplayConverter()->Value2String(iNew, (tchar*)psz);
		msTextCur = std::string((char*)psz);
	}

	// Calling inherited will also make sure we redraw
	CControl::SetConvertedValue(iNew, bNotify);
}

void CText::DoLimiting(std::string& rs)
{
	// Max length of text, i.e. control width
	int iMaxLength = GetSize().iCX;

	// The font we're using
	//IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

	// Is the text too long
	tbool bTooLong = mpFontRes->GetTextWidth((const tchar*)(rs.c_str())) > iMaxLength;

	// As long as the text is too long...
	while (bTooLong) {
		// Find position of last vocal
		tint iPos = rs.find_last_of("aeiouy");
		if (iPos == std::string::npos) {
			// No more vocals (or only the first character), so break
			break;
		}

		// Found a vocal, so remove it
		rs = rs.substr(0, iPos) + rs.substr(iPos + 1);

		// Check text again
		bTooLong = mpFontRes->GetTextWidth((const tchar*)(rs.c_str())) > iMaxLength;
	}

	if (bTooLong) {
		// We get here if there were no more small vocals, so start removing capital ones
		while (bTooLong) {
			// Find position of last vocal
			tint iPos = rs.find_last_of("AEIOUY");
			if (iPos == std::string::npos || iPos == 0) {
				// No more vocals (or only the first character), so break
				break;
			}

			// Found a vocal, so remove it
			rs = rs.substr(0, iPos) + rs.substr(iPos + 1);

			// Check text again
			bTooLong = mpFontRes->GetTextWidth((const tchar*)(rs.c_str())) > iMaxLength;
		}
	}

	if (bTooLong) {
		// We get here if there were no more vocals, and the text is still too long

		// We'll remove the center text and add a few dots.

		// The length, in pixels, of the dots
		int iDotLength = mpFontRes->GetTextWidth((const tchar*)"..");

		// Subtract the dot length from the max length, to get the new max length of the text
		iMaxLength -= iDotLength;

		// Size of text (in characters)
		int iSize = rs.size();

		// Decide which character to remove
		int iToRemove = iSize / 2;

		// Counter to keep track of whether we need to change the position to remove
		int iCounter = 0;

		while (bTooLong) {
			if (iCounter == 2) {
				iToRemove--;
				iCounter = 0;
			}

			// Remove it
			rs = rs.substr(0, iToRemove) + rs.substr(iToRemove + 1);

			// Check text again
			bTooLong = mpFontRes->GetTextWidth((const tchar*)(rs.c_str())) > iMaxLength;

			iCounter++;
		}

		// Add the dots
		rs = rs.substr(0, iToRemove) + std::string("..") + rs.substr(iToRemove);
	}
}

void CText::StartEditing()
{
	GetParentWindow()->GetMouseFocus(dynamic_cast<IControl*>(this));

	GetParentWindow()->AcquireKeyboardInput();

	SSize Size;
	GetParentWindow()->GetSize(Size);
	SRect Rect(SPos(0, 0), Size);
	GetParentWindow()->Redraw(Rect);

	miCursorPos = 0;
	mSelectionType = SelectionAll;

	msTextCurEditing = msTextCur;

	// Lasse - added 2007-05-10
#ifdef _WIN32
	// Get user defined cursor blink rate
	{
		HKEY hkey;
		LONG lRc = RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_QUERY_VALUE, &hkey);
		if (lRc == ERROR_SUCCESS) {
			BYTE pucData[10];
			DWORD dwDataSize = sizeof(pucData);
			DWORD dwType = REG_SZ;
			LONG lRc = ::RegQueryValueEx(
				hkey, 
				"CursorBlinkRate",
				NULL,
				&dwType, 
				pucData, &dwDataSize
			);
			if (lRc == ERROR_SUCCESS) {
				tint32 i = atoi((char*)pucData);
				if (i>=100 && i<=1000) {
					miCursorBlinkRateMs = i;
				}
			}
		}
	}
#endif // #ifdef _WIN32
	// .. Lasse

	SyncCursorOnNow();

	mbEditing = true;
	mbEditing_RecalcRect = true;
	mbEditing_RecalcCursorPos = true;
}

void CText::StopEditing(tbool bOK)
{
	if (bOK) {
		de::IDisplayConverter* pDispConv = GetDisplayConverter();
		if (pDispConv) {
			try {
				tint32 iGUIValue = pDispConv->String2Value((tchar*)msTextCurEditing.c_str());
				SetConvertedValue(iGUIValue, true);
				iGUIValue = GetConvertedValue();
				tchar psz[128];
				pDispConv->Value2String(iGUIValue, psz);
				msTextCur = msTextCurEditing = std::string((char*)psz);
			}
			catch (...) {
				// Nope - didn't work
			}
		}
		else {
			SetText((tchar*)(msTextCurEditing.c_str()));
		}
		this->NotifyGeneric((void*)(msTextCurEditing.c_str()));
	}

	if (mDragState==MouseDragDragging) {
		// Abort dragging in progress
		StopMouseDragging();
	}

	GetParentWindow()->ReleaseMouseFocus();

	GetParentWindow()->ReleaseKeyboardInput();

	SSize Size;
	GetParentWindow()->GetSize(Size);
	SRect Rect(SPos(0, 0), Size);
	GetParentWindow()->Redraw(Rect);

	mbEditing = false;
}

tint CText::GetCharacterIndex(const std::string& sText, tint iOffset)
{
	// Lasse - added 2007-05-10
	if (iOffset < 0) {
		return -1;
	}
	// .. Lasse

	//IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

	tchar psz[2];
	psz[1] = 0;

	psz[0] = sText[0];
	iOffset -= mpFontRes->GetTextWidth(psz);

	tuint uiIndex = 0;
	while (iOffset > 0) {
		uiIndex++;
		if (uiIndex >= sText.size()) {
			uiIndex--;
			break;
		}

		psz[0] = sText[uiIndex];
		iOffset -= mpFontRes->GetTextWidth(psz);
	}

	return (tint)uiIndex;
}

void CText::SetClicksOutsideAsSuccess()
{
	mbClicksOutsideIsSuccess = true;
}


void CText::EnableKeyScrolling(tbool bEnable /*= true*/)
{
	mbKeyScrollingEnabled = bEnable;
} // CText::EnableKeyScrolling

void CText::SetKeyScrolls(tint32 iUp /*= 1*/, tint32 iDown /*= 1*/, tint32 iPgUp /*= 10*/, tint32 iPgDn /*= 10*/)
{
	miKeyScrollUp = iUp;
	miKeyScrollDown = iDown;
	miKeyScrollBigUp = iPgUp;
	miKeyScrollBigDown = iPgDn;
} // CText::SetKeyScrolls(tint32, tint32)


void CText::EnableMouseDragging(tbool bEnable /*= true*/)
{
	if (!bEnable) {
		if (mDragState==MouseDragDragging) {
			// Abort dragging in progress
			StopMouseDragging();
		}
	}

	mbMouseDraggingEnabled = bEnable;
} // CText::EnableMouseDragging

void CText::SetMouseDraggingJumps(tint32 iUp /*= 1*/, tint32 iDown /*= 1*/, tint32 iBigUp /*= 10*/, tint32 iBigDown /*= 10*/)
{
	miMouseDragUp = iUp;
	miMouseDragDown = iDown;
	miMouseDragBigUp = iBigUp;
	miMouseDragBigDown = iBigDown;
} // CText::SetMouseDraggingJumps


tbool CText::XlatCurEditingText(tint32& riGUIValue)
{
	try {
		if (mbEditing) {
			if (msTextCurEditing==msTextCur) {
				riGUIValue = GetValue();
				return true;
			}
			else {
				de::IDisplayConverter* pDispConv = GetDisplayConverter();
				if (pDispConv) {
					tint32 iParmValue = pDispConv->String2Value((tchar*)msTextCurEditing.c_str());
					riGUIValue = (mpDataConverter) ? mpDataConverter->Parm2GUI(iParmValue) : iParmValue;
					return true;
				}
			}
		}
	}
	catch (...) {
		// Translation failed
	}

	// If nothing else works return old value
	riGUIValue = GetValue();
	return false;
} // CText::XlatCurEditingText
