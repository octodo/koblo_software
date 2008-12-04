/*!	\file CPopupMenu.cpp
	\author Michael Olsen
	\date 18/Feb/2005
	\date 18/Feb/2005
*/

#include "geInternalOS.h"


CPopupMenu::CPopupMenu()
	: miSelectedLast(-1), mpBitmapForInlineIcon(NULL)
{
	SetAutomaticResize(false);

	mbSubMenuShowing = false;
	mbPositionSidewaysFirst = false;

	mbHadToOverlay = false;
	mbDisregardFirstMouseUp = false;
	mbKeepSelectionUntilFirstMove = true;
	
	// Lasse, 2007-12-03 thru -10 - flexibility
	mpFont = NULL;
	mpFontInv = NULL;
	mpWnd = NULL;
	mpResMan = NULL;
	mpiResIDs = NULL;
	mbSetValueUpdatesParentToo = false;
	mbSetValueUpdatesParentToo_ApplyRecursively = false;
	
	memset(mppPopupMenus, 0, GE_MAX_NR_OF_POPUP_ITEMS*sizeof(IPopupMenu*));
	// .. Lasse
}

CPopupMenu::~CPopupMenu()
{
	// Lasse, modified 2007-12-03 - code reuse
	/*
	for (tint iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mppPopupMenus[iItem]) {
			mppPopupMenus[iItem]->Destroy();
			delete mItems.pItems[iItem].pSubItemList;
		}
		mppTexts[iItem]->Destroy();
		mppTextsInv[iItem]->Destroy();
	}
	*/
	DestroyItems();
	// .. Lasse

	// Lasse, 2007-12-04 - flexibility
	if (mpiResIDs) {
		delete[] mpiResIDs;
		mpiResIDs = NULL;
	}
	// .. Lasse
	
	tint iBitmap;
	for (iBitmap = 0; iBitmap < IPopupMenu::BitmapCount; iBitmap++) {
		mppBitmaps[iBitmap]->DecRefCount();
	}

	if (mpBitmapForInlineIcon) {
		mpBitmapForInlineIcon->DecRefCount();
		mpBitmapForInlineIcon = NULL;
	}
}

IPopupMenu* IPopupMenu::Create()
{
	return dynamic_cast<IPopupMenu*>(new CPopupMenu());
}

void CPopupMenu::Destroy()
{
	// Lasse, moved here 2007-12-04
	// Decrease the ref count of the fonts by one, to compensate for the loading done by the user of the popup
	if (mpFont)
		mpFont->DecRefCount();
	if (mpFontInv)
		mpFontInv->DecRefCount();
	// .. Lasse
	
	delete dynamic_cast<CPopupMenu*>(this);
}

void CPopupMenu::Init(tint32 iID, IWindow* pWnd, IResourceManager* pResMan, tint32* piIDs, IFontResource* pFont, IFontResource* pFontInv, const SMenuItemList& Items)
{
	CControl::Init(iID, NULL);

	// Create the bitmap resources
	tint iBitmap;
	for (iBitmap = 0; iBitmap < IPopupMenu::BitmapCount; iBitmap++) {
		IBitmapResource* pBitmapRes = pResMan->GetBitmap(pWnd, piIDs[iBitmap], 1, 1);
		mppBitmaps[iBitmap] = pBitmapRes;
	}

	// Lasse, changed 2007-12-04 - allow for 0 item init
	/*
	// Copy the items to local storage
	mItems = Items;
	*/
	if (&Items == NULL)
		mItems.iItemCount = 0;
	else {
		// Copy the items to local storage
		mItems = Items;
	}
	// .. Lasse
	
	// Lasse, 2007-12-03 - flexibility
	mpFont = pFont;
	mpFontInv = pFontInv;
	mpWnd = pWnd;
	mpResMan = pResMan;
	if (piIDs) {
		// Copy resource IDs (may be on stack)
		mpiResIDs = new tint32[BitmapCount];
		memcpy((void*)mpiResIDs, (void*)piIDs, (BitmapCount * sizeof(tint32)));
	}
	
	// .. Lasse
	
	// Lasse, moved here 2007-12-03 - speed
	//SSize SizeCenter;
	mppBitmaps[BitmapCenter]->GetRealSize(mSizeCenter);
	// .. Lasse

	// Create the text controls
	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		mppTexts[iItem] = IText::Create();

		pFont->IncRefCount();
		mppTexts[iItem]->Init(IControl::giNoID, pFont);
		mppTexts[iItem]->SetParentPane(dynamic_cast<IPane*>(this));

		// Lasse, moved 2007-12-03 - speed
		/*
		SSize SizeCenter;
		mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);
		*/
		mppTexts[iItem]->SetSize(mSizeCenter);

//		mppTexts[iItem]->SetHorzAlignment(IText::HorzAlignCenter);
		mppTexts[iItem]->SetHorzAlignment(IText::HorzAlignLeft);
		mppTexts[iItem]->SetVertAlignment(IText::VertAlignCenter);

		mppTexts[iItem]->SetText(mItems.pItems[iItem].pszText);

		// Inverted
		mppTextsInv[iItem] = IText::Create();

		pFontInv->IncRefCount();
		mppTextsInv[iItem]->Init(IControl::giNoID, pFontInv);
		mppTextsInv[iItem]->SetParentPane(dynamic_cast<IPane*>(this));

		mppTextsInv[iItem]->SetSize(mSizeCenter);

//		mppTextsInv[iItem]->SetHorzAlignment(IText::HorzAlignCenter);
		mppTextsInv[iItem]->SetHorzAlignment(IText::HorzAlignLeft);
		mppTextsInv[iItem]->SetVertAlignment(IText::VertAlignCenter);

		mppTextsInv[iItem]->SetText(mItems.pItems[iItem].pszText);
	}

	// Lasse, moved 2007-12-04 - can't decrease now because we might end at 0, which would cause the resource to be destroyed
	/*
	// Decrease the ref count of the fonts by one, to compensate for the loading done by the user of the popup
	pFont->DecRefCount();
	pFontInv->DecRefCount();
	*/
	// .. Lasse

	// Create sub menus
	// Lasse, modified 2007-12-10 - code re-use
	/*
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mItems.pItems[iItem].pSubItemList != NULL) {
			// Entry has a sub menu
			// First we need to reallocate the item list, since it might be on stack storage
			SMenuItemList* pList = new SMenuItemList;
			*pList = *(mItems.pItems[iItem].pSubItemList);
			mItems.pItems[iItem].pSubItemList = pList;

			// Simulate loading of fonts by increasing ref count
			pFont->IncRefCount();
			pFontInv->IncRefCount();
			// Now create popup
			IPopupMenu* pPopup = IPopupMenu::Create();
			pPopup->Init(IControl::giNoID, pWnd, pResMan, piIDs, pFont, pFontInv, *pList);
			pPopup->SetParentPane(dynamic_cast<IPane*>(this));
			// Add ourselves as listener
			pPopup->AddListener(dynamic_cast<IEventReceiver*>(this));
			pPopup->SetVisible(false);
			mppPopupMenus[iItem] = pPopup;
		}
		else {
			mppPopupMenus[iItem] = NULL;
		}
	}
	*/
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		SMenuItemList* pList = mItems.pItems[iItem].pSubItemList;
		if (pList)
			CreateSubMenuBelowItem(iItem, pList);
		else
			DestroySubMenuBelowItem(iItem);
	}
	// .. Lasse
	
	// Lasse, modified 2007-12-04 - code reuse
	/*
	// Calculate the size
	SSize SizeTop;
	mppBitmaps[BitmapTop]->GetRealSize(SizeTop);
	SSize SizeBottom;
	mppBitmaps[BitmapBottom]->GetRealSize(SizeBottom);
	SSize SizeCenter;
	mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);

	SSize SizeThis = SizeTop;
	SizeThis.iCY += SizeBottom.iCY;
	SizeThis.iCY += (SizeCenter.iCY * mItems.iItemCount);
	SetSize(SizeThis);
	 */
	SetSizeThis();
	// .. Lasse
} // CPopupMenu::Init


