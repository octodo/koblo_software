// VobisDecode2 - main.cpp
// The comment below is from the orginal ogg/vorbis distribution
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

function: simple example decoder
last mod: $Id: decoder_example.c 13293 2007-07-24 00:09:47Z xiphmont $

********************************************************************/

/* Takes a vorbis bitstream from stdin and writes raw stereo PCM to
stdout.  Decodes simple and chained OggVorbis files from beginning
to end.  Vorbisfile.a is somewhat more complex than the code below.  */

/* Note that this is POSIX, not ANSI code */


#ifndef _WIN32
	#include <iostream>
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>
	#ifndef _Mac
		#include <Carbon/Carbon.h>
		#define _Mac
	#endif //#ifndef _Mac
#endif // #ifndef _WIN32

#include "stdafx.h"

#include "bu.h"
#include "ac.h"


int main (int argc, char * const argv[]) {
    // insert code here...
    //std::cerr << "Hello, World!\n";

	tbool bSyntaxErr = false;
	if ((argc < 3) || (argc > 5)) {
		bSyntaxErr = true;
	}
	tint iBits = 24;
	if (argc >= 4) {
		char cDummy;
		if (sscanf(argv[3], "%d%c", &iBits, &cDummy) != 1) {
			bSyntaxErr = true;
		}
	}
	tint iChannels = 1;
	if (argc >= 5) {
		char cDummy;
		if (sscanf(argv[4], "%d%c", &iChannels, &cDummy) != 1) {
			bSyntaxErr = true;
		}
	}
	if (bSyntaxErr) {
		fprintf(stderr, "Syntax: VorbisDecode <ogg-file> <wave-file> [<bits> [<channels>]]\n");
		fprintf(stderr, "- Bits can be:\n  16, 24.\n- Channels can be:\n  1, 2.\n");
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
	
	CAutoDelete<IDecoder> decoder(ac::IVorbisDecoder::Create());
	if (decoder->Init(fOut, true, iBits, iChannels)) {
		decoder->Process(fIn);
		decoder->Finalize();
	}
	//decoder->Destroy();
	
	//fIn->Close();
	//fOut->Close();
	return(0);
}
