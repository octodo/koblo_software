#include "acInternalOS.h"

#ifdef _WIN32
#include <time.h>
#endif // #ifdef _WIN32


CWaveEncoder::CWaveEncoder()
{
	miLowestPossibleQualityNb = geQualityLossless16;
	miHighestPossibleQualityNb = geQualityLossless24;
	miDefaultQualityNb = geQualityLossless16;
	mpszEncoderName = "Wave Encoder";
	meAudioCodec = geAudioCodecWave;
	//mpszDefaultExt = ".wav";
	//mpszFileTypeDesc = "RIFF/Wave Files (*.wav)";

	mbWroteHeader = false;
	miPCMLen = 0;
}

CWaveEncoder::~CWaveEncoder()
{
	if ((mbIgnoreOutputFileNull) || (mpfOutputStream != NULL)) {
		Finalize();
	}
}
	
IEncoder* CWaveEncoder::Create()
{
	return dynamic_cast<IEncoder*>(new CWaveEncoder());
}

void CWaveEncoder::Destroy()
{
	delete dynamic_cast<CWaveEncoder*>(this);
}


tbool CWaveEncoder::GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo)
{
	tint32 iMono = 0;
	tint32 iStereo = 0;

	switch (iQualityNb) {
		case geQualityLossless16:	iMono = 706;		iStereo = 1411;	break;
		case geQualityLossless24:	iMono = 1058;		iStereo = 2117;	break;
	}

	if (piKBPS_Mono)
		*piKBPS_Mono = iMono;
	if (piKBPS_Stereo)
		*piKBPS_Stereo = iStereo;

	return ((iMono > 0) && (iStereo > 0));
}


tbool CWaveEncoder::Init_Descendant(EQuality eQuality)
{
	// Convert quality enum to encoder internals
	switch (eQuality) {
		case geQualityLossless16:	miOutputBitWidth = 16; break;
		case geQualityLossless24:	miOutputBitWidth = 24; break;
		default:					miOutputBitWidth = 0; break;
	}
	tchar psz[128];
	sprintf(psz, "Encoding with %d bits\n", miOutputBitWidth);	
	std::cout << psz;

	mbWroteHeader = false;
	miPCMLen = 0;
	
	return (miOutputBitWidth > 0);
}


tbool CWaveEncoder::Process_Init_Descendant()
{
	if (!mbWroteHeader) {
		if (!mbIgnoreOutputFileNull) {
			if (mpfOutputStream == NULL) {
				AppendErrMsg("CWaveEncoder::Process_Init_Descendant, internal error: Output file NULL");
				return false;
			}
			CDecoder::OutDummyPCMHeader(mpfOutputStream, miOutputBitWidth, miOutputChannels, miOutputSampleFreq);
		}
		mbWroteHeader = true;
	}

	return true;
} // Process_Init_Descendant