void CPopupMenu::SetSizeThis()
{
	// Calculate the size
	SSize SizeTop;
	mppBitmaps[BitmapTop]->GetRealSize(SizeTop);
	SSize SizeBottom;
	mppBitmaps[BitmapBottom]->GetRealSize(SizeBottom);
	//SSize SizeCenter;
	//mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);
	
	SSize SizeThis = SizeTop;
	SizeThis.iCY += SizeBottom.iCY;
	SizeThis.iCY += (mSizeCenter.iCY * mItems.iItemCount);
	SetSize(SizeThis);
} // CPopupMenu::SetSizeThis


tint32 CPopupMenu::DestroyItems(tint32 iStartIx /*= 0*/, tint32 iCount /*= -1*/)
{
	if ((iStartIx < 0) || (iStartIx >= mItems.iItemCount))
		return -1;
	
	tint32 iEndIxPlus1;
	if (iCount < 0) {
		iEndIxPlus1 = mItems.iItemCount;
	}
	else {
		iEndIxPlus1 = iStartIx + iCount;
		if (iEndIxPlus1 > mItems.iItemCount)
			iEndIxPlus1 = mItems.iItemCount;
	}
	iCount = iEndIxPlus1 - iStartIx;
	
	if (iCount > 0) {
		for (tint iIx = iStartIx; iIx < iEndIxPlus1; iIx++) {
			if (mppPopupMenus[iIx]) {
				mppPopupMenus[iIx]->Destroy();
				mppPopupMenus[iIx] = NULL;
				if (mItems.pItems[iIx].pSubItemList) {
					delete mItems.pItems[iIx].pSubItemList;
				}
				mItems.pItems[iIx].pSubItemList = NULL;
			}
			mppTexts[iIx]->Destroy();
			mppTextsInv[iIx]->Destroy();
		}
		
		if (iEndIxPlus1 < mItems.iItemCount) {
			// End not deleted - move remaining items up
			tint32 iIxSrc = iEndIxPlus1;
			tint32 iIxDst = iStartIx;
			for (; iIxSrc < mItems.iItemCount; iIxSrc++, iIxDst++) {
				mItems.pItems[iIxDst] = mItems.pItems[iIxSrc];
				mItems.pItems[iIxSrc].pSubItemList = NULL;
				
				mppPopupMenus[iIxDst] = mppPopupMenus[iIxSrc];
				mppPopupMenus[iIxSrc] = NULL;

				mppTexts[iIxDst] = mppTexts[iIxSrc];
				mppTextsInv[iIxDst] = mppTextsInv[iIxSrc];
			} 
		}
		
		mItems.iItemCount -= iCount;
	}
	
	SetSizeThis();
	
	return iCount;
} // CPopupMenu::DestroyItems


