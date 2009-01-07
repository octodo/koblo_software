// CDownloader MS-Windows only code
// Is included into CDownloader.h

protected:
	HINTERNET mhInitialize;
	HINTERNET mhConnection;
    HINTERNET mhFile;
	DWORD mdwContextID;

	tuint64 muiTotalSize;

	tbool OpenConnection();
	void CloseConnection();