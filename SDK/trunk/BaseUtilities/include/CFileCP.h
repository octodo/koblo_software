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


/*! \class CFileCP
 * \brief Implementation of crossplatform part of IFile (ReadCP / WriteCP)
*/
class CFileCP : public virtual IFile
{
public:
	//! Constructor
	CFileCP();
	//! Destructor
	virtual ~CFileCP();

	//! IFile override
	virtual tuint64 ReadCP(tint32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 WriteCP(tint32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 ReadCP(tfloat32* p, tuint64 iSize);
	//! IFile override
	virtual tuint64 WriteCP(tfloat32* p, tuint64 iSize);

protected:
};