IPopupMenu::SMenuItem* CPopupMenu::CreateAndInsertItem(tint32 iInsertIx, const tchar* pText, tint32 iValue, SMenuItemList* pSubList /*= NULL*/, IBitmapResource* pBitmapRes /*= NULL*/, tint32 iBitmapFrame /*= -1*/, tint32 iNrOfBitmapFrames /*= 0*/)
{
	// Verify creation possible
	if (iInsertIx > mItems.iItemCount) {
		// Index out of range
		return NULL;
	}

	
	// Find append position
	if (iInsertIx < 0) {
		// Append to end
		iInsertIx = mItems.iItemCount; 
	}
	
	
	// Find value
	if (iValue < 0)
		iValue = iInsertIx;
	
	
	// Keep original items sane
	if (iInsertIx < mItems.iItemCount) {
		// We'll be inserting before end of list - first move the remaining items down
		tint32 iIxDst = mItems.iItemCount;
		tint32 iIxSrc = iIxDst - 1;
		for (; iIxDst > iInsertIx; iIxDst--, iIxSrc--) {
			mItems.pItems[iIxDst] = mItems.pItems[iIxSrc];
			mppTexts[iIxDst] = mppTexts[iIxSrc];
			mppTextsInv[iIxDst] = mppTextsInv[iIxSrc];
		}
	}
	
	
	// Create new item
	SMenuItem* pNewItem = &mItems.pItems[iInsertIx];//new SMenuItem(pText, iValue, pSubList, pBitmapRes, iBitmapFrame, iNrOfBitmapFrames);
	//mItems.pItems[iInsertIx] = pNewItem;
	strcpy((char*)(pNewItem->pszText), (const char*)pText);
	pNewItem->iValue = iValue;
	//pNewItem->pSubItemList = pSubList;
	pNewItem->pBitmap = pBitmapRes;
	pNewItem->iBitmapFrame = iBitmapFrame;
	pNewItem->iNrOfBitmapFrames = iNrOfBitmapFrames;
	
	// Maybe create sub-menu
	if (pSubList)
		CreateSubMenuBelowItem(iInsertIx, pSubList);
	else
		DestroySubMenuBelowItem(iInsertIx);

	// Create IText control for new item
	mppTexts[iInsertIx] = IText::Create();
	mpFont->IncRefCount();
	mppTexts[iInsertIx]->Init(IControl::giNoID, mpFont);
	mppTexts[iInsertIx]->SetParentPane(dynamic_cast<IPane*>(this));
	mppTexts[iInsertIx]->SetSize(mSizeCenter);
	mppTexts[iInsertIx]->SetHorzAlignment(IText::HorzAlignLeft);
	mppTexts[iInsertIx]->SetVertAlignment(IText::VertAlignCenter);
	mppTexts[iInsertIx]->SetText(mItems.pItems[iInsertIx].pszText);

	// Create inverted IText control for new item
	mppTextsInv[iInsertIx] = IText::Create();
	mpFontInv->IncRefCount();
	mppTextsInv[iInsertIx]->Init(IControl::giNoID, mpFontInv);
	mppTextsInv[iInsertIx]->SetParentPane(dynamic_cast<IPane*>(this));
	mppTextsInv[iInsertIx]->SetSize(mSizeCenter);
	mppTextsInv[iInsertIx]->SetHorzAlignment(IText::HorzAlignLeft);
	mppTextsInv[iInsertIx]->SetVertAlignment(IText::VertAlignCenter);
	mppTextsInv[iInsertIx]->SetText(mItems.pItems[iInsertIx].pszText);
	
	// Remember there's one more item
	mItems.iItemCount++;
	//std::cerr << "mItems.iItemCount == " << mItems.iItemCount << "\n";

	
	SetSizeThis();
	
	return pNewItem;
} // CPopupMenu::CreateAndInsertItem


IPopupMenu::SMenuItem* CPopupMenu::CreateAndAppendItem(const tchar* pText, tint32 iValue, SMenuItemList* pSubList /*= NULL*/, IBitmapResource* pBitmapRes /*= NULL*/, tint32 iBitmapFrame /*= -1*/, tint32 iNrOfBitmapFrames /*= 0*/)
{
	return CreateAndInsertItem(-1, pText, iValue, pSubList, pBitmapRes, iBitmapFrame, iNrOfBitmapFrames);
}


tint32 CPopupMenu::AppendItems(const SMenuItemList& rList)
{
	return InsertItems(-1, rList);
} // CPopupMenu::AppendItems


tint32 CPopupMenu::InsertItems(tint32 iInsertIx, const SMenuItemList& rList)
{
	tint32 iCount = 0;
	
	// Find append position
	if (iInsertIx < 0) {
		// Append to end
		iInsertIx = mItems.iItemCount; 
	}
	
	while (iCount < rList.iItemCount) {
		const SMenuItem* pItemInList = &(rList.pItems[iCount]);
		SMenuItem* pItemAdded = CreateAndInsertItem(iInsertIx, pItemInList->pszText, pItemInList->iValue, pItemInList->pSubItemList, pItemInList->pBitmap, pItemInList->iBitmapFrame, pItemInList->iNrOfBitmapFrames);
		if (pItemAdded == NULL) {
			// Some error occurred while adding - roll back
			if (iCount > 0) {
				tint32 iRemoveIx = (iInsertIx < 0) ? (mItems.iItemCount - iCount) : (iInsertIx - iCount);
				DestroyItems(iRemoveIx, iCount);
			}
			return -1;
		}
		
		iInsertIx++;
		iCount++;
	}
	
	return iCount;
} // CPopupMenu::InsertItems



tbool CPopupMenu::DestroySubMenuBelowItem(tint32 iItemIndex)
{
	if ((iItemIndex < 0) || (iItemIndex >= mItems.iItemCount))
		return false;
	
	IPopupMenu* pSubMenu = mppPopupMenus[iItemIndex];
	if (pSubMenu) {
		// Destroy sub-menu
		pSubMenu->Destroy();
		mppPopupMenus[iItemIndex] = NULL;
		// Destroy item-list
		delete mItems.pItems[iItemIndex].pSubItemList;
		mItems.pItems[iItemIndex].pSubItemList = NULL;
	}
	
	return true;
} // CPopupMenu::DestroySubMenuBelowItem


IPopupMenu* CPopupMenu::CreateSubMenuBelowItem(tint32 iItemIndex, SMenuItemList* pSubList /*= NULL*/)
{
//	if (!DestroySubMenuBelowItem(iItemIndex))
//		return NULL;
	DestroySubMenuBelowItem(iItemIndex);

	IPopupMenu* pSubMenu = IPopupMenu::Create();
	if (pSubMenu != NULL) {
		// Simulate loading of fonts by increasing ref count
		mpFont->IncRefCount();
		mpFontInv->IncRefCount();
		// Now create popup
		//IPopupMenu* pPopup = IPopupMenu::Create();
		pSubMenu->Init(IControl::giNoID, mpWnd, mpResMan, mpiResIDs, mpFont, mpFontInv, *pSubList);
		pSubMenu->SetParentPane(dynamic_cast<IPane*>(this));
		// Add ourselves as listener
		pSubMenu->AddListener(dynamic_cast<IEventReceiver*>(this));
		pSubMenu->SetVisible(false);

		// Maybe apply same "update-parent" rule for new child
		if (mbSetValueUpdatesParentToo_ApplyRecursively)
		{
			pSubMenu->SetValueUpdatesParentToo(mbSetValueUpdatesParentToo, mbSetValueUpdatesParentToo_ApplyRecursively);
		}
	}
	mppPopupMenus[iItemIndex] = pSubMenu;
	
	return pSubMenu;
} // CPopupMenu::CreateSubMenuBelowItem(tint32, SMenuItemList*)


