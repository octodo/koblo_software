// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \class IFile
 * \brief Interface for accessing files
 *
 * Note that a file cannot be opened for simultaneous reading and writing
*/
class IFile : public virtual IDestructable
{
public:
	//! Creates IFile
	static IFile* Create();

	//! Enum for defining file access (read / write / create)
	enum EOpenFile {
		//! Open file for reading only
		FileRead = 0,
		//! Open file for writing only. File must already exist.
		FileWrite,
		//! Open file for writing only. File may or may not already exist. If already existing it will be deleted.
		FileCreate
	};

	//! Open file, given filename (full path)
	/*!
		\param pszPathName [in]: File to open (full path name).
		\param OpenFile [in]: File access to open with
		\return bool: true if success, false otherwise
	*/
	virtual tbool Open(const tchar* pszPathName, EOpenFile OpenFile) = 0;

	//! Close is automatically called when opening a new file, or when destroying object. However you can call it manually if desired
	virtual void Close() = 0;

	//! Read from file
	/*!
		\param pch [out]: Buffer to be filled
		\param iSize [in]: Number of bytes to read
		\return tuint64: Number of bytes actually read
	*/
	virtual tuint64 Read(tchar* pch, tuint64 iSize) = 0;

	//! Write to file
	/*!
		\param pch [in]: Buffer to write
		\param iSize [in]: Number of bytes to write
		\return tuint64: Number of bytes actually written
	*/
	virtual tuint64 Write(const tchar* pch, tuint64 iSize) = 0;

	//! Seek to new position (from start of file). After opening file the position is always 0.
	/*!
		\param iPos [in]: Position to seek to (from start of file)
		\return tuint64: New position
	*/
	virtual tuint64 Seek(tuint64 iPos) = 0;

	//! Returns the size of file when it was initially opened
	/*!
		\return tuint64: Size of file when it was initially opened
	*/
	virtual tuint64 GetSizeWhenOpened() const = 0;

	//! Returns current file position
	/*!
		\return tuint64: Current file position
	*/
	virtual tuint64 GetCurrentFilePosition() const = 0;

	//! Gets (full) path name used when opening file
	/*!
		\param pszPathName [out]: Pointer to buffer of min. 513 characters to be filled with path name
	*/
	virtual void GetPathName(tchar* pszPathName) const = 0;

	//! Reads tint32's with automatic crossplatform swapping
	/*!
		\param p [in]: Buffer to read into
		\param iSize [in]: Number of tint32's to read
		\return tuint64: Number of tint32's actually read
	*/
	virtual tuint64 ReadCP(tint32* p, tuint64 iSize) = 0;

	//! Writes tint32's with automatic crossplatform swapping
	/*!
		\param p [in]: Buffer to write
		\param iSize [in]: Number of tint32's to write
		\return tuint64: Number of tint32's actually write
	*/
	virtual tuint64 WriteCP(tint32* p, tuint64 iSize) = 0;

	//! Reads tfloat32's with automatic crossplatform swapping
	/*!
		\param p [in]: Buffer to read into
		\param iSize [in]: Number of tfloat32's to read
		\return tuint64: Number of tfloat32's actually read
	*/
	virtual tuint64 ReadCP(tfloat32* p, tuint64 iSize) = 0;

	//! Writes tfloat32's with automatic crossplatform swapping
	/*!
		\param p [in]: Buffer to write
		\param iSize [in]: Number of tfloat32's to write
		\return tuint64: Number of tfloat32's actually write
	*/
	virtual tuint64 WriteCP(tfloat32* p, tuint64 iSize) = 0;

	//! Static call to delete a file
	/*!
		\param pszPathName [in]: Full path name of file to delete
		\return tbool: If success true, otherwise false
	*/
	static tbool DeleteFile(const tchar* pszPathName);

