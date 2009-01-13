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
		\param pszPage [in]: Folder+page to connect to on Internet host
		\param iPort [in]: Port number (default 80)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10) = 0;

	//! Tells the server which type of answer we want for our uploading
	/*!
		\param eType [in]: Sets the data type that the uploader wants to recieve
		\return tbool: True upon success
	*/
	virtual tbool SetDesiredMIMEType(E_MIME_Type eType) = 0;

	//! Force uploader to use a particular verb for uploads (default is PUT)<br/>Regardless of the chosen verb parameters are always sent as part of the message body rather than added to the URI
	/*!
		\param eVerb [in]: E_VERB_POST: Update existing data<br/>E_VERB_PUT: Upload new data
		\return tbool: True upon succes, False otherwise
	*/
	virtual tbool SetSpecificVerb(EVerbType eVerb) = 0;

	//! Call this once for each parameter=value set to submit to upload location
	/*!
		\param pszParamName [in]: Name of parameter. Must be US-ASCII characters and numbers only (a-z, A-Z and 0-9)
		\param pcParamData [in]: Raw data to submit (may or may not be zero-terminated)
		\param iParamDataLen [in]: If >= 0 it is length of raw data. If -1 it's substituted by strlen(pcParamData)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen) = 0;

	//! Upload a portion of data
	/*!
		\param pszData [in]: Buffer containing data to be uploaded
		\param iDataLen [in]: Number of data bytes in upload buffer
		\param piActuallySent [out]: The number of bytes actually uploaded in this portion. May occationally be 0, caused by e.g. slow network
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool UploadPortion(const tchar* pszData, tint32 iDataLen, tint32* piActuallySent) = 0;

	//! Tell the server that we're done uploading. There can be no more calls to UploadPortion(..) after this.
	/*!
		\return tbool: True upon success, False upon error
	*/
	virtual tbool UploadFinish() = 0;

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
	virtual tbool GetLatestError(tchar* pszErrBuff, tint32 iErrBuffSize) = 0;
};

#endif // _ine_i_uploader