/*!	\file CTextField.cpp
	\author Max Grønlund
	\date 01/Jul/2006
	\date 17/Aug/2006
*/

#include "geInternalOS.h"

CTextField::CTextField()
	: mAlignmentHorz(HorzAlignLeft), mAlignmentVert(VertAlignTop), mbLimiting(false),
	mbEditable(false), mbEditing(false),
	mbCursorOn(false), miCursorCounter(0),
	mbClicksOutsideIsSuccess(false)
{
	muiTimerLast	=	ITime::GetTimeMS();

	// Lasse - added 2007-05-14
	miCursorBlinkRateMs = 500;
	// .. Lasse
}

CTextField::~CTextField()
{
}

ITextField* ITextField::Create()
{
	return dynamic_cast<ITextField*>(new CTextField());
}

void CTextField::Destroy()
{
	delete dynamic_cast<CTextField*>(this);
}

void CTextField::Init(tint32 iID, IFontResource* pFont)
{
	CControl::Init(iID, dynamic_cast<IBitmapResource*>(pFont));

	mRect.iX	=	20;
	mRect.iY	=	20;
	miMaxLines	=	-1;

	pFont->GetRealSize(mSize);
}

void CTextField::ChangeFont(IFontResource* pFont)
{
	// Make sure we have a font already
	ASSERT(mpResource);

	mpResource->DecRefCount();

	mpResource = dynamic_cast<IBitmapResource*>(pFont);
}

void CTextField::SetText(const tchar* pszText)
{
	std::string sTextNew = std::string((char*)pszText);

	if (mbLimiting) {
//		DoLimiting(sTextNew);
	}

	if (msTextCur.compare(sTextNew) != 0) {
		msTextCur = sTextNew;
		Redraw();
	}
}

void CTextField::GetText(tchar* pszText)
{
	strcpy((char*)pszText, msTextCur.c_str());
}

void CTextField::SetHorzAlignment(EHorizontalAlignment Alignment)
{
	mAlignmentHorz = Alignment;
}

void CTextField::SetVertAlignment(EVerticalAlignment Alignment)
{
	mAlignmentVert = Alignment;
}

void CTextField::EnableTextLimiting(tbool bEnable /*= true*/)
{
	mbLimiting = bEnable;
}

void CTextField::MakeEditable()
{
	mbEditable = true;
}

