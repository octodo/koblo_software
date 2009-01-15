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
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



/*! \class CDisplayConverter
 * \brief Implementation of IDisplayConverter
*/
class CDisplayConverter : public virtual IDisplayConverter
{
public:
	//! Constructor
	/*!
		\param iDecimals [in]: Number of decimals in value (i.e. iDecimals == 2 means that 540 should be interpreted as 5.40)
		\param iMaxNrOfStringChars [in]: Maximum number of chars in string when converting to string
		\param iNrOfStringDecimals [in]: Maximum number of decimals in string when converting to string
	*/
	CDisplayConverter(tint iDecimals, tint iMaxNrOfStringChars, tint iNrOfStringDecimals);

	//! Destructor
	virtual ~CDisplayConverter();

	//! IDestructable override
	virtual void Destroy();

	//! IDisplayConverter override
	virtual void Value2String(tint32 iValue, tchar* psz);
	//! IDisplayConverter override
	virtual tint32 String2Value(const tchar* psz) throw (IException*);
	//! IDisplayConverter override
	virtual void SetPostFix(const tchar* psz);
	//! IDisplayConverter override
	virtual void AddFixedString(tint32 iValue, const tchar* psz);

protected:
	//! Multiplier to convert in/out-coming values to/from floating point value
	tfloat64 mfMultiplier;

	//! Max number of characters in converted string
	tint miMaxNrOfStringChars;

	//! Number of decimals in converted string
	tint miNrOfStringDecimals;

	//! Number of decimals in parameter
	tint miDecimals;

	//! Postfix to use, or empty if none
	std::string msPostFix;

	//! Searches for fixed string
	/*!
		\param iValue [in]: Value to search for
		\param rs [out]: Found string, if any
		\return tbool: true if string found, false otherwise
	*/
	tbool SearchForFixedString(tint32 iValue, std::string& rs);

	//! Struct with info about a fixed string
	struct SFixedStringInfo {
		//! Value which string is attached to
		tint32 iValue;

		//! Fixed string
		std::string sString;
	};

	//! List of fixed string
	std::list<SFixedStringInfo> FixedStrings;
};