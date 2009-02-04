/*!	\file CControl.cpp
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


#include "geInternalOS.h"


const tint32 IControl::giNoID = -1;

CControl::CControl() : mpParentPane(NULL), miID(IControl::giNoID), mpResource(NULL),
	mPos(0, 0), mSize(0, 0), mbVisible(true), miValue(0), mpDataConverter(NULL), mpDisplayConverter(NULL), mbHasDefaultValue(false),
	mpszToolTipText(NULL), muiToolTipLast(0), mPosMouseToolTipLast(0, 0), mbToolTipShown(false),
	mbGrid(false), miGridPositions(-1),
	mbEnabled(true), mbCanGhost(false),
	mStickPosition(PosNone), mStickOffset(0, 0), mStickTo(StickToParent),
	mSizeMax(-1, -1)
{
	SetAutomaticResize(false);
	mSizeMin = SSize(0, 0);
}

CControl::~CControl()
{
	if (mpDataConverter) {
		mpDataConverter->Destroy();
	}

	if (mpDisplayConverter) {
		mpDisplayConverter->Destroy();
	}

	if (mpResource) {
		mpResource->DecRefCount();
	}
}

void CControl::Init(tint32 iID, IBitmapResource* pBitmap)
{
	miID = iID;
	mpResource = pBitmap;
}

void CControl::SetParentPane(IPane* pPane)
{
	mpParentPane = pPane;
}

IPane* CControl::GetParentPane()
{
	return mpParentPane;
}

const IPane* CControl::GetParentPane() const
{
	return mpParentPane;
}

IPane* CControl::GetTopPane()
{
	if (mpParentPane == NULL) {
		return dynamic_cast<IPane*>(this);
	}

	IPane* pPane = GetParentPane();
	while(pPane) {
		if (pPane->IsTopPane()) {
			return pPane;
		}
		else {
			pPane = pPane->GetParentPane();
		}
	}

	// Keep compiler happy
	return NULL;
}

const IPane* CControl::GetTopPane() const
{
	if (mpParentPane == NULL) {
		return dynamic_cast<const IPane*>(this);
	}

	const IPane* pPane = GetParentPane();
	while(pPane) {
		if (pPane->IsTopPane()) {
			return pPane;
		}
		else {
			pPane = pPane->GetParentPane();
		}
	}

	// Keep compiler happy
	return NULL;
}

IWindow* CControl::GetParentWindow()
{
	IPane* pTopPane = GetTopPane();
	return pTopPane->GetParentWindow();
}

const IWindow* CControl::GetParentWindow() const
{
	const IPane* pTopPane = GetTopPane();
	return pTopPane->GetParentWindow();
}

tint32 CControl::GetID() const
{
	return miID;
}

IResource* CControl::GetResource()
{
	return dynamic_cast<IResource*>(mpResource);
}

const IResource* CControl::GetResource() const
{
	return dynamic_cast<IResource*>(mpResource);
}

IBitmapResource* CControl::GetBitmapResource()
{
	return mpResource;
}

const IBitmapResource* CControl::GetBitmapResource() const
{
	return mpResource;
}

void CControl::SetAbsPos(const SPos& PosAbsolute)
{
	IPane* pPane = GetParentPane();
	if (pPane) {
		// Convert to relative position
		SPos PosPane;
		pPane->GetPos(PosPane);
		mPos = PosAbsolute - PosPane;
	}
	else {
		// Nothing to relate to - use absolute position as relative position
		SetRelPos(PosAbsolute);
	}
}

void CControl::SetRelPos(const SPos& PosRelative)
{
	mPos = PosRelative;
	
	IPane* pParent = GetParentPane();
	if (pParent) {
		// Update from parent pane
		dynamic_cast<CPane*>(pParent)->UpdatePositions(dynamic_cast<IControl*>(this));
	}

	CPane* pPane = dynamic_cast<CPane*>(this);
	if (pPane) {
		// We're a pane, so update positions in child controls
		pPane->UpdatePositions(NULL);
	}
}

void CControl::SetPos(const SPos& Pos) // Obsolete
{
	SetRelPos(Pos);
}

void CControl::GetAbsPos(SPos& rPosAbsolute) const
{
	rPosAbsolute = GetPos();
}

void CControl::GetRelPos(SPos& rPosRelative) const
{
	rPosRelative = mPos;
}

void CControl::GetPos(SPos& rPos) const // Obsolete
{
	GetAbsPos(rPos);
}

void CControl::SetSize(const SSize& Size)
{
	tbool bFirstResize = false;

	SSize sizeOld = mSize;
	// The size may be 0 if this is the first call
	if (sizeOld.iCX == 0) {
		sizeOld.iCX = 1;
		bFirstResize = true;
	}
	if (sizeOld.iCY == 0) {
		sizeOld.iCY = 1;
		bFirstResize = true;
	}

	mSize = Size;
	// Lasse, added 2008-05-08
	// Don't shrink smaller than minimum allowed size
	if (mSize.iCX < mSizeMin.iCX)
		mSize.iCX = mSizeMin.iCX;
	if (mSize.iCY < mSizeMin.iCY)
		mSize.iCY = mSizeMin.iCY;
	// .. Lasse

	if (mSize.iCX <= 0) {
		mSize.iCX = 1;
	}
	if (mSize.iCY <= 0) {
		mSize.iCY = 1;
	}
	if (mSizeMax.iCX != -1) {
		if (mSize.iCX > mSizeMax.iCX) {
			mSize.iCX = mSizeMax.iCX;
		}
	}
	if (mSizeMax.iCY != -1) {
		if (mSize.iCY > mSizeMax.iCY) {
			mSize.iCY = mSizeMax.iCY;
		}
	}

	if (mSize != sizeOld) {
		// Only update if there's a change in size.
		IPane* pParent = GetParentPane();
		if (pParent) {
			// Update from parent pane
#ifdef _Mac
			CPane* pPane2 = dynamic_cast<CPane*>(pParent);
			if (pPane2 == 0) {
				pPane2 = (CPane*)(void*)pParent;
			}
			pPane2->UpdatePositions(dynamic_cast<IControl*>(this));
#else
			dynamic_cast<CPane*>(pParent)->UpdatePositions(dynamic_cast<IControl*>(this));
#endif
		}

		CPane* pPane = dynamic_cast<CPane*>(this);
		if (pPane) {
			// We're a pane, so update positions and sizes in child controls
			pPane->UpdatePositions(NULL);

			if (bFirstResize == false) {
				SSize SizeDelta(mSize - sizeOld);
				pPane->UpdateSizes(SizeDelta, (tfloat32)sizeOld.iCX / mSize.iCX, (tfloat32)sizeOld.iCY / mSize.iCY);
			}
		}
	}
}

void CControl::GetSize(SSize& rSize) const
{
	rSize = mSize;
}

void CControl::GetRect(SRect& rRect) const
{
	rRect = GetRect();
}

SPos CControl::GetAbsPos() const
{
	const IPane* pPane = GetParentPane();
	if (pPane) {
		return dynamic_cast<const CPane*>(pPane)->GetPos() + mPos;
	}
	else {
		return mPos;
	}
}

SPos CControl::GetRelPos() const
{
	return mPos;
}

SPos CControl::GetPos() const // Obsolete
{
	return GetAbsPos();
}

SSize CControl::GetSize() const
{
	return mSize;
}

SRect CControl::GetRect() const
{
	return SRect(GetPos(), mSize);
}

tbool CControl::CloneRect(IControl* pOtherControl)
{
	if (pOtherControl) {
		CControl* pOther2 = dynamic_cast<CControl*>(pOtherControl);
		if (pOther2) {
			SetAbsPos(pOther2->GetAbsPos());
			SetSize(pOther2->GetSize());
		}
	}

	return false;
}

	
void CControl::SetVisible(tbool bVisible)
{
	mbVisible = bVisible;

//	Redraw();
	// We redraw this way, because Redraw() has a check for visibility
	//!! TO DO
	SPos pos = GetPos();
	if(pos.iX < 0){
		pos.iX = 0; //!!! quick fix
	}
	SRect rct(pos, GetSize() + SSize(1, 1));
	IPane* pParentPane = GetParentPane();
	if (pParentPane) {
		pParentPane->Redraw(rct);
	}
}

tbool CControl::IsVisible() const
{
	return mbVisible;
}

tbool CControl::IsVisibleSafe() const
{
	if (IsVisible() == false) {
		return false;
	}

	const IPane* pParentPane = GetParentPane();
	if (pParentPane) {
		return pParentPane->IsVisibleSafe();
	}

	// Top pane is always considered visible
	return true;
}

void CControl::Redraw()
{
//	if (IsVisible()) {
	if (IsVisibleSafe()) {
		SRect rct(GetPos(), GetSize() + SSize(1, 1));
		GetParentPane()->Redraw(rct);
	}
}

void CControl::SetValue(tint32 iNew, tbool bNotify)
{
	if (iNew > gControlMax) {
		iNew = gControlMax;
	}
	if (iNew < gControlMin) {
		iNew = gControlMin;
	}

	if (mbGrid) {
		tint64 iPositions = (tint64)gControlMax - (tint64)gControlMin;
		tfloat64 fIndex = (iNew - gControlMin) / (tfloat64)iPositions;
		tint iGridIndex = Float2Int(fIndex * (miGridPositions - 1));
		fIndex = iGridIndex / (tfloat64)(miGridPositions - 1);
		iNew = Float2Int(fIndex * iPositions) + gControlMin;
	}

	miValue = iNew;

	if (bNotify) {
		NotifyValueChange();
	}

	Redraw();
}

void CControl::SetConvertedValue(tint32 iNew, tbool bNotify)
{
	if (mpDataConverter) {
		// Converter is attached, so convert value
		iNew = mpDataConverter->Parm2GUI(iNew);
	}

	SetValue(iNew, bNotify);
}

tint32 CControl::GetValue() const
{
	return miValue;
}

tint32 CControl::GetConvertedValue() const
{
	if (mpDataConverter) {
		return mpDataConverter->GUI2Parm(GetValue());
	}
	else {
		return GetValue();
	}
}

void CControl::SetDefaultValueConverted(tint32 iValue)
{
	if (mpDataConverter) {
		// Converter is attached, so convert value
		iValue = mpDataConverter->Parm2GUI(iValue);
	}
	miDefaultValue = iValue;

	mbHasDefaultValue = true;
}

void CControl::AddListener(IEventReceiver *pListener)
{
	mListeners.insert(mListeners.begin(), pListener);
}

void CControl::NotifyValueChange()
{
	std::list<IEventReceiver*>::iterator it = mListeners.begin();
	for (; it != mListeners.end(); it++) {
		IEventReceiver* pListener = *it;
		pListener->EventValueChange(dynamic_cast<IControl*>(this), GetConvertedValue());
	}
}

void CControl::NotifyGeneric(void *pNotificationData)
{
	std::list<IEventReceiver*>::iterator it = mListeners.begin();
	for (; it != mListeners.end(); it++) {
		IEventReceiver* pListener = *it;
		pListener->EventGeneric(dynamic_cast<IControl*>(this), pNotificationData);
	}
}

ge::IContext* CControl::GetContext()
{
	return GetParentWindow()->GetContext();
}

const ge::IContext* CControl::GetContext() const
{
	return GetParentWindow()->GetContext();
}

void CControl::GetMousePos(SPos& rPos) const
{
	GetParentPane()->GetMousePos(rPos);
}

void CControl::SetConverter(de::IConverter* pConverter)
{
	if (mpDataConverter) {
		mpDataConverter->Destroy();
	}

	mpDataConverter = pConverter;
}

de::IConverter* CControl::GetConverter()
{
	return mpDataConverter;
}

const de::IConverter* CControl::GetConverter() const
{
	return mpDataConverter;
}

void CControl::SetDisplayConverter(de::IDisplayConverter* pConverter)
{
	if (mpDisplayConverter) {
		mpDisplayConverter->Destroy();
	}

	mpDisplayConverter = pConverter;
}

de::IDisplayConverter* CControl::GetDisplayConverter()
{
	return mpDisplayConverter;
}

const de::IDisplayConverter* CControl::GetDisplayConverter() const
{
	return mpDisplayConverter;
}

void CControl::SetControlValue(tint32 iControlID, tint32 iValueNew, bool bNotify)
{
	if (iControlID == GetID()) {
		SetConvertedValue(iValueNew, bNotify);
	}
}

void CControl::SetToDefaultValue()
{
	if (mbHasDefaultValue) {
		SetValue(miDefaultValue, true);
	}
}

void CControl::SetToolTipText(const tchar* pszText)
{
	mpszToolTipText = (tchar*)pszText;
}

void CControl::SetGrid(tint iGridCount)
{
	miGridPositions = iGridCount;

	mbGrid = true;
}

void CControl::Enable(tbool bEnable)
{
	mbEnabled = bEnable;

	if (mbCanGhost) {
		// Since we may be switching row, we need to redraw
		Redraw();
	}
}

void CControl::CanGhost()
{
	mbCanGhost = true;
}

void CControl::StickTo(EPosition Position, SPos Offset, EStickTo StickTo)
{
	mStickPosition = Position;
	mStickOffset = Offset;
	mStickTo = StickTo;
}

void CControl::SetMaxSize(SSize Size)
{
	mSizeMax = Size;

	// Force update, in case we're too large already
	SetSize(GetSize());
}

void CControl::SetAutomaticResize(tbool bEnabled, EResizeMode Mode, tfloat fValue)
{
	mbResizeEnabled = bEnabled;
	mResizeMode = Mode;
	mfResizeRatio = fValue;
}

void CControl::OnTimer()
{
	if( !IsVisible() )
		return;

	if (mpszToolTipText) {
		tuint32 uiTimeNow = ITime::GetTimeMS();
		SPos PosNow;
		GetMousePos(PosNow);

		if (PosNow == mPosMouseToolTipLast) {

		//	ASSERT(uiTimeNow > muiToolTipLast);
			// Same mouse position, check the timer
			if (uiTimeNow > muiToolTipLast + 1000) {
				// Timer triggered, i.e. show tool tip
				if (GetRect().Inside(PosNow)) {
					if (mbToolTipShown == false) {
						dynamic_cast<CWindow*>(GetParentWindow())->ShowToolTip(PosNow, std::string((char*)mpszToolTipText));
						mbToolTipShown = true;

						muiToolTipLast = uiTimeNow;
					}
				}
			}
		}
		else {
			// Not the same mouse position
			mPosMouseToolTipLast = PosNow;
			muiToolTipLast = uiTimeNow;

			if (mbToolTipShown) {
				dynamic_cast<CWindow*>(GetParentWindow())->HideToolTip();
				mbToolTipShown = false;
			}
		}
	}
}

void CControl::EditBegin()
{
	std::list<IEventReceiver*>::iterator it = mListeners.begin();
	for (; it != mListeners.end(); it++) {
		IEventReceiver* pListener = *it;
		pListener->EventEditBegin();
	}
}

void CControl::EditEnd()
{
	std::list<IEventReceiver*>::iterator it = mListeners.begin();
	for (; it != mListeners.end(); it++) {
		IEventReceiver* pListener = *it;
		pListener->EventEditEnd();
	}
}

// --------------------------------------------------
	// Drawing primitives
// --------------------------------------------------

void CControl::DrawBitmap(IBitmapResource *pResource, const SRect &prctUpdate, const SRect &rct )
{
	DrawBitmapFrame(pResource, prctUpdate,rct, 0, 1);
}

void CControl::DrawBitmapFrame(IBitmapResource *pResource, const SRect &rctUpdate, const SRect &rct, tint iFrame, tint iNrOfFrames, tbool bGhost)
{
	GetParentPane()->DrawBitmapFrame(pResource, rctUpdate, rct, iFrame, iNrOfFrames, bGhost);
}

void CControl::DrawFont(IFontResource *pResource, const SRect &prctUpdate, const SRect &rct, const tchar* psz)
{
	GetParentPane()->DrawFont(pResource, prctUpdate, rct, psz);
}


