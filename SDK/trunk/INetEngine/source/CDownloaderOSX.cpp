// CDownloader OS X only code

#include "ineInternalOS.h"


void CDownloader::Constructor_OSSpecific()
{
	// Nothing here yet
} // Constructor_OSSpecific

void CDownloader::Destructor_OSSpecific()
{
	// Nothing here yet
} // Destructor_OSSpecific


tbool CDownloader::DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize, tuint64* puiTotalSize)
{
	SetError("Not yet implemented");
	return false;
} // DownloadPortion


tbool CDownloader::Abort()
{
	if (mbIsDownloading) {
		// TODO: Add functionality...

		mbIsDownloading = false;
	}

	WipeParams();

	return true;
} // Abort


