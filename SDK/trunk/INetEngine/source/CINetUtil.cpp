
#include "ineInternalOS.h"


tbool IINetUtil::PreparePostParameters(const tchar* pszParamName, const tchar* pszParamRawData, tint32 iParamRawDataLen, tchar** ppszPostParameters)
{
	if (ppszPostParameters == NULL) {
		// That's wrong
		return false;
	}

	tbool bFirstParam = (*ppszPostParameters == NULL);
	tint32 iLen_PrevPost = (bFirstParam) ? 0 : strlen(*ppszPostParameters);
	if (iLen_PrevPost == 0)
		bFirstParam = true;

	tint32 iLen_ParamName = CalcURLEncodedLen(pszParamName, -1);
	tint32 iLen_ParamData = CalcURLEncodedLen(pszParamRawData, iParamRawDataLen);
	if (iLen_ParamName <= 0) {
		// That's wrong
		return false;
	}

	// Create new buffer
	tint32 iLen_Total = iLen_PrevPost + 1 + iLen_ParamName;
	if (iLen_ParamData > 0) {
		iLen_Total += 1 + iLen_ParamData;
	}
	tchar* pszNew = new tchar[iLen_Total + 1];
	tchar* pszIx = pszNew;

	// Copy any original part
	if (bFirstParam) {
		*pszIx = '?';
		pszIx++;
	}
	else {
		memcpy(pszNew, *ppszPostParameters, iLen_PrevPost);
		pszIx = pszNew + iLen_PrevPost;
		*pszIx = '&';
		pszIx++;
	}

	// Add parameter name
	tint32 iLen = URLEncode(pszParamName, -1, pszIx);
	pszIx += iLen;

	// Maybe add parameter data
	if (iLen_ParamData) {
		*pszIx = '=';
		pszIx++;
		tint32 iLen = URLEncode(pszParamRawData, iParamRawDataLen, pszIx);
		pszIx += iLen;
	}

	// Release old buffer
	if (*ppszPostParameters) {
		delete [] (*ppszPostParameters);
	}
	// Point to new buffer
	*ppszPostParameters = pszNew;
	return true;
} // PreparePostParameters


tbool URLEncodeChar_IsItAllowedAsRaw(tchar c)
{
	if (
		((c >= 'A') && (c <= 'Z'))
		||
		((c >= 'a') && (c <= 'z'))
		||
		((c >= '0') && (c <= '9'))
		||
		((c == '-') || (c == '_') || (c == '.') || (c == '!'))
		||
		((c == '~') || (c == '*') || (c == '\'') || (c == '(') || (c == ')'))
	) {
		// That's ok to send unencoded
		return true;
	}

	// Nope - this must be encoded
	return false;
} // URLEncodeChar_IsItAllowedAsRaw

tint32 CalcURLEncodedLen_OneChar(tchar c)
{
	if (
		(URLEncodeChar_IsItAllowedAsRaw(c))
		||
		(c == ' ')
	) {
		// Result will be only only one char long
		return 1;
	}

	// Use hex notation
	return 3;
} // CalcURLEncodedLen_OneChar

tint32 IINetUtil::CalcURLEncodedLen(const tchar* pszRawData, tint32 iRawDataLen)
{
	tint32 iCalcLen = 0;
	
	if (iRawDataLen == -1) {
		// It's a zero-terminated string
		tchar c = *pszRawData++;
		while (c) {
			iCalcLen += CalcURLEncodedLen_OneChar(c);
			c = *pszRawData++;
		}
	}
	else {
		// It's binary data - *not* zero-terminated
		for ( ;iRawDataLen > 0; iRawDataLen--) {
			tchar c = *pszRawData++;
			iCalcLen += CalcURLEncodedLen_OneChar(c);
		}
	}

	return iCalcLen;
} // CalcURLEncodedLen


tint32 URLEncode_OneChar(tchar cIn, tchar* pszOutChar)
{
	if (URLEncodeChar_IsItAllowedAsRaw(cIn)) {
		*pszOutChar = cIn;
		return 1;
	}

	if (cIn == ' ') {
		*pszOutChar = '+';
		return 1;
	}

	// Fall back to hex notation
	sprintf(pszOutChar, "%%%02x", cIn);
	return 3;
} // URLEncode_OneChar

