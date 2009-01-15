#include "acInternalOS.h"


CWaveDecoder::CWaveDecoder()
{
	ResetAllParams();
	mbIsLossyCompression = false;
	meAudioCodec = geAudioCodecWave;
}

CWaveDecoder::~CWaveDecoder()
{
	Maybe_Finalize();
}
	
IDecoder* CWaveDecoder::Create()
{
	return dynamic_cast<IDecoder*>(new CWaveDecoder());
}

void CWaveDecoder::Destroy()
{
	delete dynamic_cast<CWaveDecoder*>(this);
}

tbool CWaveDecoder::Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader /*= true*/, tint32 iBitWidth /*= 24*/, tint32 iChannels /*= -1*/, IFile* pfOutputWaveStream2 /*= NULL*/)
{
	tbool bOK = CDecoder::Init(pfOutputWaveStream1, bRIFFWaveHeader, iBitWidth, iChannels, pfOutputWaveStream2);
	
	miLastInputBitWidth = 0;
	miIn_Channels = 0;
	miIn_BlockAlign = -1;
	miIn_SampleRate = 0;
	miIn_Samples = 0;

	return bOK;
}

tbool CWaveDecoder::Process(IFile* pfInputWaveStream, tint32 iSampleIxToProcess /*= 0*/, tint32 iNbOfSamplesToProcess /*= -1*/, tint32* piSamplesProcessed /*= NULL*/)
{
	if (pfInputWaveStream == NULL) {
		AppendErrMsg("Input wave stream is NULL.");
		return false;
	}

	tint32 iSamplesReturnedThisCall = 0;
	if (piSamplesProcessed)
		*piSamplesProcessed = 0;

	tbool bSameFile = false;
	if (!Process_Init(pfInputWaveStream, 0, 0, 0, &bSameFile))
		return false;
	if (bSameFile) {
		// Should we move read position?
		if (iSampleIxToProcess != muiInputFileSampleIx) {
			// Yes, we must move
			if ((iSampleIxToProcess < 0) || (iSampleIxToProcess >= miIn_Samples)) {
				// ... but we can't - that's an illegal index
				tchar psz[128];
				sprintf(psz, "Can't seek to frame %d (out of range)", iSampleIxToProcess);
				AppendErrMsg(psz);
				return false;
			}
			// Calculate how to skip
			tint32 iSkipFrames = iSampleIxToProcess - miSampleIxOutputBuffs;
			tint32 iSkipBytes = iSkipFrames * miIn_BlockAlign;
			// Skip
			muiInputFilePos += iSkipBytes;
			muiInputFileSampleIx = iSampleIxToProcess;
		}
		pfInputWaveStream->Seek(muiInputFilePos);
	}
	if (!bSameFile) {
		miLastInputBitWidth = 0;
		miIn_Channels = 0;
		miIn_BlockAlign = -1;
		miIn_SampleRate = 0;
		miIn_Samples = 0;

		// Use the open file functionality of one of the encoders to parse the wave file header
		// We pick the wave encoder (but it could just as well have been the Vorbis or LameEncoder)
		IEncoder* pEncoder = IEncoder::Create(geAudioCodecWave);
		CEncoder* pEncoder2 = NULL;
		if (pEncoder)
			pEncoder2 = dynamic_cast<CEncoder*>(pEncoder);
		if (pEncoder2 == NULL) {
			AppendErrMsg("Unable to allocate IEncoder for parsing wave (out of memory?)");
			return false;
		}
		// Set flag to allow Init(NULL)
		pEncoder2->mbIgnoreOutputFileNull = true;
		if (
			(!pEncoder2->Init(NULL, geQualityLossless24))
			||
			(!pEncoder2->Process_InitFile(pfInputWaveStream, NULL, iSampleIxToProcess, &iNbOfSamplesToProcess))
		) {
			tchar pszErr[512];
			pEncoder2->GetErrMsg(pszErr, 512, true);
			AppendErrMsg(pszErr);
			pEncoder2->Destroy();
			pEncoder2 = NULL;
			pEncoder = NULL;
			return false;
		}
		if (1) {
			tchar pszWarnings[1024];
			if (pEncoder2->GetWarnMsg(pszWarnings, 1024, true)) {
				if (*pszWarnings)
					AppendWarnMsg(pszWarnings);
			}
		}
		// Save the info from headers
		miLastInputBitWidth = pEncoder2->miInputBitWidth;
		miIn_Channels = pEncoder2->miInputChannels;
		miIn_BlockAlign = pEncoder2->miInputBlockAlign;
		miIn_SampleRate = pEncoder2->miInputSampleFreq;
		miIn_Samples = pEncoder2->miInputSamples_IncludingSkipped;
		EQuality eQuality = pEncoder2->meQuality;
		if (eQuality < meLowestInputQuality) {
			meLowestInputQuality = eQuality;
		}
		// Save position of data chunk
		muiInputFilePos = pfInputWaveStream->GetCurrentFilePosition();
	}

	tint32 iSamplesDecodedCurrInput = 0;
	//tint32 iPctDisplayed = -1;

	tint32 iSamplesToRead_CurrInput;
	tint32 iSamplesLeft_CurrInput = miIn_Samples - iSampleIxToProcess;
	if ((iNbOfSamplesToProcess >= 0) && (iNbOfSamplesToProcess <= iSamplesLeft_CurrInput)) {
		// Plenty of samples - take all we need
		iSamplesToRead_CurrInput = iNbOfSamplesToProcess;
	}
	else {
		// Insufficient or unspecified samples - limit return to what's there
		iSamplesToRead_CurrInput = iSamplesLeft_CurrInput;
	}

	tint32 iSamplesToRead_Now = 2 * 1024;
	tint32 iBytesToRead_Now = iSamplesToRead_Now * miIn_BlockAlign;

	tbool bFirstDecode = !bSameFile;
	tbool bSuccess = false;
	tbool bExit = false;
	while (!bExit) {
		if (iSamplesToRead_CurrInput < iSamplesToRead_Now) {
			// Last portion
			iSamplesToRead_Now = iSamplesToRead_CurrInput;
			iBytesToRead_Now = iSamplesToRead_Now * miIn_BlockAlign;
		}
		tchar* pcInBuff = PrepareInputBuff(iBytesToRead_Now);
		tint64 iRead = pfInputWaveStream->Read(pcInBuff, iBytesToRead_Now);
		
		if (iRead < 0) {
			AppendErrMsg("input wave error");
			return false;
		}
		if (iRead == 0) {
			// All of wave was read
			
			// Exit with success
			bSuccess = true;
			bExit = true;
			//return true;
		}
		else {
			muiInputFilePos = pfInputWaveStream->GetCurrentFilePosition();
			tint32 iSamplesDecodedNow = (tint32)iRead / miIn_BlockAlign;
			if (iSamplesDecodedNow > 0) {
				if (bFirstDecode) {
					// Only go here once
					bFirstDecode = false;
					
					miLastInputSamples = 0;
					miLastInputChannels = miIn_Channels;
					
					tbool bError = false;
					if ((miIn_Channels != 1) && (miIn_Channels != 2)) {
						tchar psz[128];
						sprintf(psz, "Can only read mono or stereo streams, not %d channels", miIn_Channels);
						AppendErrMsg(psz);
						bError = true;
					}
					
					if ((miOutputSampleFreq > -1) && (miOutputSampleFreq != miIn_SampleRate)) {
						tchar psz[128];
						sprintf(psz, "Sample freq of new wave (%d Hz) differs from first wave freq (%d Hz)", miIn_SampleRate, miOutputSampleFreq);
						AppendErrMsg(psz);
						bError = true;
					}
					else if ((miIn_SampleRate != 44100) && (miIn_SampleRate != 48000) && (miIn_SampleRate != 88200) && (miIn_SampleRate != 96000)) {
						tchar psz[128];
						sprintf(psz, "Sample freq of wave (%d Hz) can't be used", miIn_SampleRate);
						AppendErrMsg(psz);
						bError = true;
					}
					else {
						miOutputSampleFreq = miIn_SampleRate;
#ifndef AC_IDECODER_NOT_BACK_COMPAT
						miSampleFreq = miIn_SampleRate;
#endif // AC_IDECODER_NOT_BACK_COMPAT
					}
					
					if (bError) {
						bExit = true;
						bSuccess = false;
						//return false;
					}
					
					if (miOutputChannels == -1) {
						miOutputChannels = miIn_Channels;
					}
					miLastInputChannels = miIn_Channels;
					
					miOutputBlockAlign = (miOutputBitWidth / 8) * (mbSplit ? 1 : miOutputChannels);
					
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

					// End of "Only go here once"
				}

				muiInputFileSampleIx += iSamplesDecodedNow;
				iSamplesDecodedCurrInput += iSamplesDecodedNow;
				iSamplesToRead_CurrInput -= iSamplesDecodedNow;
				
				// This points to the buffer(s) we're gonna write
				mpcBufferToWriteNowL = NULL;
				mpcBufferToWriteNowR = NULL;
				tint32 iBytesOutputBuffs = 0;
				miSamplesOutputBuffs = iSamplesDecodedNow * miOutputBlockAlign;
				
				if (miLastInputBitWidth == 16) {
					if (miOutputBitWidth == 16) {
						// output as 16 bit
						if (miIn_Channels == 1) {
							// input is mono
							if (miOutputChannels == 1) {
								// output 16 bit mono
								
								// Use buffer as it is
								mpcBufferToWriteNowL = pcInBuff;
								iBytesOutputBuffs = (tint32)iRead;
							}
							else {
								// output 16 bit upmix stereo
								st::Xlat::PCM16Mono_PCM16Stereo(pcInBuff, (tint32)iRead,
									&iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
								mpcBufferToWriteNowL = mpcOutputBuffL;
							}
						}
						else {
							// input is stereo
							if (miOutputChannels == 1) {
								// output 16 bit downmix mono

								st::Xlat::PCM16Stereo_PCM16Mono_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
								mpcBufferToWriteNowL = pcInBuff;							
							}
							else if (mbSplit) {
								// output 16 bit dual mono

								st::Xlat::PCM16Stereo_DualPCM16Mono_PartialInPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffR, &miSizeOutputBuffR);
								mpcBufferToWriteNowL = pcInBuff;
								mpcBufferToWriteNowR = mpcOutputBuffR;
							}
							else {
								// output 16 bit stereo
								
								// Use buffer as it is
								mpcBufferToWriteNowL = pcInBuff;
								iBytesOutputBuffs = (tint32)iRead;
							}
						}
					}
					else {
						// input = 16 bits, output as 24 bit
						if (miIn_Channels == 1) {
							// input is mono
							if (miOutputChannels == 1) {
								// output 16 => 24 bit mono
								
								st::Xlat::PCM16Mono_PCM24Mono(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
								mpcBufferToWriteNowL = mpcOutputBuffL;
							}
							else {
								// output 16 => 24 bit upmix stereo
								
								// Double every sample
								st::Xlat::PCM16Mono_PCM24Stereo(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
								mpcBufferToWriteNowL = mpcOutputBuffL;
							}
						}
						else {
							// input is stereo
							if (mbSplit) {
								// Split 16 bit stereo wave into two 24 bit mono wave files

								st::Xlat::PCM16Stereo_DualPCM24Mono_PartialInPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffR, &miSizeOutputBuffR);
								mpcBufferToWriteNowL = pcInBuff;
								mpcBufferToWriteNowR = mpcOutputBuffR;
							}
							else {
								if (miOutputChannels == 1) {
									// output 16 => 24 bit downmix mono

									st::Xlat::PCM16Stereo_PCM24Mono_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
									mpcBufferToWriteNowL = pcInBuff;
								}
								else {
									// output 16 => 24 bit stereo

									st::Xlat::PCM16Stereo_PCM24Stereo(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
									mpcBufferToWriteNowL = mpcOutputBuffL;
								}
							}
						}
					}
				}
				else {
					// input is 24 bit
					if (miOutputBitWidth == 16) {
						// Output 24 => 16 bit

						if (miIn_Channels == 1) {
							if (miOutputChannels == 1) {
								// 24 mono => 16 mono
								st::Xlat::PCM24Mono_PCM16Mono_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
								mpcBufferToWriteNowL = pcInBuff;
							}
							else {
								// 24 mono => 16 stereo
								st::Xlat::PCM24Mono_PCM16Stereo(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
								mpcBufferToWriteNowL = mpcOutputBuffL;
							}
						}
						else if (mbSplit) {
							// 24 stereo => dual 16 mono
							st::Xlat::PCM24Stereo_DualPCM16Mono_PartialInPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffR, &miSizeOutputBuffR);
							mpcBufferToWriteNowL = pcInBuff;
							mpcBufferToWriteNowR = mpcOutputBuffR;
						}
						else {
							if (miOutputChannels == 1) {
								// 24 stereo => 16 mono
								st::Xlat::PCM24Stereo_PCM16Mono_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
								mpcBufferToWriteNowL = pcInBuff;
							}
							else {
								// 24 stereo => 16 stereo
								st::Xlat::PCM24Stereo_PCM16Stereo_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
								mpcBufferToWriteNowL = pcInBuff;
							}
						}
					}
					else {
						// Both input and output is 24 bits
						if (miIn_Channels == 1) {
							if (miOutputChannels == 1) {
								// 24 mono (Use data as is)
								mpcBufferToWriteNowL = pcInBuff;
								iBytesOutputBuffs = (tint32)iRead;
							}
							else {
								// 24 mono => 24 stereo
								st::Xlat::PCM24Mono_PCM24Stereo(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffL, &miSizeOutputBuffL);
								mpcBufferToWriteNowL = mpcOutputBuffL;
							}
						}
						else {
							// input is stereo
							if (mbSplit) {
								// 24 stereo => dual 24 mono
								st::Xlat::PCM24Stereo_DualPCM24Mono_PartialInPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs, &mpcOutputBuffR, &miSizeOutputBuffR);
								mpcBufferToWriteNowL = pcInBuff;
								mpcBufferToWriteNowR = mpcOutputBuffR;
							}
							else if (miOutputChannels == 1) {
								// 24 stereo => 24 mono
								st::Xlat::PCM24Stereo_PCM24Mono_InPlace(pcInBuff, (tint32)iRead, &iBytesOutputBuffs);
								mpcBufferToWriteNowL = pcInBuff;
							}
							else {
								// 24 stereo (Use data as is)
								mpcBufferToWriteNowL = pcInBuff;
								iBytesOutputBuffs = (tint32)iRead;
							}
						}
					}
				}
				
				// Write the converted output
				if (!mbAllowNoOutputFile) {
					if (mpcBufferToWriteNowL == NULL) {
						// We've made an error
						AppendErrMsg("Internal error: mpcBufferToWriteNowL == NULL");
						return false;
					}

					if (!mbSplit) {
						// Write to one (stereo or mono) wave file
						mpfOutWaveStream1->Write((const char*)mpcBufferToWriteNowL, iBytesOutputBuffs);
					}
					else {
						// Split into two (mono) wave files
						if (mpcBufferToWriteNowR == NULL) {
							// We've made an error
							AppendErrMsg("Internal error: mpcBufferToWriteNowR == NULL");
							return false;
						}

						mpfOutWaveStream1->Write((const char*)mpcBufferToWriteNowL, iBytesOutputBuffs);
						mpfOutWaveStream2->Write((const char*)mpcBufferToWriteNowR, iBytesOutputBuffs);
					}
				}
				
				miLastInputSamples += iSamplesDecodedNow;
				miSamplesTotal += iSamplesDecodedNow;
				iSamplesReturnedThisCall += iSamplesDecodedNow;
			}
		}

		tint64 iPct = miLastInputSamples * 100;
		tint64 iTotal = miIn_Samples;
		if (iTotal > 0) {
			iPct /= iTotal;
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

