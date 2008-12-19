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
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.

/*! \class IINetUtil
 * \brief Interface to access a web address
 *
 * Supplies simple methods for accessing web-data
*/
class IINetUtil
{
public:
	//! Constructor
	IINetUtil() {}

	//! Destructor
	virtual ~IINetUtil() {}

	//! Read a given file from a web server using HTTP and port 80
	//! Uses POST if Parameters != NULL else GET
	//! Buffer *must* be released with ReleaseBuffer(..) afterwards.
	/*!
		\param Parameters [in]: inputstring (eg. userid=cri&password=cri), NULL if none
		\param pszServer [in]: The server (DO NOT include protocol prefix!, i.e. "http://"). Can be fx "172.16.3.4" or "koblo.com"
		\param pszFileName [in]: The actual path and file (eg. "/giraffe/checkxxx.aspx")
		\param OutputLength [out]: Number of bytes returned
		\param OutputBuffer [out]: Handle to a bytebuffer which is allocated and contains the web file
	*/
	static void GetWebFile(const tchar* PostParameters, const tchar* pszServer, const tchar* pszFileName, tint32* OutputLength, tchar** OutputBuffer);

	//! Assembles a URL-encoded post (a.k.a. input) parameter string for use in the GetWebFile(..) method
	/*!
		\param pszParamName [in]: Name of the parameter
		\param pszParamRawData [in]: Raw data of the parameter (*not* URL-encoded)
		\param iParamRawDataLen [in]: Length of the raw data. Set to -1 if the data is a zero terminated string => strlen(..) will be used.
		\param ppszPostParameters [out]: Recieves a pointer to an assembled post-parameter string. If *ppszPostParameters is previosly NULL (as it <b>must</b> be upon first call!) the new string starts with '?', else the previous contents is appended a '&' and the newly generated contents. Buffer will be reallocated upon every call.<br/><b>Note!</b> Remember to <b>delete</b> it after use!
		\return tbool: True upon success
	*/
	static tbool PreparePostParameters(const tchar* pszParamName, const tchar* pszParamRawData, tint32 iParamRawDataLen, tchar** ppszPostParameters);

	//! Find the value element of a string (<value>xxx</value>), allocate the buffer and copy the value (xxx) to it
	//! If no value element is found OutputLength is 0 and OutputBuffer is empty string
	//! Note: For speed optimization use GetValuePtr instead - that will save a buffer copy (Lasse)
	//! Buffer *must* be released with ReleaseBuffer(..) afterwards.
	/*!
		\param SourceString [in]: inputstring to be searched
		\param OutputLength [out]: Number of bytes returned
		\param OutputBuffer [out]: Handle to a bytebuffer which is allocated and contains the value
		\return tbool: True if a value tag was found (also true for empty contents - check OutputLength for that instead)
	*/
	static tbool GetValue(const tchar* SourceString, tint32* OutputLength, tchar** OutputBuffer);

	//! Find the first a tag element of a string (<tag>xxx</tag>). No buffer copying is done.
	/*!
		\param pszTagName [in]: The name of the tag to look for. <b>Don't include the '&lt;' and '&gt;' chars!</b>
		\param pszSourceString [in]: inputstring to be searched (doesn't have to be zero terminated)
		\param iSourceLen [in]: The length of the inputstring
		\param ppszOutput [out]: Pointer (into the inputstring buffer) to the value (not zero terminated)
		\param piOutputLength [out]: Number of chars in value string
		\return tbool: True on success
	*/
	static tbool GetTagContentsPtr(const tchar* pszTagName, const tchar* pszSourceString, tint32 iSourceLen, tchar** ppszOutput, tint32* piOutputLength);

	//! Find the value element of a string (<value>xxx</value>). No buffer copying is done.
	//! If no value element is found OutputLength is 0 and OutputBuffer is NULL
	/*!
		\param szSourceString [in]: inputstring to be searched
		\param ppszOutput [out]: Pointer (into the inputstring buffer) to the value (not zero terminated)
		\param piOutputLength [out]: Number of chars in value string
		\return tbool: True on success
	*/
	static tbool GetValuePtr(const tchar* szSourceString, tchar** ppszOutput, tint32* piOutputLength);

