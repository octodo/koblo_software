// CUploader MS-Windows only code
// Is included into CUploader.h

protected:


/// Modify:

	volatile HINTERNET mhInitialize;
	volatile HINTERNET mhConnection;
    volatile HINTERNET mhFile;
	volatile DWORD mdwContextID;

	tuint64 muiTotalSize;

