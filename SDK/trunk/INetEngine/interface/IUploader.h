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
// along with the Koblo INetEngine. If not, see <http://www.gnu.org/licenses/>.

#ifndef _ine_i_uploader
#define _ine_i_uploader

/*! \class IUploader
 * \brief Interface to upload a large file to a web address
 *
 * Supplies more complex and buffered method for posting/putting web-data
 * (lasse)
*/
class IUploader : public virtual IDestructable
{
public:
	static IUploader* Create();

	//! Prepare Internet location to connect to
	/*!
		\param pszHost [in]: Name of Internet host. DO NOT PREPEND WITH "http://" !
		\param pszPage [in]: Folder+page to connect to on Internet host; must start with "/"
		\param iPort [in]: Port number (default 80)
		\param pszUser [in]: User for authentication (default none)
		\param pszPassword [in]: Password for authentication (default none)
		\param iTimeOutSecs [in]: The max allowed amount of seconds without anything happening before error (default 10)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10) = 0;

	//! Tells the server which type of answer we want for our uploading
	/*!
		\param eType [in]: Sets the data type that the uploader wants to recieve
		\return tbool: True upon success
	*/
	virtual tbool SetReplyMIMEType(E_MIME_Type eMIME) = 0;

	//! Force uploader to use a particular verb for uploads (default: PUT if the only parameter added is a single file, POST otherwise)<br/>Regardless of the chosen verb parameters are always sent as part of the message body (multipart/form-data) rather than added to the URL
	/*!
		\param eVerb [in]: E_VERB_POST: Update existing data<br/>E_VERB_PUT: Upload new data
		\return tbool: True upon succes, False otherwise
	*/
	virtual tbool SetSpecificVerb(EVerbType eVerb) = 0;

	//! Tell the uploader whether you want to abort immediately upon 4xx error status codes. <b>Note:</b> This is probably not what you want!
	/*!
		\param bFailOnStatus [in]: True: Abort upload-reply immediately if a 4xx error status is received. False (default, reset by Init(..) ): Error is only reported after all of reply has been received
		\return tbool: True upon successfully setting the parameter
	*/
	virtual tbool SetFailOnHttpStatus(tbool bFailOnStatus) = 0;

	//! Determine memory use upon file upload
	/*!
		\param bUseStreaming [in]: True: Only the required amount of data is loaded at any time (Init(..) resets to default true); False: The entire file contents is loaded into memory before upload starts
	*/
	virtual tbool SetStreamingUpload(tbool bUseStreaming) = 0;

	//! By calling this you'll prevent the uploader from following redirect requests. Must be called after Init(..) since that resets this behaviour
	/*!
		\return tbool: True upon successfully setting the parameter
	*/
	virtual tbool DisableAutoRedirects() = 0;

	//! By calling this you'll allow the uploader to follow redirect requests (default unless DisableAutoRedirects() has been previously called)
	/*!
		\return tbool: True upon successfully setting the parameter
	*/
	virtual tbool EnableAutoRedirects() = 0;

	//! Call this once for each parameter=value set to submit to upload location
	/*!
		\param pszParamName [in]: Name of parameter. Must be "safe" characters only (a-z, A-Z and 0-9 and some others)
		\param pcParamData [in]: Raw data to submit (may or may not be zero-terminated)
		\param iParamDataLen [in]: If >= 0 it is length of raw data. If -1 it's substituted by strlen(pcParamData)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen) = 0;

	//! Call this once for each file to submit to upload location
	/*!
		\param pszParamName [in]: Name of parameter. Must be "safe" characters only (a-z, A-Z and 0-9 and some others)
		\param pfileToUpload [in]: Previously read-access opened file you wish to upload. <b>Must</b> remain open during all calls to UploadPortion! Calling app must Destroy() object after use
		\param pszDestinationName [in]: A custom name for the file; leave NULL to use real name.<br/>A custom name should always be submit for IFileMemory objects, since they don't have a name themselves
		\param eMIME [in]: A custom MIME type to set for the file; if not set the MIME type will be automatically deducted from file contents
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool AddFileParam(const tchar* pszParamName, IFile* pfileToUpload, tchar* pszDestinationName = NULL, E_MIME_Type eMIME = MIME_TYPE_DEFAULT) = 0;

	//! Start background upload of file + recieval of reply either to buffer in memory or directly to file. Will run until IsDone() == true
	/*!
		\param pfileForReply [in]: File that has been previously opened for write-access. Default NULL means that reply is kept in memory instead; in this mode you MUST call the GetReplyPortion(..) repeatedly for data (or else IsDone() will never become true!)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Start(IFile* pfileForReply = NULL) = 0;

	//! Ask for a portion of reply data (this is only possible if Start(..) was called with NULL)
	/*!
		\param pszBuffer [out]: Pre-allocated buffer to recieve the next portion of reply data
		\param iBufferSize [in]: Size of pre-allocated buffer
		\param piPortionSize [out]: The number of reply bytes actually returned in this portion. Will be 0 until upload part of transaction has completed and may occationally be 0 later, caused by e.g. slow network
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool GetReplyPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize) = 0;

	//! Get info on upload + reply progress
	/*!
		\param piUploadProgress [out]: How much of data has been uploaded
		\param piUploadSize [out]: The total number of bytes in upload data
		\param piDownloadProgress [out]: How much of downloaded data has been either 1) written to file or 2) returned via GetReplyPortion(..)
		\param piDownloadSize [out]: The total number of bytes in download data (will be unknown (i.e. 1) until reply starts)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool GetProgress(tint64* piUploadProgress, tint64* piUploadSize, tint64* piDownloadProgress, tint64* piDownloadSize) = 0;

	//! Breaks an ongoing upload operation and releases internal buffers. It's OK to call from different thread.
	virtual tbool Abort() = 0;

	//! Returns True if download has been succesfully completed
	virtual tbool IsDone() = 0;

	//! Returns True if download has failed and can't continue
	virtual tbool IsFailed() = 0;

	//! Returns pointer to human readable error description
	/*!
		\param pszErrBuff [out]: Receives a copy of the latest error description
		\param iErrBuffSize [in]: Size of buffer
		\return: True upon function success (check buffer to see error - if "" then no error exists)
	*/
	virtual tbool GetError(tchar* pszErrBuff, tint32 iErrBuffSize) = 0;
};

#endif // _ine_i_uploader