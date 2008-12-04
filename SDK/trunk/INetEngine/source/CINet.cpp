/*!	\file CINet.cpp
	\author Michael Olsen / Claus Riber
	\date 23/May/2005
	\date 23/May/2005
*/

#include "ineInternalOS.h"


/*

Til Claus: Hvordan du laver en ny exception type.

Exceptions er i BU biblioteket.

I "interface" er en fil, IException.h
Her skal du lave
A: En type (normalt ikke nødvendigt). Det er en generisk grupering
B: En grund "reason". Det er en undergrupering

I "source" er en fil, CException.cpp
Her skal du lave
A: En case i GetTypeText (hvis du lavede en ny type)
B: En case i GetReasonText

	// Eksempel på hvordan man kaster en IException*
	throw IException::Create(IException::TypeNetwork,
		IException::ReasonNetworkCannotOpen,
		(tchar*)"Network interface cannot be opened");

	// Alternativt, hvis den sidste tekst ikke skal med (i.e. det er ikke en tekst som brugeren skal se alligevel)
	throw IException::Create(IException::TypeNetwork,
		IException::ReasonNetworkCannotOpen);

*/

IINet::TUserName IINet::UserNameDefault = "";

IINet::TPassword IINet::PasswordDefault = "";


CINet::CINet()
{
}

CINet::~CINet()
{
}

IINet* IINet::Create()
{
	return dynamic_cast<IINet*>(new CINet());
}

void CINet::Destroy()
{
	delete dynamic_cast<CINet*>(this);
}

tbool CINet::CheckOnlineStatus() throw()
{
	int length;
	tchar* szS;
	tchar* szParams = (tchar*)"";
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"CheckOnlineStatus.aspx",&length,&szS);
	}
	catch (...) {
		//Any error
		CINetUtil::ReleaseBuffer(&szS);
		return false;
	}
	if ((szS == NULL) || (strstr((const char*) szS,"<result status=\"ok\"><status>OK</status><value>True</value></result>") == NULL)) {
		CINetUtil::ReleaseBuffer(&szS);
		return false;
	}
	CINetUtil::ReleaseBuffer(&szS);
	return true;
}

tint32 CINet::GetInterfaceVersion(tchar* pBuffer, tint32 iBufferSize) throw(IException*)
{
	// Lasse, changed 2007-09-07 - avoid crashing: delete Claus Riber
	/*
	int length;
	tchar* szS;
	try {
		CINetUtil::GetWebFile(NULL, (tchar*)"GetInterfaceVersion.aspx",&length,&szS);
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || strstr((const char*) szS,"<result status=\"ok\">") == NULL) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
    int valueLength;
	tchar* szValue;
	CINetUtil::GetValue(szS, &valueLength, &szValue);
	if (valueLength+1 > iBufferSize) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeStorageLimitationExceeded, IException::ReasonGeneric, EXCEPTION_INFO);
	}
	memcpy(pBuffer, szValue, valueLength);
	pBuffer[valueLength] = 0;
	CINetUtil::ReleaseBuffer(&szValue);
	CINetUtil::ReleaseBuffer(&szS);
	return valueLength+1;
	*/
	return 0;
	// .. Lasse
}