	//! Static call to move a file
	/*!
		\param pszPathNameDest [in]: Path name of destination directory
		\param pszPathNameSrc [in]: Path name of source directory
		\param pszName [in]: Name of file
		\return tbool: If success true, otherwise false
	*/
	static tbool MoveFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName);

	//! Static call to copy a file
	/*!
		\param pszPathNameDest [in]: Path name of destination directory
		\param pszPathNameSrc [in]: Path name of source directory
		\param pszName [in]: Name of file
		\return tbool: If success true, otherwise false
	*/
	static tbool CopyFile(const tchar* pszPathNameDest, const tchar* pszPathNameSrc, const tchar* pszName);

	//! Static call to create a directory
	/*!
		\param pszPathName [in]: Pathname of directory to create. May or may not have ending deliminator ('\' or ':')
		\return tbool: If success true, otherwise false. Call may return false if the directory already exists.
	*/
	static tbool CreateDirectory(const tchar* pszPathName);

	//! Enumeration of system directories
	enum ESystemDir {
		//! OSX: Users Preferences directory. Win32: Users "Application Data" directory
		SystemDirPreferences = 0,
		//! OSX: Users desktop. Win32: Users desktop.
		SystemDirDesktop,
		//! OSX: Application directory. Win32: "Program files" directory (use with caution, since application may be installed in custom location!)
		SystemDirApplications,
		//! OSX: Not implemented (should be users documents directory). Win32: Users documents directory.
		SystemDirDocuments,
		//! OSX: /Library/Application Support. Win32: "Program Files\Common" directory
		SystemDirApplicationSupport,
		//! OSX: The 'Music' folder inside the users private folder. Win32: The 'My Music' folder inside the users Documents folder
		SystemDirMyMusic,
		//! OSX: "Chewable" folder that gets cleaned upon boot. Win32: Temporary folder (same as TEMP env-variable).
		SystemDirScratch,
		//! OSX: /Library/. Win32: Not implemented
		SystemDirLibrary,
		//! OSX: User dir. Win32: Not implemented
		SystemDirUser
	};

	//! Static call to get system directory
	/*!
		\param SystemDir [in]: Directory to get.
		\param pszPathName [out]: Returned full path name. Must be preallocated with minimum 513 bytes.
	*/
	static void GetSystemDirectory(ESystemDir SystemDir, tchar* pszPathName);

	//! Converts from OS specific path to internal path. Only works with full paths (not relative).
	/*!
		\param pszPathName [in/out]: Path to be converted. Returns converted path. Note that returned path may be 1 byte longer than the input path.
	*/
	static void PathFromOS(tchar* pszPathName);

	//! Converts from internal path to OS specific path. Only works with full paths (not relative).
	/*!
		\param pszPathName [in/out]: Path to be converted. Returns converted path.
	*/
	static void PathToOS(tchar* pszPathName);

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
	
	//! Checks if a string represents an absolute path
	/*!
	 \param pszPathName [in]: The path to check. It may be in OS or internal format
	 \return tbool: True if path is absolute
	 */
	static tbool IsAbsPath2(const tchar* pszPathName);

	//! Checks if a string points to an existing file or folder
	/*!
		\param pszItem [in]: The item to check the existance of
		\param pbIsFolder [out]: True if existing item is a folder, False if not.<br>Omit parameter if you don't care
		\return tbool: True if item is an existing file or folder
	*/
	static tbool Exists(const tchar* pszItem, tbool* pbIsFolder = NULL);
	
	//! Split a full path into a path-only and a filename-only part
	/*!
		\param pszFullPath [in]: The full path to split
		\param pszPathOnly [out]: The path-only part. Should be preallocated with 512 or more bytes.
		\param pszNameOnly [out]: The name-only part. Should be preallocated with 512 or more bytes.
		\param bAcceptEmptyPath [in]: True = won't fail even if the "full path" input consisted of only a name part
		\param bAcceptEmptyName [in]: True = won't fail even if there was no filename in full path (it pointed to a path instead of a file)
		\return tbool: True = Success, the two output strings were updated
	*/
	static tbool SplitPathToPathAndName(const tchar* pszFullPath, tchar* pszPathOnly, tchar* pszNameOnly, tbool bAcceptEmptyPath = true, tbool bAcceptEmptyName = true);

	//! Creates an enum string with the names of all valid disk drives
	/*
		\param pszEnumNames [out]: Receives the drive letters (Windows) or names (OS X) as an enum string delimited by a char of your name
		\param iBuffSize [in]: Max number of chars to place in the buffer (including trailing zero)
		\param cDelimiter [in]: Character used for delimiting enum string
		\param bAddExtraInfo [in]: For Windows: Returns not only the drive letter but also the volume name. For OS X: Ignored.
		\return tbool: True upon success, False if insufficient buffer space (or other error)
	*/
	static tbool GetDriveNames(tchar* pszEnumNames, tint32 iBuffSize = -1, char cDelimiter = '@', tbool bAddExtraInfo = false);
};

