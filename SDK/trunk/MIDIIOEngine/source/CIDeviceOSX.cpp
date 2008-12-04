/*!	\file CIDeviceOSX.cpp
	\author Michael Olsen
	\date 13/Feb/2006
	\date 13/Feb/2006
*/

#include "mioeInternal.h"

void ReadProc(const MIDIPacketList *pktlist, void *refCon, void* pThis);

CIDeviceOSX::CIDeviceOSX(tint iIndex)
	: mpCallback(NULL), mbMIDIStopped(true)
{
	Open(iIndex);
}

CIDeviceOSX::~CIDeviceOSX()
{
	Close();
}

IIDevice* IIDevice::Create(void* p)
{
	tint iIndex = *((tint*)p);
	return dynamic_cast<IIDevice*>(new CIDeviceOSX(iIndex));
}

void CIDeviceOSX::Destroy()
{
	delete dynamic_cast<CIDeviceOSX*>(this);
}

void CIDeviceOSX::SetCallback(IIDeviceCallback* pCallback)
{
	mpCallback = pCallback;
}

void CIDeviceOSX::Open(tint iIndex)
{
	Close();

	mbMIDIStopped = false;

	OSStatus err = ::MIDIClientCreate(CFSTR("KeyToSound"), NULL, NULL, &client);

	err = ::MIDIInputPortCreate(client, CFSTR("Input port"), ReadProc, (void*)this, &inPort);

	MIDIEndpointRef src = ::MIDIGetSource(iIndex);

	err = ::MIDIPortConnectSource(inPort, src, NULL);
}

void CIDeviceOSX::Close()
{
	mbMIDIStopped = true;
}

void CIDeviceOSX::MIDIIn(const MIDIPacketList *pktlist, void *refCon)
{
	if (mpCallback) {
		MIDIPacket *packet = (MIDIPacket *)pktlist->packet;	// remove const (!)
		for (unsigned int j = 0; j < pktlist->numPackets; ++j) {
			mpCallback->OnMIDI(packet->data[0], packet->data[1], packet->data[2]);

			packet = MIDIPacketNext(packet);
		}
	}
}

void ReadProc(const MIDIPacketList *pktlist, void *pThis, void*)
{
	if (pThis) {
		CIDeviceOSX* pDevice = (CIDeviceOSX*)pThis;
		pDevice->MIDIIn(pktlist, NULL);
	}
}

/*void CIDeviceOSX::MIDIInProc(HMIDIIN hMIDIIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
{
	if(wMsg == MIM_DATA || wMsg == MIM_MOREDATA) {
		if (dwParam1 != 254) {
			// High word, Low-order byte contains second byte of MIDI data.
			BYTE db2 = (BYTE)(dwParam1 >>16) & 0xFF;

			// Low word, High-order byte Contains the first byte of MIDI data
			BYTE db1 = (BYTE)(dwParam1 >> 8) & 0xFF;

			// Low word, Low-order byte Contains the MIDI status.
			BYTE St  = (BYTE)dwParam1 & 0xFF;

			// Callback might be NULL, if it's not set yet
			if (mpCallback != NULL) {
				mpCallback->OnMIDI((tuchar)St, (tuchar)db1, (tuchar)db2);
			}
		}	
	}

	if (wMsg == MM_MIM_LONGDATA) {
		MIDIHDR* pHdr = (MIDIHDR*)dwParam1;

		// Callback might be NULL, if it's not set yet
		if (mpCallback != NULL) {
			mpCallback->OnMIDISysEx((const void*)(pHdr->lpData), (tint)(pHdr->dwBytesRecorded));
		}

		if (mbMIDIStopped == false) {
			// This appears to be the fastest way to add the buffer again. Whether it's "legal" is unknown, but it appears to be working
			if (pHdr->dwFlags & MHDR_DONE) {
				pHdr->dwFlags &= (~MHDR_DONE);
			}

			::midiInAddBuffer(mhMIDIIn, pHdr, sizeof(MIDIHDR));
		}
	}
}
*/