tint32 IINetUtil::URLEncode(const tchar* pszRawData, tint32 iRawDataLen, tchar* pszURLEncoded)
{
	tint32 iOutLen = 0;

	if (iRawDataLen == -1) {
		// It's a zero-terminated string
		tchar c = *pszRawData++;
		while (c) {
			tint32 iLen = URLEncode_OneChar(c, pszURLEncoded);
			pszURLEncoded += iLen;
			iOutLen += iLen;
			c = *pszRawData++;
		}
	}
	else {
		// It's binary data - *not* zero-terminated
		for ( ;iRawDataLen > 0; iRawDataLen--) {
			tchar c = *pszRawData++;
			tint32 iLen = URLEncode_OneChar(c, pszURLEncoded);
			pszURLEncoded += iLen;
			iOutLen += iLen;
		}
	}

	return iOutLen;
} // URLEncode


tint32 URLDecode_OneChar(tchar* pszIn, tchar* pcOut)
{
	tchar c0 = *pszIn;
	if (c0 == '+') {
		// Space char
		*pcOut = ' ';
		return 1;
	}

	if (c0 == '%') {
		// Hex encoded char
#ifdef _DEBUG
		tchar c1 = pszIn[1];
		tchar c2 = pszIn[2];
#endif // _DEBUG
		tint32 iCharOut;
		tint32 iConverted = sscanf(pszIn + 1, "%02x", &iCharOut);
		if (iConverted != 1) {
			// Syntax error - not hex
			return 0;
		}
		*pcOut = *((tchar*)&iCharOut);
		return 3;
	}

	if (URLEncodeChar_IsItAllowedAsRaw(c0)) {
		// Raw US-ASCII char
		*pcOut = c0;
		return 1;
	}

	// Undefined error - input is not a URL-encoded char
	return 0;
} // URLDecode_OneChar

tbool IINetUtil::URLDecode(tchar* pszInData, tint32 iInDataLen, tchar* pszOutData, tint32* piOutDataLen)
{
	*piOutDataLen = 0;

	if (iInDataLen == -1) {
		// Input is zero-terminated string
		while (*pszInData) {
			tint32 iConvertedNow = URLDecode_OneChar(pszInData, pszOutData);
			if (iConvertedNow <= 0) {
				// Some error in URL-encoded data
				return false;
			}
			pszInData += iConvertedNow;
			pszOutData++;
			(*piOutDataLen)++;
		}
	}
	else {
		// Input is binary data - not zero-terminated
		for ( ;iInDataLen > 0; iInDataLen--) {
			tint32 iConvertedNow = URLDecode_OneChar(pszInData, pszOutData);
			if (iConvertedNow <= 0) {
				// Some error in URL-encoded data
				return false;
			}
			pszInData += iConvertedNow;
			pszOutData++;
			(*piOutDataLen)++;
		}
	}

	// Success
	return true;
} // URLDecode

/*
std::string IINetUtil::Get_UUID()
{
	return "jim";
}
*/
tbool IINetUtil::GetValue(const tchar* SourceString, tint32* OutputLength, tchar** OutputBuffer)
{
	*OutputBuffer = NULL;
	*OutputLength = 0;
	// Lasse, modified 2007-09-07 - avoid Claus Riber crash
	/*
    if (strstr((const char*) SourceString,"<value>") == NULL || strstr((const char*) SourceString,"</value>") == NULL) {
		return;
	}
    tchar* szValueStart = (tchar*) strstr((const char*) SourceString,"<value>")+strlen("<value>");
    tchar* szValueEnd   = (tchar*) strstr((const char*) SourceString,"</value>");
	*OutputLength = szValueEnd-szValueStart;
	*OutputBuffer = new tchar[*OutputLength+1];
	(*OutputBuffer)[*OutputLength] = 0;
	strncpy((char*) *OutputBuffer,(const char*)szValueStart,*OutputLength);
	*/
	tchar* pszOutput = NULL;
	if (GetValuePtr(SourceString, &pszOutput, OutputLength)) {
		*OutputBuffer = new tchar[*OutputLength+1];
		(*OutputBuffer)[*OutputLength] = 0;
		strncpy((char*) *OutputBuffer, (const char*)pszOutput, *OutputLength);

		return true;
	}
	else {
		// Return empty string
		*OutputBuffer = new tchar[1];
		(*OutputBuffer)[0] = '\0';
		*OutputLength = 0;
	}
	return false;
	// .. Lasse
}


