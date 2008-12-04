/*!	\file SPos.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class SPos
 * \brief Struct that contains a position (x, y)
*/
struct SPos
{
	//! Contructor (no parameter, undefined initial values)
	SPos() {}

	//! Constructor
	/*!
		\param iX [in]: Initial x position
		\param iY [in]: Initial y position
	*/
	SPos(tint iX, tint iY) : iX(iX), iY(iY) {}

	//! += operator
	SPos&	operator+=(const SPos& pos)			{ iX += pos.iX; iY += pos.iY; return *this; }
	//! + operator
	SPos	operator+ (const SPos& pos) const	{ return SPos(iX + pos.iX, iY + pos.iY); }
	//! -= operator
	SPos&	operator-=(const SPos& pos)			{ iX -= pos.iX; iY -= pos.iY; return *this; }
	//! - operator
	SPos	operator- (const SPos& pos) const	{ return SPos(iX - pos.iX, iY - pos.iY);}
	//! == operator
	tbool	operator==(const SPos& pos) const	{ return ((pos.iX == iX) && (pos.iY == iY)); }
	//! != operator
	tbool	operator!=(const SPos& pos) const	{ return !((pos.iX == iX) && (pos.iY == iY)); }

	//! x position
	tint iX;
	//! y position
	tint iY;
};
