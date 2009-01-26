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

#ifndef _ine_i_downloader
#define _ine_i_downloader

/*! \class IDownloader
 * \brief Interface to download a large file from a web address
 *
 * Supplies more complex and buffered method for accessing web-data
 * (lasse)
*/
class IDownloader : public virtual IDestructable
{
public:
	static IDownloader* Create();

	//! Prepare Internet location to connect to
	/*!
		\param pszHost [in]: Name of Internet host. DO NOT PREPEND WITH "http://" !
		\param pszPage [in]: Folder+page to connect to on Internet host
		\param iPort [in]: Port number (default 80)
		\param pszUser [in]: User for authentication (default none)
		\param pszPassword [in]: Password for authentication (default none)
		\param iTimeOutSecs [in]: The max allowed amount of seconds without anything happening before error (default 10)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Init(const tchar* pszHost, const tchar* pszPage, tint32 iPort = 80, const tchar* pszUser = NULL, const tchar* pszPassword = NULL, tint32 iTimeOutSecs = 10) = 0;

	//! Asks the server to give us data of the specified type
	/*!
		\param eMIME [in]: Sets the data type that the downloader wants to read
		\return tbool: True upon success
	*/
	virtual tbool SetReplyMIMEType(E_MIME_Type eMIME) = 0;

	//! Force downloader to use a particular verb for downloads (default it is chosen as GET if no parameters are added and POST otherwise)
	/*!
		\param eVerb [in]: E_VERB_GET: simple download with no parameters in message body (if parameters are added anyway, they'll be sent as part of the URI)<br/>E_VERB_POST: Parameters are sent as part of the message body (default if parameters are present)
		\return tbool: True upon succes, False otherwise
	*/
	virtual tbool SetSpecificVerb(EVerbType eVerb) = 0;

	//! By calling this you'll prevent the downloader from following redirect requests. Must be called after Init(..) since that resets this behaviour
	/*!
		\return tbool: True upon successfully setting the parameter
	*/
	virtual tbool DisableAutoRedirects() = 0;

	//! By calling this you'll allow the downloader to follow redirect requests (default unless DisableAutoRedirects() has been previously called)
	/*!
		\return tbool: True upon successfully setting the parameter
	*/
	virtual tbool EnableAutoRedirects() = 0;

	//! Call this once for each parameter=value set to submit to download location
	/*!
		\param pszParamName [in]: Name of parameter. Must be US-ASCII characters and numbers only (a-z, A-Z and 0-9)
		\param pcParamData [in]: Raw data to submit (may or may not be zero-terminated)
		\param iParamDataLen [in]: If >= 0 it is length of raw data. If -1 it's substituted by strlen(pcParamData)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool AddParam(const tchar* pszParamName, const tchar* pcParamData, tint32 iParamDataLen) = 0;

	//! Start background download of data either to buffer in memory or directly to file. Will run until IsDone() == true
	/*!
		\param pfileForDownload [in]: File that has been previously opened for write-access. Default NULL means that download is kept in memory instead
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Start(IFile* pfileForDownload = NULL) = 0;

	//! Ask for a portion of download data (this is only possible if Start(..) was called with NULL)
	/*!
		\param pszBuffer [out]: Pre-allocated buffer to recieve the next portion of downloaded data
		\param iBufferSize [in]: Size of pre-allocated buffer
		\param piPortionSize [out]: The number of bytes actually returned in this portion. May occationally be 0, caused by e.g. slow network
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool GetReplyPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize) = 0;

	//! Get info on download progress
	/*!
		\param piDownloadProgress [out]: How much of downloaded data has been either 1) written to file or 2) returned via GetReplyPortion(..)
		\param piDownloadSize [out]: The total number of bytes in download data (will be unknown (i.e. 1) until reply starts)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool GetProgress(tint64* piDownloadProgress, tint64* piDownloadSize) = 0;

	//! Breaks an ongoing download operation and releases internal buffers. It's OK to call from different thread.
	virtual tbool Abort() = 0;

	//! Returns True if download has been succesfully completed and all data has been either 1) written to file or 2) delivered to calling app by use of GetReplyPortion(..)
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

#endif // _ine_i_downloader