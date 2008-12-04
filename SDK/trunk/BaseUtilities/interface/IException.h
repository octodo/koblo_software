/*!	\file IException.h
	\author Michael Olsen
	\date 06/Jan/2005
	\date 06/Jan/2005
*/


//! This macro is used to create an exception
#define EXCEPTION_INFO __FILE__, __LINE__, __DATE__


/*! \class IException
 * \brief Interface exceptions
 *
 * All exceptions thrown should be of the IException* type.
*/
class IException : public virtual IDestructable
{
public:
	//! Exception type
	enum EType {
		//! Unknown type
		TypeUnknown = 0,
		//! Falls into no other categories
		TypeGeneric,
		//! Code related exceptions
		TypeCode,
		//! Data storate limitation has been exceeded,
		TypeStorageLimitationExceeded,
		//! File related exception
		TypeFile,
		//! Network related exception
		TypeNetwork
	};

	//! Exception reason
	enum EReason {
		//! Unknown reason
		ReasonUnknown = 0,
		//! Falls into no other categories
		ReasonGeneric,
		//! Invalid code path
		ReasonCodePathInvalid,
		//! Invalid argument in function/method call
		ReasonCodeInvalidArgument,
		//! File could not be opened
		ReasonFileCannotOpen,
		//! File could not be read
		ReasonFileCannotRead,
		//! File could not be written
		ReasonFileCannotWrite,
		//! Wrong file type
		ReasonFileWrongType,
		//! Generic file error
		ReasonFileGeneric,
		//! Too large
		ReasonFileTooLarge,
		//! Generic network error
		ReasonNetworkGeneric,
		//! Network interface could not be opened
		ReasonNetworkCannotOpen,
		//! Network or server timeout
		ReasonNetworkTimeOut,
		//! Could'nt connect to server
		ReasonCouldntConnectToServer,
		//! Could'nt connect to application
		ReasonCouldntConnectToAppl,
		//! Answer not ok
		ReasonRequestFailed,
		//! Answer page was to large - ie. to many bytes in result (MAX_PAGE_SIZE)
		ReasonPageToLarge,
		//! Couldnt read data from application
		ReasonErrorReadingFromAppl,
		//! Unexpected path type
		ReasonWrongPathType
	};

	//! Creates IException
	/*!
		\parm Type [in]: Type of exception
		\parm Reason [in]: Reason for exception
		\parm pszDescription [in]: Description of error, or NULL if none
	*/
	static IException* Create(EType Type, EReason Reason, const tchar* pszFile,	tint iLine, const tchar* pszDate, const tchar* pszDescription = NULL);

	//! Returns type of exception
	/*!
		\return EType: Type of exception
	*/
	virtual EType GetType() = 0;

	//! \overload
	virtual const EType GetType() const = 0;

	//! Returns reason for exception
	/*!
		\return EReason: Reason for exception
	*/
	virtual EReason GetReason() = 0;

	//! \overload
	virtual const EReason GetReason() const = 0;

	//! Returns description of exception, or NULL if none exists
	/*!
		\return tchar*: Description of exception, or NULL if none exists
	*/
	virtual const tchar* GetDescription() const = 0;

	//! Returns full description of exception
	/*!
		\return const tchar*: Full Description of exception
	*/
	virtual const tchar* GetFullDescription() const = 0;
};




