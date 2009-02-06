//! Buffer for holding CXloader reply (when not written directly to file)
class CXloader_ReplyChainLink {
public:
	CXloader_ReplyChainLink(void* p, tuint32 uiBytes)
	{
		if (uiBytes == 0) {
			mpcBytes = 0;
		}
		else {
			mpcBytes = new tchar[uiBytes];
			memcpy(mpcBytes, p, uiBytes);
		}
		muiBytes = uiBytes;
		muiIndex = 0;
	}
	virtual ~CXloader_ReplyChainLink()
	{
		if (mpcBytes) {
			delete[] mpcBytes;
			mpcBytes = NULL;
		}
	}
	tuint32 GetBytes(tchar* pcBuff, tuint32 uiBytesWanted)
	{
		tuint32 uiBytesReturned = muiBytes - muiIndex;
		if (uiBytesReturned > uiBytesWanted) uiBytesReturned = uiBytesWanted;
		if (uiBytesReturned > 0) {
			tchar* pcIndex = mpcBytes + muiIndex;
			memcpy(pcBuff, pcIndex, uiBytesReturned);
			muiIndex += uiBytesReturned;
		}
		return uiBytesReturned;
	}
protected:
	tchar* mpcBytes;
	tuint32 muiBytes;
	tuint32 muiIndex;
}; // CXloader_ReplyChainLink
