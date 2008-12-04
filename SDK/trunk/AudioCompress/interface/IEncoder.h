#ifndef _ac_iencoder_h
#define _ac_iencoder_h
// Created by Lasse Steen Bohnstedt, 2007-10-xx

class IEncoder : public virtual IDestructable
{
public:
	//! Creates IEncoder - must be overloaded!
	static IEncoder* Create();

	//! Creates a sub-type of IEncoder
	/*!
		\param eAudioCodec [in]: Determines which type of encoder is actually created
		\param pszErrBuff [out]: Receives error info (on error). Leave NULL if not interested
		\param iErrBuffSize [in]: Size of buffer to receive any error info. Must be set if pszErrBuff is submit, don't leave at 0
		\return IEncoder*: Pointer to created encoder - is NULL on error (description in pszErrBuff)
	*/
	static IEncoder* Create(EAudioCodec eAudioCodec, tchar* pszErrBuff = NULL, tint32 iErrBuffSize = 0);
	
	//! Prepare for conversion of a series of input data or streams into a single output stream of whatever format the decoder is for
	/*!
		\param pfOutputStream [in]: Output file already opened and ready to write to. Will receive all of output (stereo or mono)
		\param eQuality [in]: Desired quality for encoded file. Info will be saved in output file. May be left at geQualityDefault
		\param iChannels [in]: Requested output channels (1 = mono, 2 = stereo, -1 = same as first source)
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Init(IFile* pfOutputStream, EQuality eQuality = geQualityDefault, tint iChannels = -1) = 0;

	//! If the audio to encode has been previously compressed/decompressed this info can be saved instead of the eQuality value. For every time this method is called only the lowest value is saved<br/><b>Note!</b> Due to file format limitation you shouldn't call this function after the first Process(..) - it probably can't do anything then (you'll get no error though)
	/*!
		\param eQualityOfOriginal [in]: The quality of earlier compression
	*/
	virtual void SetQualityOfOriginal(EQuality eQualityOfOriginal) = 0;

	//! Notifies encoder that the input to Process(...) method won't have RIFF headers and supplies such header info
	/*!
		\param bInputRawData [in]: True; the following input files will be of raw format (only PCM or tfloat32 arrays, no RIFF/WAVE headers). False; inputs are standard RIFF/WAVE compatible .wav files
		\param iChannels [in]: Number of channels for input (only valid for bInputRawData == true)
		\param bIsInterleavedStereo [in]: Whether stereo data is interleaved (true) or will be submit as two streams (false) (only valid for bInputRawData == true)
		\param iBitWidth [in]: Bitwidth in input (only valid for bInputRawData == true). Can be 16 (16 bit PCM), 24 (24 bit PCM) or 32 (32 bit float arrays)
		\param iFreqHz [in]: Frequency of input (only valid for bInputRawData == true)
		\return tbool: True; success (the params make sense). False; error (get error description with GetErrMsg() )
	*/
	virtual tbool SetRawMode(tbool bInputRawData, tint16 iChannels, tbool bIsInterleavedStereo, tint16 iBitWidth, tint32 iFreqHz) = 0;

	//! Set factor to multiply each sample by. May be called at any time (or omit, in which case normalization defaults at 1.0f)<br/><b>Note!</b> Only works for 32 bit array inputs yet!
	/*!
		\param fNormalizationFactor [in]: Factor to multiply each sample by
	*/
	virtual void SetNormalizationFactor(tfloat32 fNormalizationFactor) = 0;
	
