
//! brief: class OggVorbis::Encoder
// Code cut partially from libvorbis / vorbisenc example
// Created by Lasse Steen Bohnstedt, 2007-10-xx

// The original comment of the vorbisenc main.cpp file is as follows:
/********************************************************************
*                                                                  *
* THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
* USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
* GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
* IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
*                                                                  *
* THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
* by the Xiph.Org Foundation http://www.xiph.org/                  *
*                                                                  *
********************************************************************/


#include "acInternalOS.h"

//#include <ogg/config_types.h>
#include <ogg/os_types.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>

#ifdef _WIN32
#include <time.h>
#endif // #ifdef _WIN32


CVorbisEncoder::CVorbisEncoder()
{
	miLowestPossibleQualityNb = geQualityPoor;
	miHighestPossibleQualityNb = geQualityInsaner;
	miDefaultQualityNb = geQualityHigh;
	mpszEncoderName = "Ogg/Vorbis aoTuVb5 Encoder";
	meAudioCodec = geAudioCodecVorbis;
	//mpszDefaultExt = ".ogg";
	//mpszFileTypeDesc = "Ogg/Vorbis Files (*.ogg)";
}

CVorbisEncoder::~CVorbisEncoder()
{
	if (mpfOutputStream) {
		Finalize();
	}
}
	
IEncoder* CVorbisEncoder::Create()
{
	return dynamic_cast<IEncoder*>(new CVorbisEncoder());
}

void CVorbisEncoder::Destroy()
{
	delete dynamic_cast<CVorbisEncoder*>(this);
}


tbool CVorbisEncoder::GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo)
{
	tint32 iMono = 0;
	tint32 iStereo = 0;

	switch (iQualityNb) {
		case geQualityPoor:		iMono = 28;		iStereo = 33;	break;
		case geQualityLow:		iMono = 50;		iStereo = 66;	break;
		case geQualityMedium:	iMono = 65;		iStereo = 99;	break;
		case geQualityGood:		iMono = 73;		iStereo = 127;	break;
		case geQualityBetter:	iMono = 78;		iStereo = 144;	break;
		case geQualityHigh:		iMono = 85;		iStereo = 179;	break;
		case geQualitySuper:	iMono = 95;		iStereo = 204;	break;
		case geQualityExtreme:	iMono = 120;	iStereo = 247;	break;
		case geQualityInsane:	iMono = 158;	iStereo = 336;	break;
		case geQualityInsaner:	iMono = 240;	iStereo = 487;	break;
	}

	if (piKBPS_Mono)
		*piKBPS_Mono = iMono;
	if (piKBPS_Stereo)
		*piKBPS_Stereo = iStereo;

	return ((iMono > 0) && (iStereo > 0));
}


tbool CVorbisEncoder::Init_Descendant(EQuality eQuality)
{
	tfloat afQuality[geNbOfQualities] = { -1000.0f, -0.2f, 0.0f, 0.2f, 0.35f, 0.425f, 0.5f, 0.6f, 0.75f, 0.9f, 1.0f, 100.0f };
	mfQuality = afQuality[eQuality];
	tchar psz[128];
	sprintf(psz, "Encoding with %s quality (%0.3f)\n", GetQualityName((tint32)eQuality), mfQuality);
	
	std::cout << psz;
	
	return true;
} // Init_Descendant


tbool CVorbisEncoder::Process_Init_Descendant()
{
	// Setup encoder
	vorbis_info_init(&vi);
	//mfQuality = -0.1;
	if (vorbis_encode_init_vbr(&vi, miOutputChannels, miInputSampleFreq, mfQuality) != 0) {
		AppendErrMsg("Function vorbis_encode_init_vbr failed for unknown reason.");
		return false;
	}
	// Note: Using vorbis_encode_init rather than vorbis_encode_init_vbr makes encoding 4 times slower
	// because it will try harder at targetting the bitrate you ask of it.
	// Also I think the guy with the unpronouncable name that tuned vorbis (the aoTuV versions)
	// only bothered to optimize the "_vbr" method.. and that's quite ok.
	// Lasse
	//
	//if (vorbis_encode_init(&vi, miOutputChannels, miInputSampleFreq, 256000, 192000, 32000) != 0) {
	//	AppendErrMsg("Function vorbis_encode_init(..) failed for unknown reason.");
	//	return false;
	//}

	// TODO: add comments
	vorbis_comment_init(&vc);
	vorbis_comment_add_tag(&vc,"ENCODER","Koblo 1.0 // aoTuV b5");
	vorbis_comment_add_tag(&vc,"KOBLO_ENC_QUAL", (char*)GetQualityName(meQuality));
	
	/* set up the analysis state and auxiliary encoding storage */
	vorbis_analysis_init(&vd,&vi);
	vorbis_block_init(&vd,&vb);
	
	/* set up our packet->stream encoder */
	/* pick a random serial number; that way we can more likely build
		chained streams just by concatenation */
	srand((unsigned)time(NULL));
	ogg_stream_init(&os,rand());
	
	/* Vorbis streams begin with three headers; the initial header (with
		most of the codec setup parameters) which is mandated by the Ogg
		bitstream spec.  The second header holds any comment fields.  The
		third header holds the bitstream codebook.  We merely need to
		make the headers, then pass them to libvorbis one at a time;
		libvorbis handles the additional Ogg bitstream constraints */
	
	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;
		
		vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
		ogg_stream_packetin(&os,&header); /* automatically placed in its own
			page */
		ogg_stream_packetin(&os,&header_comm);
		ogg_stream_packetin(&os,&header_code);
		
		/* This ensures the actual
			* audio data will start on a new page, as per spec
			*/
		while(1){
			int result=ogg_stream_flush(&os,&og);
			if(result==0)break;
			//fwrite(og.header,1,og.header_len,stdout);
			//fwrite(og.body,1,og.body_len,stdout);
			WriteOutput((char*)og.header, og.header_len);
			WriteOutput((char*)og.body, og.body_len);
		}
	}

	return true;
} // Process_Init_Secendant


