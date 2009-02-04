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



/*! \class CControl
 * \brief Implementation of IControl
*/
class CControl : public virtual IControl
{
public:
	//! Constructor
	CControl();
	//! Destructor
	virtual ~CControl();

	//! IControl override
	virtual void SetParentPane(IPane* pPane);
	//! IControl override
	virtual IPane* GetParentPane();
	//! IControl override
	virtual const IPane* GetParentPane() const;
	//! IControl override
	virtual IPane* GetTopPane();
	//! IControl override
	virtual const IPane* GetTopPane() const;
	//! IControl override
	virtual IWindow* GetParentWindow();
	//! IControl override
	virtual const IWindow* GetParentWindow() const;
	//! IControl override
	virtual void Init(tint iID = IControl::giNoID, IBitmapResource* pBitmap = NULL);
	//! IControl override
	virtual tint32 GetID() const;
	//! IControl override
	virtual IResource* GetResource();
	//! IControl override
	virtual const IResource* GetResource() const;
	//! IControl override
	virtual IBitmapResource* GetBitmapResource();
	//! IControl override
	virtual const IBitmapResource* GetBitmapResource() const;
	//! IControl override
	virtual void GetSize(SSize& rSize) const;
	//! IControl override
	virtual tbool CloneRect(IControl* pOtherControl);
	//! IControl override
	virtual tbool CloneParentRect() { return CloneRect(mpParentPane); };
	//! IControl override
	virtual void SetVisible(tbool bVisible);
	//! IControl override
	virtual tbool IsVisible() const;
	//! IControl override
	virtual tbool IsVisibleSafe() const;
	//! IControl override
	virtual void SetAbsPos(const SPos& PosAbsolute);
	//! IControl override
	virtual void SetRelPos(const SPos& PosRelative);
	//! <b>Obsolete, don't use!</b><br>IControl override
	virtual void SetPos(const SPos& Pos);
	//! IControl override
	virtual void SetSize(const SSize& Size);
	//! IControl override
	virtual void GetAbsPos(SPos& rPosAbsolute) const;
	//! IControl override
	virtual void GetRelPos(SPos& rPosRelative) const;
	//! <b>Obsolete, don't use!</b><br>IControl override
	virtual void GetPos(SPos& rPos) const;
	//! IControl override
	virtual void GetRect(SRect& rRect) const;
	//! IControl override
	virtual void SetValue(tint32 iNew, tbool bNotify = true);
	//! IControl override
	virtual void SetConvertedValue(tint32 iNew, tbool bNotify = true);
	//! IControl override
	virtual tint32 GetValue() const;
	//! IControl override
	virtual tint32 GetConvertedValue() const;
	//! IControl override
	virtual void SetDefaultValueConverted(tint32 iValue);
	//! IControl override
	virtual void AddListener(IEventReceiver *pListener);
	//! IControl override
	virtual void NotifyValueChange();
	//! IControl override
	virtual void NotifyGeneric(void *pNotificationData);
	//! IControl override
	virtual void GetMousePos(SPos& rPos) const;
	//! IControl override
	virtual void OnTimer();
	//! IControl override
	virtual void DrawBitmap(IBitmapResource *pResource, const SRect &prctUpdate, const SRect &rct);
	//! IControl override
	virtual void DrawBitmapFrame(IBitmapResource *pResource, const SRect &rctUpdate, const SRect &rct, tint iFrame, tint iNrOfFrames, tbool bGhost = false);
	//! IControl override
	virtual void DrawFont(IFontResource *pResource, const SRect &prctUpdate, const SRect &rct, const tchar* psz);
	//! IControl override
	virtual void SetConverter(de::IConverter* pConverter);
	//! IControl override
	virtual de::IConverter* GetConverter();
	//! IControl override
	virtual const de::IConverter* GetConverter() const;
	//! IControl override
	virtual void SetDisplayConverter(de::IDisplayConverter* pConverter);
	//! IControl override
	virtual de::IDisplayConverter* GetDisplayConverter();
	//! IControl override
	virtual const de::IDisplayConverter* GetDisplayConverter() const;
	//! IControl override
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false);
	//! IControl override
	virtual void SetToolTipText(const tchar* pszText);
	//! IControl override
	virtual void SetGrid(tint iGridCount);
	//! IControl override
	virtual void Enable(tbool bEnable = true);
	//! IControl override
	virtual void CanGhost();
	//! IControl override
	virtual void StickTo(EPosition Position, SPos Offset = SPos(0, 0), EStickTo StickTo = StickToParent);
	//! IControl override
	virtual void SetMaxSize(SSize Size);
	//! IControl override
	virtual void SetAutomaticResize(tbool bEnabled = true, EResizeMode Mode = ResizeAbsolute, tfloat fValue = 1.0f);
	//! IControl override
	virtual tbool IsEnabled() const {return mbEnabled;}

	//! IInContext override
	virtual IContext* GetContext();
	//! IInContext override
	virtual const IContext* GetContext() const;

	//! Tell the control to reset to default value if possible. If no default value has been set, no action will occur. This means that any control can safely call this without checking for default value itself
	virtual void SetToDefaultValue();

	//! Returns the sticking position
	/*!
		\return EPosition: Position to stick to
	*/
	EPosition GetStickPosition() const {return mStickPosition;}

	//! Returns the sticking offset
	/*!
		\return SPos: Sticking offset
	*/
	SPos GetStickOffset() const {return mStickOffset;}

	//! Returns the stick-to value
	/*!
		\return EStickto: What to stick to
	*/
	EStickTo GetStickTo() const {return mStickTo;}

	//! Returns the max size
	/*!
		\return SSize: Max size
	*/
	SSize GetMaxSize() const {return mSizeMax;}

	//! Returns true if resizing is enabled
	/*!
		\return tbool: If true resizing is enabled, otherwise false
	*/
	tbool IsResizeEnabled() const {return mbResizeEnabled;}

	//! Returns resize mode
	/*!
		\param EResizeMode: The current resize mode
	*/
	EResizeMode GetResizeMode() const {return mResizeMode;}

	//! Returns resize ratio
	/*!
		\param tfloat: The current resize ratio
	*/
	tfloat GetResizeRatio() const {return mfResizeRatio;}

	//! IControl override
	virtual void SetMinSize(SSize sizeMin) { mSizeMin = sizeMin; SetSize(mSize); };

