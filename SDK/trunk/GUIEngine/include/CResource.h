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


/*! \class CResource
 * \brief Partial implementation of CResource
 *
 * This is a virtual class. Specific resource types (i.e. bitmap resource class) can inherit from this class.
*/
class CResource : public virtual IResource
{
public:
	//! Constructor
	CResource();
	//! Destructor
	virtual ~CResource();

	//! IResource override. If overriding, make sure to call the inherited version first
	virtual void Load(IWindow* pWnd, IFile* pFile, tint iSize, tint iResID);
	//! IResource override
	virtual tint GetResID() const;
	//! IResource override
	virtual tuchar* GetData();
	//! IResource override
	virtual tint32 GetDataSize() { return miDataSize; };
	//! IResource override
	virtual EResourceType GetResType() const;

protected:
	//! File resource is to be read from (only used during initialization)
	IFile* mpFile;
	//! Resource ID
	tint32 miResID;
	//! Resource type
	EResourceType mResType;
	//! Pointer to resource data
	tuchar* mpcData;
	//! Size of resource data buffer
	tint miDataSize;
};












