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






