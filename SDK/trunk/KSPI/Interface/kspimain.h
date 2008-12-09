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


//! Definition of kspi main entry point. Version number of kspi used when compiling module is returned in *pVersionNumber
typedef	IModule* (KSPI_CALL *kspiMainFunc)(void* pHostCallback, void* hHostObject, tuint32* piVersionNumber, void** ppDispatcher);

//! kspi main entry point
#ifdef WIN32
extern "C" KSPI_EXPORT kspiMainFunc kspiMain;
#endif