	//! Find string in contents. The string to search for must be lowercase
	/*!
		\param pszLowerCaseString [in]: Text to search for. Must be all lowercase!
		\param pszBuffer [in]: Contents read from the internet (is probably in XML format)
		\param iBufferLen [in]: Length of contents
		\param ppszIndexAt [out]: Upon success: Receives a pointer to the first index of location
		\param ppszIndexAfter [out]: Upon success: Receives a pointer to the rest of the contents (following the found text)
		\return tbool: True upon success, False upon error
	*/
	static tbool FindLowerCaseString(const tchar* pszLowerCaseString, const tchar* pszBuffer, tint32 iBufferLen, tchar** ppszIndexAt, tchar** ppszIndexAfter);

	//! Deallocate the buffer allocated by GetWebFile or GetValue - can be called allways, but no need when value null
	//! \param Buffer [in]: Handle to the buffer	
	static void ReleaseBuffer(tchar** Buffer);

	//! Checks ok element in string with value ok (<result status="ok">)
	//! (lasse) Note! May not be relevant for Koblo Studio ???
	/*!
        \param SourceString [in]: inputstring to be searched
	*/
	static bool IsResultOK(const tchar* SourceString);

	//! Reads web page and return integer result. Handles/throws exceptions
	//! (lasse) Note! May not be relevant for Koblo Studio ???
	/*!
		\param Parameters [in]: inputstring (eg. userid=cri&password=cri), NULL if none
		\param pszServer [in]: The server (DO NOT include protocol prefix!, i.e. "http://"). Can be fx "172.16.3.4" or "koblo.com"
		\param FileName [in]: The actual file (eg. "checkxxx.aspx")
	*/
    static tfloat64 GetNumericWebValue(const tchar* Parameters, const tchar* pszServer, const tchar* FileName) throw(IException*);

	//!returns the middle string between matching start and end and advances the buffer
	//! (lasse) Note! May not be relevant for Koblo Studio ???
	static bool GetMiddleStr(tchar **s, const char *sStart, const char *sEnd, char *sMiddle);

	//! Runs through the data and finds out how long a URL-encoded version would be
	/*!
		\param pszRawData [in]: The string or raw data to encode
		\param iRawDataLen [in]: Length of the raw data. If -1 strlen(pszRawData) is used
		\return tint32: Calculated length of the URL-encoded result string. This does <b>not</b> count a trailing zero-char!
	*/
	static tint32 CalcURLEncodedLen(const tchar* pszRawData, tint32 iRawDataLen);

	//! URL-encodes the data (prepares for sending it to a web-server)
	/*!
		\param pszRawData [in]: The string or raw data to encode
		\param iRawDataLen [in]: Length of the raw data. If -1 strlen(pszRawData) is used
		\param pszURLEncoded [out]: Receives the URL-encoded result string. This <b>must</b> be allocated with enough space to hold string including a trailing zero-char!
		\return tint32: The length of the URL-encoded output
	*/
	static tint32 URLEncode(const tchar* pszRawData, tint32 iRawDataLen, tchar* pszURLEncoded);

	// URL-decodes the data (data was returned from a web-server)<br/><b>Note!</b> It is safe to do character replacing in-place (by submitting same pointer for input and output) as raw data is always shorter than or same length as encoded data
	/*!
		\param pszInData [in]: The encoded data input
		\param iInDataLen [in]: Length of the encoded data input. If -1 strlen(pszData_InOut) is used
		\param pszOutData [out]: Will receive the raw data output (no trailing zero-char will be applied!)
		\param piOutDataLen: Receives length of the raw data output
		\return tbool: True upon success, False if some error in the URL-encoding was found. If False the string in pszData_InOut may be half converted and should be discarded
	*/
	static tbool URLDecode(tchar* pszInData, tint32 iInDataLen, tchar* pszOutData, tint32* piOutDataLen);

protected:


};


