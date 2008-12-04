/*!	\file CTextList.cpp
	\author Lasse Steen Bohnstedt
	\date 14/Sep/2006
*/

#include "geInternalOS.h"

CTextList::CTextList()
{
	mbKeyScrollingEnabled = true;

	mbMouseDraggingEnabled = true;
	miMouseDragUp = 1;
	miMouseDragDown = 1;
	miMouseDragBigUp = 1;
	miMouseDragBigDown = 1;
}

CTextList::~CTextList()
{
}

ITextList* ITextList::Create()
{
	return dynamic_cast<ITextList*>(new CTextList());
}

void CTextList::Destroy()
{
	delete dynamic_cast<CTextList*>(this);
}

void CTextList::Init(tint32 iID, IFontResource* pFont, const tchar* pszEnumList, tchar cEnumDelimiter /*= '@'*/)
{
	CText::Init(iID, pFont);

	miDefaultValue = 0;
	SetTextList(pszEnumList, cEnumDelimiter);
	msTextCur = std::string("(null)");
	mbEditable = true;
} // CTextList::Init(tint32, IFontResource*, const tchar*, tchar)


void CTextList::Init(tint32 iID, IFontResource* pFont, const tchar** ppszTextList)
{
	CText::Init(iID, pFont);

	miDefaultValue = 0;
	SetTextList(ppszTextList);
	msTextCur = std::string("(null)");
	mbEditable = true;
} // CTextList::Init(tint32, IFontResource*, const tchar**)


tint32 CTextList::SetTextList(const tchar* pszEnumList, tchar cEnumDelimiter /*= '@'*/)
{
	mlistOfValidTexts.clear();

	if (pszEnumList) {
		tchar* pcStart = (tchar*)pszEnumList;
		while (*pcStart) {
			tchar* pcEnd = pcStart;
			while ((*pcEnd)&&(*pcEnd!=cEnumDelimiter)) {
				pcEnd++;
			}
			// Insert into list
			{
				tint32 iLen = pcEnd - pcStart;
				tchar* pszTemp = new tchar[iLen + 1];
				memcpy(pszTemp, pcStart, iLen);
				pszTemp[iLen] = '\0';
				mlistOfValidTexts.insert(mlistOfValidTexts.end(), std::string((char*)pszTemp));
				delete[] pszTemp;
				pszTemp = NULL;
			}
			// Move to next
			if (*pcEnd==cEnumDelimiter) {
				// Point after delimiter char
				pcStart = pcEnd + 1;
			}
			else {
				// Last - point to ending '\0' char
				pcStart = pcEnd;
			}
		}
	}

	return mlistOfValidTexts.size();
} // CTextList::SetTextList(const tchar*, tchar)


tint32 CTextList::SetTextList(const tchar** ppszTextList)
{
	mlistOfValidTexts.clear();

	if (ppszTextList) {
		for (tint32 iIx=0; (ppszTextList[iIx])&&(ppszTextList[iIx][0]!='\0'); iIx++) {
			mlistOfValidTexts.insert(mlistOfValidTexts.end(), std::string((char*)ppszTextList[iIx]));
		}
	}

	return mlistOfValidTexts.size();
} // CTextList::SetTextList(const tchar**)


