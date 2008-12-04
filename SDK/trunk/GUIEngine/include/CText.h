/*!	\file CText.h
	\author Michael Olsen
	\date 01/Jan/2005
	\date 01/Jan/2005
*/

/*! \class CText
 * \brief Implementation of IText
*/
class CText : public virtual IText, public virtual CControl
{
public:
	//! Constructor
	CText();
	//! Destructor
	virtual ~CText();

	//! IDestructable override
	virtual void Destroy();

	//! IControl override
	virtual void OnDraw(const SRect &rUpdate);
	//! IControl override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! IControl override
	virtual tbool OnKeyDown(EKey Key);
	//! IControl override
	virtual void OnTimer();
	//! IControl override
	virtual void SetText(const tchar* pszText);
	//! IControl override
	virtual void GetText(tchar* pszText);
	//! IControl override
	virtual void SetValue(tint32 iNew, tbool bNotify = true);
	//! IControl override
	virtual void SetConvertedValue(tint32 iNew, tbool bNotify);

	//! IText override
	virtual void Init(tint iID, IFontResource* pFont);
	//! IText override
	virtual void ChangeFont(IFontResource* pFont);
	//! IText override
	virtual void SetHorzAlignment(EHorizontalAlignment Alignment);
	//! IText override
	virtual void SetVertAlignment(EVerticalAlignment Alignment);
	//! IText override
	virtual void EnableTextLimiting(tbool bEnable = true);
	//! IText override
	virtual void SpeedOptimize_RedrawUsingFixedSize(SSize sizeFixed, tbool bUseFixedPosToo = true);
	//! IText override
	virtual void SpeedOptimize_RedrawUsingFirstNonZeroSize(tbool bEnable = true, tbool bUseFixedPosToo = true);
	//! IText override
	virtual void MakeEditable(tbool bEditable = true);
	//! IText override
	virtual void EditWhenDoubleClick();
	//! IText override
	virtual void StopEditWhenDoubleClick();
	//! IText override
	virtual void SetClicksOutsideAsSuccess();
	//! IText override
	virtual void SetPassword();
	//! IText override
	virtual void SetNumericOnly();

	//! IText implementation
	virtual void EnableKeyScrolling(tbool bEnable = true);
	//! IText implementation
	virtual void SetKeyScrolls(tint32 iUp = 1, tint32 iDown = 1, tint32 iPgUp = 10, tint32 iPgDn = 10);

	//! IText implementation
	virtual void EnableMouseDragging(tbool bEnable = true);
	//! IText implementation
	virtual void SetMouseDraggingJumps(tint32 iUp = 1, tint32 iDown = 1, tint32 iBigUp = 10, tint32 iBigDown = 10);

protected:
	//! Current text
	std::string msTextCur;

	//! Current text while editing
	std::string msTextCurEditing;

	//! Internally used only!<br>When editing, this method can translate current editing text buffer to it's corresponding value.
	/*!
		\param riGUIValue [out]: Receives the translated value of the current editing text buffer. If translation fails it gets set to GetValue().
		\return tbool: True upon succesfull conversion.
	*/
	virtual tbool XlatCurEditingText(tint32& riGUIValue);

	//! Current horizontal alignment
	EHorizontalAlignment mAlignmentHorz;
	//! Current vertical alignment
	EVerticalAlignment mAlignmentVert;

	//! If true text limiting is on, otherwise it's off
	tbool mbLimiting;

	//! Explains if and how the OnDraw(..) event bases its invalidation Rect on a buffered value
	enum EBufferedSizeOptimization {
		//! The text size and position is recalculated on every OnDraw(..) event
		BUFFERED_SIZE_DISABLED,

		//! The text size and absolute position will be recalculated on the next OnDraw(..) event - after that the SSize will be buffered (not the position)
		BUFFERED_SIZE_CALC,
		//! The text size and absolute position will be recalculated on the next OnDraw(..) event - after that a full SRect will be buffered
		BUFFERED_SIZE_AND_POS_CALC,

		//! The text size is buffered and will never be recalculated in OnDraw(..) - but the position is always recalculated
		BUFFERED_SIZE_ENABLED,
		//! The text size and absolute position (making up a full SRect) is buffered and will never be recalculated in OnDraw(..)
		BUFFERED_SIZE_AND_POS_ENABLED
	};

	//! Explains how and if the OnDraw(..) event bases its invalidation SRect on a buffered value
	EBufferedSizeOptimization meBufferedSizeOptimization;

	//! The fixed Rect used in OnDraw(...) event if recalculation is suppressed (see EBufferedSizeOptimization)
	SRect mRectBuffered;

	//! If true, text is editable, otherwise it's not editable
	tbool mbEditable;

	//! If true, double click is used to identify edit start, otherwise single click is used
	tbool mbEditWhenDoubleClick;

