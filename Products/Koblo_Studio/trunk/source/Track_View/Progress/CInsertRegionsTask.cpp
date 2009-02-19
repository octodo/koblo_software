
#include "KSOS.h"

enum EInsertRegionsOrder {
	geInsertRegions_Start,

	geInsertRegions_Done
}; // EInsertRegionsOrder



CInsertRegionsTask::~CInsertRegionsTask() {
/*
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
*/
} // destructor


void CInsertRegionsTask::Destroy()
{
	delete dynamic_cast<CInsertRegionsTask*>(this);
} // Destroy


tbool CInsertRegionsTask::Init()
{
	return true;
} // Init


tbool CInsertRegionsTask::DoWork()
{
	tbool bWorkError = false;

	switch (miInsertRegionsOrder) {
		case geInsertRegions_Start:
			{
				/*
				// Update misc after load
				UpdateTrackNames();
				UpdateTrackRegions();
				LoadInserts();
				

				if (1) {
					// This may take some time
					CAutoDelete<ge::IWaitCursor> pWait(ge::IWaitCursor::Create());
					
					// Refresh display
					gpApplication->Stack_Tracks();
					gpApplication->Update_Zoom();
				}
					
				// Flush effect tails, return to position 0 (and pre-load samples)
				gpApplication->PlaybackGoToStart(true);
				*/

				gpApplication->Insert_Regions();

				miInsertRegionsOrder++;
			}
			break;

		default:
			// That's weird - what are we doing here?
			bWorkError = true;
			break;

	}

	if (bWorkError) {
		miInsertRegionsOrder = geInsertRegions_Done;
	}
	return !bWorkError;
} // DoWork


tbool CInsertRegionsTask::IsDone()
{
	return (miInsertRegionsOrder >= geInsertRegions_Done);
} // IsDone


/*
void CInsertRegionsTask::UpdateTrackNames()
{
	std::list<IChunk*>::iterator it = mlistIChunkTrackNames.begin();
	tint32 iTrack = 0;
	for ( ; it != mlistIChunkTrackNames.end(); it++) {
		IChunk* pChunk = *it;

		if (pChunk->GetSize() > 0) {
			tchar psz[1024];
			memset(psz, 0, 1024);
			pChunk->Read(psz, pChunk->GetSize());
			gpApplication->SetChannelName(iTrack, std::string((const char*)psz));
		}

		iTrack++;
	}
} // UpdateTrackNames


void CInsertRegionsTask::UpdateTrackRegions()
{
	std::list<IChunk*>::iterator it = mlistIChunkTrackRegions.begin();
	tint32 iTrack = 0;
	for ( ; it != mlistIChunkTrackRegions.end(); it++) {
		IChunk* pChunk = *it;

		gpDSPEngine->CreateRegionFromChunkData(iTrack, pChunk);

		iTrack++;
	}
} // UpdateTrackRegions


void CInsertRegionsTask::LoadInserts()
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

		kspi::IPlugIn* pInsert = gpDSPEngine->GetTrack(iChannel)->GetInsert(iInsert);
		if (pInsert) {
			tchar* p = new tchar[iSize];
			pChunk->Read(p, iSize);
			pInsert->SetChunk(p, iSize);
			delete[] p;
		}
	}
} // LoadInserts
*/