tbool CINet::GetInterfaceVersionEx(tchar* pszVersion, tint32 iVersionBufferSize, tint32 iProductID, tuint16 uiKSPIVersionMajor, tuint16 uiKSPIVersionMinor, tuint16 uiKSPIRevision, tbool* pbUpgradeNeeded, tbool* pbUpgradeAvailable, tchar* pszUpgradeURL, tint32 iUpgradeURLBufferSize)
{
	tbool bSuccess = false;

	tint iTries = 3;
	do {
		int iReceivedLen = 0;
		tchar* szReceivedBuffer = NULL;
		try {
			tchar pszInterfaceCall[1024];
#ifdef WIN32
			sprintf(
				pszInterfaceCall, 
				"GetInterfaceVersion.aspx?sOS=Windows&iProductID=%u&uiVer0=%u&uiVer1=%u&uiVer2=%u",
				iProductID,
				uiKSPIVersionMajor, uiKSPIVersionMinor, uiKSPIRevision
			);
#endif // #ifdef WIN32
#ifdef _Mac
			sprintf(
					pszInterfaceCall, 
					"GetInterfaceVersion.aspx?sOS=OSX&iProductID=%u&uiVer0=%u&uiVer1=%u&uiVer2=%u",
					iProductID,
					uiKSPIVersionMajor, uiKSPIVersionMinor, uiKSPIRevision
					);
#endif // #ifdef _Mac

			CINetUtil::GetWebFile(NULL, pszInterfaceCall, &iReceivedLen, &szReceivedBuffer);

			tbool bStatusOK = false;
			tchar* pszStatus = NULL;
			tint32 iStatusLen = 0;
			if (CINetUtil::GetTagContentsPtr("status", szReceivedBuffer, iReceivedLen, &pszStatus, &iStatusLen))
			{
				tchar* pszIndexAt;
				tchar* pszIndexAfter;
				if (
					(iStatusLen == 2)
					&&
					(CINetUtil::FindLowerCaseString("ok", pszStatus, iStatusLen, &pszIndexAt, &pszIndexAfter))
				)
				{
					bStatusOK = true;
				}
			}

			if (bStatusOK)
			{
				tchar* pszValue = NULL;
				tint32 iValueLen = 0;
				if ((CINetUtil::GetValuePtr(szReceivedBuffer, &pszValue, &iValueLen)) && (iValueLen < iVersionBufferSize))
				{
					// Copy version info into calling method's buffer
					memcpy(pszVersion, pszValue, iValueLen);
					pszVersion[iValueLen] = '\0';

					// Check for upgrades
					tchar* pszDummy = NULL;
					tint iLenDummy = 0;
					*pbUpgradeAvailable = CINetUtil::GetTagContentsPtr("bUpdateAvailable", szReceivedBuffer, iReceivedLen, &pszDummy, &iLenDummy);
					*pbUpgradeNeeded = CINetUtil::GetTagContentsPtr("bUpdateNeeded", szReceivedBuffer, iReceivedLen, &pszDummy, &iLenDummy);

					// Where do we upgrade from?
					tchar* pszURL = NULL;
					tint32 iURLLen = 0;
					CINetUtil::GetTagContentsPtr("sUpdateURL", szReceivedBuffer, iReceivedLen, &pszURL, &iURLLen);
					if ((iURLLen > 0) && (iURLLen < iUpgradeURLBufferSize))
					{
						// Copy upgrade URL into calling method's buffer
						memcpy(pszUpgradeURL, pszURL, iURLLen);
						pszUpgradeURL[iURLLen] = '\0';
					}
					else
					{
						// Couldn't save URL - not there or too long
						pszUpgradeURL[0] = '\0';
					}

					bSuccess = true;
				}
				CINetUtil::ReleaseBuffer(&szReceivedBuffer);
			}
		}
		catch (IException* ex) {
			CINetUtil::ReleaseBuffer(&szReceivedBuffer);
			ex->Destroy();
		}
		catch (...) {
			//Any other error
			CINetUtil::ReleaseBuffer(&szReceivedBuffer);
		}
	} while ((!bSuccess) && (--iTries > 0));

	if (!bSuccess)
	{
		pszVersion[0] = '\0';
		*pbUpgradeAvailable = false;
		*pbUpgradeNeeded = false;
	}

	return bSuccess;
}


#define CheckUserName_PAR "username=%s&password=%s"
tbool CINet::CheckUserName(const IINet::TUserName& UserName, const IINet::TPassword& Password) throw(IException*)
{
	int length;
	tchar* szS;
	tbool r = false;
	tchar* szParams = new tchar[strlen(CheckUserName_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)];
	sprintf((char *) szParams,CheckUserName_PAR, UserName, Password);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"CheckUserName.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (...) {
		//Any error
		CINetUtil::ReleaseBuffer(&szS);
		return false;
	}
	if (szS == NULL || !CINetUtil::IsResultOK(szS) || (strstr((const char*) szS,"<value>True</value>") == NULL && strstr((const char*) szS,"<value>False</value>") == NULL)) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (strstr((const char*) szS,"<value>True</value>") != NULL) r = true;
	CINetUtil::ReleaseBuffer(&szS);
	return r;
}

