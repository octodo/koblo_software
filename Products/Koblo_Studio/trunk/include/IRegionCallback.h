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


class IRegionCallback
{
public:
	// callback for updating GUI
	/*!
		\param uiRegionID			[in]: region id
		\param uiTrack				[in]: track
		\param uiSample_Pos			[in]: Possition on track
		\param uiSample_Start		[in]: Start sample needed to draw region corectly
		\param uiSample_Duration	[in]: duration of region
		\param sName				[in]: region name
	*/
	virtual void InsertRegion(tuint32 uiRegionID, tuint32 uiTrack, tuint64 uiTrack_Pos, tuint64 uiSample_Start, tuint64 uiSample_Duration, const std::string& sName) = 0;

	virtual void Refresh_Region_GUI(tuint32 uiID, tuint32 iTrack) = 0;

	virtual void DeleteRegion(tuint32 uiID, tuint32 uiTrack) = 0;
	
	virtual void SelectRegion(tuint32 uiID, tuint32 uiTrack) = 0;
	
	virtual void JumpToPlayhead() = 0;
};


/*

Michaels notes:

To DSP:

void CutRegion(tuint32 uiID, tuint64 uiCutStartPos, tuint64 uiCutEndPos);

void DeleteTrack(tuint32 uiTrack);



*/

