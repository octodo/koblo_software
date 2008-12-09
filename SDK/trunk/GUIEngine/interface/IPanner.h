/*!	\file IPanner.h
	\author Lasse Steen Bohnstedt
	\date 2007-09-12
*/

/*! \class IPanner
 * \brief An interface for a quad or stereo Panner
*/
class IPanner : public virtual IPane, public virtual IDestructable, public virtual IEventReceiver
{
public:
	//! Create IPanner
	static IPanner* Create();

	//! Initializer
	/*!
		\brief For a stereo panner you should leave one of the control IDs to ge::IControl::giNoID.<br>For quad panners both control IDs must be defined.
		\param iCtrlIDHorizontal [in]: Control ID for horizontal position of panner (x dimension)
		\param iCtrlIDVertical [in]: Control ID for vertical position of panner (y dimension)
		\param pBmpBack [in]: Bitmap resource to use for background of implicit IPane. Set to NULL for transparent background
		\param pBitmapHandle [in]: Bitmap resource to use for dot / handle
		\param posArea [in]: Offset of top-left corner of the area that the handle bitmap will move around inside 
		\param sizeArea [in]: The size of the area that the handle will move around inside. For stereo panners you must leave one of the dimensions to 0.
		\param bBitmapStaysInsideArea [in]: True means that the bitmap won't exceed the area size. E.g. if the area has a height of 100 and the bitmap in itself is 10 pixels high, then the total moving range of the bitmap will be 90.
	*/
	virtual void Init(tint32 iCtrlIDHorizontal, tint32 iCtrlIDVertical,
					  IBitmapResource* pBmpBack, IBitmapResource* pBmpHandle,
					  SPos posArea, SSize sizeArea, tbool bBitmapStaysInsideArea) = 0;

	//! Make handle move exactly as fast as mouse - if not called the handle is slower
	/*!
		\param bDirectXlat [in]: True => handle moves under mouse cursor, False => handle is slower than mouse
	*/
	virtual void SetDirectMouseXlat(tbool bDirectXlat = true) = 0;

	//! Move handle to mouse click pos (only valid if SetDirectMouseXlat(true) has been called)
	/*!
		\param bJumpToMouse [in]: True => if panner is clicked somewhere beside the handle, then the handle "jumps" there, False => nothing happens
	*/
	virtual void SetJumpToMouseClickPos(tbool bJumpToMouse = true) = 0;

};