#define GetCredit_PAR "username=%s&password=%s"
tfloat64 CINet::GetCredit(const IINet::TUserName& UserName, const TPassword& Password) throw(IException*)
{
	tchar* szParams = new tchar[strlen(GetCredit_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)];
	sprintf((char *) szParams,GetCredit_PAR, UserName, Password);
    tfloat64 r = CINetUtil::GetNumericWebValue(szParams, (tchar*) "GetCredit.aspx");
    delete [] szParams;
    return r;
}

#define GetElementCount_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetElementCount(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*)
{
	//System path only
	if (Path.PathType != PathTypeSystem) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);

	tchar* szParams = new tchar[strlen(GetElementCount_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)];
	sprintf((char *) szParams,GetElementCount_PAR, UserName, Password, Path.pszPathName);
    tint32 r = (tint32)CINetUtil::GetNumericWebValue(szParams, (tchar*) "GetElementCount.aspx");
    delete [] szParams;
    return r;
}

#define GetElements_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetElements(const TUserName& UserName, const TPassword& Password, SPath Path, TElementText* pTexts, EElementType* pTypes, tint32* piPrices, tint32 iMaxNrOfElements) throw(IException*)
{
	//System path only
	if (Path.PathType != PathTypeSystem) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);

	int length;
	tchar* szS;
	tchar* szParams = new tchar[strlen(GetElements_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)];
	sprintf((char *) szParams,GetElements_PAR, UserName, Password, Path.pszPathName);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"GetElements.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || !CINetUtil::IsResultOK(szS)) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeNetwork, IException::ReasonRequestFailed, EXCEPTION_INFO);
	}

	tchar *buf = szS;
	int count = 0;
	TElementText name;
	bool isFolder;
	int price;
  	while (CINetUtil::GetNextElement(&buf, name, sizeof(name)-1, &isFolder, &price)) {
	  if (count > iMaxNrOfElements) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeStorageLimitationExceeded, IException::ReasonGeneric, EXCEPTION_INFO);
	  }
      strcpy((char*) (pTexts[count]), (char*) name);
      pTypes[count] = isFolder?ElementTypeFolder:ElementTypeFile;
	  piPrices[count] = price;
	  count++;
	}
    CINetUtil::ReleaseBuffer(&szS);
	return count;
}

#define GetFileSize1_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetFileSize1(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*)
{
	//System path only
	if (Path.PathType != PathTypeSystem) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);

	tchar* szParams = new tchar[strlen(GetFileSize1_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)];
	sprintf((char *) szParams,GetFileSize1_PAR, UserName, Password, Path.pszPathName);
    tint32 r = (tint32)Base64UrlEncode::DecodeGetLength((tint32)CINetUtil::GetNumericWebValue(szParams, (tchar*) "GetFileSize1.aspx"));
	delete [] szParams;
	return r;
}

#define GetFileSize2_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetFileSize2(const TUserName& UserName, const TPassword& Password, SPath Path) throw(IException*)
{
	//System path only
	if (Path.PathType != PathTypeSystem) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);

	tchar* szParams = new tchar[strlen(GetFileSize2_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)];
	sprintf((char *) szParams,GetFileSize2_PAR, UserName, Password, Path.pszPathName);
    tint32 r = (tint32)Base64UrlEncode::DecodeGetLength((tint32)CINetUtil::GetNumericWebValue(szParams, (tchar*) "GetFileSize2.aspx"));
	delete [] szParams;
	return r;
}

#define GetFile1_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetFile1(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*)
{
	return GetFile(true, UserName, Password, Path, pBuffer, iBufferSize);
}

tint32 CINet::GetFile2(const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*)
{
	return GetFile(false, UserName, Password, Path, pBuffer, iBufferSize);
}