tbool CWaveEncoder::ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 /*iSamples*/, tint64* piAccumOverflows)
{
	tchar* pcOutputBuff;
	tint32 iOutputBytes = 0;

	switch (miInputBitWidth) {
		case 16:
			if (miOutputBitWidth == 16) {
				if (
					(miInputChannels == miOutputChannels)
					&&
					((miInputChannels == 1) || (mbIsInputInterleavedStereo))
				) {
					// same number channels in and out - Use same data
					pcOutputBuff = pcInput1;
					iOutputBytes = iInputBytes;
				}
				else if (miInputChannels == 1) {
					// mono in => stereo out - upmix
					st::Xlat::PCM16Mono_PCM16Stereo(
						pcInput1, iInputBytes, &iOutputBytes,
						&mpcOutputBuff, &miOutputBuffSize);
					pcOutputBuff = mpcOutputBuff;
				}
				else if (mbIsInputInterleavedStereo) {
					// stereo in => mono out - downmix
					st::Xlat::PCM16Stereo_PCM16Mono_InPlace(
						pcInput1, iInputBytes, &iOutputBytes);
					pcOutputBuff = pcInput1;
				}
				else {
					// dual mono in => stereo out - downmix
					st::Xlat::DualPCM16Mono_PCM16Mono_InPlace(
						pcInput1, pcInput2, iInputBytes);
					pcOutputBuff = pcInput1;
					iOutputBytes = iInputBytes;
				}
			}
			else {
				// Up from 16 => 24 bits
				if (miInputChannels == 1) {
					if (miOutputChannels == 1) {
						// mono 16 in => mono 24 out
						st::Xlat::PCM16Mono_PCM24Mono(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
					else {
						// mono 16 in => stereo 24 out
						st::Xlat::PCM16Mono_PCM24Stereo(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
				else if (mbIsInputInterleavedStereo) {
					if (miOutputChannels == 1) {
						// stereo 16 in => mono 24 out
						st::Xlat::PCM16Stereo_PCM24Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
					else {
						// stereo 16 in => stereo 24 out
						st::Xlat::PCM16Stereo_PCM24Stereo(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
				else {
					if (miOutputChannels == 1) {
						// dual mono 16 in => mono 24 out
						st::Xlat::DualPCM16Mono_PCM24Mono(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
					else {
						// dual mono 16 in => stereo 24 out
						st::Xlat::DualPCM16Mono_PCM24Stereo(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
			}
			break;

		case 24:
			if (miOutputBitWidth == 16) {
				if (miInputChannels == 1) {
					if (miOutputChannels == 1) {
						// mono 24 in => mono 16 out
						st::Xlat::PCM24Mono_PCM16Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
					else {
						// mono 24 in => stereo 16 out
						st::Xlat::PCM24Mono_PCM16Stereo(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
				else if (mbIsInputInterleavedStereo) {
					if (miOutputChannels == 1) {
						// stereo 24 in => mono 16 out
						st::Xlat::PCM24Stereo_PCM16Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
					else {
						// stereo 24 in => stereo 16 out
						st::Xlat::PCM24Stereo_PCM16Stereo_InPlace(
							pcInput1, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
				}
				else {
					if (miOutputChannels == 1) {
						// dual mono 24 in => mono 16 out
						st::Xlat::DualPCM24Mono_PCM16Mono_InPlace(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
					else {
						// dual mono 24 in => stereo 16 out
						st::Xlat::DualPCM24Mono_PCM16Stereo(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
			}
			else {
				if (miInputChannels == 1) {
					if (miOutputChannels == 1) {
						// mono 24 in => mono 24 out
						pcOutputBuff = pcInput1;
						iOutputBytes = iInputBytes;
					}
					else {
						// mono 24 in => stereo 24 out
						st::Xlat::PCM24Mono_PCM24Stereo(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
				else if (mbIsInputInterleavedStereo) {
					if (miOutputChannels == 1) {
						// stereo 24 in => mono 24 out
						st::Xlat::PCM24Stereo_PCM24Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes);
						pcOutputBuff = pcInput1;
					}
					else {
						// stereo 24 in => stereo 24 out
						pcOutputBuff = pcInput1;
						iOutputBytes = iInputBytes;
					}
				}
				else {
					if (miOutputChannels == 1) {
						// dual mono 24 in => mono 24 out
						st::Xlat::DualPCM24Mono_PCM24Mono(
							pcInput1, pcInput2, iInputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
						iOutputBytes = iInputBytes;
					}
					else {
						// dual mono 24 in => stereo 24 out
						st::Xlat::DualPCM24Mono_PCM24Stereo(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize);
						pcOutputBuff = mpcOutputBuff;
					}
				}
			}
			break;

		case 32:
			if (miOutputBitWidth == 16) {
				if (miInputChannels == 1) {
					if (miOutputChannels == 1) {
						// mono 32 in => mono 16 out
						st::Xlat::AF32Mono_PCM16Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes, piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
					}
					else {
						// mono 32 in => stereo 16 out
						st::Xlat::AF32Mono_PCM16Stereo_InPlace(
							pcInput1, iInputBytes, piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
						iOutputBytes = iInputBytes;
					}
				}
				else {
					if (miOutputChannels == 1) {
						// dual mono 32 in => mono 16 out
						st::Xlat::DualAF32Mono_PCM16Mono_InPlace(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
					}
					else {
						// dual mono 32 in => stereo 16 out
						st::Xlat::DualAF32Mono_PCM16Stereo_InPlace(
							pcInput1, pcInput2, iInputBytes, piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
						iOutputBytes = iInputBytes;
					}
				}
			}
			else {
				if (miInputChannels == 1) {
					if (miOutputChannels == 1) {
						// mono 32 in => mono 24 out
						st::Xlat::AF32Mono_PCM24Mono_InPlace(
							pcInput1, iInputBytes, &iOutputBytes, piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
					}
					else {
						// mono 32 in => stereo 24 out
						st::Xlat::AF32Mono_PCM24Stereo(
							pcInput1, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize, piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = mpcOutputBuff;
					}
				}
				else {
					if (miOutputChannels == 1) {
						// dual mono 32 in => mono 24 out
						st::Xlat::DualAF32Mono_PCM24Mono_InPlace(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = pcInput1;
					}
					else {
						// dual mono 32 in => stereo 24 out
						st::Xlat::DualAF32Mono_PCM24Stereo(
							pcInput1, pcInput2, iInputBytes, &iOutputBytes,
							&mpcOutputBuff, &miOutputBuffSize,
							piAccumOverflows,
							-mfNormalizationFactor_Inverse, mfNormalizationFactor_Inverse);
						pcOutputBuff = mpcOutputBuff;
					}
				}
			}
			break;
	}

	if (iOutputBytes > 0) {
		if (!WriteOutput(pcOutputBuff, iOutputBytes)) {
			return false;
		}
		miPCMLen += iOutputBytes;
	}

	return true;
} // ProcessRaw_Descendant


tbool CWaveEncoder::Process_Exit_Descendant()
{
	// No special handling needed for end-of-input
	return true;
} // Process_Exit_Descendant


tbool CWaveEncoder::Finalize_Descendant()
{
	if (mbWroteHeader) {
		if (!mbIgnoreOutputFileNull)
			CDecoder::OutCorrectPCMHeader(mpfOutputStream, miPCMLen, meQualityInfoToSave);
		mbWroteHeader = false;
		miPCMLen = 0;
	}	
	return true;
}
	