void CTextField::GetDrawRect(SRect& rRect, const std::string& sText)
{
	// Default position to draw (alignment: left / top)
	SPos DrawPos(GetPos());

	if (mAlignmentHorz == HorzAlignRight) {
		tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextWidth((const tchar*)(sText.c_str()));
		tint iOffset = GetSize().iCX - iPixels;
		DrawPos.iX += iOffset;
	}
	if (mAlignmentHorz == HorzAlignCenter) {
		tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextWidth((const tchar*)(sText.c_str()));
		tint iOffset = (GetSize().iCX - iPixels) / 2;
		DrawPos.iX += iOffset;
	}

	if (mAlignmentVert == VertAlignBottom) {
		tint iPixels = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextHeight();
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
		IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());
		tint iTopLine = pFontRes->GetFontTopLine();
		tint iBaseLine = pFontRes->GetFontBaseLine();
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

void CTextField::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	SRect Rect;
	if (mbEditing) {
		GetDrawRect(Rect, msTextCurEditing);
	}
	else {
		GetDrawRect(Rect, msTextCur);
	}
	if (mbEditing) {
		DrawLines(msTextCurEditing, rUpdate);
	}
	else {
		DrawLines(msTextCur, rUpdate);
	}

	if (mbEditing) {
		// We're currently editing
		switch (mSelectionType) {
			case SelectionNone:
				{
					// No selection, so draw cursor
					if (mbCursorOn) {
						// String of text which is before cursor
						std::string s = msTextCurEditing.substr(0, miCursorPos + 1);
						DrawCursor(s, Rect,rUpdate);

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
}

tbool CTextField::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	if (mbEditable == false) {
		// Text is not editable, so return
		return false;
	}

	SRect Rect;
	GetRect(Rect);

	if (mbEditing) {
		// We're currently editing, so a mouse click outside the control will stop editing
		if (MouseMsg == LeftButtonDown) {
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

				// Lasse - rewritten 2007-05-14
				
				//tint iCurLine	=	PosTextOffset.iY / 12;
				
				// Find line height
				IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());
				tint iLineHeight = pFontRes->GetTextHeight() + 2;
				// Find the line number
				tint iCurLine	=	PosTextOffset.iY / iLineHeight;

				// .. Lasse


				// Finde the text on the current line
				tint iLineNr	=	0;
				tint iOffset	=	0;
				std::string outString;

				while (iOffset < (tint)msTextCurEditing.size())
				{
					// Chop off the previous line
					GetLine(iOffset, msTextCurEditing, outString);
					// Check fore line number
					if(iLineNr == iCurLine){
						break;
					}
					else{
						// Move to next line
						iLineNr++;
						// Move the offset
						iOffset			+=	outString.size();
					}
				}
				miCursorPos =	GetCharacterIndex(outString, PosTextOffset.iX);
				miCursorPos +=	iOffset;
				if (miCursorPos >= (tint)msTextCurEditing.size()){
					miCursorPos = msTextCurEditing.size()-1;
				}		
				Redraw();
			}
			else {
				// Click was not inside control
				StopEditing(true);
				// Tell the window to resend mouse event (to avoid the user having to click twice)
				GetParentWindow()->ResendMouseEvent();
			}
		}

		return true;
	}

	// We only get here if we're not currently editing

	if (Rect.Inside(Pos)) {
		// Click was inside our rect
		if (MouseMsg == LeftButtonDown) {
			StartEditing();
		}

		return true;
	}
	
	return false;
}

tbool CTextField::OnKeyDown(EKey Key)
{
	if (IsVisible() == false) {
		return false;
	}
	// Character
	char psz[2];
	psz[0] = 0;
	psz[1] = 0;
	
	if (mbEditing) {

		// Lasse - bugfix 2007-05-14
		// Work-around for index out of range
		if (miCursorPos > ((tint)msTextCurEditing.size()) - 1) {
			// Force index in range
			miCursorPos = msTextCurEditing.size() - 1;
		}
		if (miCursorPos < -1) {
			miCursorPos = -1;
		}
		// .. Lasse

		// Lasse - why both 'if' and 'switch/case'? - removed 2007-05-14
//		if ((Key >= Keya && Key <= Keyz) ||
//			(Key >= KeyA && Key <= KeyZ) ||
//			(Key >= Key0 && Key <= Key9) ||
//			(Key == KeySpace) ||
//			(Key == KeyDot) ||
//			(Key == KeyMinus) ||
//			(Key == KeyComma) ||
//			(Key == KeyExclamation) ||
//			(Key == KeyQuotation) ||
//			(Key == KeyPound) ||
//			(Key == KeyDollar) ||
//			(Key == KeyPercent) ||
//			(Key == KeyAnd) ||
//			(Key == KeyBracketLeft) ||
//			(Key == KeyBracketRight) ||
//			(Key == KeyMul) ||
//			(Key == KeySlash) ||
//			(Key == KeyPlus) ||
//			(Key == KeyColon) ||
//			(Key == KeySemiColon) ||
//			(Key == KeyLarger) ||
//			(Key == KeyEqual) ||
//			(Key == KeySmaller) ||
//			(Key == KeyQuestion) ||
//			(Key == KeySquaredBrackedLeft) ||
//			(Key == KeyBackSlash) ||
//			(Key == KeySquaredBrackedRight) ||
//			(Key == KeyHat) ||
//			(Key == KeyUnderLine) ||
//			(Key == KeyNote) ||
//			(Key == KeyNote2) ||
//			(Key == KeyCurlyBrackedLeft) ||
//			(Key == KeyLine) ||
//			(Key == KeyCurlyBrackedRight) ||
//			(Key == KeyExclamation) ||
//			(Key == KeyTilde)) {
			
		if (Key >= Keya && Key <= Keyz)			psz[0] = 'a' + (Key - Keya);
		else if (Key >= KeyA && Key <= KeyZ)	psz[0] = 'A' + (Key - KeyA);
		else if (Key >= Key0 && Key <= Key9) 	psz[0] = '0' + (Key - Key0);
		else {
			switch(Key) {
				case KeySpace:			psz[0] = ' '; break;
				case KeyDot:			psz[0] = '.'; break;
				case KeyComma:			psz[0] = ','; break;
				case KeyMinus:			psz[0] = '-'; break;
				case KeyExclamation:	psz[0] = '!'; break;
				case KeyQuotation:		psz[0] = '"'; break;
				case KeyPound:			psz[0] = '#'; break;
				case KeyDollar:			psz[0] = '$'; break;
				case KeyPercent:		psz[0] = '%'; break;
				case KeyAnd:			psz[0] = '&'; break;
				case KeyBracketLeft:	psz[0] = '('; break;
				case KeyBracketRight:	psz[0] = ')'; break;
				case KeyMul:			psz[0] = '*'; break;
				case KeySlash:			psz[0] = '/'; break;
				case KeyPlus:			psz[0] = '+'; break;
				case KeyColon:			psz[0] = ':'; break;
				case KeySemiColon:		psz[0] = ';'; break;
				case KeyLarger:			psz[0] = '>'; break;
				case KeyEqual:			psz[0] = '='; break;
				case KeySmaller:		psz[0] = '<'; break;
				case KeyQuestion:		psz[0] = '?'; break;
				case KeySquaredBrackedLeft:	psz[0] = '['; break;
				case KeyBackSlash:		psz[0] = '\\'; break;
				case KeySquaredBrackedRight:psz[0] = ']'; break;
				case KeyHat:			psz[0] = '^'; break;
				case KeyUnderLine:		psz[0] = '_'; break;
				case KeyNote:			psz[0] = '\''; break;
				case KeyNote2:			psz[0] = '`'; break;
				case KeyCurlyBrackedLeft:psz[0] = '{'; break;
				case KeyLine:			psz[0] = '|'; break;
				case KeyCurlyBrackedRight:psz[0] = '}'; break;
				case KeyTilde:			psz[0] = '~'; break;
				case KeyAdd:			psz[0] = '@'; break;
			//	case KeyDelete: return false;
			//	case KeyBackSpace:
				default:	break;

			}
		}
		// Lasse - added because of change of 'if' and 'switch/case' structure
		if (psz[0] != 0) {
			// An ASCII char key was pressed
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
				}
				else {
					// Lasse - bugfix 2007-05-11
//					if (miCursorPos == msTextCurEditing.size() - 1) {
					if (miCursorPos >= ((tint)msTextCurEditing.size()) - 1) {
					// .. Lasse
						// Cursor is at end of line
						msTextCurEditing += std::string(psz);
					}
					else if(msTextCurEditing.size()> 0){ //1){
						// Cursor is in middle of line
						msTextCurEditing = msTextCurEditing.substr(0, miCursorPos + 1) + std::string(psz) + msTextCurEditing.substr(miCursorPos + 1);
					}
					else{
						// Add psz
						msTextCurEditing = std::string(psz);
						miCursorPos = -1; //0;
					}
				}
				miCursorPos++;
			}
			// We caught that
			Redraw();
			return true;
		}
		// Lasse - why all the "else if"'s? - changed to switch/case 2007-05-14
		else {
			switch (Key) {
				case KeyDelete: /* Delete */ {
					if (mSelectionType == SelectionAll) {
						// All selected, so delete string, 
						//msTextCurEditing = std::string("");
						psz[0] = ' ';
						msTextCurEditing = std::string(psz);
						mSelectionType = SelectionNone;
						miCursorPos = -1; //0;
					}
					else {
						if (miCursorPos < (tint)(msTextCurEditing.size() - 1)) {
							// If we're not at the end of the text
							msTextCurEditing = msTextCurEditing.substr(0, miCursorPos + 1) + msTextCurEditing.substr(miCursorPos + 2);
						}
					}
				}
				break;

				case KeyBackSpace: /* BackSpace */ {
					if (mSelectionType == SelectionAll) {
						//msTextCurEditing = std::string("");
						psz[0] = ' ';
						msTextCurEditing = std::string(psz);
						mSelectionType = SelectionNone;
						miCursorPos = -1; //0;
					}
					else {
						// Lasse - simplified 2007-05-14

//						// if the string is more than two chars long
//						if (msTextCurEditing.size() > 1) {
//							// if cursor is in end of string
//							if (miCursorPos >= 1) {
//								// Move termination down
//								msTextCurEditing = msTextCurEditing.substr(0, miCursorPos) + msTextCurEditing.substr(miCursorPos + 1);
//								miCursorPos--;
//							}
//							// If the cursor is at the start do nothing
//						}
//						else{
//							psz[0] = ' ';
//							msTextCurEditing = std::string(psz);
//							miCursorPos = 0;
//						}

						if (miCursorPos >= 0) {
							msTextCurEditing = msTextCurEditing.substr(0, miCursorPos) + msTextCurEditing.substr(miCursorPos + 1);
							miCursorPos--;
						}
						// .. Lasse
					}
				}
				break;

				case KeyUp: /* Up */
				case KeyBigUp: /* PgUp */
				{
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = -1; //0;
					}
					else {
						// Lasse - rewritten 2007-05-14

//						if (miCursorPos > 0) {
//							//--------------------------
//							// Finde the current line
//							tint iLineNr	=	0;
//							tint iOffset	=	0;
//							tint iLinePos;
//							while (iOffset < (tint)msTextCurEditing.size())
//							{
//								std::string outString;
//								// Chop off the previous line
//								GetLine(iOffset, msTextCurEditing, outString);
//								// if the chursor is on this line
//								if (miCursorPos < iOffset + (tint)outString.size()){
//									iLinePos = miCursorPos - iOffset;
//									break;
//								}
//								else{
//									// Move the offset
//									iOffset	+=	outString.size();
//									// Move to next line
//									iLineNr++;	
//								}
//							}
//							// not on the first line
//							if(iLineNr) {
//								tint iNewLine = iLineNr-1;
//								std::string outString;
//								iOffset			=	0;
//								iLineNr			=	0;
//								while (iOffset < (tint)msTextCurEditing.size())	{
//									// Chop off the previous line
//									GetLine(iOffset, msTextCurEditing, outString);
//									// We are at the line we are going to
//									if(iLineNr == iNewLine){
//										miCursorPos = iOffset + iLinePos;
//										break;
//									}
//									iOffset	+=	outString.size();
//									iLineNr++;
//								}
//							}
//						}

						// Find previous CR char
						tbool bDone = false;
						tint iPrevCRIx = miCursorPos;
						while (!bDone) {
							if (iPrevCRIx >= 0) {
								char cCurr = msTextCurEditing[iPrevCRIx];
								if (cCurr != '\r') {
									// No newline char yet
									// Try again
									iPrevCRIx--;
								}
								else {
									// We found a newline char
									bDone = true;
								}
							}
							else {
								// Sorry - can't do anything - already at first line
								bDone = true;
							}
						}

						IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

						// How many pixels to left edge of text (default 0)
						tint iPixelsToLeft = 0;
						// How many chars from left edge of text
						tint iOfsFromLeft = miCursorPos - iPrevCRIx;
						if (iOfsFromLeft > 0) {
							// Text left of cursor
							std::string s = msTextCurEditing.substr(iPrevCRIx + 1, iOfsFromLeft);
							// Calculate pixels to left edge
							iPixelsToLeft = pFontRes->GetTextWidth((const tchar*)(s.c_str()));
						}

						// Find CR for previous line
						tint iPrevPrevCRIx;
						tint iNewIx;
						if (Key == KeyBigUp) {
							// That's easy - first line
							iPrevPrevCRIx = -1;
							iNewIx = -1;
						}
						else {
							// Look for CR at line above

							iPrevPrevCRIx = (iPrevCRIx < 0) ? -1 : (iPrevCRIx - 1);
							iNewIx = iPrevPrevCRIx;
							tbool bPrevPrevIxDone = iPrevPrevCRIx < 0;
							while (!bPrevPrevIxDone) {
								if (iPrevPrevCRIx >= 0) {
									if (msTextCurEditing[iPrevPrevCRIx] != '\r') {
										// No newline char yet
										// Try again
										iPrevPrevCRIx--;
									}
									else {
										// We found a newline char
										bPrevPrevIxDone = true;
									}
								}
								else {
									// Found start of text
									bPrevPrevIxDone = true;
								}
							}
						}

						// Find offset at line above / first line
						tint iNewWidth = 0, iNewWidthPrevPrev = 0;
						iNewIx = iPrevPrevCRIx;
						tchar sCurr[2];
						sCurr[0] = '\0';
						sCurr[1] = '\0';
						while ((iNewWidth < iPixelsToLeft) && (iNewIx < iPrevCRIx) && (sCurr[0] != '\r')) {
							iNewWidthPrevPrev = iNewWidth;
							sCurr[0] = (tchar)msTextCurEditing[iNewIx + 1];
							if (sCurr[0] != '\r') {
								tint iCharWidth = pFontRes->GetTextWidth((const tchar*)(sCurr));
								iNewWidth += iCharWidth;
								iNewIx++;
							}
						}
						tint iAbsRight = abs(iPixelsToLeft - iNewWidth);
						tint iAbsLeft = abs(iPixelsToLeft - iNewWidthPrevPrev);
						if (iAbsLeft < iAbsRight) {
							// Left of curr char is a better match
							iNewIx--;
						}

						if (iPrevPrevCRIx != iPrevCRIx) {
							// Line actually changed
							miCursorPos = iNewIx;
						}
						// .. Lasse
					}
				}
				break;

				case KeyDown: /* Down */
				case KeyBigDown: /* PgDn */
				{
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = 0;
					}
					else {
						// Lasse - rewritten 2007-05-14

//						if (miCursorPos > 0) {
//							//--------------------------
//							// Finde the current line
//							tint iLineNr	=	0;
//							tint iOffset	=	0;
//							tint iLinePos;
//							while (iOffset < (tint)msTextCurEditing.size())
//							{
//								std::string outString;
//								// Chop off the previous line
//								GetLine(iOffset, msTextCurEditing, outString);
//								// if the chursor is on this line
//								if (miCursorPos < iOffset + (tint)outString.size()){
//									iLinePos = miCursorPos - iOffset;
//									break;
//								}
//								else{
//									// Move the offset
//									iOffset	+=	outString.size();
//									// Move to next line
//									iLineNr++;	
//								}
//							}
//							// Next line
//							tint iNewLine = iLineNr+1;
//							std::string outString;
//							iOffset			=	0;
//							iLineNr			=	0;
//							while (iOffset < (tint)msTextCurEditing.size())	{
//								// Chop off the previous line
//								GetLine(iOffset, msTextCurEditing, outString);
//								// We are at the line we are going to
//								if(iLineNr == iNewLine){
//									miCursorPos = iOffset + iLinePos;
//									if (miCursorPos > (tint)msTextCurEditing.size()) {
//										miCursorPos = msTextCurEditing.size()-1;
//									}
//									break;
//								}
//								iOffset	+=	outString.size();
//								iLineNr++;
//							}
//
//						}

						tbool bDone = false;
						tint iPrevCRIx = miCursorPos;
						while (!bDone) {
							if (iPrevCRIx >= 0) {
								char cCurr = msTextCurEditing[iPrevCRIx];
								if (cCurr != '\r') {
									// No newline char yet
									// Try again
									iPrevCRIx--;
								}
								else {
									// We found a newline char
									bDone = true;
								}
							}
							else {
								// We're at first line
								bDone = true;
							}
						}

						IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

						// How many pixels to left edge of text (default 0)
						tint iPixelsToLeft = 0;
						// How many chars from left edge of text
						tint iOfsFromLeft = miCursorPos - iPrevCRIx;
						if (iOfsFromLeft > 0) {
							// Text left of cursor
							std::string s = msTextCurEditing.substr(iPrevCRIx + 1, iOfsFromLeft);
							// Calculate pixels to left edge
							iPixelsToLeft = pFontRes->GetTextWidth((const tchar*)(s.c_str()));
						}

						// Find CR before next line
						tint iNextCRIx;
						tint iNewIx;
						if (Key == KeyBigDown) {
							iNextCRIx = ((tint)msTextCurEditing.size()) - 1;
							tbool bNextIxDone = iNextCRIx < 0;
							while (!bNextIxDone) {
								if (iNextCRIx > iPrevCRIx) {
									tchar cCurr = (tchar)msTextCurEditing[iNextCRIx];
									if (cCurr != '\r') {
										// No newline char yet
										// Try again
										iNextCRIx--;
									}
									else {
										// We found a newline char
										bNextIxDone = true;
									}
								}
								else {
									// Found same index - we're at last line
									bNextIxDone = true;
								}
							}
						}
						else {
							iNextCRIx = miCursorPos + 1;
							iNewIx = iNextCRIx;
							tbool bNextIxDone = false;//iNextCRIx < 0;
							while (!bNextIxDone) {
								if (iNextCRIx < ((tint)msTextCurEditing.size()) - 1) {
									tchar cCurr = (tchar)msTextCurEditing[iNextCRIx];
									if (cCurr != '\r') {
										// No newline char yet
										// Try again
										iNextCRIx++;
									}
									else {
										// We found a newline char
										bNextIxDone = true;
									}
								}
								else {
									// Found end of text (last line)
									bNextIxDone = true;
									tint32 iLastIx = ((tint32)msTextCurEditing.size()) - 1;
									if (iLastIx >= 0) {
										tchar cLast = msTextCurEditing[msTextCurEditing.size() - 1];
										if (cLast != '\r') {
											iNextCRIx = iPrevCRIx;
										}
									}
								}
							}
						}

						tint iNewWidth = 0, iNewWidthPrev = 0;
						iNewIx = iNextCRIx;
						tchar sCurr[2];
						sCurr[0] = '\0';
						sCurr[1] = '\0';
						while ((iNewWidth < iPixelsToLeft) && (iNewIx < ((tint)msTextCurEditing.size()) - 1) && (sCurr[0] != '\r')) {
							iNewWidthPrev = iNewWidth;
							sCurr[0] = (tchar)msTextCurEditing[iNewIx + 1];
							if (sCurr[0] != '\r') {
								tint iCharWidth = pFontRes->GetTextWidth((const tchar*)(sCurr));
								iNewWidth += iCharWidth;
								iNewIx++;
							}
						}
						tint iAbsRight = abs(iPixelsToLeft - iNewWidth);
						tint iAbsLeft = abs(iPixelsToLeft - iNewWidthPrev);
						if (iAbsLeft < iAbsRight) {
							// Left of curr char is a better match
							iNewIx--;
						}

						if ((iNextCRIx > iPrevCRIx) || (iNextCRIx >= ((tint)msTextCurEditing.size()) -1)) {
							// Line actually changed
							miCursorPos = iNewIx;
						}
						// .. Lasse
					}
				}
				break;

				case KeyLeft: /* Left */ {
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = -1; //0;
					}
					else {
						//if (miCursorPos > 0) {
						if (miCursorPos >= 0) {
							miCursorPos--;
						}
					}
				}
				break;

				case KeyRight: /* Right */ {
					if (mSelectionType == SelectionAll) {
						mSelectionType = SelectionNone;
						miCursorPos = msTextCurEditing.size() - 1;
					}
					else {
						if (miCursorPos < (tint)(msTextCurEditing.size() - 1)) {
							miCursorPos++;
						}
					}
				}
				break;

				case KeyHome: /* Home */ {
					while ((miCursorPos >= 0) && (msTextCurEditing[miCursorPos] != '\r')) {
						miCursorPos--;
					}
					mSelectionType = SelectionNone;
				}
				break;

				case KeyEnd: /* End */ {
					while ((miCursorPos < ((tint)msTextCurEditing.size()) - 1) && (msTextCurEditing[miCursorPos + 1] != '\r')) {
						miCursorPos++;
					}
					mSelectionType = SelectionNone;
				}
				break;

				case KeyBigHome: /* Ctrl+Home */ {
					miCursorPos = -1;
					mSelectionType = SelectionNone;
				}
				break;

				case KeyBigEnd: /* Ctrl+End */ {
					miCursorPos = msTextCurEditing.size() - 1;
					mSelectionType = SelectionNone;
				}
				break;

				case KeyReturn: /* Enter */ {
					if(GetParentWindow()->ControlPressed())
					{
						// Character
						psz[0] = 13;
						psz[1] = 0;
						// No selection, so insert character at cursor position
						// If size == 0 we have to deal with it seperately (because substr on an empty string will crash on Windows)
						if (msTextCurEditing.size() == 0) {
							msTextCurEditing = std::string(psz);
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
					else {
						// Return
						StopEditing(true);
					}
				}
				break;

				case KeyEscape: /* Escape, Cancel */ {
					StopEditing(false);
				}
				break;

				default: /* We didn't catch that */ {
					return false;
				}
			}
		}

		Redraw();
		return true;
	}
	return false;
}

