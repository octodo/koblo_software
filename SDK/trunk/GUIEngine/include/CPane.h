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


/*! \class CPane
 * \brief Implementation of IPane
*/
class CPane : public virtual IPane, public virtual CControl
{
public:
	//! Constructor
	CPane();
	//! Destructor
	virtual ~CPane();

	//! IDestructable override
	virtual void Destroy();

	//! IPane override
	virtual tbool IsTopPane() const;
	//! IPane override
	virtual void SetParentWindow(IWindow* pWindow);
	//! IPane override
	virtual IWindow* GetParentWindow();
	//! IPane override
	virtual const IWindow* GetParentWindow() const;
	//! IPane override
	virtual void AddControl(IControl* pToAdd, const SPos& Pos);
	//! IPane override
	virtual void RemoveControl(IControl* pToRemove);
	//! IPane override
	virtual void Redraw(const SRect& rct);
	//! IPane override
	virtual void SetSize(const SSize& Size, tbool bRedraw = false);
	//! IPane override
	virtual IControl* GetControl(tint32 iControlID);
	//! IPane override
	virtual void DisableDrawingRectCheck();
	//! IPane override
	virtual void DisableControlCaching();
	//! IPane override
	virtual void SetBackgroundColour(SRGB Colour);

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();
	//! IControl override
	virtual void GetMousePos(SPos& rPos) const;
	//! IControl override
	virtual void SetControlValue(tint32 iControlID, tint32 iValueNew, tbool bNotify = false);

	//! Call this to cause all controls to send an EventValueChange()
	virtual void ControlUpdateAll();

	//! Tests whether an absolute position is "inside" one of the visible controls that is within the IPane.<br>Note: IPopupMenu's are deliberately ignored since they're a special case (should have full mouse control while showing to function properly).
	/*!
		\param rPosAbsolute [in]: The position to verify.
		\param bConsiderDisabledControls [in]; True = disabled controls will be tested too. False = disabled controls will be ignored (default).
		\return tbool: True if the position is within one of the controls that the pane "owns". False otherwise.
	*/
	virtual tbool IsAbsPosWithinAControl(const SPos& rPosAbsolute, tbool bConsiderDisabledControls = false);

	//! Updates automatic position in all subcontrols, starting from specific control.
	/*!
		\param pExcludeFrom [in]: Child control after which controls should be positioned, or NULL if all should be
	*/
	void UpdatePositions(const IControl* pExcludeFrom = NULL);

	//! Updates automatic sizes in all subcontrols, but not itself.
	/*!
		\param SizeDelta [in]: The absolute size difference
		\param fRatioX [in]: The relative size difference, in the x direction (2.0f means double size, etc.)
		\param fRatioY [in]: The relative size difference, in the y direction (2.0f means double size, etc.)
	*/
	void UpdateSizes(SSize SizeDelta, tfloat32 fRatioX, tfloat32 fRatioY);

	//! Called recursively to add controls to window after initialization
	virtual void AddControlsToWindow(CWindow* pWindow);

	//! Drawing primitive, which draws 1 frame from bitmap resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
		\param iFrame [in]: Frame to draw (0-based)
		\param iNrOfFrames [in]: Number of frames in resource
		\param bGhost [in]: If true, the last variant is drawn
	*/
	virtual void DrawBitmapFrame(IBitmapResource *pResource, const SRect &rctUpdate, const SRect &rct, tint iFrame, tint iNrOfFrames, tbool bGhost = false);

	//! Drawing primitive, which draws font resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
		\param psz [in]: Text to draw
	*/
	virtual void DrawFont(IFontResource *pResource, const SRect &prctUpdate, const SRect &rct, const tchar* psz);

	//! IPane implementation
	virtual void SuppressUpdatePositions(tbool bSuppress);

protected:
	//! Pointer to parent window
	IWindow* mpParentWindow;

	//! List of controls (and panes)
	std::list<IControl*> mControls;

	//! Map used to store control IDs. As we learn about them, we add them to the map. Note: This will not work if controls are deleted, so if we want to support that, we'll need to add some sort of notification mechanism
	std::map<tint32, IControl*> mControlMap;

	//! If true (default) we check the drawing rect against the size and position to determine if we need redrawing. If false the check is skipped.
	tbool mbDoDrawingRectCheck;

	//! If true (default) controls which are retrived from ID is cached
	tbool mbCacheControls;

	//! If true the pane has a background (solid colour), if false the pane has no background
	tbool mbBackgroundColour;

	//! If mbBackgroundColour == true, this is the colour to use
	SRGB mColour;

	//! If mbBackgroundColour == true, this is the drawing primitive object. Otherwise it is NULL
	IDrawPrimitives* mpDrawPrimitives;

	//! Used to check if draw primitives has been init'ed. The value is only valid if mbBackgroundColour == true.
	tbool mbDrawPrimitivesInited;

	//! Drawing primitive, which draws 1 frame from bitmap resource
	/*!
		\param pResource [in]: Resource to draw
		\param rctUpdate [in]: Current update rectangle
		\param rct [in]: Target rectangle
		\param iFrame [in]: Frame to draw (0-based)
		\param iNrOfFrames [in]: Number of frames in resource
		\param bGhost [in]: If true, the last variant is drawn
	*/
	void DoDrawBitmap(IBitmapResource* pResource, const SRect& rctUpdate, const SRect& rct, tint iFrame, tint iNrOfFrames, tbool bGhost);

	//! True: Method UpdatePositions will return immediately without doing anything, default = False
	tbool mbSuppressUpdatePositions;
};

