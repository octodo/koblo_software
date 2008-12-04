/*!	\file SScrollPos.h
	\author Michael Olsen
	\date 16/Mar/2005
	\date 16/Mar/2005
*/

/*! \class SScrollPos
 * \brief A scroll position contains information about the size of a scrolling area and how much of it is visible. All measures are in pixels. A SScrollPos is 2 dimensional (x / y), even though often only 1 dimension is used.
*/
struct SScrollPos
{
	//! Size of the complete area (always starts at position 0,0)
	SSize AreaSize;	
	//! The part which is current visible
	SRect VisibleRect;

	tbool operator==(const SScrollPos& sp2) const
	{ return ((sp2.AreaSize == AreaSize) && (sp2.VisibleRect == VisibleRect)); };
	
	tbool operator!=(const SScrollPos& sp2) const
	{ return ((sp2.AreaSize != AreaSize) || (sp2.VisibleRect != VisibleRect)); };

	//! Empty contructor. Sets all to zero
	SScrollPos() : AreaSize(0, 0), VisibleRect(SPos(0, 0), SSize(0, 0)) {}
};