#define PutFiles_PAR "username=%s&password=%s&path=%s&file1=%s&file2=%s"
void CINet::PutFiles(const TUserName& UserName, const TPassword& Password, SPath Path, const void* pBuffer1, tint32 iBufferSize1, const void* pBuffer2, tint32 iBufferSize2) throw(IException*)
{
	int length;
	tchar* szS;
	//User path type - for safety
	if (Path.PathType != PathTypeUser) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);
	//Check file sizes
	if (iBufferSize1 > MAX_FILE_SIZE || iBufferSize2 > MAX_FILE_SIZE)
		throw IException::Create(IException::TypeFile, IException::ReasonFileTooLarge, EXCEPTION_INFO);
	tchar* szParams;
	int size = strlen(PutFiles_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)+Base64UrlEncode::EncodeGetLength(iBufferSize1)+Base64UrlEncode::EncodeGetLength(iBufferSize2);
	szParams = new tchar[size];
	tchar* szFile1Encoded = new tchar[Base64UrlEncode::EncodeGetLength(iBufferSize1)+1];
    int fileSize1 = Base64UrlEncode::Encode(szFile1Encoded, pBuffer1, iBufferSize1);
	szFile1Encoded[fileSize1] = 0;
	tchar* szFile2Encoded = new tchar[Base64UrlEncode::EncodeGetLength(iBufferSize2)+1];
	int fileSize2 = Base64UrlEncode::Encode(szFile2Encoded, pBuffer2, iBufferSize2);
	szFile2Encoded[fileSize2] = 0;
	sprintf((char *) szParams,PutFiles_PAR, UserName, Password, Path.pszPathName, szFile1Encoded, szFile2Encoded);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"PutFiles.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || !CINetUtil::IsResultOK(szS)) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeNetwork, IException::ReasonRequestFailed, EXCEPTION_INFO);
	}
    CINetUtil::ReleaseBuffer(&szS);
}


#define Purchase_PAR "username=%s&password=%s&path=%s&price=%d"
void CINet::Purchase(const TUserName& UserName, const TPassword& Password, SPath Path, tint32 iPrice) throw(IException*)
{
	int length;
	tchar* szS;
	tchar* szParams;
	
	//System path only
	if (Path.PathType != PathTypeSystem) throw IException::Create(IException::TypeCode, IException::ReasonWrongPathType, EXCEPTION_INFO);

	int size = strlen(Purchase_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)+10;
	szParams = new tchar[size];
	sprintf((char *) szParams,Purchase_PAR, UserName, Password, Path.pszPathName, iPrice);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"Purchase.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || !CINetUtil::IsResultOK(szS)) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeNetwork, IException::ReasonRequestFailed, EXCEPTION_INFO);
	}
    CINetUtil::ReleaseBuffer(&szS);
}

#define GetFile_PAR "username=%s&password=%s&path=%s"
tint32 CINet::GetFile(bool File1, const TUserName& UserName, const TPassword& Password, SPath Path, void* pBuffer, tint32 iBufferSize) throw(IException*)
{
	int length;
	tchar* szS;
	tchar* szParams = new tchar[strlen(GetFile_PAR)+strlen((const char *) UserName)+strlen((const char *) Password)+strlen((const char *) Path.pszPathName)];
	sprintf((char *) szParams,GetFile_PAR, UserName, Password, Path.pszPathName);
	try {
		if (File1) CINetUtil::GetWebFile(szParams,(tchar*)"GetFile1.aspx",&length,&szS);
		else CINetUtil::GetWebFile(szParams,(tchar*)"GetFile2.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if (szS == NULL || strstr((const char*) szS,"<result status=\"ok\">") == NULL) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
    int valueLength;
	tchar* szValue;
	CINetUtil::GetValue(szS, &valueLength, &szValue);
	if (Base64UrlEncode::DecodeGetLength(valueLength) > (unsigned int)iBufferSize) {
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeStorageLimitationExceeded, IException::ReasonFileGeneric, EXCEPTION_INFO);
	}
    tint32 lenght = Base64UrlEncode::Decode(pBuffer, szValue, valueLength);
	CINetUtil::ReleaseBuffer(&szValue);
	CINetUtil::ReleaseBuffer(&szS);
	return lenght;
}