tbool IINetUtil::FindLowerCaseString(const tchar* pszLowerCaseString, const tchar* pszBuffer, tint32 iBufferLen, tchar** ppszIndexAt, tchar** ppszIndexAfter)
{
	tint32 iLenLowerCaseString = strlen(pszLowerCaseString);
	tchar* pcBufIndexAt = (tchar*)pszBuffer;
	tint32 iBufferRemainder = iBufferLen - iLenLowerCaseString;
	while ((*pcBufIndexAt) && (iBufferRemainder >= 0)) {
		tchar* pcBuf = pcBufIndexAt;
		tchar cBuf = *pcBuf;
		if ((cBuf >= 'A') && (cBuf <= 'Z'))
			cBuf += (tchar)('a'-'A');
		if (cBuf == *pszLowerCaseString) {
			// First char match found
			tchar* pcStr = ((tchar*)pszLowerCaseString) + 1;
			tchar* pcBuf = pcBufIndexAt + 1;
			// Look for the rest of the string
			tbool bMatches = true;
			do {
				if (*pcStr == '\0') {
					// End of string reached - a match was found
					*ppszIndexAt = pcBufIndexAt;
					*ppszIndexAfter = pcBuf;
					return true;
				}
				tchar cBuf = *pcBuf;
				if ((cBuf >= 'A') && (cBuf <= 'Z'))
					cBuf += (tchar)('a'-'A');
				if (cBuf == *pcStr) {
					// So far so good - advance to next
					pcStr++;
					pcBuf++;
				}
				else {
					// Mismatch - start from next
					bMatches = false;
					pcBufIndexAt++;
					iBufferRemainder--;
				}
			} while (bMatches);
		}
		else {
			// Mismatch - advance to next
			pcBufIndexAt++;
			iBufferRemainder--;
		}
	}
	return false;
}


tbool IINetUtil::GetTagContentsPtr(const tchar* pszTagNameConst, const tchar* pszSourceString, tint32 iSourceLen, tchar** ppszOutput, tint32* piOutputLength)
{
	*ppszOutput = NULL;
	*piOutputLength = 0;

	// Create lower case string
	tchar pszTagName[256];
	tchar* pC = pszTagName;
	tchar* pCConst = (tchar*)pszTagNameConst;
	for (;;) {
		*pC = *pCConst;
		if ((*pC >= 'A') && (*pC <= 'Z'))
			*pC += (tchar)('a'-'A');
		if (*pC == '\0')
			break;
		pC++;
		pCConst++;
	}

	tchar pszStartTag[256];
	sprintf(pszStartTag, "<%s", pszTagName);
	tchar pszEndTag[256];
	sprintf(pszEndTag, "</%s", pszTagName);

	// Search for start tag
	tchar* pszStartTagIx;
	tchar* pszStartTagAfter;
	if (!FindLowerCaseString(pszStartTag, pszSourceString, iSourceLen, &pszStartTagIx, &pszStartTagAfter))
		return false;

	// Start tag found - advance to '>' char
	tchar cSecondLast = '\0';
	while (*pszStartTagAfter != '>')
	{
		if (*pszStartTagAfter == '\0')
			return false;
		cSecondLast = *pszStartTagAfter;
		pszStartTagAfter++;
	}
	// Advance past '>' char
	pszStartTagAfter++;

	tchar* pszEndTagIx;
	tchar* pszEndTagAfter;
	if (cSecondLast == '/') {
		// No end tag (= empty contents)
		pszEndTagIx = pszEndTagAfter = pszStartTagAfter;
	}
	else {
		// Search for end tag
		tint32 iLenAfterStartTag = iSourceLen - (pszStartTagAfter - pszSourceString);
		if (!FindLowerCaseString(pszEndTag, pszStartTagAfter, iLenAfterStartTag, &pszEndTagIx, &pszEndTagAfter))
			return false;
	}

	// End tag found - calc length
	*piOutputLength = (pszEndTagIx - pszStartTagAfter);
	// Return pointer to value
	*ppszOutput = pszStartTagAfter;
	return true;
}

