#include "acInternalOS.h"


//#include <ogg/config_types.h>
#include <ogg/os_types.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>


CVorbisDecoder::CVorbisDecoder()
{
	ResetAllParams();

	meAudioCodec = geAudioCodecVorbis;
	mb_ogg_sync_init = false;
	mb_vorbis_info_init = false;
	mb_ogg_sync_pageout = false;
}

CVorbisDecoder::~CVorbisDecoder()
{
	Maybe_Finalize();
}
	
IDecoder* CVorbisDecoder::Create()
{
	return dynamic_cast<IDecoder*>(new CVorbisDecoder());
}

void CVorbisDecoder::Destroy()
{
	delete dynamic_cast<CVorbisDecoder*>(this);
}

tbool CVorbisDecoder::Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader /*= true*/, tint32 iBitWidth /*= 24*/, tint32 iChannels /*= -1*/, IFile* pfOutputWaveStream2 /*= NULL*/)
{
	/********** Decode setup ************/
	
	tbool bOK = (ogg_sync_init(&oy) >= 0);
	if (bOK) {
		mb_ogg_sync_init = true;
		/* Now we can read pages */

		bOK = CDecoder::Init(pfOutputWaveStream1, bRIFFWaveHeader, iBitWidth, iChannels, pfOutputWaveStream2);
	}
		
	if (bOK) {
		// Prepare output buffer for 4K frames of 24 bit stereo + one int for avoiding overflow
		if (!mbSplit) {
			// One buffer is enough (since we're outputting one wave file only)
			PrepareOutputBuffL(4096*3*2);
		}
		else {
			// We need two buffers (because we split the signal into two wave files)
			PrepareOutputBuffL(4096*3*1);
			PrepareOutputBuffR(4096*3*1);
		}
	}
	
	return bOK;
}

