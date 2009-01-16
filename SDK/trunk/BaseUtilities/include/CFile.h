/*!	\file CFile.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/


/*! \class CFile
 * \brief Implementation of IFile
*/
class CFile : public virtual IFile, public virtual CFileCP
{
public:
	//! Constructor
	CFile();
	//! Destructor
	virtual ~CFile();

	//! IDestructable override
	virtual void Destroy();

	//! IFile override
	virtual tbool Open(const tchar* pszPathName, EOpenFile OpenFile);
	//! IFile override
	virtual void Close();
	//! IFile override
	virtual tuint64 Read(tchar* pch, tuint64 iSize);
	//! IFile override
	virtual tuint64 Write(const tchar* pch, tuint64 iSize);
	//! IFile override
	virtual tuint64 Seek(tuint64 iPos);
	//! IFile override
	virtual tuint64 GetSizeWhenOpened() const;
	//! IFile override
	virtual void GetPathName(tchar* pszPathName) const;
	//! IFile override
	virtual tuint64 GetCurrentFilePosition() const;

	//! Converts from OS specific path to internal path. Only works with full paths (not relative).
	/*!
		\param rsPathName [in/out]: Path to be converted. Returns converted path.
	*/
	static void PathFromOS(std::string& rsPathName);

	//! Converts from internal path to OS specific path. Only works with full paths (not relative).
	/*!
		\param rsPathName [in/out]: Path to be converted. Returns converted path.
	*/
	static void PathToOS(std::string& rsPathName);

	//! Converts an OS format path to internal format (':' separated)
	/*!
	 \param pszPathNameIn [in]: The path to convert. It can be relative or absolute path, may include filename or not, and it may already be in internal format (won't fail).
	 \param pszPathNameOut [out]: The converted path (you can enter the same pointer for in and out to provide in-place convertion, it won't crash).
	 \param bMakeAbsPath [in]: True: the converted path will be prepended the current working directory (but only if it is not already an absolute path).
	 \param pbIsAbsPath [out]: True: the converted path is absolute, false: the converted path is relative (doesn't start with '/').
	 \return tbool: True upon convertion success, false upon internal error. Will almost always return true, since fail-tolerance is high.
	 */
	static tbool PathFromOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath = true, tbool* pbIsAbsPath = NULL);
	
	//! Converts any internal format path to OS format (i.e. for Mac OS X => POSIX format, for Windows => DOS format)
	/*!
	 \param pszPathNameIn [in]: The path to convert. It can be relative or absolute path, may include filename or not, and it may already be in OS format (won't fail).
	 \param pszPathNameOut [out]: The converted path (you can enter the same pointer for in and out to provide in-place convertion, it won't crash).
	 \param bMakeAbsPath [in]: True: the converted path will be prepended the current working directory (but only if it is not already an absolute path).
	 \param pbIsAbsPath [out]: True: the converted path is absolute, false: the converted path is relative (doesn't start with '/').
	 \return tbool: True upon convertion success, false upon internal error. Will almost always return true, since fail-tolerance is high.
	 */
	static tbool PathToOS2(const tchar* pszPathNameIn, tchar* pszPathNameOut, tbool bMakeAbsPath = true, tbool* pbIsAbsPath = NULL);
	
	//! Split a path into an array (list) containing its elements.<br><b>Note!</b> An absolute path will have an empty string for first element.
	/*!
		\param rsPath [in]; The path to split.<br>If it doesn't contain any ':' chars, it will only result in one element.<br>If it is an empty string or NULL, it will result in no elements.
		\param rlistOfPathElements [out]: The list of path elements. If first element is "", it was an absolute path (started with ':'). If last element is "" the path contained no filename.<br><b>Note!</b> You must delete the elements of the list after use, or they will be orphaned on stack.
		\return tint32: Success: The number of elements. Failure: 0.
	*/
	static tint32 SplitPath(const std::string& rsPath, std::list<tchar*>& rlistOfPathElements);

	//! Make a path from an array (list) containing its elements.<br><b>Note!</b> To make an absolute path the first element must be an empty string.
	/*!
		\param rlistOfPathElements [in]: The list of path elements. If first element is "", it will turn into an absolute path (starting with ':')
		\param rsSplicedPath [out]; The resulting path.
		\return tint32: Success: The number of elements. Failure: 0.
	*/
	static tint32 SplicePath(const std::list<tchar*>& rlistOfPathElements, std::string& rsSplicedPath);

	//! Split a full path into a path-only and a filename-only part
	/*!
		\param pszFullPath [in]: The full path to split
		\param rsPathOnly [out]: The path-only part
		\param rsNameOnly [out]: The name-only part
		\param bAcceptEmptyPath [in]: True = won't fail even if the "full path" input consisted of only a name part
		\param bAcceptEmptyName [in]: True = won't fail even if there was no filename in full path (it pointed to a path instead of a file)
		\return tbool: True = Success, the two output strings were updated
	*/
	static tbool SplitPathToPathAndName(const tchar* pszFullPath, std::string& rsPathOnly, std::string& rsNameOnly, tbool bAcceptEmptyPath = true, tbool bAcceptEmptyName = true);


	//! Check if a path is absolute
	/*!
		\param pszPath [in]: Path to check
		\return tbool: True if path is absolute (starts with ':')
	*/
	static tbool IsAbsolutePath(const tchar* pszPath);

	//! Check if a path is relative 
	/*!
		\param pszPath [in]: Path to check
		\return tbool: True if path is relative (doesn't start with ':')
	*/
	static tbool IsRelativePath(const tchar* pszPath);


	//! Convert a path to a relative path by removing some of the first path elements.<br>If one parameter is absolute the other must be so too.
	/*!
		\param rsPath [in/out]: Path to modify
		\param pszPathToSubstract [in]: The first part of the path that should be removed
		\return tbool: True = Success, path was modified
	*/
	static tbool ToRelativePath(std::string& rsPath, const tchar* pszPathToSubstract);

	//! Convert a path to an absolute path (can be used with relative paths too)
	/*!
		\param rsPath [in/out]: Path to modify (must *not* be an absolute path!)
		\param pszPathToPrepend [in]: The path to add to the start of string
		\return tbool: True = Success, path was modified
	*/
	static tbool FromRelativePath(std::string& sPath, const tchar* pszPathToPrepend);