tbool CVorbisEncoder::ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 iSamples, tint64* piAccumOverflows)
{
	tbool bError = false;

	if (iSamples > 0){
		/* data to encode */
		
		// In my opinion this is a problem with ogg/vorbis:
		// Calling app must convert everything to 32 bit float arrays before passing into the engine
		// On the upside:
		// 1) The rest of the process is *much* faster than mp3
		// 2) The engine is itself responsible for allocating buffer, which means that buffer-overflow
		//    is virtually impossible.
		// -- Lasse

		/* expose the buffer to submit data */
		tfloat32 **ppfPreProcessBuff = vorbis_analysis_buffer(&vd, iSamples);
		tchar* pcPreProcessLeft = (tchar*)(ppfPreProcessBuff[0]);
		tchar* pcPreProcessRight = (tchar*)(ppfPreProcessBuff[1]);

		switch (miInputBitWidth) {
			case 16:
				{
					tint32 iPreProcessBytes = 0;
					if (miInputChannels == 1) {
						// Mono in
						if (miOutputChannels == 1) {
							st::Xlat::PCM16Mono_AF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							// Mono => stereo
							st::Xlat::PCM16Mono_DualAF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL,
								&pcPreProcessRight, NULL);
						}
					}
					else if (mbIsInputInterleavedStereo) {
						// Interleaved stereo in
						if (miOutputChannels == 1) {
							// Stereo => mono
							st::Xlat::PCM16Stereo_AF32Mono(
								pcInput1, iInputBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							st::Xlat::PCM16Stereo_DualAF32Mono(
								pcInput1, iInputBytes,
								&pcPreProcessLeft, NULL,
								&pcPreProcessRight, NULL);
						}
					}
					else {
						// Dual mono in
						if (miOutputChannels == 1) {
							st::Xlat::DualPCM16Mono_AF32Mono(
								pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							st::Xlat::DualPCM16Mono_DualAF32Mono(
								pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL,
								&pcPreProcessRight, NULL);
						}
					}
				}
				break;

			case 24:
				{
					tint32 iPreProcessBytes = 0;
					if (miInputChannels == 1) {
						// Mono in
						if (miOutputChannels == 1) {
							st::Xlat::PCM24Mono_AF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							// Mono => stereo
							st::Xlat::PCM24Mono_DualAF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL, &pcPreProcessRight, NULL);
						}
					}
					else if (mbIsInputInterleavedStereo) {
						// Interleaved stereo in
						if (miOutputChannels == 1) {
							// Stereo => mono
							st::Xlat::PCM24Stereo_AF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							st::Xlat::PCM24Stereo_DualAF32Mono(
								pcInput1, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL,
								&pcPreProcessRight, NULL);
						}
					}
					else {
						// Dual mono in
						if (miOutputChannels == 1) {
							// Stereo => mono
							st::Xlat::DualPCM24Mono_AF32Mono(
								pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL);
						}
						else {
							st::Xlat::DualPCM24Mono_DualAF32Mono(
								pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
								&pcPreProcessLeft, NULL,
								&pcPreProcessRight, NULL);
						}
					}
				}
				break;

			case 32:
				{
					// 32 bit float array(s) - unfortunately we have to copy contents into engine
					if (miInputChannels == 1) {
						// Mono in
						if (miOutputChannels == 1) {
							if (mfNormalizationFactor == 1.0f) {
								// Don't normalize - raw copying
								memcpy(pcPreProcessLeft, pcInput1, iInputBytes);
							}
							else {
								// Copy + normalize
								st::Xlat::AF32Mono_AF32Mono(
									pcInput1, iInputBytes,
									&pcPreProcessLeft, NULL,
									piAccumOverflows,
									-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
							}
						}
						else {
							// mono => stereo
							if (mfNormalizationFactor == 1.0f) {
								// Don't normalize - raw copying
								memcpy(pcPreProcessLeft, pcInput1, iInputBytes);
								memcpy(pcPreProcessRight, pcInput1, iInputBytes);
							}
							else {
								// Copy + normalize left..
								st::Xlat::AF32Mono_AF32Mono(
									pcInput1, iInputBytes,
									&pcPreProcessLeft, NULL,
									piAccumOverflows,
									-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
								// .. then raw copy into right too
								memcpy(pcPreProcessRight, pcPreProcessLeft, iInputBytes);
							}
						}
					}
					else {
						// Dual mono in
						if (miOutputChannels == 1) {
							// Dual mono => mono
							st::Xlat::DualAF32Mono_AF32Mono(pcInput1, pcInput2, iInputBytes,
								&pcPreProcessLeft, NULL,
								piAccumOverflows);
						}
						else {
							// Dual mono in and out
							if (mfNormalizationFactor == 1.0f) {
								// Don't normalize - raw copying
								memcpy(pcPreProcessLeft, pcInput1, iInputBytes);
								memcpy(pcPreProcessRight, pcInput2, iInputBytes);
							}
							else {
								// Copy + normalize
								st::Xlat::DualAF32Mono_DualAF32Mono(pcInput1, pcInput2, iInputBytes,
									&pcPreProcessLeft, NULL, &pcPreProcessRight, NULL,
									piAccumOverflows,
									-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
							}
						}
					}
				}
				break;
		}
		
		/* tell the library how much we actually submitted */
		vorbis_analysis_wrote(&vd, iSamples);
	
		/* vorbis does some data preanalysis, then divvies up blocks for
			more involved (potentially parallel) processing.  Get a single
			block for encoding now */
		while(vorbis_analysis_blockout(&vd,&vb)==1){
			
			/* analysis, assume we want to use bitrate management */
			vorbis_analysis(&vb,NULL);
			vorbis_bitrate_addblock(&vb);
			
			while(vorbis_bitrate_flushpacket(&vd,&op)){
				
				/* weld the packet into the bitstream */
				ogg_stream_packetin(&os,&op);
				
				/* write out pages (if any) */
				tbool bEOS = false;
				while(!bEOS){
					int result=ogg_stream_pageout(&os,&og);
					if(result==0) break;
					//fwrite(og.header,1,og.header_len,stdout);
					//fwrite(og.body,1,og.body_len,stdout);
					WriteOutput((char*)og.header, og.header_len);
					WriteOutput((char*)og.body, og.body_len);
					
					/* this could be set above, but for illustrative purposes, I do
						it here (to show that vorbis does know where the stream ends) */
					
					if(ogg_page_eos(&og))bEOS=1;
				}
			}
		}
	}
	
	return !bError;
} // ProcessRaw_Descendant


tbool CVorbisEncoder::Process_Exit_Descendant()
{
	// No special handling needed for end-of-input
	return true;
} // Process_Exit_Descendant


tbool CVorbisEncoder::Finalize_Descendant()
{
	if (miOutputSamplesTotal > 0) {
		// Actually done any processing?
		
		// Tell the library we're at end of stream so that it can handle
		// the last frame and mark end of stream in the output properly
		vorbis_analysis_wrote(&vd,0);
		
		while(vorbis_analysis_blockout(&vd,&vb)==1){
			
			/* analysis, assume we want to use bitrate management */
			vorbis_analysis(&vb,NULL);
			vorbis_bitrate_addblock(&vb);
			
			while(vorbis_bitrate_flushpacket(&vd,&op)){
				
				/* weld the packet into the bitstream */
				ogg_stream_packetin(&os,&op);
				
				/* write out pages (if any) */
				tbool bEOS = false;
				while(!bEOS){
					int result=ogg_stream_pageout(&os,&og);
					if(result==0)break;
					//fwrite(og.header,1,og.header_len,stdout);
					//fwrite(og.body,1,og.body_len,stdout);
					WriteOutput((char*)og.header, og.header_len);
					WriteOutput((char*)og.body, og.body_len);
					
					/* this could be set above, but for illustrative purposes, I do
						it here (to show that vorbis does know where the stream ends) */
					
					if(ogg_page_eos(&og))bEOS=1;
				}
			}
		}

		/* clean up and exit.  vorbis_info_clear() must be called last */
		ogg_stream_clear(&os);
		vorbis_block_clear(&vb);
		vorbis_dsp_clear(&vd);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);
		
		/* ogg_page and ogg_packet structs always point to storage in
			libvorbis.  They're never freed or manipulated directly */
	}

	tfloat fSecs = ((tfloat)miOutputSamplesTotal) / miOutputSampleFreq;
	tfloat fKbps = muiBytesTotalOutput * 8.0f / (fSecs * 1000);
	std::cout << "Done (wrote " << muiBytesTotalOutput << " bytes, " << fSecs << " secs, avg rate " << fKbps << " kbps)\n\n";
	
	return true;
}
