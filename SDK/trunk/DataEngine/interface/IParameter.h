/*!	\file IParameter.h
	\author Michael Olsen
	\date 06/Jan/2005
	\date 06/Jan/2005
*/



/*! \class IParameter
 * \brief Interface to describe one parameter
 *
 * The user must override this interface, and supply implementation
*/
class IParameter : public virtual IDestructable
{
public:
	//! Returns ID
	/*!
		\return tint32: Parameters ID
	*/
	virtual tint32 GetID() const = 0;

	//! Returns the default value
	/*!
		\return tint32: Parameters default value
	*/
	virtual tint32 GetDefaultValue() const = 0;

	//! Returns current value
	/*!
		\return tint32: Parameters current value
	*/
	virtual tint32 GetValue() const = 0;

	//! Set value
	/*!
		\parm iValueNew: New value for parameter
		\parm bCallOnChange: If true OnChange() is called, otherwise not
	*/
	virtual void SetValue(tint32 iValueNew, tbool bCallOnChange = true) = 0;

	//! Returns minimum value possible for parameter
	/*!
		\return tint32: Parameters minimum possible value
	*/
	virtual tint32 GetMinValue() const = 0;

	//! Returns minimum value possible for parameter
	/*!
		\return tint32: Parameters maximum possible value
	*/
	virtual tint32 GetMaxValue() const = 0;

	//! Called to notify a change has occured.
	virtual void OnChange() = 0;
};