protected:
#ifdef	WIN32
	//! Current file handle. -1 if no file is open
	int miHandle;
#endif	// WIN32
#ifdef	_Mac
//	short miRefNum;
	FILE* mpHandle;
#endif	// _Mac

	//! Size when file was originally opened
	tuint64 miSizeWhenOpened;
	//! (Full) path name from when file was originally openend
	std::string msPathName;
	//! Current file position
	tuint64 miCurrentFilePos;

	//! Modifies a file name or path name using custom path type to something the OS specific calls except
	static void ModifyPathName(std::string& rsPathName);
	
	//! Converts an OS format path to internal format (':' separated)
	/*!
	 \param rsPathName [in/out]: The path to convert (in-place). It can be relative or absolute path, may include filename or not, and it may already be in OS format (won't fail).
	 \param bMakeAbsPath [in]: True: the converted path will be prepended the current working directory (but only if it is not already an absolute path).
	 \param pbIsAbsPath [out]: True: the converted path is absolute, false: the converted path is relative (doesn't start with '/').
	 \return tbool: True upon convertion success, false upon internal error. Will almost always return true, since fail-tolerance is high.
	 */
	static tbool PathFromOS2(std::string& rsPathName, tbool bMakeAbsPath = true, tbool* pbIsAbsPath = NULL);
	
	//! Converts any internal format path to OS format (i.e. for Mac OS X => POSIX format, for Windows => DOS format)
	/*!
	 \param rsPathName [in/out]: The path to convert (in-place). It can be relative or absolute path, may include filename or not, and it may already be in OS format (won't fail).
	 \param bMakeAbsPath [in]: True: the converted path will be prepended the current working directory (but only if it is not already an absolute path).
	 \param pbIsAbsPath [out]: True: the converted path is absolute, false: the converted path is relative (doesn't start with '/').
	 \return tbool: True upon convertion success, false upon internal error. Will almost always return true, since fail-tolerance is high.
	 */
	static tbool PathToOS2(std::string& rsPathName, tbool bMakeAbsPath = true, tbool* pbIsAbsPath = NULL);
	
};






