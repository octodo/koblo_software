

#include "KSOS.h"

/*
	Error reporting
*/

void CLoad_Error::SoundLoadError(const tchar* psz)
{
#ifdef WIN32
	::MessageBox(NULL, (char*)psz, NULL, MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#endif	// WIN32
}

void CLoad_Error::SoundLoadGeneralParsingError(const tchar* psz)
{
	tchar psz2[512];
	sprintf((char*)psz2, "General parsing error in sound: %s", psz);
	SoundLoadError(psz2);
}