void CTextList::SetText(const tchar* pszText)
{
	if (mlistOfValidTexts.size()==0) {
//		MessageBox(NULL, "CTextList::SetText(...) - mlistOfValidTexts.size()==0", "Internal error", MB_OK);
		ASSERT(FALSE);
	}

	if (pszText==NULL) {
//		MessageBox(NULL, "CTextList::SetText(NULL)", "Internal error", MB_OK);
		ASSERT(FALSE);
	}

	tint32 iLen = strlen((char*)pszText);
	tint32 iSize = iLen + 1;
	tchar* pszTextOut = NULL;
	tint32 iNewIndex = FindListIndex(pszText, pszTextOut);

	// No match found?
	if (iNewIndex<0) {
		if (msTextCur.size()==0) {
			// Very first time: Set default
			iNewIndex = miDefaultValue;
		}
		else if (pszText[0]=='\0') {
			// Input cleared: Set default
			iNewIndex = miDefaultValue;
		}
		else {
			// Stay at last position
			iNewIndex = miValue;
		}
	}

	tbool bRedraw = false;

	// Maybe update text or value
	{
		// Set item text
		if (pszTextOut==NULL) {
			bRedraw = true;
		}
		else if (
			(msTextCur.size() == 0)
			||
			(msTextCur.compare((char*)pszTextOut) != 0)
		)
		{
			msTextCur = (char*)pszTextOut;
			bRedraw = true;
		}

		// Set value
		if (iNewIndex!=GetValue()) {
			SetValue(iNewIndex);
			bRedraw = true;
		}
	}

	// Maybe redraw
	if (bRedraw) {
		Redraw();
	}
} // void CTextList::SetText(const tchar*)


tbool CTextList::OnKeyDown(EKey Key)
{
	if (IsVisible() == false) {
		return false;
	}

	if (mbEditing) {
		if ((Key == KeyUp)||(Key == KeyDown)||(Key == KeyBigUp)||(Key == KeyBigDown)) {
			tbool bChange = false;

			// First find out if current buffer holds something valid
			{
				tchar* pszTextOut = NULL;
				tint32 iNewIndex = FindListIndex((const tchar*)msTextCurEditing.c_str(), pszTextOut);
				if (iNewIndex>=0) {
					// Match found! - go ahead from there
					miEditCurIndex = iNewIndex;
				}
			}

			// See if it's possible to scroll
			{
				tint32 iNew = miEditCurIndex;
				switch (Key) {
					case KeyUp:			iNew += miKeyScrollUp; break;
					case KeyDown:		iNew -= miKeyScrollDown; break;
					case KeyBigUp:		iNew += miKeyScrollBigUp; break;
					case KeyBigDown:	iNew -= miKeyScrollBigDown; break;
				}

				if (iNew<0) {
					iNew = 0;
				}
				else if (iNew >= (tint)mlistOfValidTexts.size()) {
					iNew = mlistOfValidTexts.size() - 1;
				}

				if (iNew!=miEditCurIndex) {
					miEditCurIndex = iNew;
					bChange = true;
				}
			}

			// Perform scroll
			if (bChange) {
				msTextCurEditing = (char*)_GetText(miEditCurIndex);
				SetText((tchar*)msTextCurEditing.c_str());

				mSelectionType = SelectionAll;
				miCursorPos = 0;
			}
			Redraw();

			return true;
		}
		else {
			return CText::OnKeyDown(Key);
		}
	}

	return false;
}


void CTextList::SetValue(tint32 iNew, tbool bNotify /*= true*/)
{
	if (mlistOfValidTexts.size()==0) {
//		MessageBox(NULL, "CTextList::SetValue(...) - mlistOfValidTexts.size()==0", "Internal error", MB_OK);
		ASSERT(FALSE);
	}

	if (iNew<0)
		iNew = 0;
	else if (iNew >= (tint)mlistOfValidTexts.size())
		iNew = mlistOfValidTexts.size() - 1;
	CControl::SetValue(iNew, bNotify);

	CText::SetText(_GetText(miValue));
} // CTextList::SetValue


tint32 CTextList::GetValue()
{
	return miValue;
} // CTextList::GetValue()


void CTextList::SetConvertedValue(tint32 iNew, tbool bNotify)
{
	CControl::SetConvertedValue(iNew, bNotify);
}


void CTextList::StartEditing()
{
	CText::StartEditing();

	// Prepare for arrow up and down scrolling
	miEditCurIndex = miValue;
}


void CTextList::StopEditing(tbool bOK)
{
	CText::StopEditing(bOK);
}


