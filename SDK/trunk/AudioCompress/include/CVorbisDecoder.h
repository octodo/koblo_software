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

#ifndef _ac_cvorbis_decoder_h
#define _ac_cvorbis_decoder_h

#include <vorbis/vorbisenc.h>


class CVorbisDecoder : public virtual IDecoder, public virtual CDecoder
{
public:
	CVorbisDecoder();
	virtual ~CVorbisDecoder();
	
	//! IDestructable override
	static IDecoder* Create();
	//! IDestructable override
	virtual void Destroy();

	//! Prepare for decoding of a set of ogg/vorbis streams into one or two WAVE streams
	/*!
		\param pfOutputWaveStream1 [in]: The output Wave stream to write to - must be previously opened with write access. If an pfOutputWaveStream2 is entered too then pfOutputWaveStream1 will receive left channel data only
		\param bRIFFWaveHeader [in]: Set false to output only the RAW PCM data (skip writing the RIFF/Wave header)
		\param iBitWidth [in]: The requested bit-width for the output Wave. Must be 16 or 24
		\param iChannels [in]: The requested channels for the output Wave (1 = mono, 2 = stereo, -1 = same as the first processed ogg/vorbis stream)
		\param pfOutputWaveStream2 [in]: An optional output Wave stream to write right channel only to - must be previously opened with write access. Requires params iBitWidth = 24 and iChannels = 1
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader = true, tint32 iBitWidth = 24, tint32 iChannels = -1, IFile* pfOutputWaveStream2 = NULL);
	
	//! Decode an ogg/vorbis stream and append to the previously initialized output Wave stream(s)
	/*!
		\param pfInputOggStream [in]: Source stream (ogg/vorbis format) already opened with read permission
		\param iSampleIxToProcess [in]: Index of first sample to encode (0 = very first)
		\param iNbSamplesToProcess [in]: Number of samples to encode (-1 = all)
		\param piSamplesProcessed [in]: Recieves number of samples actually processed. If 0 then there are no more samples in file
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	 */
	virtual tbool Process(IFile* pfInputOggStream, tint32 iSampleIxToProcess = 0, tint32 iNbOfSamplesToProcess = -1, tint32* piSamplesProcessed = NULL);
	
	// CEncoder implementation
	virtual tbool Finalize_CodecSpecific();
	
protected:
	ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
	ogg_stream_state os; /* take physical pages, weld into a logical
		stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream
		settings */
	vorbis_comment   vc; /* struct that stores all the bitstream user comments */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */

	tbool mb_ogg_sync_init;
	tbool mb_vorbis_info_init;
	tbool mb_ogg_sync_pageout;
	void MaybeCloseVorbisInfo();
	void MaybeCloseOggSync();
	
public:
	/*
	// The following are valid after Init() method
	tint32 miBitWidth;
	tbool mbRIFFWaveHeader;
	
	// The following are only valid after at least one call to the Process() method
	tint32 miChannels;
	tint32 miSampleFreq;
	
	// The following are only valid after at least one call to the Process() method, and may change with each call
	tint32 miLastInputChannels;
	tint32 miLastInputSamples;
	
	// The folowing are only valid after all calls to the Process() method
	tint32 miSamplesTotal;
	*/
};

#endif // #ifndef _ac_cvorbis_decoder_h