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

class CWaveOscStream : public virtual IWaveOscStream
{
public:
	CWaveOscStream();

	virtual ~CWaveOscStream();

	virtual void Destroy();

	//! IWaveOscStream override
	virtual void Reset();
	//! IWaveOscStream override
	virtual void SetCallback(IWaveOscStreamCallback* pCallback, tint iID);
	//! IWaveOscStream override
	virtual void SetBufferSize(tint iBufferSize);
	//! IWaveOscStream override
	virtual void SetBufferCount(tint iBufferCount);
	//! IWaveOscStream override
	virtual void Process(tfloat32* pfOut, tfloat32 fPitch, tint iC);
	//! IWaveOscStream override
	virtual void SetMaxBufferSize(tint iMaxOutputBufferSize);
	//! IWaveOscStream override
	virtual void SetMaxPitch(tfloat fMaxPitch);
	//! IWaveOscStream override
	virtual void Timer();

protected:
	IWaveOscStreamCallback* mpCallback;
	int miCallbackID;

	int miBufferSize;
	int miBufferCount;

/*	int miBufferCurRead;
	int miBufferCurWrite;
	int miBufferReadIndex;*/

//	sample** mppfBuffers;

	int miMaxOutputBufferSize;
	float mfMaxPitch;

	IResampler* mpResampler;

	tfloat32* mpfTmpBuffer;

	void FillBuffer();

	//! Initializes object
	virtual void Init();

	//! Deinitializes object
	virtual void DeInit();

#ifdef WIN32
	static void __cdecl ThreadWrap(void* p);
#else	// WIN32
	static void ThreadWrap(void* p);
#endif	// WIN32

	void Thread();

	CMutex Mutex;

//	tbool mpbBufferFilled[8];
};

