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






