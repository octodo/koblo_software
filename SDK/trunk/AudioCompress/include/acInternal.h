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

#include "ac.h"


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


//#include <ogg/config_types.h>
//#include <ogg/os_types.h>
//#include <ogg/ogg.h>
//#include <vorbis/vorbisenc.h>
//#include <lame.h>


namespace k2s {
namespace ac {
	

// There may only exists one initialized LAME engine at any time (it's not thread safe)
static volatile void* pLameInstance = NULL;


#include "CEncoder.h"
#include "CDecoder.h"
	
#include "CLameEncoder.h"
#include "CLameDecoder.h"

#include "CVorbisEncoder.h"
#include "CVorbisDecoder.h"

#include "CWaveEncoder.h"
#include "CWaveDecoder.h"

}	// namespace ac
}	// namespace k2s
