/*!	\file IFileSearch.h
	\author Michael Olsen
	\date 31/Mar/2005
	\date 31/Mar/2005
*/

/*! \class IFileSearch
 * \brief Interface for listing files in a directory
 *
 *	Note that an IFileSearch can only be used for 1 specific directory, i.e. if you need to search several directories, you'll need several file searches
*/
class IFileSearch : public virtual IDestructable
{
public:
	//! Creates IFileSearch
	static IFileSearch* Create();

	//! Initializes
	/*!
	 \param pszPathName [in]: Path where search will be performed. Windows: Standard path, including drive name (or network), ending with \, i.e. C:\MyDir\ or C:\ or \\Server\Share\MyDir\. OSX: Standard path, starting and ending with :, i.e. :MacIntoshHD:MyDir: or :MacIntoshHD:MyDir:MySubDir:
	 */
	virtual void Init(const tchar* pszPathName) = 0;
	
	//! Initializes - same functionality as Init, only returns false upon error rather than throwing an IException
	/*!
	 \param pszPathAndMask [in]: Path where search will be performed. Can be internal or OS format. Must end in a mask, either '*' or "*.<what-ever>"
	 \param pszErrMsgBuff [out]: Not NULL: Receives any error description (upon error)
	 \param iErrMsgBuffSize [in]: Not -1: Limits buffer size that receives any error description (upon error)
	 \return tbool: True: init success, false: some error (see message in error message buffer, if one was stated)
	*/
	virtual tbool Init2(const tchar* pszPathAndMask, tchar* pszErrMsgBuff = NULL, tint32 iErrMsgBuffSize = -1) = 0;
	
	//! Returns the next item in search
	/*!
		\param pszName [out]: Pointer to name string (without path). Must be preallocated with 256 bytes (i.e. name is 255 characters max)
		\param rDirectory [out]: If true this is a directory, if false it's a file
		\return tbool: Returns true if an item is returned, false otherwise (no more items)
	*/
	virtual tbool GetNext(tchar* pszName, tbool& rDirectory) = 0;
};