void CTextField::OnTimer()
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
		// Lasse - modified 2007-05-14
		//miCursorCounter = 500;
		miCursorCounter = miCursorBlinkRateMs;
		// .. Lasse
	}
}

void CTextField::SetConvertedValue(tint32 iNew, tbool bNotify)
{
	// CTextField has a slightly different behaviour when setting values, therefore we override this call

	if (GetDisplayConverter()) {
		tchar psz[128];
		GetDisplayConverter()->Value2String(iNew, (tchar*)psz);
		msTextCur = std::string((char*)psz);
	}

	// Calling inherited will also make sure we redraw
	CControl::SetConvertedValue(iNew, bNotify);
}


void CTextField::DrawLines(std::string& strIn, const SRect &rUpdate )
{

	SRect Rect;
	
	GetDrawRect(Rect, strIn);

	tint iOffset	=	0;
	tint iOffsetY	=	0;
	tint iYPos		=	0;
	std::string outString;
	tint iLineNr	=	0;

	while (iOffset < (tint)strIn.size())
	{
		iLineNr++;
		iOffsetY		=	GetLine(iOffset, strIn, outString);
		DrawFont(dynamic_cast<IFontResource*>(GetBitmapResource()), rUpdate, Rect, (const tchar*)(outString.c_str()));
		Rect.iY			+=	iOffsetY;
		iOffset			+=	outString.size();

		// Limit number off lines
		if(miMaxLines != -1) {
			if(miMaxLines == iLineNr)
				return;
		}	
	}
}

