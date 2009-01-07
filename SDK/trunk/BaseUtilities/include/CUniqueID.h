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


/*! \class CUniqueID
 * \brief Implementation of IUniqueID
*/
class CUniqueID : public virtual IUniqueID
{
public:
	//! Constructor
	CUniqueID(tuint32 uiMinValue = 0);
	//! Destructor
	virtual ~CUniqueID();

	//! IDestructable override
	virtual void Destroy();

	//! IUniqueID override
	virtual tuint32 GetID();
	//! IUniqueID override
	virtual void ReleaseID(tuint32 iID);

protected:
	//! Min value to use as ID
	tuint32 muiMinValue;

	//! Map of ID's in use. The first value is used to find our value, the second is a dummy (not used). I.e. if we can locate a map entry based on an ID that means we've used the ID, otherwise we havn't.
	std::map<tuint32, tuint32> mIDMap;
};






