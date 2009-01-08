// CDownloader MS-Windows only code
// Is included into CDownloader.h

protected:
	volatile HINTERNET mhInitialize;
	volatile HINTERNET mhConnection;
    volatile HINTERNET mhFile;
	volatile DWORD mdwContextID;

	tuint64 muiTotalSize;

	tbool OpenConnection();
	void CloseConnection();
	CMutex mMutex_Connection;