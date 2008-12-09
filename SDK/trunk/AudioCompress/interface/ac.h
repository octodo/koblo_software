/*!	\file ac.h
	\author Lasse Steen Bohnstedt
	\date 2007-10-24
*/

#ifndef _AC_INCLUDED
#define _AC_INCLUDED

#ifdef WIN32
// Identifier truncated
#pragma warning(disable: 4786)
// C++ exception specification ignored
#pragma warning(disable: 4290)
// Function was declared deprecateds
#pragma warning(disable: 4996)
#endif

#include <math.h>
#include <stdlib.h>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif	// WIN32


/*! \namespace
 * \brief Namespace for Key2Sound classes and types
 *
 * All Key2Sound code is inside this namespaces
*/
namespace k2s {
namespace ac {

#include "acUtilities.h"

#include "IDecoder.h"
#include "IEncoder.h"

}	// namespace ac
}	// namespace k2s


using namespace k2s::ac;


#endif	// _AC_INCLUDED


