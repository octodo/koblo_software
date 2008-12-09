// VorbisEncode2 - main.cpp
// The comment is from the original ogg/vorbis distribution:
/********************************************************************
*                                                                  *
* THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
* USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
* GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
* IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
*                                                                  *
* THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
* by the Xiph.Org Foundation http://www.xiph.org/                  *
*                                                                  *
********************************************************************

function: simple example encoder
last mod: $Id: encoder_example.c 13293 2007-07-24 00:09:47Z xiphmont $

********************************************************************/

/* takes a stereo 16bit 44.1kHz WAV file from stdin and encodes it into
a Vorbis bitstream */

/* Note that this is POSIX, not ANSI, code */


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

// Visual Studio requires this to be included nonconditional - XCode will ignore contents
#include "stdafx.h"

#include "bu.h"
#include "ac.h"


//int main(){
int main (int argc, char * const argv[]) {
    // insert code here...
    //std::cerr << "Hello, World!\n";
	
	CAutoDelete<k2s::ac::IEncoder> encoder(k2s::ac::IVorbisEncoder::Create());

	tbool bSyntaxErr = false;
	if ((argc < 3) || (argc > 5)) {
		bSyntaxErr = true;
	}
	ac::EQuality eQuality = ac::geQualityHigh;
	tchar pszQuality[256];
	if (argc >= 4) {
		char cDummy;
		if (sscanf(argv[3], "%s%c", pszQuality, &cDummy) != 1) {
			bSyntaxErr = true;
		}
		else {
			tint32 iQuality = encoder->GetQualityNb(pszQuality);
			if (iQuality >= 0) {
				eQuality = (ac::EQuality)iQuality;
			}
			else {
				bSyntaxErr = true;
			}
		}
	}
	int iChannels = -1;
	if (argc >= 5) {
		char cDummy;
		if (sscanf(argv[4], "%d%c", &iChannels, &cDummy) != 1) {
			bSyntaxErr = true;
		}
	}
	if (bSyntaxErr) {
		fprintf(stderr,"Syntax: VorbisEncode <wave-file> <ogg-file> [<quality> [<channels>]]\n\n");
		fprintf(stderr, "- Quality can be:\n  ");
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
		fprintf(stderr, "- Channels can be:\n  1, 2.\n");
		exit(1);
	}
	
	// Files
	CAutoDelete<IFile> fIn(IFile::Create());
	tchar pszInFile[513];
	IFile::PathToOS2((const tchar*)argv[1], (tchar*)pszInFile);
	std::cout << "Input file:\n  " << pszInFile << "\n";
	if (!fIn->Open(pszInFile, IFile::FileRead)) {
		fprintf(stderr,"Input file open failed.\n");
		exit(1);
	}
	CAutoDelete<IFile> fOut(IFile::Create());
	tchar pszOutFile[513];
	IFile::PathToOS2((const tchar*)argv[2], (tchar*)pszOutFile);
	std::cout << "Output file:\n  " << pszOutFile << "\n";
	if (!fOut->Open(pszOutFile, IFile::FileCreate)) {
		fIn->Close();
		fprintf(stderr,"Output file create failed.\n");
		exit(1);
	}
	
	tbool bSuccess = (
		(encoder->Init(fOut, eQuality, iChannels))
		&&
		(encoder->Process(fIn))
		&&
	    (encoder->Finalize())
    );
	//encoder->Destroy();
	
	//fIn->Close();
	//fOut->Close();

	return (bSuccess ? 0 : 1);
}