IPopupMenu* CPopupMenu::CreateSubMenuBelowItem(tint32 iItemIndex, tchar* pszEnumList, char cDelimiter, tint32 iFirstItemValue, IBitmapResource* pBitmapRes /*= NULL*/, tint32 iNrOfBitmapFrames /*= 1*/)
{
	IPopupMenu* pSubMenu = CreateSubMenuBelowItem(iItemIndex);
	if (pSubMenu) {
		std::list<std::string> listItems;
		PszEnumString2List(pszEnumList, cDelimiter, listItems);
		std::list<std::string>::iterator it = listItems.begin();
		tint32 iIx = 0;
		for (; it != listItems.end(); it++, iIx++) {
			pSubMenu->CreateAndAppendItem((*it).c_str(), iFirstItemValue + iIx, NULL, pBitmapRes, iNrOfBitmapFrames);
		}
	}
	
	return pSubMenu;
} // CPopupMenu::CreateSubMenuBelowItem(tint32, tchar*, char, tint32)


void CPopupMenu::SetValueUpdatesParentToo(tbool bWillSetParent /*= true*/, tbool bApplyRecursively /*= true*/)
{
	mbSetValueUpdatesParentToo = bWillSetParent;
	if (bApplyRecursively) {
		for (tint iIx = 0; iIx < mItems.iItemCount; iIx++) {
			IPopupMenu* pSubMenu = mppPopupMenus[iIx];
			if (pSubMenu)
				pSubMenu->SetValueUpdatesParentToo(bWillSetParent, bApplyRecursively);
		}
	}
} // CPopupMenu::SetValueUpdatesParentToo


void CPopupMenu::SetInlineIconBitmap(IBitmapResource* pBitmap, tint32 iFrames)
{
	mpBitmapForInlineIcon = pBitmap;
	miNrOfFramesInInlineIcon = iFrames;
} // CPopupMenu::SetInlineIconBitmap


void CPopupMenu::EventValueChange(IControl* /*pSender*/, tint32 iValueNew)
{
	// A sub menu reported a value change, so report it
	SetValue(iValueNew, true);
}

tint32 CPopupMenu::GetMouseOverItem(SPos Pos)
{
	// We shouldn't move a valid selection before first mouse-move event
	if (mbKeepSelectionUntilFirstMove) {
		// .. so we'll return current value - if it's valid
		for (tint32 iIx = 0; iIx < mItems.iItemCount; iIx++) {
			tint32 iTryValue = mItems.pItems[iIx].iValue;
			if (iTryValue == miValue) {
				// It's valid! Return it
				return iIx;
			}
		}
		// .. else -1 for none
		return -1;
	}

	// Get the sizes
	SSize SizeTop;
	mppBitmaps[BitmapTop]->GetRealSize(SizeTop);
	SSize SizeCenter;
	mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);

	// Offset with current position
	Pos -= GetPos();

	// First check the x position
	if (Pos.iX < 0) {
		return -1;
	}
	if (Pos.iX >= SizeTop.iCX) {
		return -1;
	}

	// The x position is ok, check the y position
	Pos.iY -= SizeTop.iCY;
	if (Pos.iY < 0) {
		return -1;
	}

	tint iItem = 0;
	while((Pos.iY > SizeCenter.iCY) && (iItem < mItems.iItemCount)) {
		Pos.iY -= SizeCenter.iCY;
		iItem++;
	}

	if (iItem == mItems.iItemCount) {
		return -1;
	}

	return iItem;
}

void CPopupMenu::GetText(tint32 iValue, tchar* psz)
{
	// Make empty string, so we can check if we found it when calling recursively
	*psz = 0;

	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		// Lasse, modified 2007-12-10 - flexibility
		/*
		if (mItems.pItems[iItem].pSubItemList) {
		*/
		if (mppPopupMenus[iItem]) {
		// .. Lasse
			// Has a sub-menu
			mppPopupMenus[iItem]->GetText(iValue, psz);
			// Did we find it?
			if (*psz != 0) {
				// Yes, we found it
				break;
			}
		}
		else {
			// Has no sub-menu
			if (mItems.pItems[iItem].iValue == iValue) {
				// Found it
				strcpy((char*)psz, (char*)(mItems.pItems[iItem].pszText));
				break;
			}
		}
	}
}

