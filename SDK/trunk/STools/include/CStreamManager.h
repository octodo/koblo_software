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


class CStreamManager : public virtual IStreamManager, public virtual ITimerCallback
{
public:
	//! Constructor
	CStreamManager();

	//! Destructor
	~CStreamManager();

	//! IDestructable override
	virtual void Destroy();

	//! IStreamManager override
	virtual void CreateStreams(tint32 iPolyphony);
	//! IStreamManager override
	virtual IStream* GetStream();
	//! IStreamManager override
	virtual void ReleaseStream(IStream* pStream);
	//! IStreamManager override
	virtual tbool GetStutter();

	//! ITimerCallback override
	virtual void OnTimer(tint32 iID);

protected:
	//! Our timer
	ITimer* mpTimer;

	//! List of streams
	std::list<IStream*> mStreams;

	//! Array of booleans, to keep track of which streams are currently in use.
	tbool* mabStreamInUse;
	CMutex mMutex_ForStreamIxInUse;
	tbool IsStreamIxInUse(tint32 iIx);
	void SetStreamIxInUse(tint32 iIx, tbool bInUse);
	
	//! Whether sound has stuttered resently
	volatile tbool mbStutter;
};
