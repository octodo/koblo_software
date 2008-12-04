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


#define NR_OF_SYSEX_BUFFERS		8

// Max size of 1 sysex buffer (in bytes)
#define SYSEX_BUFFER_SIZE	1024 * 128

/*! \class CIDeviceOSX
 * \brief OSX implementation of IIDevice
*/
class CIDeviceOSX : public virtual IIDevice
{
public:
	//! Contructor
	/*!
		\param p [in]: Index of device to create
	*/
	CIDeviceOSX(tint iIndex);

	//! Destructor
	virtual ~CIDeviceOSX();

	//! IDestructable override
	virtual void Destroy();

	//! IIDevice override
	virtual void SetCallback(IIDeviceCallback* pCallback);
	
	//! Called when MIDI is received
	void MIDIIn(const MIDIPacketList *pktlist, void *refCon);

protected:
	//! Callback to call on MIDI in
	IIDeviceCallback* mpCallback;

	MIDIClientRef client;

	MIDIPortRef inPort;

	//! Handle to MIDI device
//	HMIDIIN mhMIDIIn;

	//! Buffers to hold sys-ex
//	MIDIHDR mpHdrs[NR_OF_SYSEX_BUFFERS];

	//! If true MIDI has been stopped, and sys-ex buffers should not be re-prepared
	bool mbMIDIStopped;

	//! Opens the device
	virtual void Open(tint iIndex);

	//! Closes the device
	virtual void Close();

	//! Static callback function
//	static void CALLBACK MIDIInProcWrap(HMIDIIN hMIDIIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	//! Non-static callback function
//	void MIDIInProc(HMIDIIN hMIDIIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2);
};

