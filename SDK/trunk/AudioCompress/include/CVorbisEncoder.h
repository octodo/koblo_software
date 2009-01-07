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

#ifndef _ac_cvorbis_encoder_h
#define _ac_cvorbis_encoder_h
// Created by Lasse Steen Bohnstedt, 2007-10-xx

#include <vorbis/vorbisenc.h>

class CVorbisEncoder : public virtual IEncoder, public virtual CEncoder
{
public:
	CVorbisEncoder();
	virtual ~CVorbisEncoder();

	//! IDestructable override
	static IEncoder* Create();
	//! IDestructable override
	virtual void Destroy();
	
	// CEncoder overload
	virtual tbool Init_Descendant(EQuality eQuality);
	// CEncoder overload
	virtual tbool Process_Init_Descendant();
	// CEncoder overload
	virtual tbool Process_Exit_Descendant();
	// CEncoder overload
	virtual tbool ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 iSamples, tint64* piAccumOverflows);
	// CEncoder overload
	virtual tbool Finalize_Descendant();
	
	// IEncoder implementation
	virtual tbool GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo);

	
protected:
	ogg_stream_state os; /* take physical pages, weld into a logical
		stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
	
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream
		settings */
	vorbis_comment   vc; /* struct that stores all the user comments */
	
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	
	tfloat mfQuality;
	tbool mbVorbisEncoder_Initialized;
	
};

#endif //#infdef _ac_cvorbis_encoder_h