tbool IINetUtil::GetValuePtr(const tchar* szSourceString, tchar** ppszOutput, tint32* piOutputLength) {
	return GetTagContentsPtr("value", szSourceString, strlen(szSourceString), ppszOutput, piOutputLength);
/*	*ppszOutput = NULL;
	*piOutputLength = 0;

	// Search for start tag
	tchar* pszIx0 = (tchar*)strstr((const char*) szSourceString, "<value");
	if (pszIx0 == NULL)
		return false;

	// Start tag found - advance to '>' char
	pszIx0 += 6;
	while (*pszIx0 != '>')
	{
		if (*pszIx0 == '\0')
			return false;
		pszIx0++;
	}
	// Advance past '>' char
	pszIx0++;

	// Search for end tag
    tchar* pszIx1 = (tchar*)strstr((const char*)pszIx0, "</value");
	if (pszIx1 == NULL)
		return false;

	// End tag found - calc length
	*piOutputLength = (pszIx1 - pszIx0);
	// Return pointer to value
	*ppszOutput = pszIx0;
	return true;
	*/
}

void IINetUtil::ReleaseBuffer(tchar** Buffer) {
    if (*Buffer == NULL) return;
	delete *Buffer;
	*Buffer = NULL;
}

bool IINetUtil::IsResultOK(const tchar* SourceString) {
    return strstr(SourceString,"<result status=\"ok\">") != NULL;
}

tfloat64 IINetUtil::GetNumericWebValue(const tchar* Parameters, const tchar* pszServer, const tchar* FileName) throw(IException*)
{
	int length;
	tchar* szS;
	try {
		GetWebFile(Parameters,pszServer,FileName,&length,&szS);
	}
	catch (IException* ex) {
		ReleaseBuffer(&szS);
        throw(ex);
	}
	catch (...) {
		//Any other error
		ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || !IsResultOK(szS)) {
		ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
    int valueLength;
	tchar* szValue;
	GetValue(szS, &valueLength, &szValue);
	if (valueLength == 0) {
		IINetUtil::ReleaseBuffer(&szS);
		return 0;
	}
	tfloat64 r = atof((const char*)szValue);
	ReleaseBuffer(&szValue);
	ReleaseBuffer(&szS);
	return r;
}

bool IINetUtil::GetMiddleStr(tchar **s, const char *sStart, const char *sEnd, char *sMiddle)
{
	tchar *pStart;
	tchar *pEnd;
	if ((pStart = (tchar *) strstr((const char *) *s, sStart)) == NULL) return false;
	pStart = pStart+strlen(sStart);
	if ((pEnd = (tchar *) strstr((const char *) pStart, sEnd)) == NULL) return false;
	strncpy((char *) sMiddle, (const char *) pStart, pEnd-pStart);
	sMiddle[pEnd-pStart] = 0;
    *s = pEnd + strlen(sEnd);
	return true;
}


tbool CINetUtil::URLEncode_Internal(const tchar* pszRaw, std::string& rsEncoded)
{
	rsEncoded = "";

	if ((pszRaw) && (*pszRaw != '\0')) {
		tint32 iLenRaw = strlen(pszRaw);
		tint32 iLenEncoded = IINetUtil::CalcURLEncodedLen(pszRaw, iLenRaw);
		
		if (iLenEncoded > 0) {
			tchar* pszEncoded = new tchar[iLenEncoded + 1];
			tint32 iReally = IINetUtil::URLEncode(pszRaw, iLenRaw, pszEncoded);
			if (iReally != iLenEncoded) {
				// Error: Insane conversion
				delete[] pszEncoded;
				return false;
			}
			rsEncoded = pszEncoded;
			delete[] pszEncoded;
		}
	}

	return true;
} // UrlEncode
