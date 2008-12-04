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


#include "dspe.h"

#ifdef WIN32
// Inheritance via dominance
#pragma warning(disable: 4250)
// Identifier truncated to 255 characters
#pragma warning(disable: 4786)
#endif

#ifdef WIN32
// nsp
//#define nsp_UsesAll
#define nsp_UsesWin
#define nsp_UsesTransform
#define nsp_UsesConversion
#include "nsp.h"
#endif	// WIN32

#ifdef _Mac
#undef min
#undef max
#include <vDSP.h>
#endif	_Mac

namespace k2s {

namespace dspe {

// Types
//! Coefficient
typedef tfloat32 coeff;
//! Recursive history (feedback)
typedef tfloat64 historyy;
//! Non-recursive history
typedef tfloat32 historyx;

// Values
//! Pi
const tfloat64 fPi = 3.1415926535897932384626433832795;

// Windows
#include "CWindowdspe.h"

// FFT
#ifdef WIN32
#include "CFFTWin.h"
#endif	// WIN32
#ifdef _Mac
#include "CFFTOSX.h"
#endif	// _Mac


}	// namespace dspe

}	// namespace k2s


using namespace k2s;

using namespace dspe;
