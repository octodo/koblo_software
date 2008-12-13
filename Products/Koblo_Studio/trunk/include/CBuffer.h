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


//!!! max there might be an issue with the streams
// are they freed up, is one pr region the right way to do it?
const tint32 giNrOfStreams = 4096; //(giNumber_Of_Tracks * 2) +(giNumber_Of_Busses * 2); //!!! MO Check

class CKSDSP;
class IRegionCallback;

const tuint32 guiMaxNrOfChannelsInBuffer = 2;

//! A buffer consists of 1 or more channels of audio data.
class CBuffer
{
public:
	//! Constructor
	CBuffer();

	//! Destructor
	virtual ~CBuffer();

	//! Copy constructor
	CBuffer(const CBuffer& rBuffer);

	//! Assignment operator
	CBuffer operator=(const CBuffer& rBuffer);

	//! Accumulation operator. Warning: The source buffer gets changed to match the dest buffer (channel count)
	CBuffer& operator+=(CBuffer& rBuffer);

	//! Accumulation with volume. Warning: The source buffer gets changed to match the dest buffer (channel count)
	void Accumulate(CBuffer& rBuffer, tfloat32 fVolume);

	//! Clears the data
	void Clear();

	//! Will convert the data if neccasary
	void SetChannels(tuint32 uiChannels);

	//! Will not convert the data. Use this if the data is about to be overwritten anyway
	void SetChannelsNoConvert(tuint32 uiChannels) {muiChannels = uiChannels;}

	tfloat32* GetData(tuint32 uiChannel) {return mppfData[uiChannel];}

	tuint32 GetChannels() const {return muiChannels;}

protected:
	st::IMemAllocator* mpMemAllocator;

	st::IDSPTools* mpDSPTools;

	tfloat32* mppfData[guiMaxNrOfChannelsInBuffer];

	tuint32 muiDataSize;

	//! Nr of channels in buffer. Valid are 1, 2, 4, 6, 8.
	tuint32 muiChannels;

	// Convertion routines
	void Mono2Stereo();

	void Mono2Quad();

	void Mono2FiveOne();

	void Mono2SevenOne();

	void Stereo2Mono();

	void Stereo2Quad();

	void Stereo2FiveOne();

	void Stereo2SevenOne();

	void Quad2Mono();

	void Quad2Stereo();

	void Quad2FiveOne();

	void Quad2SevenOne();

	void FiveOne2Mono();

	void FiveOne2Stereo();

	void FiveOne2Quad();

	void FiveOne2SevenOne();

	void SevenOne2Mono();

	void SevenOne2Stereo();

	void SevenOne2Quad();

	void SevenOne2FiveOne();
};
