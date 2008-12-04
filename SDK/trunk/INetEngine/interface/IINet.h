// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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


/*! \class IINet
 * \brief Interface to access internet database
 *
 *	In many ways this interface works like a folder on a harddrive, i.e. it has paths, files and folders.
 *	Any entry (or "element") into the database is described as a path, using \ as a deliminator. "\" is the root.
 *	A path ends with '\' unless it's a file
 *
 *	An entry in the database consists of 2 "packages", i.e. 2 chunks of data.
*/
class IINet : public virtual IDestructable
{
public:
	//! Type which describes a username to grant access to the inet database (zero terminated)
	typedef tchar TUserName[50+1];
	typedef tchar TPassword[50+1];
    typedef tchar TPOP[(4*8+7)+1];

	//! Default username used for users with no username
	static TUserName UserNameDefault;

	//! Default password used for users with no username
	static TPassword PasswordDefault;

	//! Type which describes the text for a given element (zero terminated)
	typedef tchar TElementText[64+1];

	//! Enum used to describe a path type
	enum EPathType {
		//! System path type, i.e. relative from database root
		PathTypeSystem = 0,
		//! User path type, i.e. relative from users root
		PathTypeUser
	};

	//! Struct which describes a path in the database
	struct SPath {
		//! Path type
		EPathType PathType;

		//! The relative path (zero terminated)
		tchar pszPathName[256];
	};

	//! Element types
	enum EElementType {
		//! Standard element with no action (i.e. text)
		ElementTypeStd = 0,
		//! Folder type element (can open and close)
		ElementTypeFolder,
		//! File type element (can load)
		ElementTypeFile
	};

	//! File types
	enum EFileType {
		//! Preset file type
		FileTypePreset = 0,
		//! Multi file type
		FileTypeMulti,
		//! Performance file type
		FileTypePerformance
	};

	//! Creates an instance of an IINet object
	static IINet* Create();

	//! Checks the online status, i.e. whether we can connect to, and communicate with the internet database. Note, it doesn't throw anything, but rather returns false
	/*!
		\return tbool: True if function succeeds in connecting to and communicating with the interfnet database, otherwise false
	*/
	virtual tbool CheckOnlineStatus() throw() = 0;

	//!Returns version of (server) interface to be checked for compatibility <b>OBSOLETE! No longer works</b>
	/*!
		\param pBuffer [out]: Pointer to buffer to be filled with version string (zero terminated)
		\param iBufferSize [in]: Size of buffer (in bytes)
		\return tint32: Number of bytes written in buffer (including zero)
	*/
	virtual tint32 GetInterfaceVersion(tchar* pBuffer, tint32 iBufferSize) throw(IException*) = 0; // Obsolete!

	// Lasse, added 2007-09-07
	//!Returns version of (server) interface to be checked for compatibility
	/*!
		\param pszVersion [out]: Pointer to buffer to be filled with Web Server's version string (zero terminated)
		\param iVersionBufferSize [in]: Size of buffer (in bytes)
		\param uiKSPIVersionMajor [in]: Integer part of application version info (ex: the "1" in "1.0 r3")
		\param uiKSPIVersionMinor [in]: Decimal part of application version info (ex: the "0" in "1.0 r3")
		\param uiKSPIRevision [in]: Revision part of application version info (ex: the "3" in "1.0 r3")
		\param pbUpgradeNeeded [out]: True if an application upgrade is necesary (critical upgrade - for crash fixes)
		\param pbUpgradeAvailable [out]: True if an application upgrade is available (for new features). Is always true if pbUpgradeNeeded is true.
		\param pszUpgradeURL [out]: If an upgrade is available, this is where to get it
		\param iUpgradeURLBufferSize [in]: Size of buffer (in bytes)
		\return tbool: True on success
	*/
	virtual tbool GetInterfaceVersionEx(tchar* pszVersion, tint32 iVersionBufferSize, tint32 iProductID, tuint16 uiKSPIVersionMajor, tuint16 uiKSPIVersionMinor, tuint16 uiKSPIRevision, tbool* pbUpgradeNeeded, tbool* pbUpgradeAvailable, tchar* pszUpgradeURL, tint32 iUpgradeURLBufferSize) = 0;
	// .. Lasse

	//! Checks the validity of the username
	/*!
		\param UserName [in]: Username to check for
		\param Password [in]: Password to check for
		\return tbool: True if username/password is valid, false otherwise
	*/
	virtual tbool CheckUserName(const IINet::TUserName& UserName, const IINet::TPassword& Password) throw(IException*) = 0;

	//! Returns number of elements for a given folder
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\return tfloat64: Number of credits for this user
	*/
	virtual tfloat64 GetCredit(const TUserName& UserName, const TPassword& Password) throw(IException*) = 0;

