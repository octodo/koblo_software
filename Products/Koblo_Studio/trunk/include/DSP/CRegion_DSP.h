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



/*! \brief CRegion_DSP process one sample \
 *	add fade in fade out and volume to a sample \
 *  trim start and end \
 *	holds the number of channels\
 *  there is a one to one relation between a CRegion_DSP and a CRegion_Pane
 */


class CRegion_DSP
{
	public:
		CRegion_DSP(tint32 iUniqueID, const std::string& sSoundPathNameL, const std::string& sSoundPathNameR, const std::string& sSoundListItemName, tuint64 uiSamplePosStart, tuint64 uiSample_Duration);
		
		virtual ~CRegion_DSP();
		
		tuint64 GetDuration() const {return muiEndPos - muiStartPos + 1;}
		
		// Get end pos
		tuint64 GetEndPos(){return muiEndPos;};
		
		tuint64 GetEndPos() const {return muiEndPos;}
		
		void SetDuration(tuint64 iDuration);
		
		//! Get samples from underlying CSample_DSP >> wave(s)
		/*!
		 \param ppfData [out]: Receives the samples in 2D array
		 \param iSamples [in]: Number of samples to return
		 */
		void GetSamples(tfloat32** ppfData, tint32 iSamples);
		
		tint32 GetID() const {return muiUniqueID;}
		
		CSample_DSP* GetSound(tint32 iIndex) const {return mppSample[iIndex];}
		
		tuint64 Get_Sample_Offset() const {return muiStartPos;}
		
		const tchar* GetSoundListItemName() const { return msSoundListItemName.c_str(); }
		
		void SetStartPos(tuint64 uiStartPos)  { muiStartPos = uiStartPos ;}
		
		//	tuint64 GetEndPos() const {return muiEndPos-muiStartPos;}
		
		//! get the duration of the embedded sample
		tuint64 Get_Sample_Duration(){ return mppSample[0]->GetLength();};
		
		
		// Set sample offset
		void SetPos(tuint64 uiPos);
		
		// Set sample endpos
		void SetEndPos(tuint64 uiPos);
		
		virtual void Set_Fade_In_Duration(tuint64 uiFadeInLength){muiFadeInLength = uiFadeInLength;};
		
		virtual void Set_Fade_Out_Duration(tuint64 uiFadeOutLength){muiFadeOutLength = uiFadeOutLength;};
		
		virtual void Set_Volume(tfloat32 fVolume){mfVolume = fVolume;};
		
		tint64 Get_Fade_In_Duration(){ return muiFadeInLength;};
		
		tint64 Get_Fade_Out_Duration(){return muiFadeOutLength;};
		
		tfloat32 Get_Volume(){return mfVolume;};
		
		tint32 GetChannels() const {return miChannels;}
		
		// iSize == 0: small (1 / 1024 sample). iSize == 1: large (1 / 64 sample)
		void GetPeakFile(IFile** ppFile, tint32 iChannel, tint32 iSize);
		
	protected:
		CSample_DSP** mppSample;
		
		tint32 miChannels;
		
		IFile* mppPeakFile[4];
		
		tint32 muiUniqueID;
		
		//! First sample to play
		tuint64 muiStartPos;
		
		//! Last sample to play
		tuint64 muiEndPos;
		
		//! Name from sample browser in left side of track editor window without path and extencions
		std::string msSoundListItemName;
		
		//! Fade-in length in samples (0 is no fade).
		tuint64 muiFadeInLength;
		
		//! Fade-out length i samples (0 is no fade)
		tuint64 muiFadeOutLength;
		
		//! Region volume
		tfloat32 mfVolume;
	};