	//! If true, editing will be stopped when control is double-clicked
	tbool mbStopEditWhenDoubleClick;

	//! If true, we're currently editing, otherwise not
	tbool mbEditing;

	//! Signal used when editing only - recalculate text SRect
	tbool mbEditing_RecalcRect;

	//! Signal used when editing only - recalculate cursor position
	tbool mbEditing_RecalcCursorPos;

	//! If true, text is camouflaged ('******').
	tbool mbPassword;

	//! See SetNumericOnly()
	tbool mbNumericOnly;

	//! If we're editing, this is the position of the cursor (index in text)
	tint miCursorPos;

	//! If we're editing, this is the position of the cursor (pixel offset)
	SPos mposCursor;

	//! Selection types for editing
	enum ESelectionType
	{
		//! No selection
		SelectionNone = 0,

		//! All selected
		SelectionAll
	};

	//! If we're editing, this is the current selection type
	ESelectionType mSelectionType;

	//! If true the text cursor is currently on, otherwise it's off (blinking)
	tbool mbCursorOn;

	//! When it reaches 0 the cursor should toggle (blinking)
	tint32 miCursorCounter;

	//! Time when we were in timer last
	tuint32 muiTimerLast;

	//! If true a click outside the control will end editing as if pressed return. Otherwise it will end as if pressed cancel.
	tbool mbClicksOutsideIsSuccess;

	//! Do text limiting
	/*!
		\param rs [in/out]: Text to limit. Will be changed if neccasary
	*/
	virtual void DoLimiting(std::string& rs);

	//! Called to start editing
	virtual void StartEditing();

	//! Called to end editing
	/*!
		\param bOK [in]: If true the user didn't cancel, and the text should be updated
	*/
	virtual void StopEditing(tbool bOK);

	//! Synchronized cursor blinking so it's ON for next ReDraw(..)
	virtual void SyncCursorOnNow();


	//! Returns the draw rect for a given text
	/*!
		\param rRect [out]: Drawing rect for text
		\param sText [in]: Text to get drawing rect for
	*/
	virtual void GetDrawRect(SRect& rRect, const std::string& sText);

	//! Returns the index into text based on x offset (pixels)
	/*!
		\param sText [in]: Text to get index into
		\param iOffset [in]: Offset in pixels
		\return tint: Character index
	*/
	virtual tint GetCharacterIndex(const std::string& sText, tint iOffset);


	//! Is key Scroll allowed? Default false
	tbool mbKeyScrollingEnabled;

	//! Number of values to skip on Up key (0 = disabled)
	tint32 miKeyScrollUp;
	//! Number of values to skip on Down key (0 = disabled)
	tint32 miKeyScrollDown;
	//! Number of values to skip on PgUp key (0 = disabled)
	tint32 miKeyScrollBigUp;
	//! Number of values to skip on PgDn key (0 = disabled)
	tint32 miKeyScrollBigDown;

	//! Is mouse dragging allowed? Default false
	tbool mbMouseDraggingEnabled;

	//! Number of values to skip on mouse dragging upwards while fine-adjust key is active (0 = disabled)
	tint32 miMouseDragUp;
	//! Number of values to skip on mouse dragging downwards while fine-adjust key is active (0 = disabled)
	tint32 miMouseDragDown;
	//! Number of values to skip on mouse dragging upwards (0 = disabled)
	tint32 miMouseDragBigUp;
	//! Number of values to skip on mouse dragging downwards (0 = disabled)
	tint32 miMouseDragBigDown;

	//! Drag state
	enum EDragState {
		//! Normal drag state
		MouseDragNormal = 0,
		//! Dragging drag state (i.e. mouse has been pressed down, and not released yet)
		MouseDragDragging
	};

	//! Current drag state
	EDragState mDragState;
	//! Mouse position when dragging was initiated
	SPos mPosDragStart;
	//! Value when dragging was initiated
	tint32 miValueDragStart;
	//! Current mouse position
	SPos mMousePos;
	//! Is mouse button pressed?
	tbool mbMouseDown;
	//! Is mouse cursor visible?
	tbool mbMouseVisible;
	//! Since we're continuously updating the mouse position, to avoid leaving the screen area, we need to compensate with the amount we've changed
	SSize mMouseMoved;
	//! We cannot update the mouse position everytime it is moved, because that somehow causes Windows to not update the graphics. Instead we use this counter to do on every 10 mouse move message
	tint miMouseMoveCounter;
	//! Whether mouse adjust was down on last mouse move (or mouse down).
	tbool mbFineAdjustLast;
	//! How fast the cursor blinks in ms (fyll cycle ON-OFF = 2 x the value)
	tint miCursorBlinkRateMs;

	virtual void StartMouseDragging(const SPos& rPos);
	virtual void StopMouseDragging();

	IFontResource* mpFontRes;


};
