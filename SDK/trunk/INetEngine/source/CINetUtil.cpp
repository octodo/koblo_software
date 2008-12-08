
#include "ineInternalOS.h"

tbool IINetUtil::GetValue(tchar* SourceString, tint32* OutputLength, tchar** OutputBuffer) {
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


tbool IINetUtil::FindLowerCaseString(tchar* pszLowerCaseString, tchar* pszBuffer, tint32 iBufferLen, tchar** ppszIndexAt, tchar** ppszIndexAfter)
{
	tint32 iLenLowerCaseString = strlen(pszLowerCaseString);
	tchar* pcBufIndexAt = pszBuffer;
	tint32 iBufferRemainder = iBufferLen - iLenLowerCaseString;
	while ((*pcBufIndexAt) && (iBufferRemainder >= 0)) {
		tchar* pcBuf = pcBufIndexAt;
		tchar cBuf = *pcBuf;
		if ((cBuf >= 'A') && (cBuf <= 'Z'))
			cBuf += (tchar)('a'-'A');
		if (cBuf == *pszLowerCaseString) {
			// First char match found
			tchar* pcStr = pszLowerCaseString + 1;
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


tbool IINetUtil::GetTagContentsPtr(const tchar* pszTagNameConst, tchar* pszSourceString, tint32 iSourceLen, tchar** ppszOutput, tint32* piOutputLength)
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

tbool IINetUtil::GetValuePtr(tchar* szSourceString, tchar** ppszOutput, tint32* piOutputLength) {
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

bool IINetUtil::IsResultOK(tchar* SourceString) {
    return strstr((const char*) SourceString,"<result status=\"ok\">") != NULL;
}

tfloat64 IINetUtil::GetNumericWebValue(tchar* Parameters, tchar* pszServer, tchar* FileName) throw(IException*)
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