tint32 CTextList::FindListIndex(const tchar* psz, tchar*& rpszOut)
{
	tint32 iFoundIx = -1;

	if ((psz!=NULL)&&(psz[0]!='\0')) {
		tint32 iSize = mlistOfValidTexts.size();
		std::list<std::string>::iterator it = mlistOfValidTexts.begin();
		for (tint32 iIx=0; (iFoundIx<0) && (iIx<iSize); iIx++, it++)
		{
			std::string& s = *it;
			if (stricmp((char*)psz, s.c_str())==0) {
				rpszOut = ((tchar*)s.c_str());
				iFoundIx = iIx;
			}
		}
	}

	return iFoundIx;
} // CTextList::FindListIndex(tchar*)


tbool CTextList::GetText(tint32 iValue, tchar* pszTextOut, tint32 iSizeOfTextOut)
{
	tbool bSuccess = false;

	if (pszTextOut) {
		const tchar* psz = _GetText(iValue);
		if (psz) {
			tint32 iLen = strlen((char*)psz);
			tint32 iSize = iLen + 1;
			if (iSize <= iSizeOfTextOut) {
				strcpy((char*)pszTextOut, (char*)psz);
				bSuccess = true;
			}
		}
	}

	return bSuccess;
} // CTextList::GetText


const tchar* CTextList::_GetText(tint32 iValue)
{
	std::list<std::string>::iterator it = mlistOfValidTexts.begin();
	for (tint32 iIx=0; it!=mlistOfValidTexts.end(); iIx++) {
		if (iIx==iValue) {
			std::string& s = *it;
			return (tchar*)s.c_str();
		}
		it++;
	}

	return NULL;
} // CTextList::_GetText


tbool CTextList::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if (IsVisible() == false) {
		return false;
	}

	{
		SRect rctThis(GetPos(), GetSize());

		if ((MouseMsg == LeftButtonDown)||(MouseMsg==LeftButtonUp)) {
			return CText::OnMouse(MouseMsg, Pos);
		}

		if (MouseMsg == MouseMove) {
			if (mDragState == MouseDragDragging) {
				if (mbMouseVisible) {
					// Hide mouse while dragging
					GetParentWindow()->HideMouse();
					mbMouseVisible = false;
				}

				if (mbFineAdjustLast != GetParentWindow()->FineAdjustPressed()) {
					miValueDragStart = GetValue();
					mPosDragStart = Pos;
					mMouseMoved = SSize(0, 0);
					miMouseMoveCounter = 0;
					mbFineAdjustLast = GetParentWindow()->FineAdjustPressed();
				}

				// Compensate mouse position with the amount we've moved the mouse
				mMousePos = Pos - SPos(mMouseMoved.iCX, mMouseMoved.iCY);

				tint32 iPositions = mlistOfValidTexts.size(); //gControlMax - gControlMin + 1;
				tfloat fPixelsPerPosition = 200.0f / iPositions; //300.0f / iPositions;
				tint32 iDelta = mPosDragStart.iY - mMousePos.iY;
				// Adjust sensitivity for big scroll?
				if (iDelta > 0) {
					if (miMouseDragBigUp > 0) {
						fPixelsPerPosition *= miMouseDragBigUp;
					}
				}
				else {
					if (miMouseDragBigDown > 0) {
						fPixelsPerPosition *= miMouseDragBigDown;
					}
				}
				tint32 iPositionDelta = Float2Int(iDelta / fPixelsPerPosition);
				if (!GetParentWindow()->FineAdjustPressed()) {
					if (iPositionDelta > 0) {
						if (miMouseDragUp > 0) {
							iPositionDelta *= miMouseDragUp;
						}
					}
					else if (iPositionDelta < 0) {
						if (miMouseDragDown > 0) {
							iPositionDelta *= miMouseDragDown;
						}
					}
				}

				SetValue(miValueDragStart + iPositionDelta);
				miEditCurIndex = GetValue();
				msTextCurEditing = (char*)_GetText(miEditCurIndex);
				SetText((tchar*)msTextCurEditing.c_str());

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

				return true;
			}
		}
	}

	return CText::OnMouse(MouseMsg, Pos);
} // CTextList::OnMouse
