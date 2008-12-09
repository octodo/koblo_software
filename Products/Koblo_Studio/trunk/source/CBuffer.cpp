
#include "KSOS.h"


CBuffer::CBuffer()
	: muiDataSize(0), muiChannels(0)
{
	mpMemAllocator = st::IMemAllocator::Create(16);

	mpDSPTools = st::IDSPTools::Create();

	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < guiMaxNrOfChannelsInBuffer; uiChannel++) {
		mppfData[uiChannel] = (tfloat32*)(mpMemAllocator->Alloc(giAudioMaxBufferSize * sizeof(tfloat32)));
		mpDSPTools->Clear(mppfData[uiChannel], giAudioMaxBufferSize);
	}

	// (mo) This needs to change
	muiDataSize = 32;
}

CBuffer::~CBuffer()
{
	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < guiMaxNrOfChannelsInBuffer; uiChannel++) {
		mpMemAllocator->Free((void*)(mppfData[uiChannel]));
	}

	mpDSPTools->Destroy();
}

CBuffer::CBuffer(const CBuffer& rBuffer)
{
	muiDataSize = rBuffer.muiDataSize;
	muiChannels = rBuffer.muiChannels;

	mpMemAllocator = st::IMemAllocator::Create(16);

	mpDSPTools = st::IDSPTools::Create();

	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < guiMaxNrOfChannelsInBuffer; uiChannel++) {
		mppfData[uiChannel] = (tfloat32*)(mpMemAllocator->Alloc(giAudioMaxBufferSize * sizeof(tfloat32)));
	}
	for (uiChannel = 0; uiChannel < muiChannels; uiChannel++) {
		mpDSPTools->Copy(mppfData[uiChannel], rBuffer.mppfData[uiChannel], giAudioMaxBufferSize);
	}
}

CBuffer CBuffer::operator=(const CBuffer& rBuffer)
{
	muiDataSize = rBuffer.muiDataSize;
	muiChannels = rBuffer.muiChannels;

	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < muiChannels; uiChannel++) {
		mpDSPTools->Copy(mppfData[uiChannel], rBuffer.mppfData[uiChannel], giAudioMaxBufferSize);
	}

	return *this;
}

CBuffer& CBuffer::operator+=(CBuffer& rBuffer)
{
	// Convert the source buffer to our format
	rBuffer.SetChannels(muiChannels);

	// The 2 buffers are now compatible, and we can accumulate
	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < muiChannels; uiChannel++) {
		mpDSPTools->Add(mppfData[uiChannel], rBuffer.mppfData[uiChannel], muiDataSize);
	}

	return *this;
}

void CBuffer::Accumulate(CBuffer& rBuffer, tfloat32 fVolume)
{
	// Convert the source buffer to our format
	rBuffer.SetChannels(muiChannels);

	// The 2 buffers are now compatible, and we can accumulate
	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < muiChannels; uiChannel++) {
		mpDSPTools->Add(mppfData[uiChannel], rBuffer.mppfData[uiChannel], fVolume, muiDataSize);
	}
}

void CBuffer::Clear()
{
	tuint32 uiChannel;
	for (uiChannel = 0; uiChannel < muiChannels; uiChannel++) {
		mpDSPTools->Clear(mppfData[uiChannel], giAudioMaxBufferSize);
	}
}

void CBuffer::SetChannels(tuint32 uiChannels)
{
	if (muiChannels == uiChannels) {
	tint32 iTest =0;
	iTest++;
		// No change
		return;
	}

	if (muiChannels == 0) {
		// Channels are just initially being set, do nothing
		muiChannels = uiChannels;
		return;
	}

	if (muiChannels == 1) {
		// Currently mono
		if (uiChannels == 2) {
			Mono2Stereo();
		}
		else if (uiChannels == 4) {
			Mono2Quad();
		}
		else if (uiChannels == 6) {
			Mono2FiveOne();
		}
		else {
			Mono2SevenOne();
		}
	}
	else if (muiChannels == 2) {
		// Currently stereo
		if (uiChannels == 1) {
			Stereo2Mono();
		}
		else if (uiChannels == 4) {
			Stereo2Quad();
		}
		else if (uiChannels == 6) {
			Stereo2FiveOne();
		}
		else {
			Stereo2SevenOne();
		}
	}
	else if (muiChannels == 4) {
		// Currently quad
		if (uiChannels == 1) {
			Quad2Mono();
		}
		else if (uiChannels == 2) {
			Quad2Stereo();
		}
		else if (uiChannels == 6) {
			Quad2FiveOne();
		}
		else {
			Quad2SevenOne();
		}
	}
	else if (muiChannels == 6) {
		// Currently 5.1
		if (uiChannels == 1) {
			FiveOne2Mono();
		}
		else if (uiChannels == 2) {
			FiveOne2Stereo();
		}
		else if (uiChannels == 4) {
			FiveOne2Quad();
		}
		else {
			FiveOne2SevenOne();
		}
	}
	else {
		// Currently 7.1
		if (uiChannels == 1) {
			SevenOne2Mono();
		}
		else if (uiChannels == 2) {
			SevenOne2Stereo();
		}
		else if (uiChannels == 4) {
			SevenOne2Quad();
		}
		else {
			SevenOne2FiveOne();
		}
	}

	muiChannels = uiChannels;
}

