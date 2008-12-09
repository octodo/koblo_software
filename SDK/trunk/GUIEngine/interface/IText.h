/*!	\file IText.h
	\author Michael Olsen
	\date 01/Jan/2005
	\date 01/Jan/2005
*/

/*! \class IText
 * \brief Text control, to show font
*/
class IText : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IText
	static IText* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pFont [in]: Font resource to use
	*/
	virtual void Init(tint32 iID, IFontResource* pFont) = 0;

	//! Changes the current font (can be used to change text colour by setting a new font with a different colour). Must not be called before Init.
	/*!
		\param pFont [in]: New font. Both this and the old font will get its reference count decreased automatically.
	*/
	virtual void ChangeFont(IFontResource* pFont) = 0;

	//! Sets the current text
	/*!
		\param pszText [in]: Text to set
	*/
	virtual void SetText(const tchar* pszText) = 0;

	//! Retrieves the current text
	/*!
		\param pszText [out]: Is filled with zero-terminated text. Must be preallocated with min. 65 characters
	*/
	virtual void GetText(tchar* pszText) = 0;

	//! Enum to describe horizontal alignment
	enum EHorizontalAlignment {
		//! Left alignment (default)
		HorzAlignLeft = 0,
		//! Center alignment
		HorzAlignCenter,
		//! Right alignment
		HorzAlignRight
	};

	//! Enum to describe vertical alignment
	enum EVerticalAlignment {
		//! Top alignment (default)
		VertAlignTop = 0,
		//! Center alignment
		VertAlignCenter,
		//! Bottom alignment
		VertAlignBottom
	};

	//! Sets the horizontal alignment. If not set it will default to left
	/*!
		\param Alignment [in]: The new alignment to use
	*/
	virtual void SetHorzAlignment(EHorizontalAlignment Alignment) = 0;

	//! Sets the vertical alignment. If not set it will default to top
	/*!
		\param Alignment [in]: The new alignment to use
	*/
	virtual void SetVertAlignment(EVerticalAlignment Alignment) = 0;

	//! Enables text size limiting mode. If the text doesn't fit inside the space given it will shorten it, by first removing vocals, and then cutting out the center of the text
	virtual void EnableTextLimiting(tbool bEnable = true) = 0;

	//! Speed optimization - refrains IText from calculating the size of text on Redraw (suitable for texts that are updated frequently).<br>Make sure the size is big enough for all possible string values!
	/*!
		\param sizeFixed [in]: The size that determines which Rect to invalidate on Redraw(). Set either dimension 0 to disable, other values enable
		\param bUseFixedPosToo [in]: If True the absolute position of the text is buffered as well (so a full Rect is saved, ready to use). Don't set True if the IText control will move around
	*/
	virtual void SpeedOptimize_RedrawUsingFixedSize(SSize sizeFixed, tbool bUseFixedPosToo = true) = 0;

	//! Speed optimization - makes IText calculate it's drawing rect only once, i.e. the first time it is set to a string that is not "" (suitable for fixed width texts that are updated frequently).<br>Make sure all possible string values are same size, or smaller than the first non-zero string you enter using SetText(..)!
	/*!
		\param bEnable [in]: If True the speed optimization is enabled, False disables
		\param bUseFixedPosToo [in]: If True the absolute position of the text is buffered as well (so a full Rect is saved, ready to use). Don't set True if the IText control will move around
	*/
	virtual void SpeedOptimize_RedrawUsingFirstNonZeroSize(tbool bEnable = true, tbool bUseFixedPosToo = true) = 0;

	//! Makes a text editable
	virtual void MakeEditable(tbool bEditable = true) = 0;

	//! Changes the event to init editing from single left button click to double left button click
	virtual void EditWhenDoubleClick() = 0;

	//! Tells the control to stop editing after double click
	virtual void StopEditWhenDoubleClick() = 0;

	//! Makes clicks outside control end text edit the same way as pressing return. Otherwise clicks outside control will end text edit the same way as pressing cancel.
	virtual void SetClicksOutsideAsSuccess() = 0;

	//! Make text appear as multiplication signs only (i.e. for "secret" text, '********')
	virtual void SetPassword() = 0;

	//! Makes control only accept numeric plus '-' '+' '.' ',' characters
	virtual void SetNumericOnly() = 0;


	//! Allows Up, Down, PgUp and PgDn keys to change the value of the control
	virtual void EnableKeyScrolling(tbool bEnable = true) = 0;

	//! Sets how many values Up, Down, PgUp and PgDn arrow keys will skip.<br>Note: These jumps affect the GUI value, so when enabling a data-converter for the control, you *must* increase these jumps or nothing will happen.
	/*!
		\param iUp [in]: How many values an Up arrow key should skip; 0 means disable
		\param iDown [in]: How many values a Down arrow key should skip; 0 means disable
		\param iPgUp [in]: How many values a PgUp key should skip; 0 means disable
		\param iPgDn [in]: How many values a PgDn key should skip; 0 means disable
	*/
	virtual void SetKeyScrolls(tint32 iUp = 1, tint32 iDown = 1, tint32 iPgUp = 10, tint32 iPgDn = 10) = 0;

	
	//! Allows mouse dragging to change the value of the control
	virtual void EnableMouseDragging(tbool bEnable = true) = 0;

	//! Sets in how big jumps mouse dragging will change the value of the control.<br>Note: These jumps affect the GUI value, so when enabling a data-converter for the control, you *must* increase these jumps or nothing will happen.
	/*!
		\param iUp [in]: How much the value should change when mouse is dragged up while fine-adjust key is active; 0 means disable
		\param iDown [in]: How much the value should change when mouse is dragged down while fine-adjust key is active; 0 means disable
		\param iBigUp [in]: How much the value should change when mouse is dragged up; 0 means disable
		\param iBigDn [in]: How much the value should change when mouse is dragged down; 0 means disable
	*/
	virtual void SetMouseDraggingJumps(tint32 iUp = 1, tint32 iDown = 1, tint32 iBigUp = 10, tint32 iBigDown = 10) = 0;

};

