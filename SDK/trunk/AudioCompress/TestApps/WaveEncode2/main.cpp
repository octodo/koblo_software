// WaveEncode2 - main.cpp


#ifndef _WIN32
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <time.h>
	#include <math.h>
	
	#ifndef _Mac
		#include <Carbon/Carbon.h>
		#define _Mac
	#endif //#ifndef _Mac
#endif // #ifndef _WIN32

//#ifdef _WIN32
// Visual Studio needs this file unconditionally included - XCode will ignore contents
	#include "stdafx.h"
//#endif // #ifdef _WIN32

#include "bu.h"
#include "ac.h"


//int main(){
int main (int argc, char * const argv[]) {
    // insert code here...
    //std::cerr << "Hello, World!\n";
	
	CAutoDelete<k2s::ac::IEncoder> encoder(k2s::ac::IEncoder::Create(ac::geAudioCodecWave));
	CAutoDelete<IFile> fIn1(IFile::Create());
	CAutoDelete<IFile> fIn2(IFile::Create());
	CAutoDelete<IFile> fOut(IFile::Create());

	tbool bIsTwoSources = false;

	tbool bError = false;
	if ((argc < 3) || (argc > 6)) {
		bError = true;
	}
	ac::EQuality eQuality = ac::geQualityDefault;
	tchar pszQuality[256];
	if (argc >= 4) {
		char cDummy;
		tbool bIsQFound = false;
		if (sscanf(argv[3], "%s%c", pszQuality, &cDummy) == 1) {
			tint32 iQuality = encoder->GetQualityNb(pszQuality);
			if (iQuality >= 0) {
				eQuality = (ac::EQuality)iQuality;
				bIsQFound = true;
			}
		}
		if (!bIsQFound) {
			bIsTwoSources = true;
		}
	}
	if ((bIsTwoSources) && (argc >= 5)) {
		char cDummy;
		tbool bIsQFound = false;
		if (sscanf(argv[4], "%s%c", pszQuality, &cDummy) == 1) {
			tint32 iQuality = encoder->GetQualityNb(pszQuality);
			if (iQuality >= 0) {
				eQuality = (ac::EQuality)iQuality;
				bIsQFound = true;
			}
		}
		if (!bIsQFound) {
			bError = true;
		}
	}
	int iChannelsIx = bIsTwoSources ? 5 : 4;
	int iChannels = -1;
	if (argc > iChannelsIx) {
		char cDummy;
		if (sscanf(argv[iChannelsIx], "%d%c", &iChannels, &cDummy) != 1) {
			bError = true;
		}
		else if ((iChannels < 1) || (iChannels > 2)) {
			bError = true;
		}
	}

	if (!bError) {
		// Files
		tchar pszInFile1[513];
		tchar pszInFile2[513];
		tchar pszOutFile[513];

		IFile::PathToOS2((const tchar*)argv[1], (tchar*)pszInFile1);
		std::cout << "Input file:\n  " << pszInFile1 << "\n";
		if (bIsTwoSources) {
			IFile::PathToOS2((const tchar*)argv[2], (tchar*)pszInFile2);
			std::cout << "Input file:\n  " << pszInFile2 << "\n";
		}
		if (!fIn1->Open(pszInFile1, IFile::FileRead)) {
			fprintf(stderr,"Input file 1 open failed.\n");
			bError = true;
		}
		if ((!bError) && (bIsTwoSources)) {
			if (!fIn2->Open(pszInFile2, IFile::FileRead)) {
				fprintf(stderr,"Input file 2 open failed.\n");
				bError = true;
			}
		}
		if (!bError) {
			tint32 iOutIx = bIsTwoSources ? 3 : 2;
			IFile::PathToOS2((const tchar*)argv[iOutIx], (tchar*)pszOutFile);
			std::cout << "Output file:\n  " << pszOutFile << "\n";
			if (!fOut->Open(pszOutFile, IFile::FileCreate)) {
				//fIn1->Close();
				fprintf(stderr,"Output file create failed.\n");
				bError = true;
			}
		}

		if (!bError) {
			bError = !encoder->Init(fOut, eQuality, iChannels);
			if (!bError) {
				if (
					(!encoder->Process(fIn1))
					||
					((bIsTwoSources) && (!encoder->Process(fIn2)))
					)
				{
					bError = true;
				}
				// Must finalize regardless of errors or app will crash
				if (!encoder->Finalize()) {
					bError = true;
				}
				// Delete output file on error
				if (bError) {
					fOut->Close();
					IFile::DeleteFile(pszOutFile);
				}
			}
		}
		//encoder->Destroy();
	}
	
	if (bError) {
		fprintf(stderr, "Syntax: WaveEncode <source-wave-file> [<source-wave-file>] <dest-wave-file> [<quality> [<channels>]]\n\n");
		fprintf(stderr, "Quality can be:\n  ");
		tint32 iFirstQ, iLastQ;
		encoder->GetQualityRange(&iFirstQ, &iLastQ);
		for (tint32 iQ = iFirstQ; iQ <= iLastQ; iQ++) {
			tchar pszName[32];
			encoder->GetQualityName(iQ, pszName, 32);
			fprintf(stderr, pszName);
			if (iQ < iLastQ)
				fprintf(stderr, ", ");
		}
		fprintf(stderr, ".\n");
		fprintf(stderr, "Channels can be:\n  1, 2.\n");
		exit(1);
	}

	return 0;
}
