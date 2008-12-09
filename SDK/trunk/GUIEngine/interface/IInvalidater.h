/*!	\file IInvalidater.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IInvalidater
 * \brief Interface to invalidate rectangles.
 *
 * This interface sums up rectangles added to it. This is usefull for creating a update rectangle based on numerous small update rectangles
*/
class IInvalidater : public virtual IDestructable
{
public:
	//! Create IInvalidater
	static IInvalidater* Create();

	//! Reset, i.e. clear rect list
	virtual void Reset() = 0;

	//! Add rect to list
	/*!
		\param Rect [in]: Rect to add
	*/
	virtual void InvalidateRect(const SRect &Rect) = 0;

	// Is there currently any rect invalidated?
	/*!
		\return tbool: true if any rect is invalidated, false otherwise
	*/
	virtual tbool IsRectInvalidated() const = 0;

	// Get current invalidated rect, if any. If IsRectInvalidated() returns false, calling this will give undefined result
	/*!
		\param rRect [out]: Current invalidated rect if any, otherwise undefined.
	*/
	virtual void GetInvalidatedRect(SRect& rRect) /*const*/ = 0;
};