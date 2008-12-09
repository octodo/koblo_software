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

#include "bu.h"


#ifdef WIN32
#include <io.h>
#endif

#ifdef WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef WIN32
// Inheritance via dominance
#pragma warning(disable: 4250)
// Identifier truncated to 255 characters
#pragma warning(disable: 4786)
#endif	// WIN32

namespace k2s {

// Utilities
#ifdef WIN32
#include "CTimeWin.h"
#else
#include "CTimeOSX.h"
#endif	// WIN32
#include "CSystem.h"
#ifdef WIN32
#include "CTimerWin.h"
#else
#include "CTimerOSX.h"
#endif	// WIN32

// Exceptions
#include "CException.h"

// Unique ID generator
#include "CUniqueID.h"

// File
#include "CFileCP.h"
#include "CFile.h"
#include "CFileMemory.h"
#include "CChunk.h"
#include "CChunkMemory.h"
#include "CChunkFile.h"
#include "CChunkFileMemory.h"

// File search
#ifdef WIN32
#include "CFileSearchWin.h"
#else
#include "CFileSearchOSX.h"
#endif	// WIN32

}	// namespace k2s


using namespace k2s;

