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


#ifndef _DSPE_INCLUDED
#define _DSPE_INCLUDED

// BaseUtilities
#include "bu.h"

namespace k2s {

/*! \namespace
 * \brief Namespace for DSP engine
 *
 * This namespace is for the DSP engine
*/
namespace dspe {

// Types
//! Sample point
typedef tfloat32 sample;

// Windows
#include "IWindowdspe.h"

// FFT
#include "IFFT.h"

}	// namespace dspe

}	// namespace k2s

#endif	_DSPE_INCLUDED

