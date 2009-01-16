/*!	\file CFileSearchWin.h
	\author Michael Olsen
	\date 01/Apr/2005
	\date 01/Apr/2005
*/


/*! \class CFileSearchWin
 * \brief Windows implementation of IFileSearch
*/
class CFileSearchWin : public virtual IFileSearch
{
public:
	//! Constructor
	CFileSearchWin();
	//! Destructor
	virtual ~CFileSearchWin();

	//! IDestructable override
	virtual void Destroy();

	//! IFileSearch override
	virtual void Init(const tchar* pszPathName);
	//! IFileSearch override
	virtual tbool GetNext(tchar* pszName, tbool& rDirectory);

	//! IFileSearch override
	tbool Init2(const tchar* pszPathAndMask, tchar* pszErrMsgBuff = NULL, tint32 iErrMsgBuffSize = -1);

protected:
	//! Standard windows search handle
	HANDLE mhSearch;
	//! Standard windows search structure
	WIN32_FIND_DATA mFindFileData;

	//! Whether Init() or Init2() went well
	tbool mbInitSuccess;
};






