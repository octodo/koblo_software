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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.



/*! \brief region data object \
 *  holds region parameters used to load a region back in from a project file 
 */

class CRegion_Data:
public virtual CKSUUID
{
	
public:
	//! Constructor
	CRegion_Data();
	
	//! Destructor
	virtual ~CRegion_Data();
	
	//! get sample screen name
	std::string Sample_Screen_Name(){ return msSample_Screen_Name;};
	
	//!set sample screen name
	void Sample_Screen_Name(std::string  sSample_Screen_Name ){ msSample_Screen_Name =  sSample_Screen_Name;};
	
	//! get region possition
	tuint64 Possition(){ return muiPossition;};
	
	//! set region position
	void Possition( tuint64 uiPossition){ muiPossition = uiPossition;}; 
	
	//! get region sample offset
	tuint64 Sample_Offset(){ return muiSample_Offset;};
	
	//! set region sample offset
	void Sample_Offset( tuint64 uiSample_Offset){ muiSample_Offset = uiSample_Offset;};
	
	//! get region sample duration
	tuint64 Sample_Duration(){ return muiSample_Duration;};
	
	//! set region sample duration
	void Sample_Duration( tuint64 uiSample_Duration){ muiSample_Duration = uiSample_Duration;};
	
	//! get fade in duration
	tuint64 Fade_In_Duration(){ return muiFade_In_Duration;};
	
	//! set fade in duration
	void Fade_In_Duration( tuint64 uiFade_In_Duration){ muiFade_In_Duration = uiFade_In_Duration;};
	
	//! get fade out duration
	tuint64 Fade_Out_Duration(){ return muiFade_Out_Duration;};
	
	//! set volume duration
	void Fade_Out_Duration( tuint64 uiFade_Out_Duration){ muiFade_Out_Duration = uiFade_Out_Duration;};
	
	//! get volume duration
	tfloat32 Volume(){ return mfVolume;};
	
	//! set fade out duration
	void Volume( tfloat32 fVolume){ mfVolume = fVolume;};
	
	
	//! get sample uuid 
	std::string Sample_UUID(){ return msSample_UUID;};
	
	//! set sample uuid
	void Sample_UUID( std::string sSample_UUID){ msSample_UUID = sSample_UUID;};
	
	//! get take uuid 
	std::string Take_UUID(){ return msTake_UUID;};
	
	//! set take uuid
	void Take_UUID( std::string sTake_UUID){ msTake_UUID = sTake_UUID;};
	
	tint64 Track_ID(){ return miTrack_ID;};
	
	void Track_ID(tint32 iTrack_ID) {miTrack_ID = iTrack_ID;};
	
	
	
	
protected:
	//! track nr
	tint32 miTrack_ID;
	//! sample uuid
	std::string msSample_UUID;
	//! take uuid
	std::string msTake_UUID;
	//! screen name of sample
	std::string msSample_Screen_Name;
	//! position in samples
	tuint64 muiPossition;
	//! how much of the beginning of the sample is trimmed away
	tuint64 muiSample_Offset; 
	//! how many samples are played
	tuint64 muiSample_Duration;
	//! fade in duration
	tuint64 muiFade_In_Duration;
	//! fade out duration
	tuint64 muiFade_Out_Duration;
	//! volume
	tfloat32 mfVolume;

	
};