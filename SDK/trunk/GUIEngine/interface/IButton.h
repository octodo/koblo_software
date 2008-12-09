/*!	\file IButton.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 19/Jan/2005
*/

/*! \class IButton
 * \brief Button with notification when pressed.
 *
 * Note, this is a simple push button, which will notify when pressed. It is not a "check-box-button", i.e. it cannot stay in a second state.
*/
class IButton : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IButton
	static IButton* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use, or NULL if button is transparent (in which case the size must be set)
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap = NULL) = 0;

	//! Add a text to button
	/*!
		\param pszText [in]: Text to write
		\param pFont [in]: Font to use. Will be destroyed with the control
		\param Size [in]: Size of text
		\param Pos [in]: Position of text, or NULL to place it as 0,0
		\param HorzAlign [in]: Horizontal alignment of text
	*/
	virtual void AddText(const tchar* pszText, IFontResource* pFont, const SSize& Size, const SPos* pPos = NULL, IText::EHorizontalAlignment HorzAlign = IText::HorzAlignCenter) = 0;

	//! Changes a previously added text
	/*!
		\param pszText [in]: Text to write
	*/
	virtual void ChangeText(const tchar* pszText) = 0;

	//! Call to make events on mouse up (instead of mouse down)
	virtual void SetEventMouseUp() = 0;

	//! Call to send 2 events, 1 with value 1 when pressed, 1 with value 0 when released
	virtual void SetEventMouseUpDown() = 0;

	//! Call this to make the button light, or to turn off the light
	/*!
		\param bLight [in]: If true the button will light even if not pressed, if false it returns to default behaviour (i.e. will light only when pressed)
	*/
	virtual void SetLight(tbool bLight) = 0;

	//! Changes the cursor to a pointing hand when the mouse is over the control
	virtual void ActivateHandCursor() = 0;

	//! When enabled, the number of frames in the bitmap is 4, with the odd-numbered frames (1-indexed) being the "regular" frames, and the even-numbered being "mouse-down" frames, shown while the mouse is down.
	virtual void EnableMouseStateFrames() = 0;
};