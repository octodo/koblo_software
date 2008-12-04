/*!	\file CIDeviceWin.cpp
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

#include "mioeInternal.h"

CIDeviceWin::CIDeviceWin(tint iIndex)
	: mhMIDIIn((HMIDIIN)INVALID_HANDLE_VALUE),
	mpCallback(NULL), mbMIDIStopped(true)
{
	Open(iIndex);
}

CIDeviceWin::~CIDeviceWin()
{
	Close();
}

IIDevice* IIDevice::Create(void* p)
{
	tint iIndex = *((tint*)p);
	return dynamic_cast<IIDevice*>(new CIDeviceWin(iIndex));
}

void CIDeviceWin::Destroy()
{
	delete dynamic_cast<CIDeviceWin*>(this);
}

void CIDeviceWin::SetCallback(IIDeviceCallback* pCallback)
{
	mpCallback = pCallback;
}

void CIDeviceWin::Open(tint iIndex)
{
	Close();

	mbMIDIStopped = false;

	MMRESULT mmr = ::midiInOpen(&mhMIDIIn, iIndex, (DWORD)MIDIInProcWrap, (DWORD)this, CALLBACK_FUNCTION);
	if (mmr) {
		// Error... do something
	}

	int iBuffer;
	for (iBuffer = 0; iBuffer < NR_OF_SYSEX_BUFFERS; iBuffer++) {
		memset(&(mpHdrs[iBuffer]), 0, sizeof(MIDIHDR));
		mpHdrs[iBuffer].lpData = new char[SYSEX_BUFFER_SIZE];
		mpHdrs[iBuffer].dwBufferLength = SYSEX_BUFFER_SIZE;
		mpHdrs[iBuffer].dwUser = iBuffer;

		::midiInPrepareHeader(mhMIDIIn, &(mpHdrs[iBuffer]), sizeof(MIDIHDR));

		::midiInAddBuffer(mhMIDIIn, &(mpHdrs[iBuffer]), sizeof(MIDIHDR));
	}

	mmr = ::midiInStart(mhMIDIIn);
	if (mmr) {
		// Error... do something
		::midiInClose(mhMIDIIn);
		mhMIDIIn = (HMIDIIN)INVALID_HANDLE_VALUE;
	}
}

void CIDeviceWin::Close()
{
	if (mhMIDIIn != (HMIDIIN)INVALID_HANDLE_VALUE) {
		mbMIDIStopped = true;

		::midiInStop(mhMIDIIn);
		::midiInReset(mhMIDIIn);

		for (int iBuffer = 0; iBuffer < NR_OF_SYSEX_BUFFERS; iBuffer++) {
			::midiInUnprepareHeader(mhMIDIIn, &(mpHdrs[iBuffer]), sizeof(MIDIHDR));

			delete[] mpHdrs[iBuffer].lpData;
		}

		::midiInClose(mhMIDIIn);
		mhMIDIIn = (HMIDIIN)INVALID_HANDLE_VALUE;
	}
}

void CIDeviceWin::MIDIInProc(HMIDIIN hMIDIIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
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

void CALLBACK CIDeviceWin::MIDIInProcWrap(HMIDIIN hMIDIIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	CIDeviceWin* pDevice = (CIDeviceWin*)dwInstance;

	pDevice->MIDIInProc(hMIDIIn, wMsg, dwParam1, dwParam2);
}



