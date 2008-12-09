/*!	\file CScrollBar.h
	\author Michael Olsen
	\date 17/Mar/2005
	\date 17/Mar/2005
*/

/*! \class CScrollBar
 * \brief Implementation of IScrollBar
*/
class CScrollBar : public virtual IScrollBar, public virtual CPane, public virtual CEventReceiver
{
public:
	//! Constructor
	CScrollBar();
	//! Destructor
	virtual ~CScrollBar();

	//! IDestructable override
	virtual void Destroy();

	//! CControl override
	virtual void SetSize(const SSize& Size);

	//! CPane override
	virtual void OnDraw(const SRect &rUpdate);
	//! CPane override
	virtual tbool OnMouse(EMouseMsg MouseMsg, const SPos& Pos);
	//! CPane override
	virtual tbool OnKeyDown(EKey Key);
	//! CPane override
	virtual void SetSize(const SSize& Size, tbool bRedraw = false);
	//! CPane override
	virtual void AddControl(IControl* pToAdd, const SPos& Pos);

	//! IScrollBar override
	virtual void Init(tint32 iID, EType Type, IWindow* pWnd, IResourceManager* pResMan, tint32 piIDs[BitmapCount], IScrollPane* pScrollPane, const SScrollPos& ScrollPos);
	//! IScrollBar override
	virtual void SetScrollPos(const SScrollPos& ScrollPos, bool bRedraw);
	//! IScrollBar override
	virtual void GetScrollPos(SScrollPos& ScrollPos);
	//! IScrollBar override
	virtual void ExpandAreaSize();
	
	//! CEventReceiver override
	virtual void EventValueChange(IControl *pSender, tint32 iValueNew);

	//virtual void ScrollPaneSizeChanged(const SSize& SizeNew);

protected:
	//! Scroll pane we're connected to
	IScrollPane* mpScrollPane;

	//! Type of scrollbar, horizontal or vertical
	EType mType;

	//! Current scroll position
	SScrollPos mScrollPos;

	//! Bitmap resources for handle
	IBitmapResource* mppBitmapRes[BitmapCount];

	//! Sizes of bitmaps
	SSize mpBitmapSizes[BitmapCount];
	
	//! See ExpandAreaSize
	tbool mbExpandAreaSize;

	//! Calculates and returns the rect occupied by the handle
	/*!
		\return SRect: Rect occupied by the handle
	*/
	SRect GetHandleRect();

	//! Keeps track of whether we currently scrolling
	tbool mbScrolling;

	//! Mouse position when we started scrolling
	SPos mMousePosOrg;

	//! Scroll position when we started scrolling
	SScrollPos mScrollPosOrg;

	//! Scrollbar handle rect when we started scrolling
	SRect mScrollBarRectOrg;

	//! Positions the bitmaps according to current size
	void PositionControls();
};

