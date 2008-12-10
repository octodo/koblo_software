
#include "KSOS.h"


CProgressTask::~CProgressTask()
{
} // destructor


void CProgressTask::Destroy()
{
	delete this;
} // Destroy


void CProgressTask::DeleteDstFile(IFile* pFile)
{
	tchar pszFile[1024];
	pFile->GetPathName(pszFile);
	pFile->Close();
	IFile::DeleteFile(pszFile);
} // DeleteDstFile
