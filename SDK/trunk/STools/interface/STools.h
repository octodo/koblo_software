
#ifndef _stools_h
#define _stools_h

#ifndef _WINDOWS
#ifdef _WIN32
#define _WINDOWS
#endif // #ifdef _WIN32
#endif // #ifndef _WINDOWS


#include <list>

#include "bu.h"


#ifdef _WINDOWS
#define ST_CALL __stdcall
#endif	// _WINDOWS

#ifdef macintosh
#define ST_CALL
#endif	// macintosh


namespace k2s {

namespace st {

//typedef double sample;
//typedef tfloat32 sample;

#include "IMemAllocator.h"

#include "SMIDIEvent.h"

#include "IDSPTools.h"

#include "IVoice.h"

#include "IVoiceManager.h"
#include "IOscillatorBLIT.h"
#include "IOscillatorLOT.h"
#include "IOscillatorNoise.h"
#include "IModulate.h"
#include "ILFOLOT.h"
#include "IDrive.h"
#include "IEnvelope.h"
#include "IEnv.h"
#include "IModEnvelope.h"
#include "IVCFFilter.h"
#include "IRBJFilter.h"
#include "IChamberlinFilter.h"
#include "ITableCreator.h"
#include "IInterpolate.h"
#include "IResampler.h"
#include "IWaveOscStreamCallback.h"
#include "IWaveOscStream.h"
#include "IAmpSim.h"
#include "ILimiter.h"
#include "ISoftDistortion.h"
#include "IClipper.h"
#include "IAmp.h"
#include "IStream.h"
#include "IStreamManager.h"

#include "stXlat.h"

}	// namespace st

}	// namespace k2s


#endif // #ifndef _stools_h
