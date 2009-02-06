// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


//!!! To Do: turn mpHost in to a global pointer


/*! \class CBaseModule
 * \brief Implementation of kspi::IModule
*/
class CBaseModule : public virtual kspi::IModule
{
public:
	//! Constructor
	CBaseModule(kspi::IHost* pHost);

	//! Destructor
	virtual ~CBaseModule();

	//! IModule override
	virtual tuint32 KSPI_CALL GetNrOfPlugIns();

	//! IModule override
	virtual tint32 KSPI_CALL GetCompanyID();

	//! IModule override
	virtual void KSPI_CALL GetCompanyName(tchar* pszName);

	//! Returns instance handle
#ifdef WIN32
	virtual HINSTANCE GetInstanceHandle() {return mhInstance;}
#endif
#ifdef _Mac
	virtual void* GetInstanceHandle() {return NULL;}
#endif

	//! Returns host
	/*!
		\return kspi::IHost*: Pointer to host
	*/
	virtual kspi::IHost* GetHost() {return mpHost;}

protected:
#ifdef WIN32
	//! Instance handle
	HINSTANCE mhInstance;
#endif	// WIN32

	//! Pointer to host
	kspi::IHost* mpHost;
};


