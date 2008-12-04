/*!	\file CINet.h
	\author Michael Olsen / Claus Riber
	\date 23/May/2005 
	\date 23/May/2005
*/

/*! \class CINet
 * \brief Implementation of IINet
*/

#define MAX_FILE_SIZE 110000 //over 100K
class CINet : public virtual IINet
{
public:
	//! Constructor
	CINet();

	//! Destructor
	virtual ~CINet();

	//! IDestructable override
	virtual void Destroy();

	//! IINet override
	virtual tbool CheckOnlineStatus() throw();
	//! IINet override
	virtual tbool CheckUserName(const TUserName& UserName, const TPassword& Password) throw(IException*);
	//! IINet override
	virtual tfloat64 GetCredit(const TUserName& UserName, const TPassword& Password) throw(IException*);
	//! IINet override
	virtual tint32 GetInterfaceVersion(tchar* pBuffer, tint32 iBufferSize) throw(IException*);
	//! IInet implementation
	virtual tbool GetInterfaceVersionEx(tchar* pszVersion, tint32 iVersionBufferSize, tint32 iProductID, tuint16 uiKSPIVersionMajor, tuint16 uiKSPIVersionMinor, tuint16 uiKSPIRevision, tbool* pbUpgradeNeeded, tbool* pbUpgradeAvailable, tchar* pszUpgradeURL, tint32 iUpgradeURLBufferSize);
	//! IINet override
	virtual tint32 GetElementCount(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*);
	//! IINet override
	virtual tint32 GetElements(const TUserName& UserName, const TPassword& Password, SPath Path, TElementText* pTexts, EElementType* pTypes, tint32* piPrices, tint32 iMaxNrOfElements) throw(IException*);
	//! IINet override
	virtual tint32 GetFileSize1(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*);
	//! IINet override
	virtual tint32 GetFileSize2(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*);
	//! IINet override
	virtual tint32 GetFile1(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*);
	//! IINet override
	virtual tint32 GetFile2(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*);
	//! IINet override
	virtual void PutFiles(const TUserName& UserName, const TPassword& Password, SPath Path, const void* pBuffer1, tint32 iBufferSize1, const void* pBuffer2, tint32 iBufferSize2) throw(IException*);
	//! IINet override
	virtual void Purchase(const TUserName& UserName, const TPassword& Password, SPath Path, tint32 iPrice) throw(IException*);
	//! IINet override
 	virtual tbool RegisterPOP(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void* pResponse, tint32 iResponseSize) throw(IException*);
	//! IINet override
 	virtual tint32 RegisterPOP2(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void** ppResponse, tint32 iResponseSize, tchar* pszRespFileName, tchar* pszErrMsg);

protected:
	//! Like GetFile1 and 2
    /*!
		\param File1 [in]: true if reading file 1 else false
		\param UserName [in]: User identifier, or UserNameDefault
		\param Path [in]: Path to file to get
		\param pBuffer [out]: Pointer to buffer to be filled with file content.
		\param iBufferSize [in]: Size of buffer (in bytes)
		\return tint32: Number of bytes written in buffer
	*/
    static tint32 GetFile(bool File1, const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*);



};

