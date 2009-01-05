// CDownloader MS-Windows only code

#include "ineInternalOS.h"


tbool CDownloader::DownloadPortion(tchar* pszBuffer, tint32 iBufferSize, tint32* piPortionSize)
{
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


