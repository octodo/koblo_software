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



/*! \brief CRegion_Model finds and manipulates a CRegion_DSP \

*/


class CRegion_Model
{
public:
	CRegion_Model();

	virtual ~CRegion_Model();
	
	//! creates a region, and returns its unique ID. If uiSamplePosEnd == -1 all sound is used.
	tint32 CreateRegion(const std::string& sSoundListItemName, 
						tint32 iChannel, 
						tuint64 uiTrackPosStart,
						tuint64 uiSamplePosStart, 
						tuint64 uiSamplePosEnd = -1,
						tuint64 uiFadeInLength = 0,
						tuint64 uiFadeOutLength = 0,
						tfloat32 fRegionVolume = 1.0);
	
	
	//! set region volume
	void Set_Region_Volume(tuint32 uiRegionID, tfloat32 fRegion_Volume);
	//! callback used to communicate with the interface
	void SetRegionCallback(IRegionCallback* pRegionCallback) {mpRegionCallback = pRegionCallback;}
	
	struct SRegionInfo
	{	
		//! region identifier
		tuint32 uiRegionID;
		//! Track where the region belong
		tuint32 uiTrack;
		//! Start positon on track
		tuint64 uiStartPos;
		//! End Position on Track
		tuint64 uiEndPos;
		
	};

	
protected:
	
	IRegionCallback* mpRegionCallback;
	
	//! finds a region info for a region
	/*!
	 \param RegionInfo [out]: region info to write in
	 \param uiID [in]: unique region id
	 */
	void Get_Region_Info(SRegionInfo& RegionInfo,tuint32 uiID);
	
};

