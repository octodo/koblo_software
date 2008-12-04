
//! brief: class CLameEncoder
// Code cut partially from libvorbis / vorbisenc example
// Created by Lasse Steen Bohnstedt, 2007-10-xx

// Heavily edited for lame-mp3 by Lasse, 2008-03-xx

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

#include "lame.h"


CLameEncoder::CLameEncoder()
{
	miLowestPossibleQualityNb = geQualityPoor;
	miHighestPossibleQualityNb = geQualityExtreme;
	miDefaultQualityNb = geQualityGood;
	mpszEncoderName = "Lame Encoder";
	meAudioCodec = geAudioCodecLame;
	//mpszDefaultExt = ".mp3";
	//mpszFileTypeDesc = "MP3 Files (*.mp3)";

	mbFixedBitRate = false;
	mpLameGlobalFlags = NULL;
}


CLameEncoder::~CLameEncoder()
{
	if (mpfOutputStream) {
		Finalize();
	}
}

IEncoder* CLameEncoder::Create()
{
	return dynamic_cast<IEncoder*>(new CLameEncoder());
}

void CLameEncoder::Destroy()
{
	delete dynamic_cast<CLameEncoder*>(this);
}


tbool CLameEncoder::GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo)
{
	tint32 iMono = 0;
	tint32 iStereo = 0;

	switch (iQualityNb) {
		case geQualityPoor:		iMono = 40;		iStereo = 69;	break;
		case geQualityLow:		iMono = 64;		iStereo = 109;	break;
		case geQualityMedium:	iMono = 76;		iStereo = 134;	break;
		case geQualityGood:		iMono = 86;		iStereo = 168;	break;
		case geQualityBetter:	iMono = 92;		iStereo = 183;	break;
		case geQualityHigh:		iMono = 136;	iStereo = 248;	break;
		case geQualitySuper:	iMono = 153;	iStereo = 258;	break;
		case geQualityExtreme:	iMono = 160;	iStereo = 320;	break;
	}

	if (piKBPS_Mono)
		*piKBPS_Mono = iMono;
	if (piKBPS_Stereo)
		*piKBPS_Stereo = iStereo;

	return ((iMono > 0) && (iStereo > 0));
}


tbool CLameEncoder::Init_Descendant(EQuality eQuality)
{
	if (pLameInstance) {
		// Here comes trouble! There can be only one

		if (pLameInstance != (void*)this) {
			AppendErrMsg(
				"Internal error: There may exist only one mp3 engine at at time,\n"
				"since it's mot thread safe."
				);
			return false;
		}
	}
	pLameInstance = (void*)this;

	// Loop quality
	tint32 aiLoopQuality[geNbOfQualities]	= { -1, 7, 5, 3, 2, 1, 0, 0,  0, -1, -1, -1 };
	miLoopQuality = aiLoopQuality[eQuality];
	// Vbr quality
	tint32 aiVbrQuality[geNbOfQualities]	= { -1, 9, 7, 5, 4, 3, 2, 0, -1, -1, -1, -1 };
	miVbrQuality = aiVbrQuality[eQuality];
	// Fixed rate?
	mbFixedBitRate = (miVbrQuality < 0);
	// Explain quality
	tchar psz[128];
	if (mbFixedBitRate)
		sprintf(psz, "Encoding with %s quality (320 kbps stereo or 160 kbps mono, loopQ=%d)\n", GetQualityName(eQuality), miLoopQuality);
	else
		sprintf(psz, "Encoding with %s quality (VbrQ=%d, loopQ=%d)\n", GetQualityName(eQuality), miVbrQuality, miLoopQuality);
	std::cout << psz;
	
	return true;
} // Init_Descendant


