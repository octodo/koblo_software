/*!	\file I2D.h
	\author Michael Olsen
	\date 14/Dec/2005
	\date 14/Dec/2005
*/

/*! \class I2D
 * \brief A 2D control is a bitmap which can be moved inside a rectangle. The controls position and size determines the boundary of the rectangle inside which the control can be moved. The value of the control holds the x position in the upper 16 bit and the y position in the lower 16 bit.
*/
class I2D : public virtual IControl
{
public:
	//! Create I2D
	static I2D* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param pBitmap [in]: Bitmap resource to use
	*/
	virtual void Init(tint32 iID, IBitmapResource* pBitmap) = 0;

	//! Gets the number of possible x positions
	/*!
		\return tuint: Number of possible x positions
	*/
	virtual tuint GetXPositions() = 0;

	//! Gets the number of possible y positions
	/*!
		\return tuint: Number of possible y positions
	*/
	virtual tuint GetYPositions() = 0;
};
