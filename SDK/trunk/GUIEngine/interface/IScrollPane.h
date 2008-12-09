/*!	\file IScrollPane.h
	\author Michael Olsen
	\date 16/Mar/2005
	\date 16/Mar/2005
*/

/*! \class IScrollPane.h
 * \brief IScrollPane is a class for panes with content that doesn't all show at one time
*/
class IScrollPane : public virtual IPane
{
public:
	//! Create IScrollPane
	static IScrollPane* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param iTimerID [in]: ID to use for Timer used in SlideTo(...) method. If you don't use SlideTo(...), just enter 0. Otherwise you need to make sure that timer IDs don't collide.
	*/
	virtual void Init(tint32 iID, tint32 iTimerID) = 0;

	//! Sets the current scrolling position
	/*!
		\param ScrollPos [in]: Current scrolling position
		\param bRedraw [in]: If true control will redraw, otherwise not
	*/
	virtual void SetScrollPos(const SScrollPos& ScrollPos, bool bRedraw = true) = 0;

	//! Gets the current scrolling position
	/*!
		\param ScrollPos [out]: Current scrolling position
	*/
	virtual void GetScrollPos(SScrollPos& ScrollPos) = 0;

	//! Move view to a different location in a fancy way
	/*!
		\param posTarget [in]: Top/left corner of wanted position (Note! Position is relative to total AreaSize)
		\param iMethod [in]: Reserved for future use, set to 0
		\param iPctJump [in]: How big a jump to do per timer tick (in %)
		\param iPctSlowest [in]: When slowing down, don't move slower than this (in %)
		\return tbool: True => sliding was initilized, False => some error occurred, probably timer ID = 0? (nothing done)
		*/
	virtual tbool SlideTo(const SPos& posTarget, tint32 iMethod, tint32 iPctJump, tint32 iPctSlowest) = 0;

	//! Enables or disables automatic resizing of sub-pane (AreaSize) when outer-pane (VisibleRect) size changes.<br>If this method is not called the sub-pane size (AreaSize) will only be updated if the outer-pane (VisibleRect) would otherwise become larger than it
	/*!
		\param bEnabled [in]: If true, resizing is enabled. If false it is disabled, and the other parameters are ignored
		\param Mode [in]: Resizing mode
		\param fValue [in]: If the resize mode is ResizeRelative, this is the ratio, which can be any positive number (except 0). 1.0 means that the ratio is 100%, 0.5 means 50%, etc.
	*/
	virtual void SetAutomaticResizeOfSubPane(tbool bEnabled = true, EResizeMode Mode = ResizeAbsolute, tfloat fValue = 1.0f) = 0;

	//! Prevent sub-pane (AreaSize) from getting smaller than this size
	/*!
		\param sizeMin [in]: The minimum size limit. Set one or both dimensions to 0 to ignore it and allow shrinking to 0
	*/
	virtual void SetMinSizeOfSubPane(SSize sizeMin) = 0;

	//! Get size of sub-pane (AreaSize)
	virtual void GetSizeOfSubPane(SSize& sizeSub) = 0;

	//! Link another scrollpane to this, so they move and resize together
	/*!
		\param pSlave [in]: The ScrollPane that shall do like this
		\param eMode [in]: How the slave ScrollPane shall mimick dimensions of this<br>Note: Only the absolute/direct resisizing modes are currently supported!
	*/
	virtual void AddSlaveScrollPane(IScrollPane* pSlave, EResizeMode eMode) = 0;

	//! Remove link to another scrollpane (see AddSlaveScrollPane(...) for explanation)
	/*!
		\param pSlave [in]: The slave to set free
		\return tbool: True => slave released; False => that wasn't a slave (nothing done)
	*/
	virtual tbool RemoveSlaveScrollPane(IScrollPane* pSlave) = 0;

	//! Suppresses functionality of CPane::UpdatePositions(..)<br/>Useful for speeding up multiple control additions to a pane (remember to enable functionality afterwards!)
	/*!
		\param bSuppress [in]: True to suppress (faster AddControl calls, but doesn't update), False to allow (will also seek through entire list of controls and update all of them)
	*/
	virtual void SuppressUpdatePositions(tbool bSuppress) = 0;
};
