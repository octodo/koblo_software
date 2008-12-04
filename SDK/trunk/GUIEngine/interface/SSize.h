/*!	\file SSize.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class SSize
 * \brief Struct that contains a size (width, height)
*/
struct SSize
{
	//! Constructor (no parameters, undefined start values)
	SSize() {}

	//! Contructor
	/*!
		\param iCX [in]: Initial width
		\param iCY [in]: Initial height
	*/
	SSize(tint iCX, tint iCY) : iCX(iCX), iCY(iCY) {}

	//! += operator
	SSize&	operator+=(const SSize &size)		{ iCX += size.iCX; iCY += size.iCY; return *this; }
	//! + operator
	SSize	operator+ (const SSize &size) const	{ return SSize(iCX + size.iCX, iCY + size.iCY); }
	//! -= operator
	SSize&	operator-=(const SSize &size)		{ iCX -= size.iCX; iCY -= size.iCY; return *this; }
	//! - operator
	SSize	operator- (const SSize &size) const	{ return SSize(iCX - size.iCX, iCY - size.iCY);}
	//! == operator
	tbool	operator==(const SSize &size) const	{ return ((size.iCX == iCX) && (size.iCY == iCY)); }
	//! != operator
	tbool	operator!=(const SSize &size) const	{ return !((size.iCX == iCX) && (size.iCY == iCY)); }

	//! Width
	tint iCX;
	//! Height
	tint iCY;
};
