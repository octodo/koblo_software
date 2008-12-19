
#define CONNECT_TIMEOUT 30 //in seconds
#define MAX_PAGE_SIZE 200000 //Max no of bytes in web page

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

protected:


};