tbool CLameEncoder::Process_Init_Descendant()
{
	// Setup encoder
	std::cout << "Init encoder start...\n";
	
	//vorbis_info_init(&vi);
	mpLameGlobalFlags = lame_init();
	if (mpLameGlobalFlags == NULL) {
		AppendErrMsg("Function lame_init failed for unknown reason.");
		return false;
	}

	// Hook into lame's error message queue
	//lame_set_errorf(mpLameGlobalFlags, ((void(*)(const char*,char*))(&k2s::ac::CLameEncoder::my_errorf_hook)));
	//lame_set_debugf(mpLameGlobalFlags, this->my_warningf_hook);
	
	//if (vorbis_encode_init_vbr(&vi, miChannels, iSampleRate, mfQuality) != 0) {
	//	AppendErrMsg("Function vorbis_encode_init_vbr failed for unknown reason.");
	//	return false;
	//}
	tint iRC;
	iRC = lame_set_in_samplerate(mpLameGlobalFlags, miInputSampleFreq);
	if (iRC < 0) {
		AppendErrMsg("Function lame_set_in_samplerate failed for unknown reason.");
		return false;
	}
	
	// --already default on: lame_set_bWriteVbrTag(mpLameGlobalFlags, 1);
	//if (iRC < 0) {
	//	AppendErrMsg("Function lame_set_bWriteVbrTag failed for unknown reason.");
	//	return false;
	//}

	iRC = lame_set_quality(mpLameGlobalFlags, miLoopQuality);
	if (iRC < 0) {
		AppendErrMsg("Function lame_set_quality failed for unknown reason.");
		return false;
	}
	
	bool bHiQ = (miLoopQuality <= 2);
	MPEG_mode mpegMode = (miOutputChannels == 1) ? MONO : (mbFixedBitRate ? STEREO : JOINT_STEREO);
	
	//lame_set_mode(mpLameGlobalFlags, (miOutputChannels ==2) ? (bHiQ ? STEREO : JOINT_STEREO) : MONO);
	lame_set_mode(mpLameGlobalFlags, mpegMode);
	if (iRC < 0) {
		AppendErrMsg("Function lame_set_mode failed for unknown reason.");
		return false;
	}
	
	iRC = lame_set_VBR(mpLameGlobalFlags, mbFixedBitRate ? vbr_off : vbr_default);
	if (iRC < 0) {
		AppendErrMsg("Function lame_set_VBR failed for unknown reason.");
		return false;
	}

	if (mbFixedBitRate) {
		iRC = lame_set_brate(mpLameGlobalFlags, (miOutputChannels == 1) ? 160 : 320);
		if (iRC < 0) {
			AppendErrMsg("Function lame_set_brate failed for unknown reason.");
			return false;
		}
	}
	else {
		iRC = lame_set_VBR_q(mpLameGlobalFlags, miVbrQuality);
		if (iRC < 0) {
			AppendErrMsg("Function lame_set_VBR_q failed for unknown reason.");
			return false;
		}
		
		if (bHiQ) {
			iRC = lame_set_VBR_min_bitrate_kbps(mpLameGlobalFlags, 32);
			if (iRC < 0) {
				AppendErrMsg("Function lame_set_VBR_min_bitrate_kbps failed for unknown reason.");
				return false;
			}
			iRC = lame_set_VBR_max_bitrate_kbps(mpLameGlobalFlags, 320);
			if (iRC < 0) {
				AppendErrMsg("Function lame_set_VBR_max_bitrate_kbps failed for unknown reason.");
				return false;
			}
			if (miOutputChannels > 1) {
				iRC = lame_set_allow_diff_short(mpLameGlobalFlags, 1);
				if (iRC < 0) {
					AppendErrMsg("Function lame_set_allow_diff_short failed for unknown reason.");
					return false;
				}
			}
		}
	}
	
	iRC = lame_init_params(mpLameGlobalFlags);
	if (iRC < 0) {
		AppendErrMsg("Function lame_init_params failed for unknown reason.");
		return false;
	}
	
	// Set number of channels in mp3
	iRC = lame_set_num_channels(mpLameGlobalFlags, miOutputChannels);
	if (iRC < 0) {
		AppendErrMsg("Function lame_set_num_channels failed for unknown reason.");
		return false;
	}

	// Debug info
	lame_print_config(mpLameGlobalFlags);

	// Prepare buffer for mp3 output
	PrepareOutputBuff(LAME_MAXMP3BUFFER);

	// TODO: add comments
	//vorbis_comment_init(&vc);
	//vorbis_comment_add_tag(&vc,"ENCODER","Koblo 1.0 // aoTuV b5");

	std::cout << "Init OK.\n";

	return true;
} // Process_Init_Descendant


