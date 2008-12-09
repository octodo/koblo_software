/*!	\file IScrollBar.h
	\author Michael Olsen
	\date 17/Mar/2005
	\date 17/Mar/2005
*/

/*! \class IScrollBar.h
 * \brief IScrollBar is a horizontal or vertical scrollbar, which communicates with an IScrollPane.
*/
class IScrollBar : public virtual IPane
{
public:
	//! Create IScrollBar
	static IScrollBar* Create();

	//! Enum for bitmap indices.
	enum EBitmapIndices {
		//! Left/Top arrow. Must have 2 frames.
		BitmapLeftTop = 0,
		//! Right/Down arrow. Must have 2 frames.
		BitmapRightDown,

		//! Left/Top part of handle
		BitmapLeftTopHandle,
		//! Right/Down part of handle
		BitmapRightDownHandle,
		//! Center part of handle
		BitmapCenterHandle,

		//! Nr of bitmaps
		BitmapCount
	};

	//! Enum for scrollbar type
	enum EType {
		TypeHorizontal = 0,
		TypeVertical
	};

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param Type [in]: Type of scrollbar, horizontal or vertical
		\param pWnd [in]: Window control resides in
		\param pResMan [in]: Resource manager
		\param piIDs [in]: Array of IDs of bitmap resources to use. See EBitmapIndices
		\param pScrollPane [in]: Scroll pane to communicate with.
		\param ScrollPos [in]: Initial scroll position
	*/
	virtual void Init(tint32 iID, EType Type, IWindow* pWnd, IResourceManager* pResMan, tint32 piIDs[BitmapCount], IScrollPane* pScrollPane, const SScrollPos& ScrollPos) = 0;

	//! Sets the scrolling position
	/*!
		\param ScrollPos [in]: Scroll position
		\param bRedraw [in]: If true will redraw, if false will not redraw
	*/
	virtual void SetScrollPos(const SScrollPos& ScrollPos, bool bRedraw = true) = 0;

	//! Gets the scrolling position
	/*!
		\param ScrollPos [out]: Scroll position
	*/
	virtual void GetScrollPos(SScrollPos& ScrollPos) = 0;

	//! If called, the scrollbar will expand the area size if it received a new scroll size from the pane which exceeds its area
	virtual void ExpandAreaSize() = 0;
	
};
