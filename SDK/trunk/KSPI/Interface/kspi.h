// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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


#ifndef _KSPI_INCLUDED
#define _KSPI_INCLUDED

#ifdef KSPI_RAW

namespace k2s {

typedef int tint32;

typedef unsigned int tuint32;

typedef short tint16;

typedef unsigned short tuint16;

#ifdef WIN32
typedef __int64 tint64;
#endif
#ifdef _Mac
typedef long long tint64;
#endif

#ifdef WIN32
typedef unsigned __int64 tuint64;
#endif
#ifdef _Mac
typedef unsigned long long tuint64;
#endif

typedef signed char tchar;

typedef unsigned char tuchar;

typedef float tfloat32;

/*! \typedef
 * \brief 64-bit float
 *
 * Use this when excactly 64-bit, or min. 64-bit, is required
*/
typedef double tfloat64;

};

#else	// KSPI_RAW

// BaseUtilities
#include "bu.h"

// Data Engine
#include "de.h"

#endif	// KSPI_RAW


#ifdef WIN32
	#ifdef KSPI_DONT_USE_EXPORTS
		#define KSPI_EXPORT
	#else // KSPI_DONT_USE_EXPORTS
		// Setup symbols for import (host) or export (plug-in)
		#ifdef KSPI_EXPORTS
			#define KSPI_EXPORT __declspec(dllexport)
			#define KSPI_IMPORT __declspec(dllimport)
		#else	// KSPI_EXPORTS
			#ifdef KSPI_IMPORTS
				#define KSPI_EXPORT __declspec(dllimport)
				#define KSPI_IMPORT __declspec(dllexport)
			#else	// KSPI_IMPORTS
				#error You need to define either KSPI_EXPORTS or KSPI_IMPORTS
			#endif	// KSPI_IMPORTS
		#endif	// KSPI_EXPORTS
	#endif // KSPI_DONT_USE_EXPORTS

	#define KSPI_CALL __cdecl

#endif	// WIN32

#ifdef _Mac
	#define KSPI_IMPORT
	#define KSPI_EXPORT
	#define KSPI_CALL
#endif

#ifdef _Mac
// Neccasary because of the altivec vector macro
#ifdef vector
#undef vector
#endif
#endif

#include <list>
#include <vector>

// Set struct alignment to 8 while remembering current state
#ifdef WIN32
#pragma pack(push, 8)
#endif

#ifdef WIN32
#define KSPI_PACKED
#endif	// WIN32
#ifdef _Mac
#define KSPI_PACKED PACKED
#endif	// _Mac


#ifdef WIN32
// Needs to have dll-linkage
#pragma warning(disable: 4251)
#endif

#include "kspiC.h"

namespace k2s {

/*! \namespace
 * \brief Namespace for KSPI Plug-In architechture
 *
 * This namespace contains interfaces for accessing and developing a kspi plug-in
*/
namespace kspi {

//! Current revision. Used to check that plug-in and host are compatible. Should only be incremented in between official releases.
const tuint32 giCurrentRevision = 2;

#include "SParameterInfo.h"

#include "SMIDIEvent.h"

#include "IPlugIn.h"
#include "IGUI.h"
#include "IModule.h"

#include "IHost.h"

#include "KSPIMain.h"

}	// namespace kspi

}	// namespace k2s

// Reset struct alignment to original
#ifdef WIN32
#pragma pack(pop)
#endif

#endif	// _KSPI_INCLUDED