tint CTextField::GetLine(tint iOffSet, std::string& strIn, std::string& strOut )
{
	// Clear strOut
	std::string e;
	strOut= e;
	// Max length of text, i.e. control width
	int iMaxLength			= GetSize().iCX;
	// The font we're using
	IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

	// Size in characters
	tint iSize				=	strIn.size();
	tint i					=	iOffSet;

	while(true) {
		// If there is no more chars in the string
		if(i >= iSize) {
			return 0;
		}
		// char
		std::string strChar;
		// Get one char from the in string
		strChar		= strIn[i];
		// Check length of out string
		tint iOutStrLength	=	pFontRes->GetTextWidth((const tchar*)(strOut.c_str()));
		iOutStrLength		+=	pFontRes->GetTextWidth((const tchar*)(strChar.c_str()));

		if( strIn[i] == 13) {
			strOut	+= strChar;
			return 12;
		}
		else if(iOutStrLength < iMaxLength){
			// Add char to out string
			strOut	+= strChar;
		}
		else {
			return 12;
		}
		i++;
	}
	return 0;

}

void CTextField::DrawCursor(std::string& strIn, SRect Rect, const SRect &rUpdate)
{
	GetDrawRect(Rect, strIn); //!!!

	tint iOffset	=	0;
	tint iOffsetY	=	0;
	tint iYPos		=	0;
	std::string outString;


	while (iOffset < (tint)strIn.size())
	{
		iOffsetY		+=	GetLine(iOffset, strIn, outString);
		iOffset			+=	outString.size();
	}
	tint iCursorPos = 0;
	//if(strIn[iOffset-1] != 13) {
	if ((iOffset>0) && (strIn[iOffset-1] != 13)) {
		// Width of text which is before cursor
		 iCursorPos = dynamic_cast<IFontResource*>(GetBitmapResource())->GetTextWidth((const tchar*)(outString.c_str()));
	}
	// The position of the cursor is the text drawing position + the width of text before cursor - 1
	SPos CursorPos((SPos)Rect + SPos(iCursorPos, 1) - SPos(1, 0));
	// The size of the cursor is 2 pixels wide, 10 pixels height
	SSize CursorSize(2, 10);	
	CursorPos.iY	+=	iOffsetY;
	// Draw the cursor
	IDrawPrimitives* pPrimitive = IDrawPrimitives::Create();
	pPrimitive->Init(GetParentWindow());
	SRect CursorRect(CursorPos, CursorSize);
	pPrimitive->DrawRect(rUpdate, CursorRect, SRGB(10, 10, 10));

}