tbool CVorbisDecoder::Process(IFile* pfInputOggStream, tint32 iSampleIxToProcess /*= 0*/, tint32 iNbOfSamplesToProcess /*= -1*/, tint32* piSamplesProcessed /*= NULL*/)
{
	if (pfInputOggStream == NULL) {
		AppendErrMsg("Input ogg/vorbis stream is NULL.");
		return false;
	}
	
	char *buffer;
	int  bytes;
	int convsize=4096;
	
	tint32 iSamplesReturnedThisCall = 0;
	if (piSamplesProcessed)
		*piSamplesProcessed = 0;

	//tint32 iPctDisplayed = -1;

	tint32 iOverflowBits = 0;
	//tint32 iOverflow = 0;
	tint64 iOverflowCount = 0;

	tbool bSameFile = false;
	if (!Process_Init(pfInputOggStream, 0, 0, 0, &bSameFile))
		return false;
	if (bSameFile) {
		// Are we trying to rewind?
		tint32 iSamplesBuffered = miSamplesOutputBuffs - miSampleIxOutputBuffs;
		tuint64 uiUsedSampleIx = muiInputFileSampleIx - iSamplesBuffered;
		if (iSampleIxToProcess < uiUsedSampleIx) {
			// No rewinding is possible for ogg/vorbis
			// Pretend it's a new file, so we read from beginning
			bSameFile = false;
		}
		else {
			// Maybe skip some samples
			iSampleIxToProcess -= (tint32)uiUsedSampleIx;
		}
	}
	if (!bSameFile) {
		MaybeCloseOggSync();
		MaybeCloseVorbisInfo();
	}

	tbool bDone = false;
	
	int eos=0;
	//while(1){ /* we repeat if the bitstream is chained */
	//for (tint iChainIx=0; true; iChainIx++) { // Bit stream may be chained
	for (tint iChainIx = 0; !bDone; iChainIx++) { // Bit stream may be chained
		/*int*/ eos=0;
		int i;
		
		// First try for previously buffered samples that hasn't been written to file(s) yet
		tbool bWriteSuccess = Process_MaybeWriteBufferedOutput(&bDone, iNbOfSamplesToProcess, &iSamplesReturnedThisCall);
		if (!bWriteSuccess)
			return false;
		if (bDone)
			return true;

		if (!mb_vorbis_info_init) {
			/* grab some data at the head of the stream.  We want the first page
				(which is guaranteed to be small and only contain the Vorbis
				 stream initial header) We need the first page to get the stream
				serialno. */
			
			/* submit a 4k block to libvorbis' Ogg layer */
			buffer=ogg_sync_buffer(&oy,4096);
			//bytes=fread(buffer,1,4096,stdin);
			bytes = (tint32)pfInputOggStream->Read(buffer,4096);
			//std::cerr << "Read " << bytes << " bytes.\n";
			ogg_sync_wrote(&oy,bytes);
			
			/* Get the first page. */
			if(ogg_sync_pageout(&oy,&og)!=1){
				/* have we simply run out of data?  If so, we're done. */
				if(bytes<4096)break;
				
				/* error case.  Must not be Vorbis data */
				AppendErrMsg("Input does not appear to be an Ogg bitstream.");
				return false;
			}
			
			/* Get the serial number and set up the rest of decode. */
			/* serialno first; use it to set up a logical stream */
			ogg_stream_init(&os,ogg_page_serialno(&og));
			
			/* extract the initial header from the first page and verify that the
				Ogg bitstream is in fact Vorbis data */
			
			/* I handle the initial header first instead of just having the code
				read all three Vorbis headers at once because reading the initial
				header is an easy way to identify a Vorbis bitstream and it's
				useful to see that functionality seperated out. */
			
			vorbis_info_init(&vi);
			mb_vorbis_info_init = true;
			vorbis_comment_init(&vc);
			if(ogg_stream_pagein(&os,&og)<0){ 
				/* error; stream version mismatch perhaps */
				AppendErrMsg("Error reading first page of Ogg bitstream data.");
				return false;
			}
			
			if(ogg_stream_packetout(&os,&op)!=1){ 
				/* no page? must not be vorbis */
				AppendErrMsg("Error reading initial header packet.");
				return false;
			}
			
			if(vorbis_synthesis_headerin(&vi,&vc,&op)<0){ 
				/* error case; not a vorbis header */
				AppendErrMsg("This Ogg bitstream does not contain Vorbis audio data.");
				return false;
			}
			
			/* At this point, we're sure we're Vorbis.  We've set up the logical
				(Ogg) bitstream decoder.  Get the comment and codebook headers and
				set up the Vorbis decoder */
			
			/* The next two packets in order are the comment and codebook headers.
				They're likely large and may span multiple pages.  Thus we reead
				and submit data until we get our two pacakets, watching that no
				pages are missing.  If a page is missing, error out; losing a
				header page is the only place where missing data is fatal. */
			
			i=0;
			while(i<2){
				while(i<2){
					int result=ogg_sync_pageout(&oy,&og);
					if(result==0)break; /* Need more data */
					/* Don't complain about missing or corrupt data yet.  We'll
						catch it at the packet output phase */
					if(result==1){
						ogg_stream_pagein(&os,&og); /* we can ignore any errors here
						as they'll also become apparent
						at packetout */
						while(i<2){
							result=ogg_stream_packetout(&os,&op);
							if(result==0)break;
							if(result<0){
								/* Uh oh; data at some point was corrupted or missing!
								We can't tolerate that in a header.  Die. */
								AppendErrMsg("Corrupt secondary header.  Exiting.");
								return false;
							}
							vorbis_synthesis_headerin(&vi,&vc,&op);
							i++;
						}
					}
				}
				/* no harm in not checking before adding more */
				buffer=ogg_sync_buffer(&oy,4096);
				//bytes=fread(buffer,1,4096,stdin);
				bytes = (tint32)pfInputOggStream->Read(buffer,4096);
				if(bytes==0 && i<2){
					AppendErrMsg("End of file before finding all Vorbis headers!");
					return false;
				}
				ogg_sync_wrote(&oy,bytes);
			}
			
			/* Throw the comments plus a few lines about the bitstream we're
				decoding */
			{
				tbool bIsKobloEncoded = false;
				tint32 iQuality = -1;
				const tchar* kpszEncoderKoblo = "ENCODER=Koblo";
				const tchar* kpszEncoderQuality = "KOBLO_ENC_QUAL=";

				char **ptr=vc.user_comments;
				while(*ptr){
					fprintf(stderr,"%s\n",*ptr);

					// Check for Koblo encoded
					if (strnicmp(*ptr, kpszEncoderKoblo, strlen(kpszEncoderKoblo)) == 0) {
						bIsKobloEncoded = true;
					}
					// Check for compression quality info
					else if (strnicmp(*ptr, kpszEncoderQuality, strlen(kpszEncoderQuality)) == 0) {
						const tchar* pszQuality = *ptr + strlen(kpszEncoderQuality);
						iQuality = IEncoder::GetQualityNb(pszQuality);
					}
					++ptr;
				}
				fprintf(stderr,"\nBitstream is %d channel, %ldHz\n",vi.channels,vi.rate);
				fprintf(stderr,"Encoded by: %s\n\n",vc.vendor);

				// No quality info but koblo encoded?
				if ((bIsKobloEncoded) && (iQuality < 0)) {
					// This is one of our old oggs - they were all "super" encoded
					iQuality = geQualitySuper;
				}

				if ((iQuality < 0) || (iQuality >= geNbOfQualities)) {
					iQuality = geQualityUnknown;
				}
				if (iQuality < meLowestInputQuality) {
					meLowestInputQuality = (EQuality)iQuality;
				}
			}
			
			convsize=4096/vi.channels;
			
			tint32 iChannels = vi.channels;
			tint32 iSampleRate = vi.rate;
			miLastInputSamples = 0;
			miLastInputChannels = iChannels;
			
			tbool bError = false;
			if ((iChannels != 1) && (iChannels != 2)) {
				tchar psz[128];
				sprintf(psz, "Can only read mono or stereo streams, not %d channels", iChannels);
				AppendErrMsg(psz);
				bError = true;
			}

			if ((mbSplit) && (iChannels < 2)) {
				AppendErrMsg("Can't split a mono ogg/vorbis file into two wave streams");
				bError = true;
			}
		
			if ((miOutputSampleFreq > -1) && (miOutputSampleFreq != iSampleRate)) {
				tchar psz[128];
				sprintf(psz, "Sample freq of new ogg/vorbis (%d Hz) differs from first ogg/vorbis freq (%d Hz)", iSampleRate, miOutputSampleFreq);
				AppendErrMsg(psz);
				bError = true;
			}
			else if ((iSampleRate != 44100) && (iSampleRate != 48000) && (iSampleRate != 88200) && (iSampleRate != 96000)) {
				tchar psz[128];
				sprintf(psz, "Sample freq of ogg/vorbis (%d Hz) can't be used", iSampleRate);
				AppendErrMsg(psz);
				bError = true;
			}
			else {
				miOutputSampleFreq = iSampleRate;
#ifndef AC_IDECODER_NOT_BACK_COMPAT
				miSampleFreq = iSampleRate;
#endif // AC_IDECODER_NOT_BACK_COMPAT
			}

			if (bError)
				return false;
		
			if (miOutputChannels == -1) {
				miOutputChannels = iChannels;
			}
			miOutputBlockAlign = (miOutputBitWidth / 8) * miOutputChannels;
		
			miLastInputChannels = iChannels;
			
			/* OK, got and parsed all three headers. Initialize the Vorbis
				packet->PCM decoder. */
			vorbis_synthesis_init(&vd,&vi); /* central decode state */
			vorbis_block_init(&vd,&vb);     /* local state for most of the decode
				so multiple block decodes can
				proceed in parallel.  We could init
				multiple vorbis_block structures
				for vd here */
			
			// First time here? Write the RIFF/Wave header(s)
			if ((mbRIFFWaveHeader) && (mpfOutWaveStream1->GetCurrentFilePosition() == 0)) {
				if (!mbSplit) {
					// A single output wave file receives both channels
					OutDummyPCMHeader(mpfOutWaveStream1, miOutputBitWidth, miOutputChannels, miOutputSampleFreq);
				}
				else {
					// Must split data into two mono wave files
					OutDummyPCMHeader(mpfOutWaveStream1, miOutputBitWidth, 1, miOutputSampleFreq);
					OutDummyPCMHeader(mpfOutWaveStream2, miOutputBitWidth, 1, miOutputSampleFreq);
				}
			}
		}
			
		/* The rest is just a straight decode loop until end of stream */
		tbool bLoop_InPage = !eos;
		//while(!eos){
		while (bLoop_InPage) {
			tbool bLoop_ThruPart = true;
			//while(!eos){
			while (bLoop_ThruPart) {
				int result;
				tbool bLoop_Convert = false;
				if (mb_ogg_sync_pageout) {
					bLoop_Convert = true;
				}
				else {
					result=ogg_sync_pageout(&oy,&og);
					if(result==0) {
						bLoop_ThruPart = false;
						mb_ogg_sync_pageout = false;
						//break; /* need more data */
					}
					else if(result<0){ /* missing or corrupt data at this page position */
						AppendWarnMsg("Corrupt or missing data in bitstream; continuing...");
						mb_ogg_sync_pageout = false;
					}else{
						mb_ogg_sync_pageout = true;
						bLoop_Convert = true;

						ogg_stream_pagein(&os,&og); /* can safely ignore errors at
						this point */
					}
				}
				//while(1){
				//while(!eos){
				while (bLoop_Convert) {
					result=ogg_stream_packetout(&os,&op);
					
					if(result==0) {
						//bLoop_ThruPart = false;
						bLoop_Convert = false;
						mb_ogg_sync_pageout = false;
						//break; /* need more data */
					}
					else if(result<0){ /* missing or corrupt data at this page position */
						/* no reason to complain; already complained above */
					}else{
						/* we have a packet.  Decode it */
						//float **pcm;
						float** ppfSrc;
						int samples;
						
						if(vorbis_synthesis(&vb,&op)==0) /* test for success! */
							vorbis_synthesis_blockin(&vd,&vb);
						/* 
							
						 **pcm is a multichannel float vector.  In stereo, for
						 example, pcm[0] is left, and pcm[1] is right.  samples is
						 the size of each channel.  Convert the float values
						 (-1.<=range<=1.) to whatever PCM format and write it out */
						
						//while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0){
						//while ((samples = vorbis_synthesis_pcmout(&vd, &ppfSrc)) > 0) {
						//while ((!eos) && ((samples = vorbis_synthesis_pcmout(&vd, &ppfSrc)) > 0)) {
						while ((bLoop_Convert) && ((samples = vorbis_synthesis_pcmout(&vd, &ppfSrc)) > 0)) {
							//int j;
							//int clipflag=0;
							//tbool bClip = false;
							//int bout=(samples<convsize?samples:convsize);
							tint32 iSamplesConsumed = (samples<convsize?samples:convsize);
							muiInputFileSampleIx += iSamplesConsumed;
							tint32 iSamplesToProcess = 0;
							if (iSampleIxToProcess >= iSamplesConsumed) {
								// We must skip this entire range
								iSampleIxToProcess -= iSamplesConsumed;
								std::cout << "Skip range\n";
							}
							else {
								iSamplesToProcess = iSamplesConsumed - iSampleIxToProcess;
								/*
								if (iNbOfSamplesToProcess > -1) {
									tint64 iNewTotal = miLastInputSamples + iSamplesToProcess;
									if (iNewTotal > iNbOfSamplesToProcess) {
										// Limit number of samples to meet the amount we wanted
										iSamplesToProcess = iNbOfSamplesToProcess - miLastInputSamples;
									}
								}
								*/
								//std::cout << "Samples comsumed: " << iSamplesConsumed << " Samples to process: " << iSamplesToProcess << "\n";
								mpcBufferToWriteNowL = NULL;
								mpcBufferToWriteNowR = NULL;
								miSampleIxOutputBuffs = 0;
								miSamplesOutputBuffs = iSamplesToProcess;
								//tint32 iBytesOutputBuffs = miSamplesOutputBuffs * miOutputBlockAlign;
								tint32 iOutputBytes_Dummy;
								tint32 iInputBytes = iSamplesToProcess * sizeof(tfloat32);
									
								if (miOutputBitWidth == 16) {
									// output as 16 bit
									if (miLastInputChannels == 1) {
										// input is mono
										float* pfSrc = ppfSrc[0] + iSampleIxToProcess;
										if (miOutputChannels == 1) {
											// output 16 bit mono
											st::Xlat::AF32Mono_PCM16Mono(
												(tchar*)pfSrc, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
										else if (mbSplit) {
											// output 16 bit outmix dual mono
											st::Xlat::AF32Mono_PCM16Mono(
												(tchar*)pfSrc, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
											mpcBufferToWriteNowR = mpcOutputBuffL;
										}
										else {
											// output 16 bit outmix stereo
											st::Xlat::AF32Mono_PCM16Stereo(
												(tchar*)pfSrc, iInputBytes, 
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
									}
									else {
										// input is stereo
										float* pfSrcL = ppfSrc[0] + iSampleIxToProcess;
										float* pfSrcR = ppfSrc[1] + iSampleIxToProcess;
										//char* pcDest = mpcOutputBuffL;
										if (mbSplit) {
											// output 16 bit dual mono
											st::Xlat::DualAF32Mono_DualPCM16Mono(
												(tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL, &mpcOutputBuffR, &miSizeOutputBuffR,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
											mpcBufferToWriteNowR = mpcOutputBuffR;
										}
										else if (miOutputChannels == 1) {
											// output 16 bit downmix mono
											st::Xlat::DualAF32Mono_PCM16Mono(
												(tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
										else {
											// output 16 bit stereo
											st::Xlat::DualAF32Mono_PCM16Stereo(
												(tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
									}
								}
								else {
									// output as 24 bits
									if (miLastInputChannels == 1) {
										// input is mono
										float* pfSrc = ppfSrc[0] + iSampleIxToProcess;
										if (miOutputChannels == 1) {
											// output 24 bit mono
											st::Xlat::AF32Mono_PCM24Mono(
												(tchar*)pfSrc, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
										else if (mbSplit) {
											// Not implemented
										}
										else {
											// output 24 bit upmix stereo
											st::Xlat::AF32Mono_PCM24Stereo((tchar*)pfSrc, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
									}
									else {
										// input is stereo
										float* pfSrcL = ppfSrc[0] + iSampleIxToProcess;
										float* pfSrcR = ppfSrc[1] + iSampleIxToProcess;
										if (mbSplit) {
											// Split stereo ogg/vorbis into 2 mono 24 bit wave files
											st::Xlat::DualAF32Mono_DualPCM24Mono(
												(tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL, &mpcOutputBuffR, &miSizeOutputBuffR,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
											mpcBufferToWriteNowR = mpcOutputBuffR;
										}
										else if (miOutputChannels == 1) {
											// output 24 bits downmix mono
											st::Xlat::DualAF32Mono_PCM24Mono(
												(tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
										else {
											// output 24 bits stereo
											st::Xlat::DualAF32Mono_PCM24Stereo((tchar*)pfSrcL, (tchar*)pfSrcR, iInputBytes, &iOutputBytes_Dummy,
												&mpcOutputBuffL, &miSizeOutputBuffL,
												&iOverflowCount);
											mpcBufferToWriteNowL = mpcOutputBuffL;
										}
									}
								}
							}

							/*
							tint32 iBytesToWrite = (miOutputBitWidth / 8) * miOutputChannels * iSamplesToProcess;
							if (!mbAllowNoOutputFile) {
								if (!mbSplit) {
									// Write to one (stereo or mono) wave file
									mpfOutWaveStream1->Write((const char*)mpcOutputBuffL, iBytesToWrite);
								}
								else {
									// Write to two (mono) wave files
									mpfOutWaveStream1->Write((const char*)mpcOutputBuffL, iBytesToWrite);
									mpfOutWaveStream2->Write((const char*)mpcOutputBuffR, iBytesToWrite);
								}
							}
							*/
							
							vorbis_synthesis_read(&vd, iSamplesConsumed); /* tell libvorbis how
								many samples we
								actually consumed */
							//miLastInputSamples += iSamplesToProcess;
							//miSamplesTotal += iSamplesToProcess;
							//iSamplesReturnedThisCall += iSamplesToProcess;
							tbool bWriteSuccess = Process_MaybeWriteBufferedOutput(&bDone, iNbOfSamplesToProcess, &iSamplesReturnedThisCall);
							if (!bWriteSuccess)
								return false;

							// Lasse, allow for break if we have all we need
							if (bDone) {
								//bDone = true;
								//eos = 1;
								bLoop_InPage = false;
								bLoop_ThruPart = false;
								bLoop_Convert = false;
							}
							//if (eos) bLoop = false;
						}	    
					}
				}
				//if(ogg_page_eos(&og))eos=1;
				if (ogg_page_eos(&og)) {
					eos=1;
					mb_ogg_sync_pageout = false;
					//bLoop_InPage = false;
					bLoop_ThruPart = false;
				}
			}
			
			tint64 iPct = (pfInputOggStream->GetCurrentFilePosition() * 100);
			tint64 iSize = pfInputOggStream->GetSizeWhenOpened();
			if (iSize > 0)
				iPct /= iSize;
			if (iPct != miPctDisplayed) {
				tchar psz[32];
				sprintf(psz, "\r%3d%% ", iPct);
				std::cerr << psz;
				miPctDisplayed = (tint32)iPct;
			}
			
			//if(!eos){
			if (bLoop_InPage) {
				buffer=ogg_sync_buffer(&oy,4096);
				//bytes=fread(buffer,1,4096,stdin);
				bytes = (tint32)pfInputOggStream->Read(buffer,4096);
				if (bytes < 0) {
					// Read error
					AppendErrMsg("Error reading file");
					return false;
				}
				muiInputFilePos = pfInputOggStream->GetCurrentFilePosition();
				ogg_sync_wrote(&oy,bytes);
				//if(bytes==0)eos=1;
				if(bytes==0) {
					eos=1;
					bDone = true;
					bLoop_InPage = false;
				}
			}
		}

		if (false) { //eos) {
			// We're forced to close stream, since it's empty anyway

			/* clean up this logical bitstream; before exit we see if we're
				followed by another [chained] */
			//ogg_stream_clear(&os);
			MaybeCloseOggSync();

			/* ogg_page and ogg_packet structs always point to storage in
				libvorbis.  They're never freed or manipulated directly */
			//vorbis_block_clear(&vb);
			//vorbis_dsp_clear(&vd);
			//vorbis_comment_clear(&vc);
			//vorbis_info_clear(&vi);  /* must be called last */
			MaybeCloseVorbisInfo();
		}
	}

	std::cerr << " = " << miLastInputSamples << " samples.\n\n";
	if (iOverflowCount) {
		tchar pszOverflowMsg[512];
		tint32 iBits = 0;
		tint32 iBit = 0x00000001;
		for (tint32 i = 1; i <= 31; i++) {
			if (iBit & iOverflowBits) {
				iBits = i;
			}
			iBit <<= 1;
		}
		tfloat fOverflowPct = (miLastInputSamples <= 0) ? 0.0f : 100.0f * iOverflowCount / miLastInputSamples;
		sprintf(pszOverflowMsg,
				"\n  Overflow / clipping occurred in %d frames (= %0.2f%%).\n  At most %d bit(s) were ducked (bit-flags 0x%08X).",
				iOverflowCount, fOverflowPct, iBits, iOverflowBits
		);
		AppendWarnMsg(pszOverflowMsg);
	}

	// Did we succeed?
	if (piSamplesProcessed)
		*piSamplesProcessed = iSamplesReturnedThisCall;
	return true;
} // Process


tbool CVorbisDecoder::Finalize_CodecSpecific()
{
	MaybeCloseOggSync();
	MaybeCloseVorbisInfo();

	if (mb_ogg_sync_init) {
		/* OK, clean up the framer */
		ogg_sync_clear(&oy);
		
		mb_ogg_sync_init = false;
	}

	return true;
} // Finalize_CodecSpecific


void CVorbisDecoder::MaybeCloseVorbisInfo()
{
	if (mb_vorbis_info_init) {
		mb_vorbis_info_init = false;

		/* ogg_page and ogg_packet structs always point to storage in
			libvorbis.  They're never freed or manipulated directly */
		vorbis_block_clear(&vb);
		vorbis_dsp_clear(&vd);
		vorbis_comment_clear(&vc);
		vorbis_info_clear(&vi);  /* must be called last */
	}
} // MaybeCloseVorbisInfo


void CVorbisDecoder::MaybeCloseOggSync()
{
	if (mb_ogg_sync_pageout) {
		mb_ogg_sync_pageout = false;

		/* clean up this logical bitstream; before exit we see if we're
			followed by another [chained] */
		ogg_stream_clear(&os);
	}
} // MaybeCloseOggSync
