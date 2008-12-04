#include "acInternalOS.h"


#include <lame.h>


CLameDecoder::CLameDecoder()
{
	ResetAllParams();
	meAudioCodec = geAudioCodecLame;

	mb_lame_decode_init = false;
} // constructor


CLameDecoder::~CLameDecoder()
{
	Maybe_Finalize();
} // destructor

	
IDecoder* CLameDecoder::Create()
{
	return dynamic_cast<IDecoder*>(new CLameDecoder());
} // Create


void CLameDecoder::Destroy()
{
	delete dynamic_cast<CLameDecoder*>(this);
} // Destroy


tbool CLameDecoder::Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader /*= true*/, tint32 iBitWidth /*= 24*/, tint32 iChannels /*= -1*/, IFile* pfOutputWaveStream2 /*= NULL*/)
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

	/********** Decode setup ************/
	
	tint32 iRC;
	iRC = lame_decode_init();
	if (iRC < 0) {
		AppendErrMsg("lame_decode_init failed for unknown reason");
		return false;
	}
	mb_lame_decode_init = true;
	
	tbool bOK = CDecoder::Init(pfOutputWaveStream1, bRIFFWaveHeader, iBitWidth, iChannels, pfOutputWaveStream2);

	return bOK;
} // Init


tbool CLameDecoder::Process(IFile* pfInputMp3Stream, tint32 iSampleIxToProcess /*= 0*/, tint32 iNbOfSamplesToProcess /*= -1*/, tint32* piSamplesProcessed /*= NULL*/)
{
	const tint32 kiInBuffSize = 512;//2048;

	// Unfortunately the LAME engine doesn't allow us to tell it how large the
	// receiving buffer is.
	//
	// We must be prepared for "worst-case" expansion of data.
	// But you can't simply compare input bit-rate to expected output bit-rate, because
	// sometimes a large number of input chunks are buffered before any intermediate
	// results get output.
	// Empirical tests (with 2048 byte input chunks) showed that everything below 1/2 
	// second had a risk of crashing.
	// To be on the safe side we allocate intermediate buffer for 2 secs.
	
	tint32 iMaxFreq = 96000;

	// Intermediate output from LAME engine is always 16 bit
	tint32 iIntermediateBuff_BytesPerSample = 16 / 8;	
	tint32 iIntermediateBuff_BytesTwoSecs = 2 * iMaxFreq * iIntermediateBuff_BytesPerSample;
	// Unfortunately Lame needs both channel buffers for mono too... no idea why.
	tbool bSameFile_Dummy = false;
	if (!Process_Init(pfInputMp3Stream, iIntermediateBuff_BytesTwoSecs, iIntermediateBuff_BytesTwoSecs, iIntermediateBuff_BytesTwoSecs, &bSameFile_Dummy))
		return false;

	// Info about stream (init some to -1 to avoid misunderstanding)
	mp3data_struct mp3headers;
	mp3headers.nsamp = -1;
	mp3headers.totalframes = -1;
	
	//tint32 iSamplesDecodedCurrInput = 0;
	//tint32 iPctDisplayed = -1;

	tint32 iSamplesReturnedThisCall = 0;
	if (piSamplesProcessed)
		*piSamplesProcessed = 0;

	pfInputMp3Stream->Seek(muiInputFilePos);
	tbool bFirstDecode = (muiInputFileSampleIx == 0);
	if (bFirstDecode) {
		miLastInputSamples = 0;
	}
	tbool bSuccess = false;
	tbool bExit = false;
	while (!bExit) {

		// First try for previously buffered samples that hasn't been written to file(s) yet
		bSuccess = Process_MaybeWriteBufferedOutput(&bExit, iNbOfSamplesToProcess, &iSamplesReturnedThisCall);

		if ((bSuccess) && (!bExit)) {
			tint64 iRead = pfInputMp3Stream->Read(mpcInputBuff, kiInBuffSize);
			
			if (iRead < 0) {
				AppendErrMsg("input mp3 error");
				lame_decode_exit();
				return false;
			}
			if (iRead == 0) {
				// All of mp3 was read
				
				// Do output file clean-up
				// ...
				
				// Exit with success
				bSuccess = true;
				bExit = true;
				//return true;
			}
			else {
				// Something was decoded into intermediate buffers

				muiInputFilePos += iRead;

				//std::cerr << "heste\n";
				tint32 iSamplesDecodedNow = lame_decode_headers((unsigned char*)mpcInputBuff, (int)iRead, (short*)mpcIntermediateBuffL, (short*)mpcIntermediateBuffR, &mp3headers);
				if (iSamplesDecodedNow < 0) {
					AppendErrMsg("Function lame_decode_headers failed for unknown reason");
					bExit = true;
					bSuccess = false;
					//lame_decode_exit();
					//return false;
				}
				else if (iSamplesDecodedNow == 0) {
					// Nothing decoded yet
					miSamplesOutputBuffs = miSampleIxOutputBuffs = 0;
				}
				else if (!mp3headers.header_parsed) {
					AppendErrMsg("Function lame_decode_headers didn't return headers");
					bExit = true;
					bSuccess = false;
				}
				else {
					tbool bError = false;
					tint32 iChannels = mp3headers.stereo;
					tint32 iSampleRate = mp3headers.samplerate;
					//tint32 iBitRate = mp3headers.bitrate; // For statistics only - ignore
					
					if (bFirstDecode) {
						// Only go here once
						//bFirstDecode = false;

						miLastInputChannels = iChannels;
											
						// This is where we would detect which encoding was used for mp3 - for now we just set as "unknown"
						EQuality eQuality = geQualityUnknown;
						if (eQuality < meLowestInputQuality) {
							meLowestInputQuality = eQuality;
						}
						
						if ((iChannels != 1) && (iChannels != 2)) {
							tchar psz[128];
							sprintf(psz, "Can only read mono or stereo streams, not %d channels", iChannels);
							AppendErrMsg(psz);
							bError = true;
						}
						
						if ((mbSplit) && (iChannels < 2)) {
							AppendErrMsg("Can't split a mono mp3 file into two wave streams");
							bError = true;
						}
						
						if ((miOutputSampleFreq > -1) && (miOutputSampleFreq != iSampleRate)) {
							tchar psz[128];
							sprintf(psz, "Sample freq of new mp3 (%d Hz) differs from first mp3 freq (%d Hz)", iSampleRate, miOutputSampleFreq);
							AppendErrMsg(psz);
							bError = true;
						}
						else if ((iSampleRate != 44100) && (iSampleRate != 48000) && (iSampleRate != 88200) && (iSampleRate != 96000)) {
							tchar psz[128];
							sprintf(psz, "Sample freq of mp3 (%d Hz) can't be used", iSampleRate);
							AppendErrMsg(psz);
							bError = true;
						}
						else {
							miOutputSampleFreq = iSampleRate;
#ifndef AC_IDECODER_NOT_BACK_COMPAT
							miSampleFreq = iSampleRate;
#endif // AC_IDECODER_NOT_BACK_COMPAT
						}
						
						if (bError) {
							bExit = true;
							bSuccess = false;
						}

						if (miOutputChannels == -1) {
							miOutputChannels = iChannels;
						}
						miLastInputChannels = iChannels;
						miOutputBlockAlign = (miOutputBitWidth / 8) * miOutputChannels;
						
						// First time here? Write the RIFF/Wave header
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

						// Do some things only once
						bFirstDecode = false;
					}

					if (!bError) {
						// Limit number of samples (maybe)
						tuint64 uiSampleIxNew = muiInputFileSampleIx + iSamplesDecodedNow;
						tint32 iSampleIxToProcessNow = 0;
						tint32 iSamplesToProcessNow = 0;
						if (uiSampleIxNew < iSampleIxToProcess) {
							// We must skip this entire range - just increase sample index
							muiInputFileSampleIx = uiSampleIxNew;
							std::cout << "Skip range\n";
						}
						else {
							iSamplesToProcessNow = (tint32)(uiSampleIxNew - muiInputFileSampleIx);
							iSampleIxToProcessNow = iSamplesDecodedNow - iSamplesToProcessNow;
							muiInputFileSampleIx = uiSampleIxNew;
						}
						
						
						// This points to the buffers we're really gonna use
						mpcBufferToWriteNowL = NULL;
						mpcBufferToWriteNowR = NULL;
						miSampleIxOutputBuffs = 0;
						miSamplesOutputBuffs = iSamplesToProcessNow;
						tint32 iBytesOutputBuffs = miSamplesOutputBuffs * miOutputBlockAlign;
						
						if (miOutputBitWidth == 16) {
							// output as 16 bit
							if (iChannels == 1) {
								// input is mono
								if (miOutputChannels == 1) {
									// output 16 bit mono
									
#ifdef _Mac_PowerPC
									// Motorola CPU - we need to byte-swap every sample, damn!
									mpcBufferToWriteNowL = mpcIntermediateBuffL;
									short* ps = (short*)mpcIntermediateBuffL;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										// Swap in-place
										*ps++ = CPSwap(*ps);
									}
#else _Mac_PowerPC
									// Intel CPU - use buffer as it is
									mpcBufferToWriteNowL = mpcIntermediateBuffL;
#endif // #else _Mac_PowerPC
									
								}
								else {
									// output 16 bit upmix stereo
									
									// Both CPUs - double every sample (CPSwap does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
									short* psSrc = (short*)mpcIntermediateBuffL;
									short* psDst = (short*)mpcBufferToWriteNowL;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										*psDst++ = *psDst++ = CPSwap(*psSrc++);;
									}
								}
							}
							else {
								// input is stereo
								if (miOutputChannels == 1) {
									// output 16 bit downmix mono

									// Both CPUs - downmix every sample (CPSwap does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = mpcIntermediateBuffL;
									short* psL = (short*)mpcIntermediateBuffL;
									short* psR = (short*)mpcIntermediateBuffR;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										// Convert in-place
										tint32 iAvg = (((tint32)(*psL) + (tint32)(*psR++))) >> 1;
										*psL++ = CPSwap((short)iAvg);
									}
									
								}
								else {
									// output 16 bit stereo
									
									// Both CPUs - interleave samples (CPSwap does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
									short* psL = (short*)mpcIntermediateBuffL;
									short* psR = (short*)mpcIntermediateBuffR;
									short* psDst = (short*)mpcBufferToWriteNowL;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										*psDst++ = CPSwap(*psL++);
										*psDst++ = CPSwap(*psR++);
									}
									
								}
							}
						}
						else {
							// output as 24 bit
							if (iChannels == 1) {
								// input is mono
								if (miOutputChannels == 1) {
									// output 24 bit mono
									
									// Both CPUs - stretch 16 to 24 bit (Inplace32to24 does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
									short* psSrc = (short*)mpcIntermediateBuffL;
									char* pcDst = mpcBufferToWriteNowL;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										tint32 iData = ((tint32)(*psSrc++)) << 8;
										Inplace32to24(iData);
										*(tint32*)(pcDst) = iData;
										pcDst += 3;
										//std::cerr << "\ngiraf " << i << " : " << ((tint32)(pcDst-mpcPCMBuff1));
									}
								}
								else {
									// output 24 bit upmix stereo

									// Both CPUs - stretch 16 to 24 bit and double (Inplace32to24 does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
									short* psSrc = (short*)mpcIntermediateBuffL;
									char* pcDst = mpcBufferToWriteNowL;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										tint32 iData = ((tint32)(*psSrc++)) << 8;
										Inplace32to24(iData);
										*(tint32*)(pcDst) = iData;
										pcDst += 3;
										*(tint32*)(pcDst) = iData;
										pcDst += 3;
									}
								}
							}
							else {
								// input is stereo
								if (mbSplit) {
									// Split stereo mp3 into 2 mono 24 bit wave files
										
									// Both CPUs - stretch 16 to 24 bit (Inplace32to24 does the byte-swapping for Motorola only)
									mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
									mpcBufferToWriteNowR = PrepareOutputBuffR(iBytesOutputBuffs);
									short* psL = (short*)mpcIntermediateBuffL;
									short* psR = (short*)mpcIntermediateBuffR;
									char* pcDstL = mpcBufferToWriteNowL;
									char* pcDstR = mpcBufferToWriteNowR;
									for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
										tint32 iData = ((tint32)(*psL++)) << 8;
										Inplace32to24(iData);
										*(tint32*)(pcDstL) = iData;
										pcDstL += 3;
										
										iData = ((tint32)(*psR++)) << 8;
										Inplace32to24(iData);
										*(tint32*)(pcDstR) = iData;
										pcDstR += 3;
									}
								}
								else {
									// Not split
									if (miOutputChannels == 1) {
										// output 24 bit downmix stereo

										// Both CPUs - stretch 16 to 24 bit and downmix (Inplace32to24 does the byte-swapping for Motorola only)
										mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
										short* psL = (short*)mpcIntermediateBuffL;
										short* psR = (short*)mpcIntermediateBuffR;
										char* pcDst = mpcBufferToWriteNowL;
										for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
											tint32 iData = (tint32)(*psL++);
											iData += (tint32)(*psR++);
											iData <<= (8 - 1);
											Inplace32to24(iData);
											*(tint32*)(pcDst) = iData;
											pcDst += 3;
										}
									}
									else {
										// output 24 bit stereo
										
										// Both CPUs - stretch 16 to 24 bit and interleave (Inplace32to24 does the byte-swapping for Motorola only)
										mpcBufferToWriteNowL = PrepareOutputBuffL(iBytesOutputBuffs);
										short* psL = (short*)mpcIntermediateBuffL;
										short* psR = (short*)mpcIntermediateBuffR;
										char* pcDst = mpcBufferToWriteNowL;
										for (tint32 i = miSamplesOutputBuffs; i > 0; i--) {
											tint32 iData = ((tint32)(*psL++)) << 8;
											Inplace32to24(iData);
											*(tint32*)(pcDst) = iData;
											pcDst += 3;
											
											iData = ((tint32)(*psR++)) << 8;
											Inplace32to24(iData);
											*(tint32*)(pcDst) = iData;
											pcDst += 3;
										}
									}
								}
							}
						}
					}
				}
			}

			tint64 iPct = (pfInputMp3Stream->GetCurrentFilePosition() * 100);
			tint64 iSize = pfInputMp3Stream->GetSizeWhenOpened();
			if (iSize > 0)
				iPct /= iSize;
			if (iPct != miPctDisplayed) {
				tchar psz[32];
				sprintf(psz, "\r%3d%% ", (tint32)iPct);
				std::cerr << psz;
				miPctDisplayed = (tint32)iPct;
			}
		}			
	}
	
	// Did we succeed?
	if (piSamplesProcessed)
		*piSamplesProcessed = iSamplesReturnedThisCall;
	return bSuccess;
} // Process


tbool CLameDecoder::Finalize_CodecSpecific()
{
	tbool bSuccess = true;
	// Release hook on LAME engine
	if (pLameInstance == (void*)this) {
		pLameInstance = NULL;
	}

	if (mb_lame_decode_init) {
		// Close LAME
		tint32 iError = lame_decode_exit();
		if (iError != 0) {
			bSuccess = false;
			tchar pszMsg[256];
			sprintf(pszMsg, "lame_decode_exit() returned error code %d", iError);
			AppendErrMsg(pszMsg);
		}
	
		mb_lame_decode_init = false;
	}

	return bSuccess;
} // Finalize_CodecSpecific
