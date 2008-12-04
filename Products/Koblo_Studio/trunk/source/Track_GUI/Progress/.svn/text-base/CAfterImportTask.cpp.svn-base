
#include "KSOS.h"


CAfterImportTask::~CAfterImportTask() {
	std::list<IChunk*>::iterator it;

	for (it = mlistIChunkTrackNames.begin(); it != mlistIChunkTrackNames.end(); it++) {
		IChunk* pChunk = *it;
		pChunk->Destroy();
	}
	for (it = mlistIChunkTrackRegions.begin(); it != mlistIChunkTrackRegions.end(); it++) {
		IChunk* pChunk = *it;
		pChunk->Destroy();
	}
	for (it = mlistIChunkInserts.begin(); it != mlistIChunkInserts.end(); it++) {
		IChunk* pChunk = *it;
		pChunk->Destroy();
	}
} // destructor


void CAfterImportTask::Destroy()
{
	delete dynamic_cast<CAfterImportTask*>(this);
} // Destroy


tbool CAfterImportTask::Init(CKSPlugIn* pPlugIn)
{
	mpPlugIn = pPlugIn;
	miAfterImportOrder = 0;

	return true;
} // Init


tbool CAfterImportTask::DoWork()
{
	tbool bWorkError = false;

	switch (miAfterImportOrder) {
		case geAfterImport_Start:
			{
				// Update misc after load
				UpdateTrackNames();
				UpdateTrackRegions();
				LoadInserts();

				if (1) {
					// This may take some time
					CAutoDelete<ge::IWaitCursor> pWait(ge::IWaitCursor::Create());
					
					// Refresh display
					mpPlugIn->Stack_Tracks();
					mpPlugIn->Update_Zoom();
				}
					
				// Flush effect tails, return to position 0 (and pre-load samples)
				mpPlugIn->PlaybackGoToStart(true);

				miAfterImportOrder++;
			}
			break;

		default:
			// That's weird - what are we doing here?
			bWorkError = true;
			break;

	}

	if (bWorkError) {
		miAfterImportOrder = geAfterImport_Done;
	}
	return !bWorkError;
} // DoWork


tbool CAfterImportTask::IsDone()
{
	return (miAfterImportOrder >= geAfterImport_Done);
} // IsDone


void CAfterImportTask::UpdateTrackNames()
{
	std::list<IChunk*>::iterator it = mlistIChunkTrackNames.begin();
	tint32 iTrack = 0;
	for ( ; it != mlistIChunkTrackNames.end(); it++) {
		IChunk* pChunk = *it;

		if (pChunk->GetSize() > 0) {
			tchar psz[1024];
			memset(psz, 0, 1024);
			pChunk->Read(psz, pChunk->GetSize());
			mpPlugIn->SetChannelName(iTrack, std::string((const char*)psz));
		}

		iTrack++;
	}
} // UpdateTrackNames


void CAfterImportTask::UpdateTrackRegions()
{
	std::list<IChunk*>::iterator it = mlistIChunkTrackRegions.begin();
	tint32 iTrack = 0;
	for ( ; it != mlistIChunkTrackRegions.end(); it++) {
		IChunk* pChunk = *it;

		gpDSPEngine->CreateRegionFromChunkData(iTrack, pChunk);

		iTrack++;
	}
} // UpdateTrackRegions


void CAfterImportTask::LoadInserts()
{
	std::list<IChunk*>::iterator itChunk = mlistIChunkInserts.begin();
	for ( ; itChunk != mlistIChunkInserts.end(); itChunk++) {
		IChunk* pChunk = *itChunk;

		tint32 iSize = pChunk->GetSize();
		iSize -= sizeof(tint32) * 2;

		tint32 iChannel;
		pChunk->ReadCP(&iChannel, 1);
		tint32 iInsert;
		pChunk->ReadCP(&iInsert, 1);

		kspi::IPlugIn* pInsert = gpDSPEngine->GetChannel(iChannel)->GetInsert(iInsert);
		if (pInsert) {
			tchar* p = new tchar[iSize];
			pChunk->Read(p, iSize);
			pInsert->SetChunk(p, iSize);
			delete[] p;
		}
	}
} // LoadInserts
