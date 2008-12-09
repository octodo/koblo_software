/*!	\file IContextCallback.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

/*! \class IContextCallback
 * \brief Callback interface used by context. This needs to be implemented by the user of the GUI library
*/
class IContextCallback
{
public:
	//! Returns the name of application / plug-in
	/*!
		\param psz [out]: Points to 64 characters + NULL termination, i.e. min. 65 characters
	*/
	virtual void GetName(tchar* psz) = 0;

	//! Keyboard key down event
	/*!
		\param iKey [in]: Key that was pressed
		\return tbool: True if handled, false otherwise
	*/
	virtual tbool OnKeyDown(EKey Key) = 0;
};



