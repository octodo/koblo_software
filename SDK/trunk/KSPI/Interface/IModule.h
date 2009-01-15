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


/*! \brief Interface to access 1 or more plug-ins
*
*	A Module is a dynamic library (a .dll on Windows), which contains 1 or more plug-ins.
*/

class /*KSPI_EXPORT*/ IModule
{
public:
	//! Destroys module. Use this instead of delete, since it will make sure deallocation is done the right place
	virtual void KSPI_CALL Destroy() = 0;

	//! Returns the number of plug-ins in the module
	virtual tuint32 KSPI_CALL GetNrOfPlugIns() = 0;

	//! Creates a plug-in with a given index and returns it. The index is used to identify the plug-in type (from 0 to NumberOfPlugInsInModule - 1)
	virtual IPlugIn* KSPI_CALL CreateProcess(tuint32 iIndex) = 0;

	//! Returns the unique company ID of the module (all plug-ins in the module are from the same company). ID 0 is reserved for development.
	virtual tint32 KSPI_CALL GetCompanyID() = 0;

	//! Returns the company name. pszName is preallocated by the host with at least 33 characters (32 for the name, 1 for the trailing zero).
	virtual void KSPI_CALL GetCompanyName(tchar* pszName) = 0;
};