#define RegisterPOP_PAR "pop=%s&challenge=%s"
tbool CINet::RegisterPOP(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void* pResponse, tint32 iResponseSize) throw(IException*)
{
	int length;
	tchar* szS;
	tchar* szParams;
	int size = strlen(RegisterPOP_PAR)+strlen((const char *) POP)+Base64UrlEncode::EncodeGetLength(iChallengeSize);
	szParams = new tchar[size];
	tchar* szChallengeEncoded = new tchar[Base64UrlEncode::EncodeGetLength(iChallengeSize)+1];
    int challengeSize = Base64UrlEncode::Encode(szChallengeEncoded, pChallenge, iChallengeSize);
	szChallengeEncoded[challengeSize] = 0;
	sprintf((char *) szParams,RegisterPOP_PAR, POP, szChallengeEncoded);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"RegisterPOP.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
        throw(ex);
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
	}
	if ((strstr((const char*) szS,"<result status=\"fail\"><status>Exception:POP registered for other challenge code</status></result>") != NULL)) {
		CINetUtil::ReleaseBuffer(&szS);
		return false;
	}
	// Lasse, the following code should be modified at some time - but it works
	if (szS == NULL || strstr((const char*) szS,"<result status=\"ok\">") == NULL) {
		// Lasse - modified 2007-05-11
//		CINetUtil::ReleaseBuffer(&szS);
//		throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
		const tchar pszStatusTag[] = "<status>";
		const tint iStatusTagLen = sizeof(pszStatusTag) - 1;
		tchar* pszAfterStatusTag = (tchar*)strstr((const char*)szS, (const char*)pszStatusTag);
		if (pszAfterStatusTag != NULL) {
			pszAfterStatusTag += iStatusTagLen;
			tchar* pszSlashStatusTag = strchr(pszAfterStatusTag, '<');
			if (pszSlashStatusTag == NULL) {
				pszAfterStatusTag = NULL;
			}
			else {
				// Create a copy of the description
				tint iDescLen = pszSlashStatusTag - pszAfterStatusTag;
				std::string sDescCopy = std::string(pszAfterStatusTag, iDescLen);
				// Release szS memory before throwing exception
				CINetUtil::ReleaseBuffer(&szS);
				// Throw exception
				throw IException::Create(IException::TypeGeneric, IException::ReasonGeneric, EXCEPTION_INFO, sDescCopy.c_str());
			}
		}

		if (pszAfterStatusTag == NULL) {
			// Unable to extract description - throw unknown exception
			CINetUtil::ReleaseBuffer(&szS);
			throw IException::Create(IException::TypeUnknown, IException::ReasonUnknown, EXCEPTION_INFO);
		}
	}

    int valueLength;
	tchar* szValue;
	CINetUtil::GetValue(szS, &valueLength, &szValue);
	//assuming size is ok as Base64UrlEncode::DecodeGetLength(valueLength) gives a bit to large result
	tint32 lenght = Base64UrlEncode::Decode(pResponse, szValue, valueLength);
	CINetUtil::ReleaseBuffer(&szValue);
	CINetUtil::ReleaseBuffer(&szS);
	return true;
}

