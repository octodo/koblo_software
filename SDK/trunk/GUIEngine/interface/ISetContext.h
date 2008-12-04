/*!	\file ISetContext.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class ISetContext
 * \brief Interface to inherit from if the class is informed about the context it lives in
*/
class ISetContext
{
public:
	//! Sets context
	/*!
		\param pContext [in]: Context to set
	*/
	virtual void SetContext(IContext* pContext) = 0;
};