void CPopupMenu::OnDraw(const SRect &rUpdate)
{
	if( !IsVisible() )
		return;

	// Get the selected item (if any)
	SPos MousePos;
	GetParentWindow()->GetMousePos(MousePos);
	miSelectedLast = GetMouseOverItem(MousePos);

	SSize SizeTop;
	mppBitmaps[BitmapTop]->GetRealSize(SizeTop);
	SSize SizeBottom;
	mppBitmaps[BitmapBottom]->GetRealSize(SizeBottom);
	SSize SizeCenter;
	mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);
	SSize SizeArrow;
	mppBitmaps[BitmapFolderArrow]->GetRealSize(SizeArrow);

	// Top
	SRect Rct;
	Rct = SRect(GetPos(), SizeTop);
	DrawBitmap(mppBitmaps[BitmapTop],
		rUpdate,
		Rct);
	Rct.iY += SizeTop.iCY;

	// Center
	Rct.iCX = SizeCenter.iCX;
	Rct.iCY = SizeCenter.iCY;
	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		/*		if (iItem == miSelectedLast) {
			DrawBitmap(mppBitmaps[BitmapCenterInv],
				rUpdate,
				Rct);

			// Text
			if (mppTextsInv[iItem]) {
				mppTextsInv[iItem]->SetPos((SPos)Rct - GetPos() + SPos(4, 0));
				mppTextsInv[iItem]->OnDraw(rUpdate);
			}
		}
		else {
			DrawBitmap(mppBitmaps[BitmapCenter],
				rUpdate,
				Rct);

			// Text
			if (mppTexts[iItem]) {
				mppTexts[iItem]->SetPos((SPos)Rct - GetPos() + SPos(4, 0));
				mppTexts[iItem]->OnDraw(rUpdate);
			}
		}*/

		DrawBitmap(mppBitmaps[BitmapCenter],
			rUpdate,
			Rct);

		// Lasse: Maybe draw inline icon
		/*
		SSize sizeInlineIcon;
		if (mpBitmapForInlineIcon) {
			//mpBitmapForInlineIcon->SetPos((SPos)Rct - GetPos() + SPos(4, 0));
			if (iItem<miNrOfFramesInInlineIcon) { //true) { //(iItem<mpBitmapForInlineIcon->GetNrOfFrames()) {
				mpBitmapForInlineIcon->GetRealSize(sizeInlineIcon);
				SRect rectInlineIcon;
				rectInlineIcon.iX = Rct.iX+1;
				rectInlineIcon.iY = Rct.iY+1;
				rectInlineIcon.iCX = sizeInlineIcon.iCX;
				rectInlineIcon.iCY = sizeInlineIcon.iCY;

				DrawBitmapFrame(mpBitmapForInlineIcon, rUpdate, rectInlineIcon, iItem, miNrOfFramesInInlineIcon, false);
			}
		}*/
		SSize sizeInlineIcon;
		IBitmapResource* pBitmap = mItems.pItems[iItem].pBitmap;
		if (pBitmap) {
			pBitmap->GetRealSize(sizeInlineIcon);

			SRect rectInlineIcon;
			rectInlineIcon.iX = Rct.iX+1;
			rectInlineIcon.iY = Rct.iY+1;
			rectInlineIcon.iCX = sizeInlineIcon.iCX;
			rectInlineIcon.iCY = sizeInlineIcon.iCY;
			
			tint32 iBitmapFrame = mItems.pItems[iItem].iBitmapFrame;
			tint32 iNrOfBitmapFrames = mItems.pItems[iItem].iNrOfBitmapFrames;

			DrawBitmapFrame(pBitmap, rUpdate, rectInlineIcon, iBitmapFrame, iNrOfBitmapFrames, false);
		}
		// .. Lasse
		
		// Text
		if (mppTexts[iItem]) {
			// Lasse: Modified
			//mppTexts[iItem]->SetPos((SPos)Rct - GetPos() + SPos(4, 0));
			SPos posTextOfs = SPos(4, 0);
			if (pBitmap) {
				posTextOfs.iX += sizeInlineIcon.iCX;
			}
			mppTexts[iItem]->SetRelPos((SPos)Rct - GetAbsPos() + posTextOfs);
			// .. Lasse
			mppTexts[iItem]->OnDraw(rUpdate);
		}

		if (mItems.pItems[iItem].pSubItemList) {
			// Folder

			SRect RctArrow;
			RctArrow.iX = Rct.iX + Rct.iCX - SizeArrow.iCX - 6;
			RctArrow.iY = Rct.iY + (Rct.iCY - SizeArrow.iCY) / 2;
			RctArrow.iCX = SizeArrow.iCX;
			RctArrow.iCY = SizeArrow.iCY;

			DrawBitmap(mppBitmaps[BitmapFolderArrow],
				rUpdate,
				RctArrow);
		}

		if (iItem == miSelectedLast) {
			SRect RctInvert(Rct);
			RctInvert.iX += 1;
			RctInvert.iY += 1;
			RctInvert.iCX -= 2;
			RctInvert.iCY -= 2;

			IDrawPrimitives* pPrimitive = IDrawPrimitives::Create();
			pPrimitive->Init(GetParentWindow());
			pPrimitive->InvertRect(rUpdate, RctInvert);
			pPrimitive->Destroy();
		}

		Rct.iY += SizeCenter.iCY;
	}

	// Bottom
	Rct.iCX = SizeCenter.iCX;
	Rct.iCY = SizeCenter.iCY;
	DrawBitmap(mppBitmaps[BitmapBottom],
		rUpdate,
		Rct);

	// Sub menus
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mppPopupMenus[iItem]) {
			mppPopupMenus[iItem]->OnDraw(rUpdate);
		}
	}
} // CPopupMenu::OnDraw


SRect CPopupMenu::GetItemPos(tint32 iItem)
{
	// Get the sizes
	SSize SizeTop;
	mppBitmaps[BitmapTop]->GetRealSize(SizeTop);
	SSize SizeCenter;
	mppBitmaps[BitmapCenter]->GetRealSize(SizeCenter);

	// Start with top-left position
	SRect rct(GetPos(), SizeCenter);

	// Add top bitmap
	rct += SPos(0, SizeTop.iCY);

	// Offset with item index
	rct += SPos(0, SizeCenter.iCY * iItem);

	return rct;
}

void CPopupMenu::SetValue(tint32 iNew, tbool bNotify)
{
	CPane::SetValue(iNew, bNotify);

	// Lasse, added 2007-12-06 - flexibility
	// Maybe update parent menu as well
	if (mbSetValueUpdatesParentToo) {
		IPane* pPane = GetParentPane();
		IPopupMenu* pParentMenu = dynamic_cast<IPopupMenu*>(pPane);
		if (pParentMenu) {
			pParentMenu->SetValue(iNew, bNotify);
		}
		else {
			IDropDownListBox* pParentDropDown = dynamic_cast<IDropDownListBox*>(pPane);
			if (pParentDropDown) {
				pParentDropDown->SetValue(iNew, bNotify);
			}
		}
	}
	// .. Lasse
}

