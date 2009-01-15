/*!	\file CFileSearchOSX.h
	\author Michael Olsen
	\date 01/Apr/2005
	\date 01/Apr/2005
*/


/*! \class CFileSearchOSX
 * \brief OSX implementation of IFileSearch
*/
class CFileSearchOSX : public virtual IFileSearch
{
public:
	//! Constructor
	CFileSearchOSX();
	//! Destructor
	virtual ~CFileSearchOSX();

	//! IDestructable override
	virtual void Destroy();

	//! IFileSearch override
	virtual void Init(const tchar* pszPathName);
	//! IFileSearch override
	virtual tbool GetNext(tchar* pszName, tbool& rDirectory);
	
	//! IFileSearch override
	tbool Init2(const tchar* pszPathAndMask, tchar* pszErrMsgBuff = NULL, tint32 iErrMsgBuffSize = -1);


protected:
	//! Reference to folder that is iterated thru
	FSRef mFSRef;

	//! Index of search (not index into array) - should this be deleted?
	short mSearchIndex;
	
	// Prepare for defining array to a closest match of 4 VM pages (speed optimization)
#define kRequestCountPerIteration ((4096 * 4) / sizeof(FSCatalogInfo))
	
	//! File search iterator
	FSIterator mIterator;

	//! Array of info - should be changed to CAutoBuffer
	FSCatalogInfo mCatalogInfoArray[kRequestCountPerIteration];
	//! Array of names - should be changed to CAutoBuffer(?)
	HFSUniStr255 mNames[kRequestCountPerIteration];

	//! Actually read number of files (in arrays)
	ItemCount mActualCountBulk;
	//! Current index of arrays (used for GetNext() operations)
	tint miBulkIx;
	
	//! Filter (a.k.a. mask) used for comparing retrieved filenames
	std::string msFilter;
	
	//! True if file-search has been successfully initialized
	tbool mbInitSuccess;
};






