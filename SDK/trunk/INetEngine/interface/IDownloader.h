// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool Init(tchar* pszHost, tchar* pszPage, tint32 iPort = 80) = 0;

	//! Call this once for each parameter=value set to submit to download location
	/*!
		\param pszParamName [in]: Name of parameter. Must be US-ASCII characters and numbers only (a-z, A-Z and 0-9)
		\param pcParamData [in]: Raw data to submit (may or may not be zero-terminated)
		\param iParamDataLen [in]: If >= 0 it is length of raw data. If -1 it's substituted by strlen(pcParamData)
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool AddParam(tchar* pszParamName, tchar* pcParamData, tint32 iParamDataLen) = 0;

	//! Poll for a portion of download data
	/*!
		\param pszBuffer [out]: Pre-allocated buffer to recieve the next portion of downloaded data
		\param iBufferSize [in]: Size of pre-allocated buffer
		\param piPortionSize [out]: The number of chars returned in this portion. May occationally be 0, caused by e.g. slow network
		\return tbool: True upon success, false upon error.
	*/
	virtual tbool DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize) = 0;

	//! Breaks an ongoing download operation and releases internal buffers
	virtual tbool Abort() = 0;

	//! Returns True if download has been succesfully completed
	virtual tbool IsDone() = 0;

};

#endif // _ine_i_downloader