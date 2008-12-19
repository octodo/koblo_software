
#include "ineInternalOS.h"

CDownloader::CDownloader()
{
} // constructor


CDownloader::~CDownloader()
{
} // destructor


IDownloader* IDownloader::Create()
{
	return dynamic_cast<IDownloader*>(new CDownloader());
} // Create

void CDownloader::Destroy()
{
	delete dynamic_cast<CDownloader*>(this);
} // Destroy