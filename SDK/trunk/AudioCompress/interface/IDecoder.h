
#ifndef _ac_IDecoder_h
#define _ac_IDecoder_h

class IDecoder : public virtual IDestructable
{
public:
	//! Creates a decoder that matches eAudioCodec (ogg/vorbis, mp3 or wave).<br/>On error the decoder is destroyed again.
	/*!
		\param eAudioCodec [in]: Should be ogg/vorbis, mp3 or wave.
		\param pszErrMsgBuff [out]: Receives error description (empty string upon success). Leave pointer to NULL if irrelevant
		\param iErrMsgBuffSize [in]: Size of buffer to receive error description. Leave at -1 for no buffer size check
		\return IDecoder*: Upon success: A pointer to a decoder, that will handle the file. Upon error: NULL.
	*/
	static IDecoder* Create(EAudioCodec eAudioCodec, tchar* pszErrMsgBuff = NULL, tint32 iErrMsgBuffSize = -1);

	//! Creates a decoder that matches the type of file entered as input (ogg/vorbis, mp3 or wave).<br/>Will first scan file for ID and if that fails will depend on filename extension; then an appropriate decoder is allocated and single sample decode is attempted. On error the decoder is destroyed again.
	/*!
		\param pSrcFile [in]: A previously opened file that is to be decoded. Should be ogg/vorbis, mp3 or wave.<br/><b>Note!</b> Will scan for recognizable ID at current file-pos and will return to that position before exit.
		\param pszErrMsgBuff [out]: Receives error description (empty string upon success). Leave pointer to NULL if irrelevant
		\param iErrMsgBuffSize [in]: Size of buffer to receive error description. Leave at -1 for no buffer size check
		\return IDecoder*: Upon success: A pointer to a decoder, that will handle the file. Upon error: NULL.
	*/
	static IDecoder* Create(IFile* pSrcFile, tchar* pszErrMsgBuff = NULL, tint32 iErrMsgBuffSize = -1);

	//! Prepare for decoding of a set of compressed audio streams into a single Wave or Raw stream
	/*!
		\param pfOutputWaveStream1 [in]: The output Wave stream to write to - must be previously opened with write access. If an pfOutputWaveStream2 is entered too then pfOutputWaveStream1 will be left channel output only
		\param bRIFFWaveHeader [in]: Set false to output only the RAW PCM data (skip writing the RIFF/Wave header)
		\param iBitWidth [in]: The requested bit-width for the output Wave. Must be 16 or 24
		\param iChannels [in]: The requested channels for the output Wave (1 = mono, 2 = stereo, -1 = same as the first processed compressed audio stream)
		\param pfOutputWaveStream2 [in]: An optional output Wave stream to write right channel only to - must be previously opened with write access. Requires param iChannels = 1
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Init(IFile* pfOutputWaveStream1, tbool bRIFFWaveHeader = true, tint32 iBitWidth = 24, tint32 iChannels = -1, IFile* pfOutputWaveStream2 = NULL) = 0;

	//! Replace the output file(s) in the middle of processing.<br/>Note! Only valid for file output without RIFF/Wave headers. If one file was Init then only one *must* be submit here. If two files were Init then two *must* be submit here!<br/>No checking done! Calling app must ensure that file position etc is ok.
	/*!
		\param pfOutputWaveStream1 [in]: The output Wave stream to write to - must be previously opened with write access. If an pfOutputWaveStream2 is entered too then pfOutputWaveStream1 will be left channel output only
		\param pfOutputWaveStream2 [in]: An optional output Wave stream to write right channel only to - must be previously opened with write access. Requires param iChannels = 1
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool HardcoreSwapOutputFiles(IFile* pfOutputWaveStream1, IFile* pfOutputWaveStream2 = NULL) = 0;
	
	//! Decode a compressed audio stream and append to the previously initialized output Wave stream(s)
	/*!
		\param pfInputCompressedStream [in]: Source stream (compressed format) already opened with read permission
		\param iSampleIxToProcess [in]: Index of first sample to encode (0 = very first)
		\param iNbSamplesToProcess [in]: Number of samples to encode (-1 = all)
		\param piSamplesProcessed [in]: Recieves number of samples actually processed. If 0 then there are no more samples in file
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	 */
	virtual tbool Process(IFile* pfInputCompressedStream, tint32 iSampleIxToProcess = 0, tint32 iNbOfSamplesToProcess = -1, tint32* piSamplesProcessed = NULL) = 0;
	
	//! Finalize decoding and update header of the output wave stream(s) - if calling app doesn't call Finalize() directly, IDecoder will do so upon Destroy() (but it is still advicible to call Finalize() explicitly, expecially in error situations, since closing the output wave before destroying decoder would crash application)
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	 */
	virtual tbool Finalize() = 0;

	//! Test a compressed file to see if it will decode.<br/><b>Note!</b> This calls Init, Process and Finalize, so it will disrupt any other ongoing business
	/*!
		\param pfInputCompressedStream [in]: Source stream (compressed format) already opened with read permission
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool TestFile(IFile* pfInputCompressedStream) = 0;
	
	//! Get any error messages accumulated since Init()
	/*!
		\param pszErrMsgBuff [out]: Receives the error messages
		\param iBuffSize [in]: Size of the buffer that shall receive the error messages
		\param bClearMsg [in]: True => messages are reset and won't be received again
		\return tbool: True upon success, false if failed because receiving buffer is too small
	 */
	virtual tbool GetErrMsg(tchar* pszErrMsgBuff, tint iBuffSize, tbool bClearMsg = false) = 0;
	
	//! Get any warning messages accumulated since Init()
	/*!
		\param pszWarnMsgBuff [out]: Receives the error messages
		\param iBuffSize [in]: Size of the buffer that shall receive the error messages
		\param bClearMsg [in]: True => messages are reset and won't be received again
		\return tbool: True upon success, false if failed because receiving buffer is too small
	 */
	virtual tbool GetWarnMsg(tchar* pszWarnMsgBuff, tint iBuffSize, tbool bClearMsg = false) = 0;

	//! Tells which actual type of decoder is allocated
	virtual EAudioCodec GetAudioCodec() const = 0;

public:
	// The following are valid after Init() method
	tint32 miOutputBitWidth;
	tbool mbRIFFWaveHeader;
	tbool mbIsLossyCompression;
	
	// The following are only valid after at least one call to the Process() method
	tint32 miOutputChannels;
	tint32 miOutputSampleFreq;
	tint32 miOutputBlockAlign;
	
	// The following are only valid after at least one call to the Process() method, and may change with each call
	tint32 miLastInputChannels;
	tint32 miLastInputSamples;
	tint32 miLastInputBitWidth; // Only valid for uncompressed files (like waves)!
	//EQuality meLastInputQuality;
	
	// The folowing are only valid after all calls to the Process() method
	tint32 miSamplesTotal;
	EQuality meLowestInputQuality;

#ifndef AC_IDECODER_NOT_BACK_COMPAT
	tint32 miSampleFreq;
	tint32 miChannels;
	tint32 miBitWidth;
#endif // AC_IDECODER_NOT_BACK_COMPAT
};

#endif // #ifndef _ac_IDecoder_h