tbool CLameEncoder::ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 iSamples, tint64* piAccumOverflows)
{
	tint32 iOutputBytes = 0;

	if (miInputBitWidth == 16) {

#ifdef _Mac_PowerPC
		// 16 bit PCM

		// Motorola CPU (big-endian)
		// Since WAV files are little-endian (it's a Microsoft standard) we need to flip around the
		// bytes in every sample before submitting to the LAME engine. While we're at it, we may as
		// well convert it all to floats, since this saves a little CPU usage inside the LAME engine.

		if (miInputChannels == 1) {
			// Mono in - mono/stereo out will be handled implicitly by LAME engine
			tint32 iPreProcessBytes = 0;
			st::Xlat::PCM16Mono_AF32Mono(
				pcInput1, iInputBytes, &iPreProcessBytes,
				&mpcPreProcessBuff1, &miPreProcessBuff1Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)mpcPreProcessBuff1, (tfloat32*)mpcPreProcessBuff1, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else if (mbIsInputInterleavedStereo) {
			// Interleaved stereo in - mono/stereo out will be handled implicitly by LAME engine
			st::Xlat::PCM16Stereo_DualAF32Mono_PartialInPlace(
				pcInput1, iInputBytes,
				&mpcPreProcessBuff2, &miPreProcessBuff2Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)pcInput1, (tfloat32*)mpcPreProcessBuff2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else {
			// Dual mono in - mono/stereo out will be handled implicitly by LAME engine
			tint32 iPreProcessBytes = 0;
			st::Xlat::DualPCM16Mono_DualAF32Mono(
				pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
				&mpcPreProcessBuff1, &miPreProcessBuff1Size, &mpcPreProcessBuff2, &miPreProcessBuff2Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)mpcPreProcessBuff1, (tfloat32*)mpcPreProcessBuff2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}

#else _Mac_PowerPC
		// 16 bit PCM

		// Intel CPU (little-endian):
		// LAME mp3 is very easy to use for Intel CPU when using 16 bit streams.
		// There's no need to uninterleave (and thus copy) data, the LAME engine does that implicitly.

		if (miInputChannels == 1) {
			// Mono in - mono/stereo out will be handled implicitly by LAME engine
			iOutputBytes = lame_encode_buffer(
				mpLameGlobalFlags,
				(tint16*)pcInput1, (tint16*)pcInput1, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else if (mbIsInputInterleavedStereo) {
			// Interleaved stereo in - mono/stereo out will be handled implicitly by LAME engine
			iOutputBytes = lame_encode_buffer_interleaved(
				mpLameGlobalFlags,
				(tint16*)pcInput1, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else {
			// Dual mono in - mono/stereo out will be handled implicitly by LAME engine
			iOutputBytes = lame_encode_buffer(
				mpLameGlobalFlags,
				(tint16*)pcInput1, (tint16*)pcInput2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
#endif // #else _Mac_PowerPC

	}

	else if (miInputBitWidth == 24) {
		// 24 bit PCM

		// LAME engine can't handle 24 bit PCM, so we must convert it to 32 bit floats
		// This will implicitly convert from little-endian format if that's necessary (Motorola CPUs)

		if (miInputChannels == 1) {
			// mono in - mono/stereo out will be handled implicitly by LAME engine
			tint32 iPreProcessBytes = 0;
			st::Xlat::PCM24Mono_AF32Mono(
				pcInput1, iInputBytes, &iPreProcessBytes,
				&mpcPreProcessBuff1, &miPreProcessBuff1Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)mpcPreProcessBuff1, (tfloat32*)mpcPreProcessBuff1, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else if (mbIsInputInterleavedStereo) {
			// Interleaved stereo in - mono/stereo out will be handled implicitly by LAME engine
			tint32 iPreProcessBytes = 0;
			st::Xlat::PCM24Stereo_DualAF32Mono_PartialInPlace(
				pcInput1, iInputBytes, &iPreProcessBytes,
				&mpcPreProcessBuff2, &miPreProcessBuff2Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)pcInput1, (tfloat32*)mpcPreProcessBuff2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else {
			// Dual mono in - mono/stereo out will be handled implicitly by LAME engine
			tint32 iPreProcessBytes = 0;
			st::Xlat::DualPCM24Mono_DualAF32Mono(
				pcInput1, pcInput2, iInputBytes, &iPreProcessBytes,
				&mpcPreProcessBuff1, &miPreProcessBuff1Size, &mpcPreProcessBuff2, &miPreProcessBuff2Size,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)mpcPreProcessBuff1, (tfloat32*)mpcPreProcessBuff2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
	}
	
	else if (miInputBitWidth == 32) {
		// 32 bit float array

		// LAME engine accepts 32 bit float arrays directly - sort of...

		// But unfortunately we still have convert to 16 bit int range (demented!)
		// Un the upside this spares the LAME engine of spikes > 1.0 and < -1.0
		// that can confuse it (and even make it crash)
		if (miInputChannels == 1) {
			// Mono
			
			// Because of wrong data range we can't do this:
			//iOutputBytes = lame_encode_buffer_float(
			//	mpLameGlobalFlags,
			//	(tfloat32*)pcInput1, (tfloat32*)pcInput1, iSamples,
			//	(unsigned char*)mpcOutputBuff, miOutputBuffSize);
			
			// Instead we do this:
			st::Xlat::AF32Mono_AF32Mono_InPlace(
				pcInput1, iInputBytes,
				piAccumOverflows,
				-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)pcInput1, (tfloat32*)pcInput1, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
		else {
			// Dual mono

			// Because of wrong data range we can't do this:
			//iOutputBytes = lame_encode_buffer_float(
			//	mpLameGlobalFlags,
			//	(tfloat32*)pcInput1, (tfloat32*)pcInput2, iSamples,
			//	(unsigned char*)mpcOutputBuff, miOutputBuffSize);

			// Instead we do this:
			st::Xlat::DualAF32Mono_DualAF32Mono_InPlace(
				pcInput1, pcInput2, iInputBytes,
				piAccumOverflows,
				-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse,
				-32768.0f, 32767.0f);
			iOutputBytes = lame_encode_buffer_float(
				mpLameGlobalFlags,
				(tfloat32*)pcInput1, (tfloat32*)pcInput2, iSamples,
				(unsigned char*)mpcOutputBuff, miOutputBuffSize);
		}
	}

	if (iOutputBytes > 0)
		return WriteOutput(mpcOutputBuff, iOutputBytes);

	if (iOutputBytes < 0) {
		tchar pszErr[1024];
		sprintf(pszErr, 
			"CLameEncoder::ProcessRaw_Descendant(..):\n"
			"LAME engine reported error %d encoding portion\n"
			"(sample range %d to %d)",
			iOutputBytes,
			(tint32)miOutputSamplesTotal, (tint32)(miOutputSamplesTotal + iSamples)
			);
		AppendErrMsg(pszErr);
		return false;
	}

	return true;
} // ProcessRaw_Descendant


tbool CLameEncoder::Process_Exit_Descendant()
{
	// No special handling needed for end-of-input
	return true;
} // Process_Exit_Descendant


tbool CLameEncoder::Finalize_Descendant()
{
	// Release hook on LAME engine
	if (pLameInstance == (void*)this) {
		pLameInstance = NULL;
	}

	if (mpLameGlobalFlags == NULL) {
		// Never opened engine
		return true;
	}

	// "no_gap" isn't good - doesn't write ID3v1 tags and violates the official mp3 specs:
	//tint32 iOutputBytes = lame_encode_flush_nogap(

	tint32 iOutputBytes = lame_encode_flush(
		mpLameGlobalFlags,
		(unsigned char*)mpcOutputBuff, miOutputBuffSize);
	
	tbool bError = (iOutputBytes < 0);
	if (bError) {
		AppendErrMsg("lame_encode_flush(..) failed for unknown reason");
	}
	else if (!WriteOutput(mpcOutputBuff, iOutputBytes)) {
		bError = true;
	}
	else {
		// We must close and re-open the file for VBR tag (and id3 tags) to get written...
		// (That's disgusting! It was Xing that implemented tags first, so they're to blame!)
		tchar pszMp3FilePath[1024];
		mpfOutputStream->GetPathName(pszMp3FilePath);
		std::cout << "Closing file " << pszMp3FilePath << " ...\n";
		tint64 iFilePosSave = mpfOutputStream->GetCurrentFilePosition();
		mpfOutputStream->Close();
		std::cout << "Delaying a little to allow for OS to actually close file (HACK!)\n";
		ITime::SleepMS(10);
		std::cout << "Applying VBR / id3 tag(s)\n";
		tchar pszMp3FilePath_OS[1024];
		tbool bConverted = IFile::PathToOS2(pszMp3FilePath, pszMp3FilePath_OS);
		FILE* pfile = (bConverted) ? fopen(pszMp3FilePath_OS, "r+") : NULL;
		if (pfile == NULL) {
			AppendErrMsg("Error applying Xing VBR/ID3 tags for mp3 - file open failed");
			bError = true;
		}
		else {
			// Look and behold: There's no return code for success or error!
			// Now does that suck big time or what?!!?
			/*tint32 iRC = */lame_mp3_tags_fid(mpLameGlobalFlags, pfile);
			//if (iRC < 0) {
			//	AppendWarnMsg("Couldn't write VBR / id3 tags");
			//}
			fclose(pfile);
		}

		// Reopen file for calling program to see
		std::cout << "(Re-opening file so calling app won't crash.)\n";
		mpfOutputStream->Open(pszMp3FilePath, IFile::FileWrite);
		mpfOutputStream->Seek(iFilePosSave);
			
		lame_close(mpLameGlobalFlags);
		mpLameGlobalFlags = NULL;
		
		tfloat fSecs = ((tfloat)miOutputSamplesTotal) / miOutputSampleFreq;
		tfloat fKbps = muiBytesTotalOutput * (float)8.0 / (fSecs * 1000);
		std::cout << "Done (wrote " << muiBytesTotalOutput << " bytes, " << fSecs << " secs, avg rate " << fKbps << " kbps)\n\n";
	}

	return !bError;
}
	

void CLameEncoder::my_warningf_hook(const char *format, va_list ap)
{
	tchar pszWarn[1024];
	sprintf(pszWarn, format, ap);
	AppendWarnMsg(pszWarn);
	//    (void) vfprintf(stdout, format, ap);
}

void CLameEncoder::my_errorf_hook(const char *format, va_list ap)
{
	tchar pszErr[1024];
	sprintf(pszErr, format, ap);
	AppendErrMsg(pszErr);
	//    (void) vfprintf(stdout, format, ap);
}

