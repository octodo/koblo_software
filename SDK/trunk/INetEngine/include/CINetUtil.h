
#define CONNECT_TIMEOUT 30 //in seconds
#define MAX_PAGE_SIZE 200000 //Max no of bytes in web page

class CINetUtil : public virtual IINetUtil
{
public:
	//! Constructor
	CINetUtil() {}

	//! Destructor
	virtual ~CINetUtil() {}

	//! Internal use only URL-Encoder
	static tbool URLEncode_Internal(const tchar* pszRaw, std::string& rsEncoded);

protected:


};