void CTextField::StartEditing()
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

	// Lasse - added 2007-05-14
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

	mbEditing = true;
}

void CTextField::StopEditing(tbool bOK)
{
	if (bOK) {
		SetText((tchar*)(msTextCurEditing.c_str()));

		this->NotifyGeneric((void*)(msTextCurEditing.c_str()));
	}

	GetParentWindow()->ReleaseMouseFocus();

	GetParentWindow()->ReleaseKeyboardInput();

	SSize Size;
	GetParentWindow()->GetSize(Size);
	SRect Rect(SPos(0, 0), Size);
	GetParentWindow()->Redraw(Rect);

	mbEditing = false;
}

tint CTextField::GetCharacterIndex(const std::string& sText, tint iOffset)
{
	IFontResource* pFontRes = dynamic_cast<IFontResource*>(GetBitmapResource());

	tchar psz[2];
	psz[0] = sText[0];
	psz[1] = 0;

	tint iIndex = -1;
	if (iOffset >= 0) {
		iOffset -= pFontRes->GetTextWidth(psz);

		while (iOffset >= 0) {
			iIndex++;
			if (iIndex >= (tint)sText.size()) {
				iIndex--;
				break;
			}

			psz[0] = sText[iIndex];
			iOffset -= pFontRes->GetTextWidth(psz);
		}
	}

	return iIndex;
}

void CTextField::SetClicksOutsideAsSuccess()
{
	mbClicksOutsideIsSuccess = true;
}

void CTextField::SetTextFieldSize(ge::SSize Size)
{
	mRect.iX	=	Size.iCX;
	mRect.iY	=	Size.iCY;
}

void CTextField::SetMaxNumberLines(tint iMaxLines )
{
	miMaxLines	=	iMaxLines;

}






