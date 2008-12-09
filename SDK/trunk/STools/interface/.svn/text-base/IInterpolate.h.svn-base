/*!	\file IInterpolate.h
	\author Max Grønlund
	\date 09/Feb/2005 
	\date 09/Feb/2005
*/

/*! \class CPart
	\brief class using interpolation to finde a value from look up table.
*/

class IInterpolate
{
public:
	static IInterpolate* CreateLinar();

	/*! Set the LOT and the size of the LOT, Notice the size of the LOT has to be 2 bigger than the size.
	\	param pfLOT [in]: Pointer to LOT used by object
	\	param iSize [in]: Size of the table
	*/
	virtual void SetLOT(const tfloat* pfLOT, const tint iSize)	= 0;

	//! Returns the interpolated value from the LOT
	/*!
		\return float: value at the fIndex
		param fIndex [in]: index
	*/
	virtual tfloat Get(const tfloat fIndex)				= 0;

	


};



