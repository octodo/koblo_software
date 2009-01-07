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

#ifndef _ac_c_decoder_h
#define _ac_c_decoder_h
// Created by Lasse Steen Bohnstedt, 2008-08-xx

class CDecoder : public virtual IDecoder
{
	friend class CWaveEncoder;
public:
	CDecoder();
	virtual ~CDecoder();
	
	//! IDestructable override
	virtual void Destroy();

	//! Prepare for decoding of a set of compressed streams into one or two WAVE streams
	/*!
		\param pfOutputWaveStream1 [in]: The output Wave stream to write to - must be previously opened with write access. If an pfOutputWaveStream2 is entered too then pfOutputWaveStream1 will receive left channel data only
		\param bRIFFWaveHeader [in]: Set false to output only the RAW PCM data (skip writing the RIFF/Wave header)
		\param iBitWidth [in]: The requested bit-width for the output Wave. Must be 16 or 24
		\param iChannels [in]: The requested channels for the output Wave (1 = mono, 2 = stereo, -1 = same as the first processed ogg/vorbis stream)
		\param pfOutputWaveStream2 [in]: An optional output Wave stream to write right channel only to - must be previously opened with write access. Requires params iBitWidth = 24 and iChannels = 1
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader = true, tint32 iBitWidth = 24, tint32 iChannels = -1, IFile* pfOutputWaveStream2 = NULL);

	//! Idecoder implementation
	virtual tbool HardcoreSwapOutputFiles(IFile* pfOutputWaveStream1, IFile* pfOutputWaveStream2 = NULL);

	//! Dummy method! (use override classes)<br>Decode a compressed stream and append to the previously initialized output Wave stream(s)
	/*!
		\param pfInputCompressedStream [in]: Source stream (compressed) already opened with read permission
		\param iSampleIxToProcess [in]: Index of first sample to decode (0 = very first)
		\param iNbSamplesToProcess [in]: Number of samples to decode (-1 = all)
		\param piSamplesProcessed [in]: Recieves number of samples actually processed. If 0 then there are no more samples in file
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Process(IFile* pfInputCompressedStream, tint32 iSampleIxToProcess = 0, tint32 iNbOfSamplesToProcess = -1, tint32* piSamplesProcessed = NULL);

	//! Internal. Is called from Process to setup buffers etc
	/*!
		\param pfInputCompressedStream [in]: Source stream (compressed) already opened with read permission
		\param iSizeBuffInput [in]: Required size in bytes of input buffer
		\param iSizeBuffsIntermediate [in]: Required size of the intermediate buffers
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Process_Init(IFile* pfInputCompressedStream, tint32 iSizeBuffInput, tint32 iSizeBuffIntermediateL, tint32 iSizeBuffIntermediateR, tbool* pbSameFile);
	
	//! Finalize decoding and update header of the output wave stream(s)
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Finalize();

	//! Internal. Called from Finalize() may be overloaded by individual decoders
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Finalize_CodecSpecific() { return true; };
	
	//! Test a compressed file to see if it will decode.<br/><b>Note!</b> This calls Init, Process and Finalize, so it will disrupt any other ongoing business
	/*!
		\param pfInputCompressedStream [in]: Source stream (compressed format) already opened with read permission
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool TestFile(IFile* pfInputCompressedStream);
	
	//! Get any error messages accumulated since Init()
	/*!
		\param pszErrMsgBuff [out]: Receives the error messages
		\param iBuffSize [in]: Size of the buffer that shall receive the error messages
		\param bClearMsg [in]: True => messages are reset and won't be received again
		\return tbool: True upon success, false if failed because receiving buffer is too small
	*/
	virtual tbool GetErrMsg(tchar* pszErrMsgBuff, tint iBuffSize, tbool bClearMsg = false);
	
	//! Get any warning messages accumulated since Init()
	/*!
		\param pszWarnMsgBuff [out]: Receives the error messages
		\param iBuffSize [in]: Size of the buffer that shall receive the error messages
		\param bClearMsg [in]: True => messages are reset and won't be received again
		\return tbool: True upon success, false if failed because receiving buffer is too small
	*/
	virtual tbool GetWarnMsg(tchar* pszWarnMsgBuff, tint iBuffSize, tbool bClearMsg = false);

	//! IDecoder implementation
	virtual EAudioCodec GetAudioCodec() const
	{ return meAudioCodec; };

	
protected:
	EAudioCodec meAudioCodec;
	std::string msErrMsg;
	std::string msWarnMsg;
	
	IFile* mpfOutWaveStream1;
	IFile* mpfOutWaveStream2;
	tbool mbSplit;
	static void OutDummyPCMHeader(IFile* pfOut,  tint32 iBitWidth, tint32 iChannels, tint32 iSampleFreq);
	static void OutCorrectPCMHeader(IFile* pfOut, tint32 iPCMLen, EQuality eLowestInputQuality);
	
	void AppendErrMsg(tchar* psz);
	void AppendWarnMsg(tchar* psz);

	void ResetAllParams();
	void Maybe_Finalize();

	tbool mbAllowNoOutputFile;
	tint32 miPctDisplayed;
	
	std::string msInputFilePathName;
	tuint64 muiInputFilePos;
	tuint64 muiInputFileSampleIx;
	//
	tchar* mpcInputBuff;
	tint32 miSizeInputBuff;
	tchar* PrepareInputBuff(tint32 iRequiredBytes)
	{ return ac::PrepareWhateverBuff(&mpcInputBuff, &miSizeInputBuff, iRequiredBytes); };
	//
	tchar* mpcIntermediateBuffL;
	tint32 miSizeIntermediateBuffL;
	tchar* PrepareIntermediateBuffL(tint32 iRequiredBytes)
	{ return ac::PrepareWhateverBuff(&mpcIntermediateBuffL, &miSizeIntermediateBuffL, iRequiredBytes); };
	tchar* mpcIntermediateBuffR;
	tint32 miSizeIntermediateBuffR;
	tchar* PrepareIntermediateBuffR(tint32 iRequiredBytes)
	{ return ac::PrepareWhateverBuff(&mpcIntermediateBuffR, &miSizeIntermediateBuffR, iRequiredBytes); };
	//
	tchar* mpcOutputBuffL;
	tint32 miSizeOutputBuffL;
	tchar* PrepareOutputBuffL(tint32 iRequiredBytes)
	{ return ac::PrepareWhateverBuff(&mpcOutputBuffL, &miSizeOutputBuffL, iRequiredBytes); };
	tchar* mpcOutputBuffR;
	tint32 miSizeOutputBuffR;
	tchar* PrepareOutputBuffR(tint32 iRequiredBytes)
	{ return ac::PrepareWhateverBuff(&mpcOutputBuffR, &miSizeOutputBuffR, iRequiredBytes); };
	//
	tint32 miSampleIxOutputBuffs;
	tint32 miSamplesOutputBuffs;
	tchar* mpcBufferToWriteNowL;
	tchar* mpcBufferToWriteNowR;
	//
	tbool Process_MaybeWriteBufferedOutput(tbool* pbExit, tint32 iNbOfSamplesToProcess, tint32* piAccumSamplesReturned);
};

#endif //#infdef _ac_c_decoder_h