	//! Encode a WAVE (or raw PCM) file and append to the previously initialized output audio stream
	/*!
		\param pfInputWaveStream1 [in]: Source stream (WAVE or PCM format) already opened with read permission. This must be only input (stereo or mono) or a left side (mono) for generating a stereo output
		\param pfInputWaveStream2 [in]: Source stream (WAVE or PCM format) already opened with read permission. This must be NULL or a right side (mono) for generating a stereo output
		\param iSampleIxToProcess [in]: Index of first sample to encode: 0 = start at very first sample. -1 = continue just after where we last ended (i.e. for a new file it's the same as 0)
		\param iNbSamplesToProcess [in]: Number of samples to encode (-1 = all)
		\param piActualSamplesProcessed [out]: Number of samples actually processed (leave pointer at NULL if you don't care)
		\param pbDone [out]: Note! Can only be used with iMaxTime > -1!<br/>True if all of input stream(s) have been processed, False if there's more
		\param iMaxTimeMS [in]: If > -1 then Process(...) will run no more than that amount of milliseconds before returning. If all is done within the time limit then *pbDone is set true; otherwise the next call to Process(...) should be of the same input file(s) so encoding can continue where it paused. This is usefull for encoding bit-by-bit and allowing for other threads (e.g. a progress bar) to run in-between
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Process(IFile* pfInputWaveStream1, IFile* pfInputWaveStream2, tint32 iSampleIxToProcess = -1, tint32 iNbSamplesToProcess = -1, tint64* piActualSamplesProcessed = NULL, tbool* pbDone = NULL, tint32 iMaxTimeMS = -1, tint64* piAccumOverflows = NULL) = 0;

	//! Same as the other Process - just without file 2 (for backwards compatibility)
	virtual tbool Process(IFile* pfInputWaveStream1, tint32 iSampleIxToProcess = 0, tint32 iNbSamplesToProcess = -1, tint64* piActualSamplesProcessed = NULL) = 0;

	//! Encode a portion of data (PCM or tfloat32 array) and append to the previously initialized output audio stream<br/><b>Warning!</b> Will use in-place conversion when-ever possible, i.e. input may be altered!
	/*!
		\param pcInput1 [in/out]: Source data for encoding. Either left channel or interleaved stereo (in which case only PCM is supported)<br/><b>Warning!</b> Contents may be altereded after call returns!
		\param pcInput2 [in]: Source data for encoding. Right channel, may be NULL
		\param iInputBytes [in]: Number of bytes (note! not samples!) in input(s). If two buffers are submitted then the total amount of bytes is double
		\param piSamples [in]: If != NULL it will receive the number of samples that the raw input translates to
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool ProcessRaw(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint64* piAccumOverflows, tint64* piSamples = NULL) = 0;
	
	//! Finalize encoding and flush memory buffer to the output compressed audio stream - if calling app doesn't call Finalize() directly, IEncoder will do so upon Destroy()
	/*!
		\return tbool: True upon success, false upon error (get error description with GetErrMsg() )
	*/
	virtual tbool Finalize() = 0;
	
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

	//! Gets the range of qualities that apply to this encoder
	/*!
		\param piFirstQ [out]: First quality
		\param piLastQ [out]: First quality
		\param piDefault [out]: Default quality
	*/
	virtual void GetQualityRange(tint32* piFirstQ, tint32* piLastQ, tint32* piDefaultQ = NULL) = 0;
	
	//! Translates a quality number to a name
	/*!
	 \param iQualityNb [in]: The quality to get name of
	 \param pszName [out]: Buffer that receives name
	 \param iNameBuffSize [in]: Size of buffer that will receive name (it will be truncated if buffer too small)
	 \return tbool: True on success
	 */
	static tbool GetQualityName(tint32 iQualityNb, tchar* pszName, tint32 iNameBuffSize);

	//! Translates a quality number to a name
	/*!
	 \param iQualityNb [in]: The quality to get name of
	 \return const tchar*: Name on success, "" on error
	 */
	static const tchar* GetQualityName(tint32 iQualityNb);
		
	//! Translates a quality name to a number
	/*!
	 \param pszName [in]: The name of the quality to look up
	 \return tint32: The number of the named quality (-1 upon error)
	 */
	static tint32 GetQualityNb(const tchar* pszName);
	
	//! Translates a quality to an approximate bitrate (in kbps)
	/*!
		\param iQualityNb [in]: The quality to get name of
		\param piKBPS_Mono [out]: The approximate bitrate in kbps for mono output (input may be mono or stereo, doesn't matter)
		\param piKBPS_Stereo [out]: The approximate bitrate in kbps for stereo input and output
		\return tbool: True on success
	*/
	virtual tbool GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo) = 0;
	
	//! Tells which actual type of encoder is allocated
	virtual EAudioCodec GetAudioCodec() const = 0;

	//! Tells which file-extension is the default for the allocated encoder ('*' char is not included)
	virtual const tchar* GetDefaultExt() = 0;

	//! Tells which file-extension is the default for a codec ('*' char is not included)
	/*!
		\param eCodec [in]: The codec to get extension for
		\return tchar*: The extension
	*/
	static const tchar* GetDefaultExt(EAudioCodec eCodec);

	//! Tells which file-type description is appropriate for the allocated encoder
	virtual const tchar* GetFileTypeDesc() = 0;

	//! Tells which file-type description is appropriate for a codec
	/*!
		\param eCodec [in]: The codec to get extension for
		\return tchar*: The description
	*/
	static const tchar* GetFileTypeDesc(EAudioCodec eCodec);

public:
	// The following are valid after Init() method
	EQuality meQuality;

	// The following are only valid after at least one call to the Process() method
	EQuality meQualityInfoToSave;
	tint16 miOutputChannels;
	tint32 miOutputSampleFreq;
	
	// The following are only valid after at least one call to the Process() method, and may change with each call
	tbool mbIsInputRawData;
	tbool mbIsInputInterleavedStereo;
	tint16 miInputChannels;
	tint16 miInputBitWidth;
	tint32 miInputSampleFreq;
	tint32 miInputSamples;
	tint32 miInputSamples_IncludingSkipped;
	
	// The folowing are only valid after all calls to the Process() method
	tint64 miOutputSamplesTotal;

};

#endif //#infdef _ac_iencoder_h
