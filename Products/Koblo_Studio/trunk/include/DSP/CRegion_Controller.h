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



/*! \brief CRegion_Controller finds and manipulates all CRegion_DSP data \

*/
class CTrack_DSP;
class CRegion_DSP;

class CRegion_Controller
{
public:
	CRegion_Controller();

	virtual ~CRegion_Controller();
	
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
	void Set_Region_Volume(tuint32 uiRegionID, tfloat32 fRegion);
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
	
	/*! cut a region; creates two region from one
	 \	param uiTrack [in]:	track
	 \	param uiRegionID [in]: region id
	 \	uiCutPos lC [in]: cut pos on region
	 */
	void CutRegion(tuint32 uiTrack, tuint32 uiRegionID, tuint64 uiCutPos);
	
	/*! trim a region; cuts off start ore end
	 \	param uiTrack [in]:	track
	 \	param uiRegionID [in]: region id
	 \	bStart lC [in]: if true we are cutting of the start of the region otherwise it's the end we are edditing
	 */
	void TrimRegion(tuint32 uiTrack, tuint32 uiRegionID, tbool bStart, tint64 uiSamplePos);
	
	//! duplicate a region
	void DuplicateRegion();
	
	//! normalise a region
	void NormaliseRegion();
	
	//! inverse a region
	void InverseRegion();
	
	//! fade In
	tuint64 Fade_In(tuint32 uiRegionID, tuint64 uiFadeInLength);
	
	//! fade Out
	tuint64 Fade_Out(tuint32 uiRegionID, tuint64 uiFadeOutLength);
	
		
	//! move a region
	void Move_Region(tuint32 uiRegionID, tint32 iChannelNew, tuint64 uiTrackPosStartNew);
	
	// save region
	tbool SaveTrackRegionDataToChunk(tint32 iTrack, IChunk* pChunk);
	// load region
	tbool CreateRegionFromChunkData(tint32 iTrack, IChunk* pChunk);
	
	//! refresh region view
	void Refresh_Region_GUI(tint32 iRegion, tuint32 iTrack);
	
	
	//! delete  region
	void Delete_Region(tuint32 uiTrack, tuint32 uiRegionID);
	
	//! delete view
	void Delete_Region_View(tuint32 uiTrack, tuint32 uiRegionID);
	 
	//! select region
	STrackSelectionInfo Select_Region(tint32 iRegionID);
	
	/*! get the region size in samplepoints
	 \	param sSoundPathName [in]:	sound path
	 \	param uiSamplePosStart [in]: start position of region
	 \	param uiSamplePosEnd [in]: evd position of region
	 */
	tuint64 GetRegionSize(const std::string& sSoundPathName, tuint64 uiSamplePosStart, tint64 uiSamplePosEnd);
	
	//! Get the region size in samplepoints
	tuint64 GetRegionSize(tuint32 uiID);
	
	//! Get Drawing info for a region
	SRegion_Drawing_Info Get_Region_Drawing_Info(tuint uiRegion);
	
	void DeleteAllRegionsForTrack(tint32 iTrack);
	
	const tchar* GetClipNameOfSelectedRegion();
	
	// iSize == 0: small (1 / 1024 sample). iSize == 1: large (1 / 64 sample)
	void GetRegionPeakFile(tuint32 uiRegionID, IFile** ppFile, tint32 iTrack, tint32 iSize);
	
	
	
protected:
	
	IRegionCallback* mpRegionCallback;
	
	//! finds a region info for a region
	/*!
	 \param RegionInfo [out]: region info to write in
	 \param uiID [in]: unique region id
	 */
	void Get_Region_Info(SRegionInfo& RegionInfo,tuint32 uiID);
	
	//! prepare pointers to region and track
	void Prepare(tuint32 uiRegionID);
	
	SRegionInfo		mRegionInfo;
	CTrack_DSP*		mpTrack_DSP;
	CRegion_DSP*	mpRegion_DSP;
	tint64			miTrack;
	
	
	
};

