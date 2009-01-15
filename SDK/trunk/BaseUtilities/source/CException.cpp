/*!	\file CException.cpp
	\author Michael Olsen
	\date 06/Jan/2005
	\date 06/Jan/2005
*/

#include "buInternalOS.h"

CException::CException(EType Type, EReason Reason, const tchar* pszFile, tint32 iLine, const tchar* pszDate, const tchar* pszDescription)
	: mType(Type), mReason(Reason), msDescription(pszDescription), msFile(pszFile), msDate(pszDate)
{
	tchar psz[32];
	sprintf(psz, "%d", iLine);
	msLine = std::string(psz);

#ifdef _Mac
	// (mo) Not only does XCode give us the full path to the file, it gives us the path to the project, and from there relative to the file, i.e.
	// /MacIntoshHD/Projects/KeyToSound/Products/KeyToSound/INetSynth3/Projects/OSXXCode/../../../../../SDK/BaseClasses/Source/CBaseGUI.cpp
	// This string is way more than we need, esp. because OSX has a 255 character limit on the message box, so we limit it to the part after the last '/'
	int Pos = msFile.find_last_of('/');
	if (Pos != std::string::npos) {
		msFile = msFile.substr(Pos + 1);
	}
#endif	// _Mac

	MakeFullDescription();
}

CException::~CException()
{
}

IException* IException::Create(EType Type, EReason Reason, const tchar* pszFile, tint32 iLine, const tchar* pszDate, const tchar* pszDescription)
{
	return dynamic_cast<IException*>(new CException(Type, Reason, pszFile, iLine, pszDate, pszDescription));
}

void CException::Destroy()
{
	delete dynamic_cast<CException*>(this);
}

IException::EType CException::GetType()
{
	return mType;
}

const IException::EType CException::GetType() const
{
	return mType;
}

IException::EReason CException::GetReason()
{
	return mReason;
}

const IException::EReason CException::GetReason() const
{
	return mReason;
}

const tchar* CException::GetDescription() const
{
	if (msDescription.size() == 0) {
		return 0;
	}
	else {
		return msDescription.c_str();
	}
}

const tchar* CException::GetFullDescription() const
{
	return (const tchar*)(msFullDescription.c_str());
}

void CException::MakeFullDescription()
{
	msFullDescription = std::string("Exception\n\n");

	msFullDescription += std::string("Type: ");
	GetTypeText(msFullDescription);
	msFullDescription += std::string("\n\n");

	msFullDescription += std::string("Reason: ");
	GetReasonText(msFullDescription);
	msFullDescription += std::string("\n\n");

	msFullDescription += std::string("File: ");
	msFullDescription += msFile;
	msFullDescription += std::string("\nLine: ");
	msFullDescription += msLine;
	msFullDescription += std::string("\nDate: ");
	msFullDescription += msDate;

	if (GetDescription()) {
		msFullDescription += std::string("\n\n");
		msFullDescription += std::string("Description: ");
		msFullDescription += std::string((char*)GetDescription());
	}
}

void CException::GetTypeText(std::string& rsString)
{
	switch(mType) {
		case TypeUnknown:
			rsString += std::string("Unknown");
			break;
		case TypeGeneric:
			rsString += std::string("Generic");
			break;
		case TypeCode:
			rsString += std::string("Code");
			break;
		case TypeStorageLimitationExceeded:
			rsString += std::string("Data storage limitation has been exceeded");
			break;
		case TypeFile:
			rsString += std::string("File");
			break;
		case TypeNetwork:
			rsString += std::string("Network");
			break;
	}
}

void CException::GetReasonText(std::string& rsString)
{
	switch(mReason) {
		case ReasonUnknown:
			rsString += std::string("Unknown");
			break;
		case ReasonGeneric:
			rsString += std::string("Generic");
			break;
		case ReasonCodePathInvalid:
			rsString += std::string("Code, path invalid");
			break;
		case ReasonCodeInvalidArgument:
			rsString += std::string("Code, argument invalid");
			break;
		case ReasonFileCannotOpen:
			rsString += std::string("File, cannot open");
			break;
		case ReasonFileCannotRead:
			rsString += std::string("File, cannot read");
			break;
		case ReasonFileCannotWrite:
			rsString += std::string("File, cannot write");
			break;
		case ReasonFileWrongType:
			rsString += std::string("File, wrong type");
			break;
		case ReasonFileGeneric:
			rsString += std::string("File, generic");
			break;
		case ReasonNetworkGeneric:
			rsString += std::string("Generic network error");
			break;
	    case ReasonFileTooLarge:
			rsString += std::string("File, too large");
			break;
		case ReasonNetworkCannotOpen:
			rsString += std::string("Network interface could not be opened");
			break;
		case ReasonNetworkTimeOut:
			rsString += std::string("Network or server time-out");
			break;
		case ReasonCouldntConnectToServer:
			rsString += std::string("Could'nt connect to server");
			break;
		case ReasonCouldntConnectToAppl:
			rsString += std::string("Could'nt connect to application");
			break;
		case ReasonRequestFailed:
			rsString += std::string("Request failed, answer not ok");
			break;
		case ReasonPageToLarge:
			rsString += std::string("Answer page was to large");
			break;
		case ReasonErrorReadingFromAppl:
			rsString += std::string("Couldnt read data from application");
			break;
		case ReasonWrongPathType:
			rsString += std::string("Unexpected path type");
			break;
	}
}




