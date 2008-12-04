/*!	\file SRect.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class SRect
 * \brief Struct that contains a rectangle, i.e. a position and a size (x, y, width, height)
*/
struct SRect : public virtual SPos, public virtual SSize
{
	//! Constructor (no parameter, initial value undefined)
	SRect() {}

	//! Constructor
	/*!
		\param iX [in]: Initial x position
		\param iY [in]: Initial y position
		\param iCX [in]: Initial width
		\param iCY [in]: Initial height
	*/
	SRect(tint iX, tint iY, tint iCX, tint iCY) : SPos(iX, iY), SSize(iCX, iCY) {}

	//! Constructor
	/*!
		\param Pos [in]: Initial position
		\param Size [in]: Initial Size
	*/
	SRect(const SPos& Pos, const SSize& Size) : SPos(Pos), SSize(Size) {}

	//! += operator
	SRect &operator+=( const SRect &rct ) { iX+=rct.iX; iY+=rct.iY; iCX+=rct.iCX; iCY+=rct.iCY; return *this; }

	//! += operator
	SRect&	operator+=(const SPos& pos)			{ iX += pos.iX; iY += pos.iY; return *this; }
	//! -= operator
	SRect&	operator-=(const SPos& pos)			{ iX -= pos.iX; iY -= pos.iY; return *this; }
	//! += operator
	SRect&	operator+=(const SSize &size)		{ iCX += size.iCX; iCY += size.iCY; return *this; }
	//! -= operator
	SRect&	operator-=(const SSize &size)		{ iCX -= size.iCX; iCY -= size.iCY; return *this; }
	//! == operator
	tbool	operator==(const SRect& rct) const	{ return SPos::operator==((SPos)rct) && SSize::operator==((SSize)rct); }
	//! != operator
	tbool	operator!=(const SRect& rct) const	{ return !(operator==(rct)); }

	//! Is position inside rect?
	/*!
		\param Pos [in]: Position to test for
		\return tbool: Returns true if position lies inside rectangle, otherwise false
	*/
	tbool Inside( const SPos &Pos ) const {if ((Pos.iX >= iX) && (Pos.iX <= iX + iCX - 1) && (Pos.iY >= iY) && (Pos.iY <= iY + iCY - 1)) return true; else return false;}

	//! Is rect inside rect?
	/*!
		\param rct [in]: Rectangle to test for
		\return a: Returns true if any part of rct lies inside rectangle, otherwise false
	*/
	tbool Inside( const SRect &rct ) const {
		//	Test X
		tint	start1=iX;
		tint	start2=rct.iX;
		tint	end1=start1+iCX-1;
		tint	end2=start2+rct.iCX-1;
		tint	t;
		if( start2<start1 ) { t=start2; start2=start1; start1=t; }
		if( end2<end1 ) { end1=end2; /*t=end2; end2=end1; end1=t;*/ }
		if( start2>end1 ) return false;

		//	Test Y
		start1=iY;
		start2=rct.iY;
		end1=start1+iCY-1;
		end2=start2+rct.iCY-1;
		if( start2<start1 ) { t=start2; start2=start1; start1=t; }
		if( end2<end1 ) { end1=end2; /*t=end2; end2=end1; end1=t;*/ }
		if( start2>end1 ) return false;

		return true;
	}

	//! Fit objects retangle inside other rectangle
	/*!
		\param rct [in]: Rectangle we should fit inside
	*/
	void FitInside( const SRect &rct )
	{
		if( iX<rct.iX ) {iCX-=rct.iX-iX;iX=rct.iX;}
		if( iY<rct.iY ) {iCY-=rct.iY-iY;iY=rct.iY;}
		if( iX+iCX>rct.iX+rct.iCX ) iCX=rct.iX+rct.iCX-iX;
		if( iY+iCY>rct.iY+rct.iCY ) iCY=rct.iY+rct.iCY-iY;
	}

	// Return true if rct completely covers this
	//! 
	/*!
		\param rct [in]: Rectangle to test for
		\return tbool: If rct completely covers this true, otherwise false
	*/
	tbool CoveredBy( const SRect &rct ) const {if ((rct.iX <= iX) && (rct.iY <= iY) && ((rct.iX+rct.iCX) >= (iX+iCX)) && ((rct.iY+rct.iCY) >= (iY+iCY))) return true; return false;}

#if WIN32
	//! Constructor
	/*!
		\param rct [in]: Windows rectangle
	*/
	SRect(const RECT &rct);

	//! typecast operator
	operator RECT() const;
#endif	// WIN32
};

