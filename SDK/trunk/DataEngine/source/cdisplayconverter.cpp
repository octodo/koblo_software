/*!	\file CDisplayConverter.cpp
	\author Michael Olsen
	\date 07/Jan/2005
	\date 07/Jan/2005
	\todo Implement String2Value
*/


#include "deInternalOS.h"

#ifdef WIN32
// Function deprecated (strcpy)
#pragma warning(disable:4996)
#endif	// WIN32


CDisplayConverter::CDisplayConverter(tint iDecimals, tint iMaxNrOfStringChars, tint iNrOfStringDecimals)
	: miDecimals(iDecimals), miMaxNrOfStringChars(iMaxNrOfStringChars), miNrOfStringDecimals(iNrOfStringDecimals)
{
	mfMultiplier = 1 / pow((tfloat64)10, miDecimals);
}

CDisplayConverter:: ~CDisplayConverter()
{
}

IDisplayConverter* IDisplayConverter::Create(tint iDecimals, tint iMaxNrOfStringChars, tint iMaxNrOfStringDecimals)
{
	return dynamic_cast<IDisplayConverter*>(new CDisplayConverter(iDecimals, iMaxNrOfStringChars, iMaxNrOfStringDecimals));
}

void CDisplayConverter::Destroy()
{
	delete dynamic_cast<CDisplayConverter*>(this);
}

void CDisplayConverter::SetPostFix(const tchar* psz)
{
	msPostFix = std::string((char*)psz);
}

void CDisplayConverter::Value2String(tint32 iValue, tchar* pszRet)
{
	std::string sRet;

	// Check if there's a fixed string
	if (SearchForFixedString(iValue, sRet)) {
		// There was... copy it and return
		strcpy((char*)pszRet, sRet.c_str());
		return;
	}

	// Convert to floating point value
	tfloat64 fValue = iValue * mfMultiplier;

	// To avoid unpleasant surprises later on, we need to round now
	// We round by shifting the decimal point (in a 10-based system) according to the number of decimals we can have.
	// We can then use a standard rounding call, and shift back
	tint32 iMul = (tint32)pow((tfloat64)10, (tint)miNrOfStringDecimals);
	tfloat64 fTmp = fValue * iMul;
	fTmp = Round(fTmp);
	tint32 iTmp = (tint32)fTmp;
	fValue = (tfloat64)iTmp / iMul;

	// String we're working with
	tchar psz[128];
	// Points to where we currently are
	tchar* pszCur = psz;

	// Our current index
	tint iIndex = 0;

	// Check if value is negative
	if (fValue < 0.0) {
		// It is... start by adding a minus
		*pszCur = '-';
		pszCur++;
		iIndex = 1;
		// We invert the value, so it is now positive. Since we have the minus sign this will work.
		fValue = 0 - fValue;
	}

	// Integer part of value
	tint32 iInt = (tint32)fValue;
	// Decimal part of value
	fValue -= iInt;
	tchar pszTmp[128];
	// Create integer part of value as string
	sprintf((char*)pszTmp, "%d", iInt);

	// Copy integer part of value
	strcpy((char*)pszCur, (char*)pszTmp);
	pszCur += strlen((char*)pszTmp);
	iIndex += strlen((char*)pszTmp);

	// Check if we have a decimal part
//	if (fValue > 0.0) {
		// We do... Check if we're allowed any decimals
		if (miNrOfStringDecimals > 0) {
			// We are... Check if we're allowed any more characters
			if (miMaxNrOfStringChars - iIndex > 1) {
				// We are... add decimal point
				*pszCur = '.';
				pszCur++;
				iIndex++;
			}
			else {
				// No more characters allowed. Return with what we have
				*pszCur = 0;
				sRet = std::string((char*)psz);
				sRet += msPostFix;
				strcpy((char*)pszRet, sRet.c_str());
				return;
			}
		}

		// Number of characters we're still allowed
		tint iCharsRemaining = miMaxNrOfStringChars - iIndex;
		if (miNrOfStringDecimals < iCharsRemaining) {
			iCharsRemaining = miNrOfStringDecimals;
		}

		// No need to write more decimals than we have
		if (miDecimals < iCharsRemaining) {
			iCharsRemaining = miDecimals;
		}

		// Iterate for each remaining character
		tint i;
		for (i = 0; i < iCharsRemaining; i++) {
			// Multiply value to move value to convert to the left of the decimal point
			tint iPow = Float2Int(pow((tfloat64)10, (tint)(i + 1)));
			// Do the multiplication
			tfloat64 fTmp = fValue * iPow;
			if (fTmp <= 0) {
				fTmp = 0;
			}

			tint iTmp;
			// Check if this the last value
			if (i == iCharsRemaining - 1) {
				// It is, so we need to round
				iTmp = Float2Int(fTmp);
			}
			else {
				// It is not the last character, so we simply truncate it
				fTmp = floor(fTmp);
				iTmp = (tint)fTmp;
			}

			// Check if our value has 2 digits (might because of the rounding)
			if (iTmp == 10) {
				// It has... so we need to increment at least 1 number
				*pszCur = '0';
				pszCur++;
				iIndex++;

				// Go 2 characters back
				tchar* psz2 = pszCur;
				psz2 -= 2;
				bool bContinue = true;
				while (bContinue) {
					// Check if it is a decimal point
					if (*psz2 == '.') {
						// It is, so go one more step back
						psz2--;
					}
					else {
						// Not a decimal point, so check if the value is 9
						if (*psz2 == '9') {
							// The value is 9, so we need to change at least 1 more number
							*psz2 = '0';
							psz2--;
						}
						else {
							// The value was not 9, so we can simply increment it
							*psz2 = *psz2 + 1;
							// Done, so set to false
							bContinue = false;
						}
					}
				}
			}
			else {
				// There wasn't 2 digits, so convert normally
				sprintf((char*)pszTmp, "%d", iTmp);
				*pszCur = *pszTmp;
				pszCur++;
				iIndex++;

				// Update fValue, to reflect the part we already converted
				fValue -= (double)iTmp * pow((tfloat64)10, (tint)(-(i + 1)));
			}
		}
//	}

	*pszCur = 0;

	sRet = std::string((char*)psz);

	sRet += msPostFix;

	strcpy((char*)pszRet, sRet.c_str());
}

