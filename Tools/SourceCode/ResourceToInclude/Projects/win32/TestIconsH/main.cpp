#include "bu.h"
#include "..\..\..\icons.h"

int main()
{
	CAutoDelete<IFile> fOut(IFile::Create());
	fOut->Open("test.ico", IFile::FileCreate);
	fOut->Write((tchar*)gp_KobloStudio_ico, giSize_KobloStudio_ico);
}