	//! Returns number of elements for a given folder
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path folder to get element count for
		\return tint32: Number of elements in folder
	*/
	virtual tint32 GetElementCount(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*) = 0;

	//! Gets the elements for a given path
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path folder to get elements for
		\param pTexts [out]: Array of element texts to be filled
		\param pTypes [out]: Array of element types to be filled
		\param piPrices [out]: Array of prices (* 100) to be filled. * 100 means that for instance 4.95 is represented (returned) as 495.
		\param iMaxNrOfElements [in]: Number of elements in arrays
		\return tint32: Number of elements returned (0 to iMaxNrOfElements)
	*/
	virtual tint32 GetElements(const TUserName& UserName, const TPassword& Password, SPath Path, TElementText* pTexts, EElementType* pTypes, tint32* piPrices, tint32 iMaxNrOfElements) throw(IException*) = 0;

	//! Returns size of first chunk of a given file
	//! NB: Because of encoding this is only an approximation usable for allocation
	//! Real filesize returned by GetFile1 is never greater than this value
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to get size of
		\return tint32: Size of first chunk of file (in bytes)
	*/
	virtual tint32 GetFileSize1(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*) = 0;

	//! Returns size of second chunk of a given file
	//! NB: Because of encoding this is only an approximation usable for allocation
	//! Real filesize returned by GetFile2 is never greater than this value
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to get size of
		\return tint32: Size of second chunk of file (in bytes)
	*/
	virtual tint32 GetFileSize2(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*) = 0;

	//! Gets the contents of first chunk of a file
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to get
		\param pBuffer [out]: Pointer to buffer to be filled with file content.
		\param iBufferSize [in]: Size of buffer (in bytes)
		\return tint32: Number of bytes written in buffer
	*/
	virtual tint32 GetFile1(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*) = 0;

	//! Gets the contents of second chunk of a file
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to get
		\param pBuffer [out]: Pointer to buffer to be filled with file content.
		\param iBufferSize [in]: Size of buffer (in bytes)
		\return tint32: Number of bytes written in buffer
	*/
	virtual tint32 GetFile2(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*) = 0;

	//! Puts a file in the users folder
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to put (must be PathTypeUser)
		\param pBuffer1 [in]: Pointer to buffer containing first chunk
		\param iBufferSize1 [in]: Size of first buffer (in bytes)
		\param pBuffer2 [in]: Pointer to buffer containing second chunk
		\param iBufferSize2 [in]: Size of second buffer (in bytes)
	*/
	virtual void PutFiles(const TUserName& UserName, const TPassword& Password, SPath Path, const void* pBuffer1, tint32 iBufferSize1, const void* pBuffer2, tint32 iBufferSize2) throw(IException*) = 0;

	//! Purchase a file
	/*!
		\param UserName [in]: User identifier, or UserNameDefault
		\param Password [in]: Password to check for
		\param Path [in]: Path to file to purchase
		\param iPrice [in]: Price (* 100) of file as reported by software to user (must match price in database). * 100 means that for instance 4.95 is represented (send) as 495.
	*/
	virtual void Purchase(const TUserName& UserName, const TPassword& Password, SPath Path, tint32 iPrice) throw(IException*) = 0;

	//! <b>Deprecated! Used RegisterPop2 instead</b> Register a POP code and return response code from challenge
	/*!
		\param POP [in]: POP code p.t. 8x4 chars, space delimited
		\param pChallenge [in]: Challenge code which contains machine specific data
		\param iChallengeSize [in]: Size (length) of Challenge
		\param pResponse [out]: Pointer to buffer to be filled with file content.
		\param iResponseSize [in]: Size of buffer (in bytes)
		\return bool: True if registration was ok, false if not (because allready registered with other challenge)
	*/
	virtual tbool RegisterPOP(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void* pResponse, tint32 iResponseSize) throw(IException*) = 0;

	//! Register a POP code and return response code from challenge
	/*!
		\param POP [in]: POP code p.t. 8x4 chars, space delimited
		\param pChallenge [in]: Challenge code which contains machine specific data
		\param iChallengeSize [in]: Size (length) of Challenge
		\param ppResponse [out]: Unallocated pointer that will be assigned buffer with response file content.
		\param iResponseSize [in]: Size of 1 response (in bytes)
		\param pszErrMsg [out]: Pointer to string (buffer at least 128 char) that will recieve error message (only upon error - untouched otherwise)
		\return tint32: Number of reponses for PoP upon succes, 0 upon error
	*/
	virtual tint32 RegisterPOP2(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void** ppResponse, tint32 iResponseSize, tchar* pszRespFileName, tchar* pszErrMsg) = 0;
};