tbool CPopupMenu::OnMouse(EMouseMsg MouseMsg, const SPos& Pos)
{
	if( !IsVisible() )
		return false;

	tint iSelected = GetMouseOverItem(Pos);
	if (iSelected != miSelectedLast) {
		CControl::Redraw();
	}

	if ((mbDisregardFirstMouseUp) && (iSelected == -1)) {
		// We're outside control - count next mouse-up as valid
		mbDisregardFirstMouseUp = false;
	}

	if (MouseMsg == LeftButtonUp) {
		if (mbDisregardFirstMouseUp)
			mbDisregardFirstMouseUp = false;
		else if (iSelected != -1) {
			// Left button was released over one of the selections
			SetValue(mItems.pItems[iSelected].iValue);
			// Lasse: Added 2006-10-30
			return true;
			// ., Lasse
		}
	}

	// Lasse - added
	if ((MouseMsg == LeftButtonDown) || (MouseMsg == RightButtonDown) || (MouseMsg == RightButtonUp)) {
		if (iSelected != -1) {
			// Click was inside this pop-up! Don't let it fall through to controls below
			// "Eat" the message so nothing further happens
			return true;
		}
		else {
			if (!mbSubMenuShowing) {
				// No sub-menu - we're the deepest level opened

				// Move back upward to see if another CPopupMenu has mouse over
				tbool bOtherInFocus = false;
				IPane* pPaneOwner = NULL;
				IPopupMenu* pPopupTopLevel = this;
				{
					IPane* pPane = GetParentPane();
					while (pPaneOwner==NULL) {
						CPopupMenu* pPopup = dynamic_cast<CPopupMenu*>(pPane);
						if (pPopup==NULL) {
							// This is not a pop-up. Must be owning pane
							pPaneOwner = pPane;
						}
						else {
							pPopupTopLevel = pPopup;
							tint32 iOtherSelected = pPopup->GetMouseOverItem(Pos);
							if (iOtherSelected != -1) {
								// Yes! This pop-up has mouse over
								bOtherInFocus = true;
							}
							else {
								pPane = pPopup->GetParentPane();
							}
						}
					}
				}

				if (bOtherInFocus) {
					// "Eat" the message so nothing further happens
					return true;
				}
				else {
					// Clicked on one of the controls of the owning pane? (CDropDownListBox or CMultiStateDropDownListBox)
					if (pPaneOwner) {
						CPane* pCPaneOwner = dynamic_cast<CPane*>(pPaneOwner);
						tbool bClickOnOwningsPanesControls = pCPaneOwner->IsAbsPosWithinAControl(Pos);

						if (!bClickOnOwningsPanesControls) {
							// The click was outside the pane (CDropDownListBox or CMultiStateDropDownListBox) that owns us

							// Tell listener (CDropDownListBox or CMultiStateDropDownListBox) to hide us
							{
								SEventGeneric event;
								event.eType = giEventMouse;
								event.bInsideControlRect = false;
								event.iMsg = MouseMsg;
								event.pos = Pos;

								pPopupTopLevel->NotifyGeneric(&event);
							}

							// Resend the mouse event
							{
								IWindow* pIWindow = GetParentWindow();
								if (pIWindow) {
									CWindow* pWindow = dynamic_cast<CWindow*>(pIWindow);
									if (pWindow) {
										pWindow->ResendMouseEvent();
									}
								}
							}
						}
					}

				}
			}
		}
	}
	// .. Lasse

	if (MouseMsg == MouseMove) {
		if (mbKeepSelectionUntilFirstMove)
			mbKeepSelectionUntilFirstMove = false;
		if (iSelected != -1) {
			// Lasse, modified 2007-12-06 - flexibility
			/*
			if (mItems.pItems[iSelected].pSubItemList) {
			 */
			if (mppPopupMenus[iSelected]) {
			// .. Lasse
				// Item has a sub menu, open it

				// Lasse - rewritten 2006-12-02
				// Find best position for sub menu
				SPos posAbsPopup;
				{
					SRect rctItem = GetItemPos(iSelected);
					SPos posItem = SPos(rctItem.iX, rctItem.iY);
					SSize sizeItem = SSize(rctItem.iCX, rctItem.iCY);
					IPopupMenu* pPopup = mppPopupMenus[iSelected];
					pPopup->PositionSidewaysFirst();
					posAbsPopup = pPopup->GetBestAbsPos(posItem, sizeItem);
				}
				// .. Lasse

				// Make it visible, while making all other popups invisible
				tint iItem;
				for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
					if (mppPopupMenus[iItem]) {
						if (iItem == iSelected) {
							//mppPopupMenus[iItem]->SetPos(posPopup);
							mppPopupMenus[iItem]->SetAbsPos(posAbsPopup);
							mppPopupMenus[iItem]->SetVisible(true);
						}
						else {
							mppPopupMenus[iItem]->SetVisible(false);
						}
					}
				}

				// Lasse - added
				mbSubMenuShowing = true;
				// .. Lasse

				// Redraw whole window
				SSize SizeWindow;
				GetParentWindow()->GetSize(SizeWindow);
				SRect rctRedraw(SPos(0, 0), SizeWindow);
				GetParentWindow()->Redraw(rctRedraw);
			}
			else {
				// Item doesn't have sub menu, so close all other sub menus
				tint iItem;
				for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
					if (mppPopupMenus[iItem]) {
						mppPopupMenus[iItem]->SetVisible(false);
					}
				}

				// Lasse - added
				mbSubMenuShowing = false;
				// .. Lasse
			}
		}
	}

	bool bRet = false;
	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mppPopupMenus[iItem]) {
			bool bRetTmp = mppPopupMenus[iItem]->OnMouse(MouseMsg, Pos);
			if (bRetTmp) {
				bRet = bRetTmp;
			}
		}
	}

	//bRet |= CMouseTrap::OnMouse(MouseMsg,Pos);

	return bRet;
}

