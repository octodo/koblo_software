// CXloader MS-Windows only code
// Is included into CXloader.h

protected:
	volatile HINTERNET mhInitialize;
	volatile HINTERNET mhConnection;
    volatile HINTERNET mhFile;
	volatile DWORD mdwContextID;

	tuint64 muiTotalSize;

