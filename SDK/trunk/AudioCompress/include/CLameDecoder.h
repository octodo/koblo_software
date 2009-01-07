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

#ifndef _ac_clame_decoder_h
#define _ac_clame_decoder_h

//#include <lame.h>


class CLameDecoder : public virtual IDecoder, public virtual CDecoder
{
public:
	CLameDecoder();
	virtual ~CLameDecoder();
	
	//! IDestructable override
	static IDecoder* Create();
	//! IDestructable override
	virtual void Destroy();

	//! Prepare for decoding of a set of mp3 streams into a one or two WAVE streams
	/*!
		\param pfOutputWaveStream1 [in]: The output Wave stream to write to - must be previously opened with write access. If an pfOutputWaveStream2 is entered too then pfOutputWaveStream1 will receive left channel data only
		\param bRIFFWaveHeader [in]: Set false to output only the RAW PCM data (skip writing the RIFF/Wave header)
		\param iBitWidth [in]: The requested bit-width for the output Wave. Must be 16 or 24
		\param iChannels [in]: The requested channels for the output Wave (1 = mono, 2 = stereo, -1 = same as the first processed ogg/vorbis stream)
		\param pfOutputWaveStream2 [in]: An optional output Wave stream to write right channel only to - must be previously opened with write access. Requires params iBitWidth = 24 and iChannels = 1
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader = true, tint32 iBitWidth = 24, tint32 iChannels = -1, IFile* pfOutputWaveStream2 = NULL);
	
	//! Decode an mp3 stream and append to the previously initialized output Wave stream(s)
	/*!
		\param pfInputMp3Stream [in]: Source stream (ogg/vorbis format) already opened with read permission
		\param iSampleIxToProcess [in]: Index of first sample to encode (0 = very first)
		\param iNbSamplesToProcess [in]: Number of samples to encode (-1 = all)
		\param piSamplesProcessed [in]: Recieves number of samples actually processed. If 0 then there are no more samples in file
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	 */
	virtual tbool Process(IFile* pfInputMp3Stream, tint32 iSampleIxToProcess = 0, tint32 iNbOfSamplesToProcess = -1, tint32* piSamplesProcessed = NULL);

	// CEncoder implementation
	virtual tbool Finalize_CodecSpecific();
	
protected:

	tbool mb_lame_decode_init;
};

#endif // #ifndef _ac_clame_decoder_h