tbool CPopupMenu::OnKeyDown(EKey Key)
{
	if (!IsVisible()) {
		return false;
	}

	// Lasse
	switch (Key) {
		case KeyEscape:
		{
			SPos posMouse;
			GetParentWindow()->GetMousePos(posMouse);

			SEventGeneric event;
			event.eType = giEventKeyboard;
			event.bInsideControlRect = GetRect().Inside(posMouse);
			event.iMsg = Key;
			event.pos = ge::SPos(0,0);

			// For some reason we must send message from top-level menu or it won't work
			IPopupMenu* pPopupTopLevel = this;
			tbool bDone = false;
			while (!bDone) {
				IPopupMenu* pPopup = dynamic_cast<IPopupMenu*>(pPopupTopLevel->GetParentPane());
				if (pPopup) {
					pPopupTopLevel = pPopup;
				}
			}
			pPopupTopLevel->NotifyGeneric(&event);
			//NotifyGeneric(&event);
		}
		break;

	}
	// .. Lasse

	bool bRet = false;
	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mppPopupMenus[iItem]) {
			bool bRetTmp = mppPopupMenus[iItem]->OnKeyDown(Key);
			if (bRetTmp) {
				bRet = bRetTmp;
			}
		}
	}

	return bRet;
}

void CPopupMenu::OnTimer()
{
	CControl::OnTimer();

	if (!IsVisible()) {
		return;
	}

	SPos MousePos;
	GetParentWindow()->GetMousePos(MousePos);
	tint iSelected = GetMouseOverItem(MousePos);
	if (iSelected != miSelectedLast) {
		CControl::Redraw();
	}

	tint iItem;
	for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
		if (mppPopupMenus[iItem]) {
			mppPopupMenus[iItem]->OnTimer();
		}
	}

	// Lasse
	IWindow* pWindow = GetParentWindow();
	if (pWindow) {
		if (true) { //(dynamic_cast<CWindow*>(pWindow)->IsKeyboardAcquired()) {
#ifdef WIN32
			if (pWindow->AltPressed()) {
				// (mo) We need to find another solution, not only because of the WIN32 macro, but because it seems sort of like a hack to check for alt-f4 this way.
				// Don't stay around when Alt key is down - might be Alt+F4
				// We rely upon the listener to interprete the Escape key as a "close me" trigger
				SPos posMouse;
				GetParentWindow()->GetMousePos(posMouse);

				SEventGeneric event;
				event.eType = giEventKeyboard;
				event.bInsideControlRect = GetRect().Inside(posMouse);
				event.iMsg = KeyEscape;
				event.pos = ge::SPos(0,0);

				NotifyGeneric(&event);
			}
#endif	// WIN32
		}
	}
	// .. Lasse
}


SPos CPopupMenu::GetBestAbsPos(IControl* pControlThatInitiated)
{
	SPos posAbsoluteWanted = SPos(0,0);
	SSize sizeOfObstructiveObject = SSize(0,0);

	// Try not to hide the initiating control
	if (pControlThatInitiated) {
		pControlThatInitiated->GetAbsPos(posAbsoluteWanted);
		pControlThatInitiated->GetSize(sizeOfObstructiveObject);
	}

	return GetBestAbsPos(posAbsoluteWanted, sizeOfObstructiveObject);
} // CPopupMenu::GetBestAbsPos(SSize, IControl*)


enum EPositionPreferences {
	Below_AlignLeft,	Below_AlignRight,	Below_NoAlign,
	Above_AlignLeft,	Above_AlignRight,	Above_NoAlign,

	Right_AlignUpper,	Right_AlignLower,	Right_NoAlign,
	Left_AlignUpper,	Left_AlignLower,	Left_NoAlign,

	Overlap_NoAlign,
	
	NrOfPositionPreferences
};

EPositionPreferences gPositionPreferencesSidewaysFirst[NrOfPositionPreferences + 1] = 
{
	Right_AlignUpper,	Right_AlignLower,	Right_NoAlign,
	Left_AlignUpper,	Left_AlignLower,	Left_NoAlign,
	
	Below_AlignLeft,	Below_AlignRight,	Below_NoAlign,
	Above_AlignLeft,	Above_AlignRight,	Above_NoAlign,

	Overlap_NoAlign,
	
	NrOfPositionPreferences
};