void CBuffer::Mono2Stereo()
{
	// o1 = o2 = i1
	mpDSPTools->Copy(mppfData[1], mppfData[0], muiDataSize);
}

void CBuffer::Mono2Quad()
{
	// o1 = o2 = o3 = o4 = i1
	mpDSPTools->Copy(mppfData[1], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[2], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[3], mppfData[0], muiDataSize);
}

void CBuffer::Mono2FiveOne()
{
	// o1 = o2 = o3 = o4 = o5 = o6 = i1
	mpDSPTools->Copy(mppfData[1], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[2], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[3], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[4], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[5], mppfData[0], muiDataSize);
}

void CBuffer::Mono2SevenOne()
{
	// o1 = o2 = o3 = o4 = o5 = o6 = o7 = o8 = i1
	mpDSPTools->Copy(mppfData[1], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[2], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[3], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[4], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[5], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[6], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[7], mppfData[0], muiDataSize);
}

void CBuffer::Stereo2Mono()
{
	// o1 = (i1 + i2) / 2
	mpDSPTools->Add(mppfData[0], mppfData[1], muiDataSize);
	mpDSPTools->Mul(mppfData[0], 0.5f, muiDataSize);
}

void CBuffer::Stereo2Quad()
{
	// o1 = i1. i4 = i1
	mpDSPTools->Copy(mppfData[3], mppfData[0], muiDataSize);
	// o2 = i2. i3 = i2
	mpDSPTools->Copy(mppfData[2], mppfData[1], muiDataSize);
}

void CBuffer::Stereo2FiveOne()
{
	// o1 = i1
	// o5 = i1
	mpDSPTools->Copy(mppfData[4], mppfData[0], muiDataSize);
	// o3 = i2
	mpDSPTools->Copy(mppfData[2], mppfData[1], muiDataSize);
	// o4 = i2
	mpDSPTools->Copy(mppfData[3], mppfData[1], muiDataSize);
	// o6 = (i1 + i2) / 2
	mpDSPTools->Add(mppfData[5], mppfData[0], mppfData[1], muiDataSize);
	mpDSPTools->Mul(mppfData[5], 0.5f, muiDataSize);
	// o2 = (i1 + i2) x 0.707106
	mpDSPTools->Add(mppfData[1], mppfData[0], muiDataSize);
	mpDSPTools->Mul(mppfData[1], 0.707106f, muiDataSize);
}

void CBuffer::Stereo2SevenOne()
{
	// o1 = i1
	// o6 = i1
	// o7 = i1
	mpDSPTools->Copy(mppfData[5], mppfData[0], muiDataSize);
	mpDSPTools->Copy(mppfData[6], mppfData[0], muiDataSize);
	// o3 = i2
	mpDSPTools->Copy(mppfData[2], mppfData[1], muiDataSize);
	// o4 = i2
	mpDSPTools->Copy(mppfData[3], mppfData[1], muiDataSize);
	// o5 = i2
	mpDSPTools->Copy(mppfData[4], mppfData[1], muiDataSize);
	// o8 = (i1 + i2) / 2
	mpDSPTools->Add(mppfData[7], mppfData[0], mppfData[1], muiDataSize);
	mpDSPTools->Mul(mppfData[7], 0.5f, muiDataSize);
	// o2 = (i1 + i2) x 0.707106
	mpDSPTools->Add(mppfData[1], mppfData[0], muiDataSize);
	mpDSPTools->Mul(mppfData[1], 0.707106f, muiDataSize);
}

void CBuffer::Quad2Mono()
{
	// o1 = (i1 + i2 + i3 + i4) / 4
	mpDSPTools->Add(mppfData[0], mppfData[1], muiDataSize);
	mpDSPTools->Add(mppfData[0], mppfData[2], muiDataSize);
	mpDSPTools->Add(mppfData[0], mppfData[3], muiDataSize);
	mpDSPTools->Mul(mppfData[0], 0.25f, muiDataSize);
}

void CBuffer::Quad2Stereo()
{
	// o1 = (i1 + i4) / 2
	mpDSPTools->Add(mppfData[0], mppfData[3], muiDataSize);
	mpDSPTools->Mul(mppfData[0], 0.5f, muiDataSize);
	// o2 = (i2 + i3) / 2
	mpDSPTools->Add(mppfData[1], mppfData[2], muiDataSize);
	mpDSPTools->Mul(mppfData[1], 0.5f, muiDataSize);
}

void CBuffer::Quad2FiveOne()
{
}

void CBuffer::Quad2SevenOne()
{
}

void CBuffer::FiveOne2Mono()
{
}

void CBuffer::FiveOne2Stereo()
{
}

void CBuffer::FiveOne2Quad()
{
}

void CBuffer::FiveOne2SevenOne()
{
}

void CBuffer::SevenOne2Mono()
{
}

void CBuffer::SevenOne2Stereo()
{
}

void CBuffer::SevenOne2Quad()
{
}

void CBuffer::SevenOne2FiveOne()
{
}

