// CDownloader MS-Windows only code
// Is included into CDownloader.h

protected:
	HINTERNET mhInitialize;
	HINTERNET mhConnection;
    HINTERNET mhFile;
	DWORD mdwContextID;
	static DWORD dwContextID_Seed = 0;

	tuint64 muiTotalSize;

	tbool OpenConnection();
	void CloseConnection();