SPos CPopupMenu::GetBestAbsPos(SPos posAbsoluteWanted, SSize sizeOfObstructiveObject /*= SSize(1,0)*/ )
{
	SPos posReturn = SPos(0,0);

	IPane* pPane = GetTopPane();

	SSize size;
	GetSize(size);

	// Get constraints (default for no pane: menu's own size)
	SSize sizeOfContainer = size;
	if (pPane) {
		pPane->GetSize(sizeOfContainer);
	}

	tint32 iYBelow = posAbsoluteWanted.iY + sizeOfObstructiveObject.iCY;
	tint32 iXToRight = posAbsoluteWanted.iX + sizeOfObstructiveObject.iCX;

	tbool bFitsBelow = (iYBelow + size.iCY) <= sizeOfContainer.iCY;
	tbool bFitsAbove = (posAbsoluteWanted.iY - size.iCY) >= 0;
	tbool bFitsRightOf = (iXToRight + size.iCX) <= sizeOfContainer.iCX;
	tbool bFitsLeftOf = (posAbsoluteWanted.iX - size.iCX) >= 0;

	mbHadToOverlay = false;

	tint32 iPos = 0;
	tbool bDone = false;
	for (; (!bDone)&&(iPos<NrOfPositionPreferences); iPos++) {
		EPositionPreferences ePos;
		if (mbPositionSidewaysFirst) {
			// Get preference order from "sideways" array
			ePos = gPositionPreferencesSidewaysFirst[iPos];
		}
		else {
			// No translation, use typedef'ed order
			ePos = (EPositionPreferences)iPos;
		}

		switch (ePos) {
			case Below_AlignLeft:
			{
				if (bFitsBelow) {
					tint32 iXRightEdge = posAbsoluteWanted.iX + size.iCX;
					if (iXRightEdge <= sizeOfContainer.iCX) {
						bDone = true;
						posReturn = SPos(posAbsoluteWanted.iX, iYBelow);
					}
				}
			}
			break;

			case Below_AlignRight:
			{
				if (bFitsBelow) {
					tint32 iXLeftEdge = iXToRight - size.iCX;
					if (iXLeftEdge >= 0) {
						bDone = true;
						posReturn = SPos(iXLeftEdge, iYBelow);
					}
				}
			}
			break;

			case Above_AlignLeft:
			{
				if (bFitsAbove) {
					tint32 iXRightEdge = posAbsoluteWanted.iX + size.iCX;
					if (iXRightEdge <= sizeOfContainer.iCX) {
						bDone = true;
						posReturn = SPos(posAbsoluteWanted.iX, posAbsoluteWanted.iY - size.iCY);
					}
				}
			}
			break;

			case Above_AlignRight:
			{
				if (bFitsAbove) {
					tint32 iXLeftEdge = iXToRight - size.iCX;
					if (iXLeftEdge >= 0) {
						bDone = true;
						posReturn = SPos(iXLeftEdge, posAbsoluteWanted.iY - size.iCY);
					}
				}
			}
			break;

			case Right_AlignUpper:
			{
				if (bFitsRightOf) {
					tint32 iYBottomEdge = posAbsoluteWanted.iY + size.iCY;
					if (iYBottomEdge <= sizeOfContainer.iCY) {
						bDone = true;
						posReturn = SPos(iXToRight, posAbsoluteWanted.iY);
					}
				}
			}
			break;
			
			case Right_AlignLower:
			{
				if (bFitsRightOf) {
					tint32 iYTopEdge = iYBelow - size.iCY;
					if (iYTopEdge >= 0) {
						bDone = true;
						posReturn = SPos(iXToRight, iYTopEdge);
					}
				}
			}
			break;

			case Left_AlignUpper:
			{
				if (bFitsLeftOf) {
					tint32 iYBottomEdge = posAbsoluteWanted.iY + size.iCY;
					if (iYBottomEdge <= sizeOfContainer.iCY) {
						bDone = true;
						posReturn = SPos(posAbsoluteWanted.iX - size.iCX, posAbsoluteWanted.iY);
					}
				}
			}
			break;
			
			case Left_AlignLower:
			{
				if (bFitsLeftOf) {
					tint32 iYTopEdge = iYBelow - size.iCY;
					if (iYTopEdge >= 0) {
						bDone = true;
						posReturn = SPos(posAbsoluteWanted.iX - size.iCX, iYTopEdge);
					}
				}
			}
			break;

			case Below_NoAlign:
			{
				if (bFitsBelow) {
					// Place as far right as possible
					tint32 iXLeftEdge = sizeOfContainer.iCX - size.iCX;
					if (iXLeftEdge < 0) {
						// Control is too big horizontally - place at X-pos 0
						iXLeftEdge = 0;
					}
					bDone = true;
					posReturn = SPos(iXLeftEdge, iYBelow);
				}
			}
			break;

			case Above_NoAlign:
			{
				if (bFitsAbove) {
					// Place as far right as possible
					tint32 iXLeftEdge = sizeOfContainer.iCX - size.iCX;
					if (iXLeftEdge < 0) {
						// Control is too big horizontally - place at X-pos 0
						iXLeftEdge = 0;
					}
					bDone = true;
					posReturn = SPos(iXLeftEdge, posAbsoluteWanted.iY - size.iCY);
				}
			}
			break;

			case Right_NoAlign:
			{
				if (bFitsRightOf) {
					// Place as low as possible
					tint32 iYTopEdge = sizeOfContainer.iCY - size.iCY;
					if (iYTopEdge < 0) {
						// Control is too big vertically - place at Y-pos 0
						iYTopEdge = 0;
					}
					bDone = true;
					posReturn = SPos(iXToRight, iYTopEdge);
				}
			}
			break;
			
			case Left_NoAlign:
			{
				if (bFitsLeftOf) {
					// Place as low as possible
					tint32 iYTopEdge = sizeOfContainer.iCY - size.iCY;
					if (iYTopEdge < 0) {
						// Control is too big vertically - place at Y-pos 0
						iYTopEdge = 0;
					}
					bDone = true;
					posReturn = SPos(posAbsoluteWanted.iX - size.iCX, iYTopEdge);
				}
			}
			break;

			case Overlap_NoAlign:
			{
				// Unfortunately we have to overlap the "forbidden" area

				// Place as far right as possible
				tint32 iXLeftEdge = sizeOfContainer.iCX - size.iCX;
				if (iXLeftEdge < 0) {
					// Control is too big horizontally - place at X-pos 0
					iXLeftEdge = 0;
				}

				// Place as low as possible
				tint32 iYTopEdge = sizeOfContainer.iCY - size.iCY;
				if (iYTopEdge < 0) {
					// Control is too big vertically - place at Y-pos 0
					iYTopEdge = 0;
				}

				mbHadToOverlay = true;

				bDone = true;
				posReturn = SPos(iXLeftEdge, iYTopEdge);
			}
			break;

			// Include last enum in switch too (to keep compiler happy)
			case NrOfPositionPreferences:
				// We should never come here
				bDone = true;
				break;
		}
	}

	return posReturn;
} // CPopupMenu::GetBestAbsPos


void CPopupMenu::SetVisible(tbool bVisible)
{
	if (!bVisible) {
		// Lasse - hide recursively (so pop-ups don't re-appear in open state)
		tint iItem;
		for (iItem = 0; iItem < mItems.iItemCount; iItem++) {
			if (mppPopupMenus[iItem]) {
				if (mppPopupMenus[iItem]->IsVisible()) {
					mppPopupMenus[iItem]->SetVisible(false);
				}
			}
		}
		mbSubMenuShowing = false;
		// .. Lasse
	}

	mbKeepSelectionUntilFirstMove = true;
	mbDisregardFirstMouseUp = true;

	CPane::SetVisible(bVisible);
} // CPopupMenu::SetVisible
