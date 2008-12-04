/*!	\file IDisplayConverter.h
	\author Michael Olsen
	\date 07/Jan/2005
	\date 07/Jan/2005
*/


/*! \class IDisplayConverter
 * \brief Interface to describe a display converter
 *
 * A display converter can converter to and from a regular parameter format and a string representation
*/
class IDisplayConverter : public virtual IDestructable
{
public:
	//! Create a standard display converter.
	/*!
		\param iDecimals [in]: Number of decimals in value (i.e. iDecimals == 2 means that 540 should be interpreted as 5.40)
		\param iMaxNrOfStringChars [in]: Maximum number of chars in string when converting to string
		\param iNrOfStringDecimals [in]: Maximum number of decimals in string when converting to string
		\return IDisplayConverter: Display converter
	*/
	static IDisplayConverter* Create(tint iDecimals, tint iMaxNrOfStringChars, tint iNrOfStringDecimals);

	//! Convert a value to a string representation
	/*!
		\param iValue [in]: Value to convert (in parameter format)
		\param psz [out]: Converted string
	*/
	virtual void Value2String(tint32 iValue, tchar* psz) = 0;

	//! Convert a string representation to a value. Throws an TypeCode/ReasonCodeInvalidArgument exception in case of error
	/*!
		\param psz [in]: String to convert
		\return tint32: Converted value
	*/
	virtual tint32 String2Value(const tchar* psz) throw (IException*) = 0;

	//! Adds a postfix to add to converted string. Is not used for fixed strings (AddFixedString)
	/*!
		\param psz [in]: String to use as postfix
	*/
	virtual void SetPostFix(const tchar* psz) = 0;

	//! Add a fixed string for a certain value, i.e. a string which is not generated dynamically, but rather found in a list (typically strings like "Off", "-Inf." and such)
	/*!
		\param iValue [in]: Value to add string for
		\param psz [in]: String to add
	*/
	virtual void AddFixedString(tint32 iValue, const tchar* psz) = 0;
};