protected:
	//! Current value
	tint32 miValue;

	//! Pointer to parent pane
	IPane* mpParentPane;

	//! Control ID
	tint32 miID;

	//! Pointer to bitmap resource
	IBitmapResource* mpResource;

	//! Control position
	SPos mPos;

	//! Control size
	SSize mSize;

	//! Minimum control size - SetSize will never make control smaller than this value
	SSize mSizeMin;

	//! Is control currently visible?
	tbool mbVisible;

	//! List of event listeners
	std::list<IEventReceiver*> mListeners;

	//! Attached converter, or NULL
	de::IConverter* mpDataConverter;

	//! Attached display converter, or NULL
	de::IDisplayConverter* mpDisplayConverter;

	//! If the control has a default value set true, otherwise false
	tbool mbHasDefaultValue;

	//! Default value to use if mbHasDefaultValue == true. This is a raw control value
	tint32 miDefaultValue;

	//! Pointer to toop tip text, or NULL if no tool tip for this control
	tchar* mpszToolTipText;

	//! Time of last timer call (for tool tips)
	tuint32 muiToolTipLast;

	//! Position of mouse at last call (for tool tips)
	SPos mPosMouseToolTipLast;

	//! Keeps track of whether tool tip is currently shown
	tbool mbToolTipShown;

	//! If true grid (position locking) is activated, otherwise not
	tbool mbGrid;

	//! If grid is enabled (see mbGrid) the number of positions in grid, otherwise undefined
	tint miGridPositions;

	//! If true control is enabled (receives mouse + keyboard), otherwise disabled
	tbool mbEnabled;

	//! If true, the control has a row of ghost frames. See CanGhost.
	tbool mbCanGhost;

	//! Sticking position, or PosNone if no sticking
	EPosition mStickPosition;

	//! Sticking offset
	SPos mStickOffset;

	//! What to stick to, pane or sibling
	EStickTo mStickTo;

	//! Max size, or -1,-1 if no max size. -1 in just one of the directions disables max size for that direction only
	SSize mSizeMax;

	//! If true automatic resizing is enabled
	tbool mbResizeEnabled;

	//! Resize mode
	EResizeMode mResizeMode;

	//! Resize ratio
	tfloat mfResizeRatio;

	//! Gets control's absolute positions (relative to client window)
	/*!
		\return SPos: Control's absolute position
	*/
	virtual SPos GetAbsPos() const;

	//! Gets control's position relative to containing pane(s)
	/*!
		\return SPos: Control's relative position
	*/
	virtual SPos GetRelPos() const;

	//! <b>Obsolete, don't use!</b><br>Use CControl::GetAbsPos() - which works in the same way
	/*!
		\return SPos: Control's absolute position
	*/
	virtual SPos GetPos() const; // Obsolete - Use GetAbsPos() instead

	//! Override
	virtual SSize GetSize() const;

	//! Override
	virtual SRect GetRect() const;

	//! Redraws control, if visible
	virtual void Redraw();

	//! Notifies event listeners
	virtual void EditBegin();

	//! Notifies event listeners
	virtual void EditEnd();
};