#define RESPONSE_SYNTAX_ERR "Incomplete server response"
tint32 CINet::RegisterPOP2(const TPOP& POP, const void* pChallenge, tint32 iChallengeSize, void** ppResponse, tint32 iResponseSize, tchar* pszRespFileName, tchar* pszErrMsg)
{
	int length;
	tchar* szS;
	tchar* szParams;
	int size = strlen(RegisterPOP_PAR)+strlen((const char *) POP)+Base64UrlEncode::EncodeGetLength(iChallengeSize);
	szParams = new tchar[size];
	tchar* szChallengeEncoded = new tchar[Base64UrlEncode::EncodeGetLength(iChallengeSize)+1];
    int challengeSize = Base64UrlEncode::Encode(szChallengeEncoded, pChallenge, iChallengeSize);
	szChallengeEncoded[challengeSize] = 0;
	sprintf((char *) szParams,RegisterPOP_PAR, POP, szChallengeEncoded);
	try {
		CINetUtil::GetWebFile(szParams,(tchar*)"RegisterPOP.aspx",&length,&szS);
		delete [] szParams;
	}
	catch (IException* ex) {
		CINetUtil::ReleaseBuffer(&szS);
		strncpy(pszErrMsg, ex->GetFullDescription(), 128);
		ex->Destroy();
		return 0;
	}
    catch (...) {
		//Any other error
		CINetUtil::ReleaseBuffer(&szS);
		strcpy(pszErrMsg, "Unhandled (...) exception");
		return 0;
	}

	// Lasse, the following code should be modified at some time - but it works
	// Search for OK result tag
	tchar* pszOk = strstr(szS, "<result status=\"ok\">");
	if (pszOk == NULL)
	{
		// Search for error description
		const tchar pszStatusTag[] = "<sComment>";
		const tint iStatusTagLen = sizeof(pszStatusTag) - 1;
		tchar* pszAfterStatusTag = (tchar*)strstr((const char*)szS, (const char*)pszStatusTag);
		if (pszAfterStatusTag != NULL) {
			pszAfterStatusTag += iStatusTagLen;
			tchar* pszSlashStatusTag = strchr(pszAfterStatusTag, '<');
			if (pszSlashStatusTag == NULL) {
				pszAfterStatusTag = NULL;
			}
			else {
				// Create a copy of the description
				tint iDescLen = pszSlashStatusTag - pszAfterStatusTag;
				tchar* psz = pszErrMsg + sprintf(pszErrMsg, "Server reply: ");
				strncpy(psz, pszAfterStatusTag, iDescLen);
				psz[iDescLen] = '\0';
				// Release szS memory before exit
				CINetUtil::ReleaseBuffer(&szS);
				return 0;
			}
		}

		// Unable to extract description - return syntax error
		CINetUtil::ReleaseBuffer(&szS);
		strcpy(pszErrMsg, RESPONSE_SYNTAX_ERR);
		return 0;
	}

	int iResponses = 0;
    int valueLength;
	tchar* szValue;
	// Lasse, modified 2007-09-07 - avoid theoretical crash
	//CINetUtil::GetValuePtr(pszOk + 20, &szValue, &valueLength);
	if (!CINetUtil::GetValuePtr(pszOk + 20, &szValue, &valueLength)) {
		CINetUtil::ReleaseBuffer(&szS);
		return 0;
	}
	// .. Lasse

	// Allocate return buffer that is at least big enough
	*ppResponse = (void*)new tchar[valueLength];
	//assuming size is ok as Base64UrlEncode::DecodeGetLength(valueLength) gives a bit to large result
	tint32 iDecodedLen = Base64UrlEncode::Decode(*ppResponse, szValue, valueLength);
	iResponses = iDecodedLen / iResponseSize;
	if ((iResponses * iResponseSize) != iDecodedLen)
	{
		// Number of bytes isn't an integer multiply of iResponseSize
		
		// Release buffers
		CINetUtil::ReleaseBuffer(&szS);
		delete[] (tchar*)*ppResponse;
		*ppResponse = NULL;

		// Explain error
		strcpy(pszErrMsg, RESPONSE_SYNTAX_ERR);
		return 0;
	}

	// Search for response file name
	tint32 iRespFileNameLen = 0;
	tchar* pszRespFileNameIx0 = strstr(szValue + valueLength, "<sRespFileName>");
	if (pszRespFileNameIx0 != NULL)
	{
		pszRespFileNameIx0 += 15;
		tchar* pszRespFileNameIx1 = pszRespFileNameIx0;
		while ((*pszRespFileNameIx1 != '\0')&&(iRespFileNameLen == 0))
		{
			if (*pszRespFileNameIx1 == '<')
				iRespFileNameLen = (pszRespFileNameIx1 - pszRespFileNameIx0);
			else
				pszRespFileNameIx1++;
		}
	}
	if (iRespFileNameLen == 0)
	{
		// Default name
		strcpy(pszRespFileName, "data.resp");
	}
	else
	{
		// Special name
		strncpy(pszRespFileName, pszRespFileNameIx0, iRespFileNameLen);
		pszRespFileName[iRespFileNameLen] = '\0';
	}

	CINetUtil::ReleaseBuffer(&szS);
	return iResponses;
}
