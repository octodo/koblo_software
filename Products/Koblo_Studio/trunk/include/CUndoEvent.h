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

class CUndoEvent
{
public:
	enum EEventType {
		//! Data engine-style parameter
		TypeParameter = 0,
		//! Region info. Create and/or change region
		TypeRegion,
		//! Region info. Delete region
		TypeDeleteRegion
	};

	//! Data for TypeParameter
	struct SParameterData {
		tint32 iSection;
		tint32 iParameter;
		tint32 iValue;
	};

	//! Data for TypeRegion
	struct SRegionData {
	};

	//! Data for TypeDeleteRegion
	struct SDeleteRegionData {
	};

	//! Constructor
	CUndoEvent(SParameterData Data)
		: mType(TypeParameter), mParameterData(Data) {}

	//! Constructor
	CUndoEvent(SRegionData Data)
		: mType(TypeRegion), mRegionData(Data) {}

	//! Constructor
	CUndoEvent(SDeleteRegionData Data)
		: mType(TypeDeleteRegion), mDeleteRegionData(Data) {}

	EEventType GetType() const {return mType;}

	SParameterData GetParameterData() const {return mParameterData;}

	SRegionData GetRegionData() const {return mRegionData;}

	SDeleteRegionData GetDeleteRegionData() const {return mDeleteRegionData;}

protected:
	EEventType mType;

	union {
		SParameterData mParameterData;
		SRegionData mRegionData;
		SDeleteRegionData mDeleteRegionData;
	};
};

