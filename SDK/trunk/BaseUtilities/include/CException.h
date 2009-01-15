/*!	\file CException.h
	\author Michael Olsen
	\date 06/Jan/2005
	\date 06/Jan/2005
*/


/*! \class CException
 * \brief Implementation of IException
*/
class CException : public virtual IException
{
public:
	//! Constructor
	CException(EType Type, EReason Reason, const tchar* pszFile, tint32 iLine, const tchar* pszDate, const tchar* pszDescription = NULL);
	//! Destructor
	virtual ~CException();

	//! IDestructable override
	virtual void Destroy();

	//! IException override
	virtual EType GetType();
	//! IException override
	virtual const EType GetType() const;
	//! IException override
	virtual EReason GetReason();
	//! IException override
	virtual const EReason GetReason() const;
	//! IException override
	virtual const tchar* GetDescription() const;
	//! IException override
	virtual const tchar* GetFullDescription() const;

protected:
	EType mType;
	EReason mReason;
	std::string msDescription;
	std::string msFile;
	std::string msLine;
	std::string msDate;
	std::string msFullDescription;

	//! Builds full description string
	virtual void MakeFullDescription();

	//! Appends type text to string
	/*!
		\parm rsString [in/out]: String to be appended
	*/
	virtual void GetTypeText(std::string& rsString);

	//! Appends reason text to string
	/*!
		\parm rsString [in/out]: String to be appended
	*/
	virtual void GetReasonText(std::string& rsString);
};






