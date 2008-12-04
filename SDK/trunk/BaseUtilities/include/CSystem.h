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

class CSystem : public virtual ISystem
{
public:
	CSystem();
	virtual ~CSystem();

	//! ISystem override
	virtual void Destroy();
	//! ISystem override
	virtual EOSType GetOS() const {return mOSType;}
	//! ISystem override
	virtual EOSTypeMinor GetOSMinor() const {return mOSTypeMinor;}
	//! ISystem override
	virtual tint32 GetOSVersionNumberMajor() const {return miOSVersion;}
	//! ISystem override
	virtual tint32 GetOSVersionNumberMinor() const {return miOSVersionMinor;}
	//! ISystem override
	virtual tuint32 GetCPUCount() const {return muiCPUCount;}
	//! ISystem override
	virtual ECPUType GetCPU() const {return mCPUType;}
	//! ISystem override
	virtual tint32 GetCPUFeatures() const {return miCPUFeatures;};

protected:
	EOSType mOSType;

	EOSTypeMinor mOSTypeMinor;

	tint32 miOSVersion;

	tint32 miOSVersionMinor;

	tuint32 muiCPUCount;

	ECPUType mCPUType;

	tint32 miCPUFeatures;

#ifdef WIN32
	virtual void DetectOS();
	virtual void DetectCPU();
#endif	// WIN32
};
