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

#include "STools.h"

//#include "bu.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#define DEBUG_BREAK __asm int 3
//#define ASSERT(a) if ((a) == 0) DEBUG_BREAK
#else
#define DEBUG_BREAK
//#define ASSERT(a)
#endif
#endif

#include <math.h>

// non dll-interface class
#pragma warning(disable: 4275)
#ifdef WIN32
#include <complex>
#endif	// WIN32

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif	// M_PI

#define E 2.7182818284590452353602874713527

#define MIN_UF_VOL (1e-20)

inline void UnderflowFix(float& rf)
{
	if (fabsf(rf) < MIN_UF_VOL) {
		rf = 0;
	}
}
inline void UnderflowFix(double& rd)
{
	if (fabs(rd) < MIN_UF_VOL) {
		rd = 0;
	}
}

#ifndef ASSERT
#define ASSERT(a)
#endif

#ifdef _Mac
#define __min(a, b) ((a) < (b) ? (a) : (b))
#define __max(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace k2s {

namespace st {

typedef double coeff;

#include "CMemAllocator.h"
#include "CDSPTools.h"
#ifdef WIN32
#include "CDSPToolsWin.h"
#endif	// WIN32
#ifdef _Mac
#include "CDSPToolsOSX.h"
#endif	// _Mac
#include "COscillatorBLIT.h"
#include "COscillatorLOT.h"
#include "COscillatorNoise.h"
#include "CModulate.h"
#include "CLFOLOT.h"
#include "CDrive.h"
#include "CEnvelope.h"
#include "CEnv.h"
#include "CModEnvelope.h"
#include "CVCFFilter.h"
#include "CRBJFilter.h"
#include "CChamberlinFilter.h"
#include "CTableCreator.h"
#include "CResampler.h"
#include "CInterpolateLinar.h"
#include "CWaveOscStream.h"
#include "CAmpSim.h"
#include "CLimiter.h"
#include "CSoftDistortion.h"
#include "CClipper.h"
#include "CAmp.h"
#include "CStream.h"
#include "CStreamManager.h"

#include "CVoiceManager.h"


}	// namespace st

}	// namespace k2s

using namespace k2s::st;