tint32 CDisplayConverter::String2Value(const tchar* psz) throw(IException*)
{
	tbool bNegative = false;
	// Check for initial minus
	if (psz[0] == '-') {
		bNegative = true;
		psz++;
	}

	if (psz[0] == 0) {
		// Empty string
		throw IException::Create(IException::TypeCode,
			IException::ReasonCodeInvalidArgument,
			EXCEPTION_INFO,
			(tchar*)"Empty String");
	}

	// Length of string
	tint iLength = strlen((char*)psz);

	// Current index into string
	tint iIndex = 0;

	// Current value
	double dValue = 0;

	// Do we have a decimal point?
	tbool bDecimal = false;

	// If we have a decimal point, what position are we reading?
	int iDecimalPosition = 1;

	while (iIndex != iLength) {
		tchar ch = psz[iIndex];

		if ((ch >= '0' && ch <= '9') ||
			ch == '.' ||
			ch == ',') {
			// Valid character
			if (ch == '.' || ch == ',') {
				// Decimal
				bDecimal = true;
			}
			else {
				// Number
				if (bDecimal) {
					// We're to the right of the decimal point
					double d = ch - '0';
					int i = iDecimalPosition;
					while (i--) {
						d *= 0.1;
					}
					dValue += d;

					iDecimalPosition++;
				}
				else {
					// We don't have decimal (yet)
					// Shift current value to the left to "make space"
					dValue *= 10;

					// Add the value
					dValue += (ch - '0');
				}
			}
		}
		else {
			// Invalid character
			if (iIndex == 0) {
				// First character, so we don't have any value yet
				throw IException::Create(IException::TypeCode,
					IException::ReasonCodeInvalidArgument,
					EXCEPTION_INFO,
					(tchar*)"Invalid string");
			}
			else {
				// Not the first character, so continue with what we have
				break;
			}
		}

		iIndex++;
	}

	tint32 iRet = (tint32)(dValue / mfMultiplier);

	if (bNegative) {
		iRet = -iRet;
	}

	return iRet;
}

void CDisplayConverter::AddFixedString(tint32 iValue, const tchar* psz)
{
	SFixedStringInfo Info;
	Info.iValue = iValue;
	Info.sString = std::string((char*)psz);

	FixedStrings.insert(FixedStrings.end(), Info);
}

tbool CDisplayConverter::SearchForFixedString(tint32 iValue, std::string& rs)
{
	std::list<SFixedStringInfo>::iterator it = FixedStrings.begin();
	for (; it != FixedStrings.end(); it++) {
		if (it->iValue == iValue) {
			rs = it->sString;
			return true;
		}
	}

	return false;
}

