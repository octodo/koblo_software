// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



#ifdef WIN32
#pragma pack(push, 1)
#endif

/*! \brief Information about 1 KSPI parameter. This is used for automateable parameters
*
*	It is important to note that the descructor will not delete pCurveData
*/
struct SParameterInfo
{
	//! The parameters name. Several versions of the name can exist, sperated by \@, for instance "Oscillator 1 Pitch\@Osc 1 Pitch\@Osc1Pitch\@O1Pitch\@O1P". The longest version should always come first, then the second etc, ended by the shortest. No part of the string maybe exceed 31 characters. Can be "" for no name.
	tchar pszName[256];
	//! Name of the parameters unit, for instance "dB" or "ms.". Can be "" for no name.
	tchar pszUnitName[256];

	//! There's 2 different types of parameters. Enums are typically discrete parameters (on/off etc.), and Long are typically continuous parameters, but it is possible to have a discrete Long parameter (it'll just don't have the individual position names as the Enum type does).
	enum EType {
		//! A Long parameter is used for continuous parameters, and discrete parameters where each position don't have a name. Floating point values don't exist, but fixed point decimals is achieved by setting iDecimals to non-zero.
		TypeLong = 0,
		//! An Enum parameter is used for discrete parameters, where each position has a name ("On"/"Off", "Saw"/"Square"/"Sine").
		TypeEnum,

		//! Not used. Present to force enum to 32-bit
		TypeForce = 0xffffffff
	};
	//! Type of the parameter
	EType Type;

	//! Parameters minimum value
	tint32 iMinValue;
	//! Parameters maximum value
	tint32 iMaxValue;
	//! Parameters default value
	tint32 iDefaultValue;
	//! Number of decimals in parameter. Only valid for TypeLong parameter types. If, for instance, iDecimals == 2, then 429 will be interpreted as 4.29
	tuint32 iDecimals;
	//! Special case text for the minimum possition, if any. Otherwise "". For instance "-Inf."
	tchar pszMinText[32];
	//! Special case text for the maximum possition, if any. Otherwise "". For instance "Inf."
	tchar pszMaxText[32];

	//! Sometimes it is an advantage to map a parameter to a control (in a HUI for instance) non-linear. The most common example is a frequency, which is always mapped logarithmic. ECurveType describe what type of mapping is used
	enum ECurveType {
		//! Linear mapping
		CurveTypeLinear = 0,
		//! Logarithmic mapping. For instance a frequency parameter. A logarithmic curve has a stength, which indicates "how much" logarithm is applied. The following formula is used: ParameterValue = (10 ^ (NormalisedValue * log10(Strength)) - 1) / (Strength - 1), where NormalisedValue is the parameter normalised to be between 0 and 1, and ParameterValue is also normalised, but no longer linear. A strength of 1000 will yield a quite standard logarithm, and 500 a more soft one, useable for instance for attack/release values in compressors.
		CurveTypeLog,
		//! "True" logarithmic mapping (using natural logarithm)
		CurveTypeTrueLog,
		//! Curve to simulate analogue volume fader
		CurveTypeX3,

		//! Not used. Present to force enum to 32-bit
		CurveTypeForce = 0xffffffff
	};
	//! Curve
	ECurveType CurveType;
	//! Extra data for curves other than linear. For Log: Points to tint32 with log strength (see CurveTypeLog). For TrueLog: Not used
	void* pCurveData;

	//! For TypeEnum type parameters a list of name, one for each value, seperated by \@. For TypeLong type parameters "". Example: "Saw\@Square\@Sine"
	tchar pszEnumText[1024];

	SParameterInfo() : Type(TypeLong), iMinValue(0), iMaxValue(0), iDefaultValue(0),
		iDecimals(0), CurveType(CurveTypeLinear), pCurveData(NULL) {
		pszName[0] = 0;
		pszUnitName[0] = 0;
		pszMinText[0] = 0;
		pszMaxText[0] = 0;
		pszEnumText[0] = 0;
	}
} KSPI_PACKED;

#ifdef WIN32
#pragma pack(pop)
#endif	// WIN32
