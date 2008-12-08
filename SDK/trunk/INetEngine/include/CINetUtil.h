
#define CONNECT_TIMEOUT 30 //in seconds
#define MAX_PAGE_SIZE 200000 //Max no of bytes in web page

class CINetUtil : public virtual IINetUtil
{
public:
	//! Constructor
	CINetUtil() {}

	//! Destructor
	virtual ~CINetUtil() {}

	/*
	//! IINetUtil implementation
	static void GetWebFile(tchar* PostParameters, tchar* FileName, tint32* OutputLength, tchar** OutputBuffer);

	//! IINetUtil implementation
	static tbool GetValue(tchar* SourceString, tint32* OutputLength, tchar** OutputBuffer);

	//! IINetUtil implementation
	static tbool GetTagContentsPtr(const tchar* pszTagName, tchar* pszSourceString, tint32 iSourceLen, tchar** ppszOutput, tint32* piOutputLength);

	//! IINetUtil implementation
	static tbool GetValuePtr(tchar* szSourceString, tchar** ppszOutput, tint32* piOutputLength);

	//! IINetUtil implementation
	static tbool FindLowerCaseString(tchar* pszLowerCaseString, tchar* pszBuffer, tint32 iBufferLen, tchar** ppszIndexAt, tchar** ppszIndexAfter);

	//! IINetUtil implementation
	static void ReleaseBuffer(tchar** Buffer);

	//! IINetUtil implementation
	static bool IsResultOK(tchar* SourceString);

	//! IINetUtil implementation
    static tfloat64 GetNumericWebValue(tchar* Parameters, tchar* FileName) throw(IException*);

	//! IINetUtil implementation
    static bool GetNextElement(tchar **S, tchar *Name, int NameLength, bool *IsFolder, tint32 *Price);

	//! IINetUtil implementation
	static bool GetMiddleStr(tchar **s, const char *sStart, const char *sEnd, char *sMiddle);
	*/

protected:


};


