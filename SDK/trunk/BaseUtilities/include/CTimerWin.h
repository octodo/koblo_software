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


/*! \class CTimerWin
 * \brief Win32 implementation of ITimer
*/
class CTimerWin : public virtual ITimer
{
public:
	//! Constructor
	CTimerWin();
	//! Destructor
	virtual ~CTimerWin();

	//! IDestructable override
	virtual void Destroy();

	//! ITimer override
	virtual void Init(tint32 iID, ITimerCallback* pCallback, tint32 iTimeMS);

	//! ITimer override
	virtual void Stop();

	//! Called when the extra thread starts
	virtual void ThreadWrap();

protected:
	//! Callback to call on timer events
	ITimerCallback* mpCallback;

	ITimerCallback_CanStop* mpCallback_CanStop;

	//! ID to pass to callback
	tint32 miID;

	//! Ms. between each timer
	tint32 miSleepTimeMS;

	static void __cdecl Thread(void* p);

	//! True if thread has stopped
	volatile tbool mbThreadStopped;

	//! True if thread should stop
	volatile tbool mbStopThread;
};






